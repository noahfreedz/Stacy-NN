#include <crow.h>
#include <string>
#include "SVM.h"
#include <iostream>
#include <random>
#include <ctime>
#include <ostream>
#include <fstream>
#include <filesystem>
#include <cerrno>
#include <sstream>
#include <numeric>
#include <algorithm>

using namespace stacy;
using namespace std;
using namespace MNIST;

int main() {
    // Parameters
    const int numTrainImages = 60000;
    const int numTestImages = 10000;
    const int imageRows = 28;
    const int imageCols = 28;

    crow::SimpleApp app;
    MNISTF data;
    SVM svm(1,0.01);

    auto trainImages = data.readMNISTImages("set1-images.idx3-ubyte", numTrainImages, imageRows, imageCols);
    auto trainLabels = data.readMNISTLabels("set1-labels.idx1-ubyte", numTrainImages);
    auto trainData = svm.convertToMNSTDataFormat(trainImages, trainLabels);

    auto testImages = data.readMNISTImages("Testing-Data-Images.idx3-ubyte", numTestImages, imageRows, imageCols);
    auto testLabels = data.readMNISTLabels("TestingData-labels.idx1-ubyte", numTestImages);
    auto testData = svm.convertToMNSTDataFormat(testImages, testLabels);

    auto classifiers = svm.LoadSVMData("../models.bin");


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
        SVM svm(1,0.01);
        svm.evaluateSingleImage(sample, classifiers, prediction);

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
        SVM svm(1,0.01);
        svm.evaluateSingleImage(sample, classifiers, prediction);

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