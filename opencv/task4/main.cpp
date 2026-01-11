#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

// --------------------------------------------------------------------------------
// 通用颜色处理函数
// --------------------------------------------------------------------------------
void processColor(Mat hsv_img, Scalar lower, Scalar upper, Mat &display_img, Scalar draw_color, string maskWindowName)
{
    Mat mask;
    // 1. 颜色提取
    inRange(hsv_img, lower, upper, mask);

    // 2. 形态学操作 (去噪 + 填坑)
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);

    // 显示 Mask
    imshow(maskWindowName, mask);

    // 3. 寻找轮廓并画框
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++)
    {
        Rect box = boundingRect(contours[i]);
        // 面积过滤：如果你的物体离得远，可能需要把 500 改小
        if (box.area() > 500)
        {
            rectangle(display_img, box, draw_color, 2);
            putText(display_img, maskWindowName, Point(box.x, box.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, draw_color, 2);
        }
    }
}

int main()
{
    // 打开视频文件
    VideoCapture cap("video.mp4");

    if (!cap.isOpened())
    {
        cout << "无法打开视频！请确认 build 目录下有 video.mp4" << endl;
        return -1;
    }

    double fps = cap.get(CAP_PROP_FPS);
    int delay_ms = 1000 / fps;

    // 如果读取不到 FPS (有时会发生)，就默认 30 帧
    if (fps == 0)
        delay_ms = 33;

    Mat frame;

    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            // 视频放完了，从头开始播放（实现循环播放效果）
            cap.set(CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        // 1. 转 HSV
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // ==========================================================
        // 2. 颜色识别区域 (修改了这里)
        // ==========================================================

        // --- 黄色 (Yellow) ---

        processColor(hsv, Scalar(20, 43, 46), Scalar(35, 255, 255), frame, Scalar(0, 255, 255), "Yellow");

        // --- 红色 (Red) ---

        processColor(hsv, Scalar(170, 43, 46), Scalar(180, 255, 255), frame, Scalar(0, 0, 255), "Red");

        // ==========================================================

        imshow("Result", frame);

        if (waitKey(delay_ms) == 'q')
        {
            break;
        }
    }

    cap.release();
    destroyAllWindows();
    return 0;
}