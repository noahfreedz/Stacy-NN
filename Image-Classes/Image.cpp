#include <iostream>
#include <opencv2/highgui.hpp>
#include "opencv2/videoio.hpp"

using namespace cv;

int main() {
  VideoCapture cap(0);

    // Check if the webcam is open
    if(!cap.isOpened()) {
        std::cout << "Error opening webcam!" << std::endl;
        return -1;
    }

    namedWindow("Webcam", cv::WINDOW_AUTOSIZE);

    // Create a Mat object to store each frame of the video
    Mat frame;

    // Continuously capture and display video from the webcam
    while(1) {
        // Capture a new frame from the webcam
        cap >> frame;
        // Display the frame in the window
        cv::imshow("Webcam", frame);
        // Wait for the user to press a key before capturing the next frame
        if (cv::waitKey(1) >= 0) break;
     }

     // Release the webcam and destroy the window when you are done using it
     cap.release();
     cv::destroyWindow("Webcam");
     return 0;
}
