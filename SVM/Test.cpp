#include <crow.h>
#include <string>
#include "SVM.h"
#include <iostream>
#include <random>
#include <ctime>
#include <ostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <cerrno>

using namespace stacy;

int32_t swap32(int32_t value) {
   return ((value & 0xFF000000) >> 24) |
          ((value & 0x00FF0000) >> 8) |
          ((value & 0x0000FF00) << 8) |
          ((value & 0x000000FF) << 24);
}

double getRandom(double min, double max) {
   static std::random_device rd;
   static std::mt19937 gen(rd());
   std::uniform_real_distribution<double> dis(min, max);
   return dis(gen);
}

vector<vector<double>> readMNISTImages(const string& filePath, int numImages, int numRows, int numCols) {
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

vector<int> readMNISTLabels(const string& filePath, int numLabels) {
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

vector<MNSTData> convertToMNSTDataFormat(const vector<vector<double>>& images,
                                    const vector<int>& labels) {
   vector<MNSTData> mnistData;
   for(size_t i = 0; i < images.size(); i++) {
       MNSTData data;
       data.data = images[i];
       data.lable = labels[i];
       mnistData.push_back(data);
   }
   return mnistData;
}

vector<SVMData> initializeSVMClassifiers(int inputDim) {
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

void evaluateModel(const vector<MNSTData>& testData, const vector<SVMData>& classifiers) {
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

string loadHtmlTemplate() {
   return R"DELIMITER(<!DOCTYPE html>
<html>
<head>
   <title>MNIST Digit Recognition</title>
   <style>
       body {
           font-family: Arial;
           margin: 0;
           padding: 20px;
       }
       canvas { border: 1px solid #ccc; }
       button { margin: 5px; padding: 8px 16px; }
   </style>
</head>
<body>
   <canvas id="drawingCanvas" width="280" height="280"></canvas>
   <br>
   <button onclick="clearCanvas()">Clear</button>
   <button onclick="predict()">Predict</button>
   <div id="prediction"></div>
   <script>
       const canvas = document.getElementById('drawingCanvas');
       const ctx = canvas.getContext('2d');
       let isDrawing = false;

       ctx.fillStyle = 'white';
       ctx.fillRect(0, 0, canvas.width, canvas.height);
       ctx.strokeStyle = 'black';
       ctx.lineWidth = 12;
       ctx.lineCap = 'round';

       canvas.onmousedown = (e) => {
           isDrawing = true;
           const rect = canvas.getBoundingClientRect();
           ctx.beginPath();
           ctx.moveTo(e.clientX - rect.left, e.clientY - rect.top);
       };

       canvas.onmousemove = (e) => {
           if (!isDrawing) return;
           const rect = canvas.getBoundingClientRect();
           ctx.lineTo(e.clientX - rect.left, e.clientY - rect.top);
           ctx.stroke();
       };

       canvas.onmouseup = () => isDrawing = false;
       canvas.onmouseleave = () => isDrawing = false;

       function clearCanvas() {
           ctx.fillStyle = 'white';
           ctx.fillRect(0, 0, canvas.width, canvas.height);
           document.getElementById('prediction').textContent = '';
       }

       async function predict() {
           const tempCanvas = document.createElement('canvas');
           tempCanvas.width = 28;
           tempCanvas.height = 28;
           const tempCtx = tempCanvas.getContext('2d');

           tempCtx.drawImage(canvas, 0, 0, 28, 28);
           const imageData = tempCtx.getImageData(0, 0, 28, 28);

           const pixels = [];
           for (let i = 0; i < imageData.data.length; i += 4) {
               const grayscale = (imageData.data[i] + imageData.data[i + 1] + imageData.data[i + 2]) / 3;
               pixels.push(grayscale / 255);
           }

           try {
               const response = await fetch('/predict', {
                   method: 'POST',
                   headers: { 'Content-Type': 'application/json' },
                   body: JSON.stringify({ image: pixels })
               });

               const data = await response.json();
               document.getElementById('prediction').textContent =
                   'Prediction: ' + data.prediction;
           } catch (error) {
               console.error('Prediction failed:', error);
           }
       }
   </script>
</body>
</html>)DELIMITER";
}

int main() {
   const int numTrainImages = 60000;
   const int numTestImages = 10000;
   const int imageRows = 28;
   const int imageCols = 28;
   const double learningRate = 0.00007;
   const double cost = .01;
   const int numEpochs = 7;

    crow::SimpleApp app;
    SVMMulti multiSvm(learningRate, cost);

   auto trainImages = readMNISTImages("set1-images.idx3-ubyte", numTrainImages, imageRows, imageCols);
   auto trainLabels = readMNISTLabels("set1-labels.idx1-ubyte", numTrainImages);
   auto trainData = convertToMNSTDataFormat(trainImages, trainLabels);

   auto testImages = readMNISTImages("Testing-Data-Images.idx3-ubyte", numTestImages, imageRows, imageCols);
   auto testLabels = readMNISTLabels("TestingData-labels.idx1-ubyte", numTestImages);
   auto testData = convertToMNSTDataFormat(testImages, testLabels);

    auto classifiers = initializeSVMClassifiers(imageRows * imageCols);

    multiSvm.train(trainData, classifiers, numEpochs);
    evaluateModel(testData, classifiers);


   return 0;
}