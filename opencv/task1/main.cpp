#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    string image_path = "test.png";
    Mat img = imread(image_path);

    if (img.empty())
    {
        cout << "Error: Unable to read the image! Please check filename or path." << endl;
        return -1;
    }

    imshow("Original Image", img);

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    imshow("Gray Image", gray);

    cout << "Press any key to exit..." << endl;
    waitKey(0);

    return 0;
}