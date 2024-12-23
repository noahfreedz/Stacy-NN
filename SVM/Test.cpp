#include <crow.h>
#include <string>
#include "SVM.h"
#include <iostream>
#include <random>
#include <ctime>
#include <ostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <cerrno>
#include <sstream>
#include <numeric>
#include <algorithm>

using namespace stacy;

int32_t swap32(int32_t value) {
   return ((value & 0xFF000000) >> 24) |
          ((value & 0x00FF0000) >> 8) |
          ((value & 0x0000FF00) << 8) |
          ((value & 0x000000FF) << 24);
}

double getRandom(double min, double max) {
   static std::random_device rd;
   static std::mt19937 gen(rd());
   std::uniform_real_distribution<double> dis(min, max);
   return dis(gen);
}

vector<vector<double>> readMNISTImages(const string& filePath, int numImages, int numRows, int numCols) {
   ifstream file(filePath, ios::binary);
   vector<vector<double>> images;

   if (file.is_open()) {
       int magicNumber = 0;
       int numberOfImages = 0;
       int rows = 0;
       int cols = 0;

       file.read(reinterpret_cast<char*>(&magicNumber), 4);
       file.read(reinterpret_cast<char*>(&numberOfImages), 4);
       file.read(reinterpret_cast<char*>(&rows), 4);
       file.read(reinterpret_cast<char*>(&cols), 4);

       magicNumber = swap32(magicNumber);
       numberOfImages = swap32(numberOfImages);
       rows = swap32(rows);
       cols = swap32(cols);

       for (int i = 0; i < numImages; ++i) {
           vector<double> image;
           for (int j = 0; j < numRows * numCols; ++j) {
               unsigned char pixel = 0;
               file.read(reinterpret_cast<char*>(&pixel), 1);
               image.push_back(static_cast<double>(pixel) / 255.0);
           }
           images.push_back(image);
       }
       file.close();
   }
   return images;
}

vector<int> readMNISTLabels(const string& filePath, int numLabels) {
   ifstream file(filePath, ios::binary);
   vector<int> labels;

   if (file.is_open()) {
       int magicNumber = 0;
       int numberOfLabels = 0;

       file.read(reinterpret_cast<char*>(&magicNumber), 4);
       file.read(reinterpret_cast<char*>(&numberOfLabels), 4);

       magicNumber = swap32(magicNumber);
       numberOfLabels = swap32(numberOfLabels);

       for (int i = 0; i < numLabels; ++i) {
           unsigned char label = 0;
           file.read(reinterpret_cast<char*>(&label), 1);
           labels.push_back(static_cast<int>(label));
       }
       file.close();
   }
   return labels;
}

vector<MNSTData> convertToMNSTDataFormat(const vector<vector<double>>& images,
                                    const vector<int>& labels) {
   vector<MNSTData> mnistData;
   for(size_t i = 0; i < images.size(); i++) {
       MNSTData data;
       data.data = images[i];
       data.lable = labels[i];
       mnistData.push_back(data);
   }
   return mnistData;
}

vector<SVMData> initializeSVMClassifiers(int inputDim) {
   vector<SVMData> classifiers(10);
   for(auto& classifier : classifiers) {
       double scale = sqrt(2.0 / inputDim);
       classifier.W.resize(inputDim);
       for(auto& w : classifier.W) {
           w = getRandom(-scale, scale);
       }
       classifier.B = 0;
   }
   return classifiers;
}

void evaluateModel(const vector<MNSTData>& testData, const vector<SVMData>& classifiers) {
   int correct = 0;
   int total = testData.size();

   for(const auto& sample : testData) {
       double bestScore = -std::numeric_limits<double>::infinity();
       int prediction = -1;

       for(int digit = 0; digit < 10; digit++) {
           double score = SVM(1, 0.01).predictOne(sample.data, classifiers[digit].W, classifiers[digit].B);
           if(score > bestScore) {
               bestScore = score;
               prediction = digit;
           }
       }

       if(prediction == sample.lable) {
           correct++;
       }
   }

   double accuracy = static_cast<double>(correct) / total * 100;
   cout << "Accuracy: " << accuracy << "%" << endl;
}

void evaluateSingleImage(const MNSTData& sample, const std::vector<SVMData>& classifiers, int& prediction, std::vector<double>& allScores) {
    double bestScore = -std::numeric_limits<double>::infinity();
    prediction = -1;

    // Get prediction from each classifier
    for(int digit = 0; digit < 10; digit++) {
        double score = SVM(1, 0.01).predictOne(sample.data, classifiers[digit].W, classifiers[digit].B);
        allScores[digit] = score;

        if(score > bestScore) {
            bestScore = score;
            prediction = digit;
        }
    }
}

