#include "SVM.h"
#include <iostream>
#include <random>
#include <ctime>
#include <ostream>
#include <fstream>


using namespace stacy;
// Helper function for random number generation
double getRandom(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

vector< vector<double>> readMNISTImages(const  string& filePath, int numImages, int numRows, int numCols) {
     ifstream file(filePath,  ios::binary);
     vector< vector<double>> images;

    if (file.is_open()) {
        int magicNumber = 0;
        int numberOfImages = 0;
        int rows = 0;
        int cols = 0;

        // Read and convert the magic number and header values
        file.read(reinterpret_cast<char*>(&magicNumber), 4);
        file.read(reinterpret_cast<char*>(&numberOfImages), 4);
        file.read(reinterpret_cast<char*>(&rows), 4);
        file.read(reinterpret_cast<char*>(&cols), 4);

        // Convert from big-endian to little-endian if needed
        magicNumber = __builtin_bswap32(magicNumber);
        numberOfImages = __builtin_bswap32(numberOfImages);
        rows = __builtin_bswap32(rows);
        cols = __builtin_bswap32(cols);

        for (int i = 0; i < numImages; ++i) {
             vector<double> image;
            for (int j = 0; j < numRows * numCols; ++j) {
                unsigned char pixel = 0;
                file.read(reinterpret_cast<char*>(&pixel), 1);
                image.push_back(static_cast<double>(pixel) / 255.0); // Normalize to [0, 1]
            }
            images.push_back(image);
        }
        file.close();
    } else {
         cerr << "Failed to open the file: " << filePath << "\n";
    }

    return images;
}

vector<int> readMNISTLabels(const  string& filePath, int numLabels) {
         ifstream file(filePath,  ios::binary);
         vector<int> labels;

        if (file.is_open()) {
            int magicNumber = 0;
            int numberOfLabels = 0;

            // Read and convert the magic number and header values
            file.read(reinterpret_cast<char*>(&magicNumber), 4);
            file.read(reinterpret_cast<char*>(&numberOfLabels), 4);

            // Convert from big-endian to little-endian if needed
            magicNumber = __builtin_bswap32(magicNumber);
            numberOfLabels = __builtin_bswap32(numberOfLabels);

            for (int i = 0; i < numLabels; ++i) {
                unsigned char label = 0;
                file.read(reinterpret_cast<char*>(&label), 1);
                labels.push_back(static_cast<int>(label));
            }
            file.close();
        } else {
             cerr << "Failed to open the file: " << filePath << "\n";
        }

        return labels;
    }

// Convert raw MNIST data to MNSTData vector
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

// Initialize SVM classifiers
vector<SVMData> initializeSVMClassifiers(int inputDim) {
    vector<SVMData> classifiers(10); // One for each digit

    for(auto& classifier : classifiers) {
        // Initialize weights using Xavier initialization
        double scale = sqrt(2.0 / inputDim);
        classifier.W.resize(inputDim);
        for(auto& w : classifier.W) {
            w = getRandom(-scale, scale);
        }

        // Initialize bias
        classifier.B = 0;
    }

    return classifiers;
}

// Evaluation function
void evaluateModel(const vector<MNSTData>& testData, const vector<SVMData>& classifiers) {
    int correct = 0;
    int total = testData.size();

    for(const auto& sample : testData) {
        double bestScore = -std::numeric_limits<double>::infinity();
        int prediction = -1;

        // Get prediction from each classifier
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


int main() {
    // Parameters
    const int numTrainImages = 60000;
    const int numTestImages = 10000;
    const int imageRows = 28;
    const int imageCols = 28;
    const double learningRate = 0.001;
    const double cost = 7.5;
    const int numEpochs = 7;

    // Load training data
    cout << "Loading training data..." << endl;
    auto trainImages = readMNISTImages("set1-images.idx3-ubyte", numTrainImages, imageRows, imageCols);
    auto trainLabels = readMNISTLabels("set1-labels.idx1-ubyte", numTrainImages);
    auto trainData = convertToMNSTDataFormat(trainImages, trainLabels);

    // Load test data
    cout << "Loading test data..." << endl;
    auto testImages = readMNISTImages("Testing-Data-Images.idx3-ubyte", numTestImages, imageRows, imageCols);
    auto testLabels = readMNISTLabels("TestingData-labels.idx1-ubyte", numTestImages);
    auto testData = convertToMNSTDataFormat(testImages, testLabels);

    // Initialize SVM classifiers
    cout << "Initializing classifiers..." << endl;
    auto classifiers = initializeSVMClassifiers(imageRows * imageCols);

    // Create and train multi-class SVM
    cout << "Training model..." << endl;
    stacy::SVMMulti multiSvm(learningRate, cost);
    multiSvm.train(trainData, classifiers, numEpochs);

    // Evaluate on test set
    cout << "Evaluating model..." << endl;
    evaluateModel(testData, classifiers);

    return 0;
}