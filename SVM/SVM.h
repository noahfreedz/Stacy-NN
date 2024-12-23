#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>



using namespace std;
namespace stacy {

    class MNSTData {
    public:
        vector<double> data;
        int lable;
    };

    class SVMData
    {
     public:
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
            void train(const vector<stacy::MNSTData> &MNSTData, int TargetNumber, SVMData &currentClass);

        private:
            double C; // this the cost for getting things wrong
            double L; // this is the learning rate

        };

    class printImages {
        public:
        printImages(const vector<MNSTData>& data): imageData(data){}

        void printImage(int index) {
            if (index >= imageData.size() || index < 0) {
                std::cout << "Invalid image index!" << std::endl;
                return;
            }

            std::cout << "Image " << index << " (Label: " << static_cast<int>(imageData[index].lable) << ")\n";
            std::cout << std::string(30, '-') << std::endl;

            const auto& image = imageData[index].data;
            for (int i = 0; i < 28; ++i) {
                for (int j = 0; j < 28; ++j) {
                    double pixel = image[i * 28 + j];
                    // Convert pixel value to ASCII character for different intensity levels
                    char c = getAsciiChar(pixel);
                    std::cout << c << c; // Print each character twice for better aspect ratio
                }
                std::cout << std::endl;
            }
            std::cout << std::string(30, '-') << std::endl;
        }
    private:
        char getAsciiChar(double intensity) {
            // Convert intensity (0-1) to ASCII characters for different levels
            // Using common ASCII characters from darkest to lightest
            const std::string chars = "@%#*+=-:. ";
            int index = static_cast<int>(intensity * (chars.length() - 1));
            return chars[chars.length() - 1 - index];
        }
        vector<MNSTData> imageData;
    };

    class SVMPersistence {
    public:
        // Save SVMData to a binary file
        static void SaveSVMData(const std::vector<SVMData>& classifiers, const std::string& filename) {
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

        // Load SVMData from a binary file
        static std::vector<SVMData> LoadSVMData(const std::string& filename) {
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
    };
}