int main() {
    // Parameters
    const int numTrainImages = 60000;
    const int numTestImages = 10000;
    const int imageRows = 28;
    const int imageCols = 28;
    const double learningRate = 0.00007;
    const double cost = .008;
    const int numEpochs = 7;

    crow::SimpleApp app;

    auto trainImages = readMNISTImages("set1-images.idx3-ubyte", numTrainImages, imageRows, imageCols);
    auto trainLabels = readMNISTLabels("set1-labels.idx1-ubyte", numTrainImages);
    auto trainData = convertToMNSTDataFormat(trainImages, trainLabels);

    auto testImages = readMNISTImages("Testing-Data-Images.idx3-ubyte", numTestImages, imageRows, imageCols);
    auto testLabels = readMNISTLabels("TestingData-labels.idx1-ubyte", numTestImages);
    auto testData = convertToMNSTDataFormat(testImages, testLabels);

    auto classifiers = initializeSVMClassifiers(imageRows * imageCols);

    SVMMulti multiSvm(learningRate, cost);
    multiSvm.train(trainData, classifiers, numEpochs);

    // Evaluate on test set
    cout << "Evaluating model..." << endl;
    evaluateModel(testData, classifiers);

    SVMPersistence::SaveSVMData(classifiers, "../models.bin");


    CROW_ROUTE(app, "/mnist.jsx")
    ([]() {
    std::ifstream file("web/mnist.jsx");
    std::stringstream buffer;
    buffer << file.rdbuf();
    return crow::response(buffer.str());
});

    CROW_ROUTE(app, "/")
    ([&trainData, &classifiers]() {
    // First, get 9 random samples and their predictions
    std::vector<size_t> indices(trainData.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    // Create the JSON data string
    std::stringstream jsonData;
    jsonData << "window.initialMNISTData = [";

    for (size_t i = 0; i < 9; i++) {
        const auto& sample = trainData[indices[i]];
        int prediction = -1;
        std::vector<double> allScores(10);
        evaluateSingleImage(sample, classifiers, prediction, allScores);

        if (i > 0) jsonData << ",";
        jsonData << "{\"data\":[";
        for (size_t j = 0; j < sample.data.size(); ++j) {
            if (j > 0) jsonData << ",";
            jsonData << sample.data[j];
        }
        jsonData << "],\"prediction\":" << prediction << "}";
    }
    jsonData << "];";

    crow::response res;
    res.set_header("Content-Type", "text/html");
    res.body = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <title>MNIST Visualization</title>
            <script src="https://unpkg.com/react@18/umd/react.development.js"></script>
            <script src="https://unpkg.com/react-dom@18/umd/react-dom.development.js"></script>
            <script src="https://unpkg.com/babel-standalone@6/babel.min.js"></script>
            <link href="https://cdn.jsdelivr.net/npm/tailwindcss@2.2.19/dist/tailwind.min.css" rel="stylesheet">
        </head>
        <body>
            <div id="root">Loading...</div>
            <script>)" + jsonData.str() + R"(</script>
            <script type="text/babel">
                const MNISTDigit = ({ imageData }) => {
                    const canvasRef = React.useRef(null);

                    React.useEffect(() => {
                        const canvas = canvasRef.current;
                        const ctx = canvas.getContext('2d');
                        const imageDataObj = ctx.createImageData(28, 28);

                        for (let i = 0; i < imageData.length; i++) {
                            imageDataObj.data[i * 4] = imageData[i] * 255;
                            imageDataObj.data[i * 4 + 1] = imageData[i] * 255;
                            imageDataObj.data[i * 4 + 2] = imageData[i] * 255;
                            imageDataObj.data[i * 4 + 3] = 255;
                        }

                        ctx.putImageData(imageDataObj, 0, 0);
                    }, [imageData]);

                    return (
                        <canvas
                            ref={canvasRef}
                            width={28}
                            height={28}
                            className="w-24 h-24 border border-gray-300 bg-white"
                            style={{ imageRendering: 'pixelated' }}
                        />
                    );
                };

                const MNISTGrid = () => {
                    const [digits, setDigits] = React.useState(window.initialMNISTData || []);
                    const [isLoading, setIsLoading] = React.useState(false);

                    const handleShuffle = async () => {
                        setIsLoading(true);
                        try {
                            const response = await fetch('/api/shuffle');
                            const data = await response.json();
                            setDigits(data.samples);
                        } catch (error) {
                            console.error('Error shuffling digits:', error);
                        }
                        setIsLoading(false);
                    };

                    return (
                        <div className="max-w-2xl mx-auto p-6">
                            <div className="bg-white rounded-lg shadow-lg">
                                <div className="p-6">
                                    <h2 className="text-2xl font-bold text-center mb-6">MNIST Digit Recognition</h2>
                                    <div className="grid grid-cols-3 gap-4 mb-6">
                                        {digits.map((digit, i) => (
                                            <div key={i} className="flex flex-col items-center p-4 bg-gray-50 rounded-lg">
                                                <MNISTDigit imageData={digit.data} />
                                                <div className="mt-2 text-lg font-semibold">
                                                    Prediction: {digit.prediction}
                                                </div>
                                            </div>
                                        ))}
                                    </div>
                                    <div className="flex justify-center">
                                        <button
                                            onClick={handleShuffle}
                                            disabled={isLoading}
                                            className="px-4 py-2 bg-blue-500 text-white rounded hover:bg-blue-600 disabled:opacity-50"
                                        >
                                            {isLoading ? 'Loading...' : 'Shuffle Digits'}
                                        </button>
                                    </div>
                                </div>
                            </div>
                        </div>
                    );
                };

                const root = ReactDOM.createRoot(document.getElementById('root'));
                root.render(<MNISTGrid />);
            </script>
        </body>
        </html>
    )";
    return res;
});


    CROW_ROUTE(app, "/api/shuffle")
    ([&trainData, &classifiers]() {
    std::vector<size_t> indices(trainData.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    crow::json::wvalue result;
    std::vector<crow::json::wvalue> samples;

    for (size_t i = 0; i < 9; i++) {
        const auto& sample = trainData[indices[i]];
        int prediction = -1;
        std::vector<double> allScores(10);
        evaluateSingleImage(sample, classifiers, prediction, allScores);

        crow::json::wvalue entry;
        entry["data"] = std::vector<double>(sample.data.begin(), sample.data.end());
        entry["prediction"] = prediction;
        samples.push_back(std::move(entry));
    }

    result["samples"] = std::move(samples);
    return crow::response(result);
});

    app.port(3000).run();



    return 0;
}