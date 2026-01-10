#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

// 改进后的函数：增加了 maskWindowName 参数，用于分别显示不同颜色的Mask
void processColor(Mat hsv_img, Scalar lower, Scalar upper, Mat &display_img, Scalar draw_color, string maskWindowName)
{
    Mat mask;
    // 1. 颜色提取
    inRange(hsv_img, lower, upper, mask);

    // 2. 形态学操作（解决你遇到的噪点问题）
    // 稍微加大一点核的大小，或者多做一次腐蚀膨胀
    Mat kernel = getStructuringElement(MORPH_RECT, Size(7, 7));

    // 先开运算(先腐蚀后膨胀)去除背景噪点
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
    // 再闭运算(先膨胀后腐蚀)填补物体内部空洞
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);

    // 【关键点】显示二值化后的 Mask (满足任务要求)
    imshow(maskWindowName, mask);

    // 3. 寻找轮廓
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 4. 画框
    for (size_t i = 0; i < contours.size(); i++)
    {
        Rect box = boundingRect(contours[i]);

        // 面积过滤：根据你的图片，可能需要调整这个阈值
        // 如果噪点多，就把 100 改大，比如 500
        if (box.area() > 500)
        {
            rectangle(display_img, box, draw_color, 2);

            // (可选) 在框旁边写上颜色名字
            putText(display_img, maskWindowName, Point(box.x, box.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, draw_color, 2);
        }
    }
}

int main()
{
    Mat img = imread("test.png");
    if (img.empty())
    {
        return -1;
    }

    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    // ---------------------------------------------------------
    // 🚩 请在这里填入你刚才用 tuning.cpp 调试出来的数值！
    // ---------------------------------------------------------

    // 蓝色范围
    Scalar blue_lower(100, 90, 0);
    Scalar blue_upper(125, 255, 255);

    // 黄色范围
    // 注意：黄色的 H 很容易和橙色/绿色混淆，需要仔细微调 H_min 和 H_max
    Scalar yellow_lower(20, 135, 76);
    Scalar yellow_upper(42, 255, 255);

    // ---------------------------------------------------------

    // 处理蓝色，并显示 Mask 窗口名为 "Blue Mask"
    processColor(hsv, blue_lower, blue_upper, img, Scalar(255, 0, 0), "Blue Mask");

    // 处理黄色，并显示 Mask 窗口名为 "Yellow Mask"
    processColor(hsv, yellow_lower, yellow_upper, img, Scalar(0, 255, 255), "Yellow Mask");

    imshow("Result", img);

    cout << "按任意键退出..." << endl;
    waitKey(0);
    return 0;
}