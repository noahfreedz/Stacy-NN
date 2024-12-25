#include "SVM.h"
#include <iostream>
#include <ostream>
#include <sstream>
#include <cmath>
#include <vector>

using namespace std;
using namespace MNIST;
using namespace stacy;

double stacy::getRandom(double min, double max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

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

vector<SVMData> stacy::SVM::initializeSVMClassifiers(int inputDim) {
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

vector<MNSTData> stacy::SVM::convertToMNSTDataFormat(const vector<vector<double>>& images, const vector<int>& labels) {
    vector<MNSTData> mnistData;
    for(size_t i = 0; i < images.size(); i++) {
        MNSTData data;
        data.data = images[i];
        data.lable = labels[i];
        mnistData.push_back(data);
    }
    return mnistData;
}

void stacy::SVM::evaluateSingleImage(const MNSTData& sample, const std::vector<SVMData>& classifiers, int& prediction) {
                double bestScore = -std::numeric_limits<double>::infinity();
                 prediction = -1;

                // Get prediction from each classifier
                for(int digit = 0; digit < 10; digit++) {
                    double score = SVM(1, 0.01).predictOne(sample.data, classifiers[digit].W, classifiers[digit].B);

                    if(score > bestScore) {
                        bestScore = score;
                        prediction = digit;
                    }
                }
                cout <<"We think that it is a :" << prediction << endl;
            }

void stacy::SVM::evaluateModel(const vector<MNSTData>& testData, const vector<SVMData>& classifiers) {
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

void stacy::SVM::SaveSVMData(const std::vector<SVMData>& classifiers, const std::string& filename) {
                std::ofstream outFile(filename, std::ios::binary);
                if (!outFile) {
                    throw std::runtime_error("Cannot open file for writing: " + filename);
                }

                // Write number of classifiers
                size_t numClassifiers = classifiers.size();
                outFile.write(reinterpret_cast<const char*>(&numClassifiers), sizeof(numClassifiers));

                // Write each classifier
                for (const auto& classifier : classifiers) {
                    // Write weights vector size
                    size_t weightSize = classifier.W.size();
                    outFile.write(reinterpret_cast<const char*>(&weightSize), sizeof(weightSize));

                    // Write weights
                    if (!classifier.W.empty()) {
                        outFile.write(reinterpret_cast<const char*>(classifier.W.data()),
                                      weightSize * sizeof(double));
                    }

                    // Write bias
                    outFile.write(reinterpret_cast<const char*>(&classifier.B), sizeof(classifier.B));
                }

                if (!outFile) {
                    throw std::runtime_error("Error writing to file: " + filename);
                }
                outFile.close();
            }

vector<SVMData> stacy::SVM:: LoadSVMData(const std::string& filename) {
                std::ifstream inFile(filename, std::ios::binary);
                if (!inFile) {
                    throw std::runtime_error("Cannot open file for reading: " + filename);
                }

                std::vector<SVMData> classifiers;

                // Read number of classifiers
                size_t numClassifiers;
                inFile.read(reinterpret_cast<char*>(&numClassifiers), sizeof(numClassifiers));

                // Read each classifier
                for (size_t i = 0; i < numClassifiers; ++i) {
                    SVMData classifier;

                    // Read weights vector size
                    size_t weightSize;
                    inFile.read(reinterpret_cast<char*>(&weightSize), sizeof(weightSize));

                    // Read weights
                    classifier.W.resize(weightSize);
                    if (!classifier.W.empty()) {
                        inFile.read(reinterpret_cast<char*>(classifier.W.data()),
                                    weightSize * sizeof(double));
                    }

                    // Read bias
                    inFile.read(reinterpret_cast<char*>(&classifier.B), sizeof(classifier.B));

                    classifiers.push_back(classifier);
                }

                if (!inFile) {
                    throw std::runtime_error("Error reading from file: " + filename);
                }
                inFile.close();

                return classifiers;
            }