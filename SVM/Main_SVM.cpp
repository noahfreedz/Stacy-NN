#include "SVM.h"
#include <iostream>
#include <ostream>
#include <sstream>

using namespace stacy;
using namespace std;
using namespace MNIST;

int main() {
    // Parameters
    const int numTrainImages = 60000;
    const int numTestImages = 10000;
    const int imageRows = 28;
    const int imageCols = 28;
    const double learningRate = 0.00007;
    const double cost = .01;
    const int numEpochs = 7;

    MNISTF data;
    SVM svm(1,0.01);

    auto trainImages = data.readMNISTImages("set1-images.idx3-ubyte", numTrainImages, imageRows, imageCols);
    auto trainLabels = data.readMNISTLabels("set1-labels.idx1-ubyte", numTrainImages);
    auto trainData = svm.convertToMNSTDataFormat(trainImages, trainLabels);

    auto testImages = data.readMNISTImages("Testing-Data-Images.idx3-ubyte", numTestImages, imageRows, imageCols);
    auto testLabels = data.readMNISTLabels("TestingData-labels.idx1-ubyte", numTestImages);
    auto testData = svm.convertToMNSTDataFormat(testImages, testLabels);

    auto classifiers = svm.LoadSVMData("../models.bin");

    MNISTF printing(testData);
    while (true) {
        int number;
        std::cout << "Enter a number: ";
        std::cin >> number;
        printing.printImage(number);
        svm.evaluateSingleImage(testData[number], classifiers);
    }



    return 0;
}