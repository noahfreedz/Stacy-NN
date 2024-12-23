#include "main.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace Eigen;
using namespace Stacy;

int main() {
    try {
        // Create a neural network
        cout << "Creating Neural_Network..." << endl;
        NeuralNetwork originalNetwork(3, 2, 4, 2, 0.01, 1);

        // Print the original network
        cout << "\nOriginal Neural_Network:" << endl;
        originalNetwork.print_network();

        // Export the network to a file
        cout << "\nExporting Neural_Network to 'network_data'..." << endl;
        originalNetwork.export_network("network_data");

        // Import the network from the file
        cout << "\nImporting Neural_Network from 'network_data'..." << endl;
        auto [weights, biases] = originalNetwork.import_network("network_data");

        // Create a new network using the imported weights and biases
        NeuralNetwork importedNetwork(0.01, weights, biases, 1);

        // Print the imported network
        cout << "\nImported Neural_Network:" << endl;
        importedNetwork.print_network();

        // Check if the import/export worked correctly
        cout << "\nVerification complete. Neural_Network successfully exported and imported." << endl;

    } catch (const std::exception &e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1;
    }

    return 0;
}
