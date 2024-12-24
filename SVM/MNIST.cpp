#include "MNIST.h"

using namespace std;

MNIST::MNISTF::MNISTF(){}

MNIST::MNISTF::MNISTF(vector<MNSTData> imageData_): imageData(imageData_) {}

vector<vector<double>> MNIST::MNISTF::readMNISTImages(const string& filePath, int numImages, int numRows, int numCols) {
                ifstream file(filePath, ios::binary);
                vector<vector<double>> images;

                if (file.is_open()) {
                    int magicNumber = 0;
                    int numberOfImages = 0;
                    int rows = 0;
                    int cols = 0;

                    file.read(reinterpret_cast<char*>(&magicNumber), 4);
                    file.read(reinterpret_cast<char*>(&numberOfImages), 4);
                    file.read(reinterpret_cast<char*>(&rows), 4);
                    file.read(reinterpret_cast<char*>(&cols), 4);

                    magicNumber = swap32(magicNumber);
                    numberOfImages = swap32(numberOfImages);
                    rows = swap32(rows);
                    cols = swap32(cols);

                    for (int i = 0; i < numImages; ++i) {
                        vector<double> image;
                        for (int j = 0; j < numRows * numCols; ++j) {
                            unsigned char pixel = 0;
                            file.read(reinterpret_cast<char*>(&pixel), 1);
                            image.push_back(static_cast<double>(pixel) / 255.0);
                        }
                        images.push_back(image);
                    }
                    file.close();
                }
                return images;
            }

vector<int> MNIST::MNISTF::readMNISTLabels(const string& filePath, int numLabels) {
                ifstream file(filePath, ios::binary);
                vector<int> labels;

                if (file.is_open()) {
                    int magicNumber = 0;
                    int numberOfLabels = 0;

                    file.read(reinterpret_cast<char*>(&magicNumber), 4);
                    file.read(reinterpret_cast<char*>(&numberOfLabels), 4);

                    magicNumber = swap32(magicNumber);
                    numberOfLabels = swap32(numberOfLabels);

                    for (int i = 0; i < numLabels; ++i) {
                        unsigned char label = 0;
                        file.read(reinterpret_cast<char*>(&label), 1);
                        labels.push_back(static_cast<int>(label));
                    }
                    file.close();
                }
                return labels;
            }

void MNIST::MNISTF::printImage(int index) {
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

int32_t MNIST::MNISTF::swap32(int32_t value) {
            return ((value & 0xFF000000) >> 24) |
                   ((value & 0x00FF0000) >> 8) |
                   ((value & 0x0000FF00) << 8) |
                   ((value & 0x000000FF) << 24);
        }

char MNIST::MNISTF::getAsciiChar(double intensity) {
                    // Convert intensity (0-1) to ASCII characters for different levels
                    // Using common ASCII characters from darkest to lightest
                    const std::string chars = "@%#*+=-:. ";
                    int index = static_cast<int>(intensity * (chars.length() - 1));
                    return chars[chars.length() - 1 - index];
                }