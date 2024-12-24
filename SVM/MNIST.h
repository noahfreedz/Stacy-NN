#pragma once
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <filesystem>

#ifndef MNIST_H
#define MNIST_H

using namespace std;
namespace MNIST
{
    struct  MNSTData
    {
        vector<double> data;
        int lable;
    };

    class MNISTF
    {
        public:
            MNISTF();
        
            MNISTF(vector<MNSTData> imageData_);

            vector<vector<double>> readMNISTImages(const string& filePath, int numImages, int numRows, int numCols);

            vector<int> readMNISTLabels(const string& filePath, int numLabels);

            void printImage(int index);

        private:
            int32_t swap32(int32_t value);

            char getAsciiChar(double intensity);

            vector<MNSTData> imageData;
};

};
#endif //MNIST_H
