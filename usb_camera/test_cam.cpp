#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture cap(0); // open the default camera
    if (!cap.isOpened()) // check if we succeeded
    {
        std::cout << "Failed to open camera" << std::endl;
        return -1;
    }

    while (true)
    {
        cv::Mat frame;
        cap.read(frame); // read a new frame from camera
        if (frame.empty())
            break;
        cv::imshow("Camera", frame); // display the frame
        if (cv::waitKey(1) == 'q') // press 'q' to exit
            break;
    }

    return 0;
}
