#pragma once
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <filesystem>
#include <cerrno>
#include <numeric>
#include <algorithm>
#include "MNIST.h"



using namespace std;
using namespace MNIST;

namespace stacy {

    double getRandom(double min, double max);

    struct SVMData
    {
        vector<double> W; // this is the weights or the direction of the hyperplain
        double B; // this is the Bais or the offest of the hyperplain from the origain
    };

    class SVMMulti {
        public:
            SVMMulti(const double learningRate, const double Cost);
            double predictOne(const vector<double>& image, const vector<double>& weights, double bias);
            double calculateNorm(const vector<double>& weights);
            void train(const vector<MNSTData>& MNSTData, vector<SVMData>& SVMData, const int num_epochs );
        private:
            double C;
            double L;
    };

    class SVM{
        public:

            double calculateNorm(const vector<double> &weights);

            SVM(const double cost, const double learningRate);

            double predictOne(const vector<double>& image, const vector<double>& weights, double bias);

            void train(const vector<MNSTData> &MNSTData, int TargetNumber, SVMData &currentClass);

            vector<MNSTData> convertToMNSTDataFormat(const vector<vector<double>>& images, const vector<int>& labels);

            vector<SVMData> initializeSVMClassifiers(int inputDim);

            void evaluateSingleImage(const MNSTData& sample, const std::vector<SVMData>& classifiers, int& prediction);

            void evaluateModel(const vector<MNSTData>& testData, const vector<SVMData>& classifiers);

            // Save SVMData to a binary file
            static void SaveSVMData(const std::vector<SVMData>& classifiers, const std::string& filename);

            // Load SVMData from a binary file
            static std::vector<SVMData> LoadSVMData(const std::string& filename);

        private:
            double C; // this the cost for getting things wrong
            double L; // this is the learning rate

        };
}

