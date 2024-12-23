#include "SVM.h"
#include <iostream>
#include <ostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <iomanip>  // For formatting output
#include "SFML/Window/Keyboard.hpp"

using namespace std;



stacy::SVMMulti::SVMMulti(const double learningRate, const double Cost): C(Cost), L(learningRate) {}

double stacy::SVMMulti::predictOne(const vector<double>& image, const vector<double>& weights, double bias)
{
    double dot_product = 0.0;
    for(size_t i = 0; i < weights.size(); i++) {
        dot_product += weights[i] * image[i];
    }
    return dot_product + bias;
}

void stacy::SVMMulti::train(const vector<MNSTData>& MNSTData, vector<SVMData>& SVMData, const int num_epochs) {
    SVM newSVM = SVM(C, L);
    for(int numberTrianed = 0; numberTrianed < num_epochs; ++numberTrianed) {
        for(int i = 0; i < 10; ++i) {
            newSVM.train(MNSTData, i, SVMData[i]);
        }
    }
}

stacy::SVM::SVM(const double cost, const double learningRate): C(cost), L(learningRate) {}

double stacy::SVM::predictOne(const vector<double>& image, const vector<double>& weights, double bias)
{
    double dot_product = 0.0;
    for(size_t i = 0; i < weights.size(); i++) {
        dot_product += weights[i] * image[i];
    }
    return dot_product + bias;
}

void stacy::SVM::train(const vector<MNSTData>& MNSTData, int TargetNumber, SVMData& currentClass) {

    for(int i = 0; i < MNSTData.size(); i++) {
        int y = (MNSTData[i].lable == TargetNumber) ? 1 : -1;

        double prediction = predictOne(MNSTData[i].data, currentClass.W, currentClass.B);

        double margin = prediction * y;

        if(margin < 1.0) {
            for(size_t j = 0; j < currentClass.W.size(); j++) {
                currentClass.W[j] = (1.0 - L * C) * currentClass.W[j] + L * y * MNSTData[i].data[j];
            }
            currentClass.B += L * y;

        } else {
            for(size_t j = 0; j < currentClass.W.size(); j++) {
                currentClass.W[j] *= (1.0 - L * C);
            }
        }
    }
}