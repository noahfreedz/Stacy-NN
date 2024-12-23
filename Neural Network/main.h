#pragma region Includes
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

using namespace std;
using namespace Eigen;

namespace Stacy {

    class NeuralNetwork;

    class NeuralNetwork {
    public:

        #pragma region Constructors
            NeuralNetwork(int iNode_count, int hLayer_count, int hNode_count, int oNode_count, double _learning_rate, int backprop_after);

            NeuralNetwork(double _learning_rate, vector<MatrixXd> _startingWeights, vector<VectorXd> _startingBiases, int backprop_after);

            //NeuralNetwork(int iNode_count, int hLayer_count, int hNode_count, int oNode_count,
            //              double _learning_rate, const string& FilePath, int backprop_after);

            //NeuralNetwork(int iNode_count, int hLayer_count, int hNode_count, int oNode_count,
            //              double _learning_rate,const string& FilePath, int backprop_after, bool fileSorting);

            //~NeuralNetwork();
        #pragma endregion

        VectorXd run_network(const VectorXd &input);
        void propogate_network();

        int get_layer_count() {
            int count = network_biases.size();
            return count;
        }

        void print_network();

        void export_network(const std::string &filename);

        pair<vector<MatrixXd>, vector<VectorXd>> import_network(const std::string &filename);

    private:
        #pragma region Weight & Bias Initalizations
            // Uses Xavier Initialization to get random weight for network
            // which is based on number of input and output nodes.
            // Use when activation function is symmetric, like sigmoid or tanh.
            double get_xavier_weight() {
                static random_device random;
                static mt19937 generator(random());
                long long int input_count = network_biases.front().size();
                long long int output_count = network_biases.back().size();
                double min = -sqrt(6/(input_count+output_count));
                double max = sqrt(6/(input_count+output_count));
                uniform_real_distribution<> distribution(min, max);
                return distribution(generator);
            }

            // Uses He Initialization to get random weight for network
            // which is based on number of input nodes.
            // Use when activation function is ReLU or its variants. (loki is so good :D)
            double get_he_weight() {
                static random_device random;
                static mt19937 generator(random());
                long long int input_count = network_biases.front().size();
                uniform_real_distribution<> distribution(0, sqrt((2.0 / input_count)));
                return distribution(generator);
            }

            // Returns biases with small random values which is
            // used when symmetry breaking is needed.
            // Values are typically in a small range, such as [-0.l, 0.1]
            static double get_small_random_value() {
                static random_device random;
                static mt19937 generator(random());
                uniform_real_distribution<> distribution(-0.1, 0.1);
                return distribution(generator);
            }

            // Returns small constant to ensure ReLU neurons (if used) activate initially.
            static double get_relu_bias() {
                return 0.01;
            }

        #pragma endregion

        #pragma region Activation Functions
            static double sigmoid_activation(double activation) {
                return 1.0 / (1.0 + exp(-activation));
            }

            static double tan_h_activation(double activation) {
                return tanh(activation);
            }

            static double relu_activation(double activation) {
                return max(0.0, activation);
            }

            double leaky_relu_activation(double activation) {
                if(activation > 0) {
                    return activation;
                } else {
                    return activation * relu_leak;
                }
            }
        #pragma endregion

        #pragma region Error / Cost Calculations
            VectorXd mean_squared_error(VectorXd predictions, VectorXd targets) {

            }
        #pragma endregion

        vector<MatrixXd> network_weights;
        vector<VectorXd> network_biases;
        double relu_leak = 0.01;
        double learning_rate;
    };
}