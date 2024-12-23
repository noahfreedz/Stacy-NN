// Custom Includes
#include "main.h"

#pragma region Other Includes
//Pre Stacy
#include <iostream>
#include <map>
#include <vector>
#include <numeric>
#include <thread>
#include <mutex>
#include <valarray>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <random>
#include <unordered_map>
#include <queue>
#include <vector>
#include <map>
#include <cmath>
#include <future>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <memory>

//Post Rebecca
#include <Eigen/Dense> // Used for Matrices

#pragma endregion

using namespace Stacy;

#pragma region Neural Network Constructors
// | @sum Defualt Constructor for Neural_Network
// | @inputs size of network, learning rate
// | @dev creates new network based on parameters,
// | randomly assigning weights & biases
NeuralNetwork::NeuralNetwork(int input_node_count, int hidden_layers, int hidden_node_count,
                             int output_node_count, double _learning_rate, int backprop_after)
{
    cout << " - INIT NETWORK -  " << endl;
    cout << " | Input Neurons: " << input_node_count << endl;
    cout << " | Hidden Layers/Neurons: " << hidden_layers << "/" << hidden_node_count << endl;
    cout << " | Output Neurons: " << output_node_count << endl;
    // Set default learning rate
    learning_rate = _learning_rate;

    // Init biases for hidden layers
    for(int layer = 0; layer < hidden_layers; layer++) {
        VectorXd hidden_biases(hidden_node_count);
        hidden_biases = hidden_biases.unaryExpr([this](double) {return get_small_random_value();});
        network_biases.push_back(hidden_biases);
    }

    // Init biases for output layer
    VectorXd output_biases(output_node_count);
    output_biases = output_biases.unaryExpr([this](double) {return get_small_random_value();});
    network_biases.push_back(output_biases);

    // Init weights for input layer to first hidden layer
    // Separated due to input layer being different size than hidden layer
    MatrixXd input_weights(input_node_count, hidden_node_count);
    input_weights = input_weights.unaryExpr([this](double) {return get_xavier_weight();});
    network_weights.push_back(input_weights);

    // Init weights from first hidden layer to output layer
    int network_layers = get_layer_count() - 1;
    for(int layer = 0; layer < network_layers; layer++) {
        long long int rows = network_biases[layer].size();
        long long int columns = network_biases[layer + 1].size();
        MatrixXd hidden_weights(rows, columns);
        hidden_weights = hidden_weights.unaryExpr([this](double) {return get_xavier_weight();});
        network_weights.push_back(hidden_weights);
    }

}

NeuralNetwork::NeuralNetwork(double _learning_rate, vector<MatrixXd> _startingWeights,
                             vector<VectorXd> _startingBiases, int backprop_after)
{
    // Load learning rate
    learning_rate = _learning_rate;

    // Load biases
    network_biases = _startingBiases;

    // Load Weights
    network_weights = _startingWeights;

}
#pragma endregion

VectorXd NeuralNetwork::run_network(const VectorXd &input) {
    VectorXd activation = input;
    int network_size = network_weights.size();
    for (int i = 0; i < network_size; ++i) {
        VectorXd next_activation = network_weights[i] * activation + network_biases[i];

        if(i < network_size - 1) {
            // If Hidden Layer Activation
            activation = next_activation.unaryExpr([](double val) { return max(0.0, val); });
        } else {
            // If Output Layer Activation
            activation = next_activation.unaryExpr([](double val) { return max(0.0, val); });
        }
    }
    return activation;
}

void NeuralNetwork::propogate_network() {

}

void NeuralNetwork::print_network() {
    std::cout << "Neural_Network Structure:" << std::endl;
    std::cout << "===================================" << std::endl;

    for (int i = 0; i < network_weights.size(); ++i) {
        std::cout << "Weights between Layer " << i << " and Layer " << (i + 1) << ":" << std::endl;
        std::cout << network_weights[i] << std::endl;
        std::cout << "-----------------------------------" << std::endl;
    }

    for (int i = 0; i < network_biases.size(); ++i) {
        std::cout << "Biases for Layer " << (i + 1) << ":" << std::endl;
        std::cout << network_biases[i].transpose() << std::endl; // Transpose for better display
        std::cout << "-----------------------------------" << std::endl;
    }

    std::cout << "===================================" << std::endl;
}

void NeuralNetwork::export_network(const std::string &filename) {
    std::ofstream file(filename + ".txt");

    file << learning_rate << "\n";

    file << "WEIGHTS\n";
    for (auto &matrix : network_weights) {
        file << matrix.rows() << " " << matrix.cols() << "\n";
        for (int i = 0; i < matrix.rows(); ++i) {
            for (int j = 0; j < matrix.cols(); ++j) {
                file << matrix(i, j) << " ";
            }
            file << "\n";
        }
    }

    file << "BIASES\n";
    for (auto &vector : network_biases) {
        file << vector.size() << "\n";
        for (int i = 0; i < vector.size(); ++i) {
            file << vector[i] << " ";
        }
        file << "\n";
    }

    file.close();
}

pair<vector<MatrixXd>, vector<VectorXd>> NeuralNetwork::import_network(const std::string &filename) {
    std::ifstream file(filename + ".txt");

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    file >> learning_rate;

    vector<MatrixXd> weights;
    vector<VectorXd> biases;

    std::string section;

    file >> section;
    if (section == "WEIGHTS") {
        while (true) {
            int rows, cols;
            file >> rows >> cols;
            if (file.fail()) break;

            MatrixXd matrix(rows, cols);
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    file >> matrix(i, j);
                }
            }
            weights.push_back(matrix);
        }
    }

    file >> section;
    if (section == "BIASES") {
        while (true) {
            int size;
            file >> size;
            if (file.fail()) break;

            VectorXd vector(size);
            for (int i = 0; i < size; ++i) {
                file >> vector[i];
            }
            biases.push_back(vector);
        }
    }
    file.close();
    return make_pair(weights, biases);
}