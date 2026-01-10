#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 全局变量，用于滑块存储数值
int h_min = 0, h_max = 180;
int s_min = 0, s_max = 255;
int v_min = 0, v_max = 255;

// 回调函数（滑块动了就会调用这个，这里什么都不用做，主循环会处理）
void on_trackbar(int, void *) {}

int main()
{
    Mat img = imread("test.png");
    if (img.empty())
    {
        cout << "No image!" << endl;
        return -1;
    }

    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    namedWindow("Trackbars", WINDOW_AUTOSIZE);

    // 创建6个滑块：H/S/V 的最小值和最大值
    createTrackbar("H Min", "Trackbars", &h_min, 180, on_trackbar);
    createTrackbar("H Max", "Trackbars", &h_max, 180, on_trackbar);
    createTrackbar("S Min", "Trackbars", &s_min, 255, on_trackbar);
    createTrackbar("S Max", "Trackbars", &s_max, 255, on_trackbar);
    createTrackbar("V Min", "Trackbars", &v_min, 255, on_trackbar);
    createTrackbar("V Max", "Trackbars", &v_max, 255, on_trackbar);

    while (true)
    {
        Mat mask;
        // 使用滑块的值进行二值化
        Scalar lower(h_min, s_min, v_min);
        Scalar upper(h_max, s_max, v_max);
        inRange(hsv, lower, upper, mask);

        // 显示结果
        imshow("Original", img);
        imshow("Mask (Adjust these bars!)", mask);

        // 按 'q' 退出
        if (waitKey(1) == 'q')
            break;
    }

    // 退出时打印出你调试好的数值
    cout << "Final Values:" << endl;
    cout << "Scalar lower(" << h_min << ", " << s_min << ", " << v_min << ");" << endl;
    cout << "Scalar upper(" << h_max << ", " << s_max << ", " << v_max << ");" << endl;

    return 0;
}