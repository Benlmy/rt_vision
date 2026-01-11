#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    // 1. 读取图像
    // 预处理通常是在【灰度图】上进行的，所以我们在读取时直接转灰度
    // IMREAD_GRAYSCALE 相当于手动调用了 cvtColor(..., COLOR_BGR2GRAY)
    Mat gray = imread("test.png", IMREAD_GRAYSCALE);

    if (gray.empty())
    {
        cout << "无法读取图像！" << endl;
        return -1;
    }

    // 显示原始灰度图
    imshow("1. Original Gray", gray);

    // -----------------------------------------------------------
    // 步骤一：高斯滤波 (Gaussian Blur)
    // -----------------------------------------------------------
    // 作用：去噪。想象一下把磨砂玻璃放在图片上，细小的噪点就看不清了。
    // 参数 Size(5, 5)：核的大小。必须是奇数。数值越大，越模糊。
    Mat blurred;
    GaussianBlur(gray, blurred, Size(5, 5), 0);
    imshow("2. Gaussian Blur", blurred);

    // -----------------------------------------------------------
    // 步骤二：二值化 (Threshold) - 自动挡！
    // -----------------------------------------------------------
    // 作用：将灰度图变成纯黑(0)和纯白(255)。
    // 这里我们使用 THRESH_OTSU (大津法)。
    // 它的原理是：自动计算一个阈值，使得前景和背景的类间方差最大。
    // 注意：当使用了 THRESH_OTSU 时，第三个参数(手动阈值)会被忽略，写 0 即可。
    Mat binary;
    double auto_thresh = threshold(blurred, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

    cout << "Otsu 算法计算出的最佳阈值是: " << auto_thresh << endl;
    imshow("3. Threshold (Otsu)", binary);

    // -----------------------------------------------------------
    // 步骤三：形态学操作 (Morphology)
    // -----------------------------------------------------------
    // 我们可以任选一种：
    // Erode (腐蚀)：让黑色区域“吞噬”白色区域 -> 去除白色的细小噪点
    // Dilate (膨胀)：让白色区域“扩张” -> 连接断裂的白色线条

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

    Mat eroded, dilated;

    // 3.1 腐蚀演示
    erode(binary, eroded, kernel);
    imshow("4. Erode (Thinner)", eroded);

    // 3.2 膨胀演示
    dilate(binary, dilated, kernel);
    imshow("5. Dilate (Thicker)", dilated);

    // -----------------------------------------------------------

    cout << "按任意键退出..." << endl;
    waitKey(0);

    return 0;
}