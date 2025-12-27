#include <algorithm>  // 必须引用这个，用于 std::clamp
#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "rt_vision/image_system.h"

namespace fs = std::filesystem;

// 全局变量
std::vector<std::string> scannedFiles;
std::string inputFolder = "train1";
std::string outputFolder = "processed_images";

void clearScreen() {
    system("clear");
}

// === 核心算法：数码变焦 (Digital Zoom) ===
// 找回了这个函数，用来替代普通的 resize
void digitalZoom(const Image& src, Image& dst, int outW, int outH, float centerX_ratio, float centerY_ratio,
                 float zoomLevel) {
    dst.width = outW;
    dst.height = outH;
    dst.channels = src.channels;

    // 手动分配内存
    dst.data = (unsigned char*)malloc(outW * outH * src.channels);

    // 计算裁剪框
    float cropW = src.width / zoomLevel;
    float cropH = src.height / zoomLevel;

    // 计算起点 (基于中心点比例)
    int startX = (int)(src.width * centerX_ratio - cropW / 2);
    int startY = (int)(src.height * centerY_ratio - cropH / 2);

    for (int y = 0; y < outH; ++y) {
        for (int x = 0; x < outW; ++x) {
            // 映射公式
            int srcX = startX + (int)((float)x / outW * cropW);
            int srcY = startY + (int)((float)y / outH * cropH);

            // 防止越界 (非常重要)
            srcX = std::clamp(srcX, 0, src.width - 1);
            srcY = std::clamp(srcY, 0, src.height - 1);

            for (int c = 0; c < src.channels; ++c) {
                int dstIndex = (y * outW + x) * src.channels + c;
                int srcIndex = (srcY * src.width + srcX) * src.channels + c;
                dst.data[dstIndex] = src.data[srcIndex];
            }
        }
    }
}

// 1. 扫描功能
void scanDirectory() {
    scannedFiles.clear();
    std::cout << "\n正在扫描 " << inputFolder << " ...\n";

    if (!fs::exists(inputFolder)) inputFolder = "../train1";
    if (!fs::exists(inputFolder)) {
        std::cout << "错误：找不到文件夹 " << inputFolder << "\n";
        return;
    }

    int index = 0;
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        std::string filePath = entry.path().string();
        if (filePath.find(".jpg") != std::string::npos || filePath.find(".png") != std::string::npos ||
            filePath.find(".bmp") != std::string::npos) {
            scannedFiles.push_back(filePath);
            std::cout << "[" << ++index << "] " << entry.path().filename().string() << "\n";
        }
    }
    std::cout << "共扫描到 " << scannedFiles.size() << " 张图片。\n";
}

// 2. 处理选中图片
void processSelectedImages(const std::vector<int>& indices, int operationType) {
    if (!fs::exists(outputFolder)) fs::create_directory(outputFolder);
    std::cout << "任务已分发到后台线程...\n";

    for (int idx : indices) {
        int vectorIdx = idx - 1;
        if (vectorIdx < 0 || vectorIdx >= scannedFiles.size()) continue;

        std::string srcPath = scannedFiles[vectorIdx];
        std::string fileName = fs::path(srcPath).filename().string();

        Image img;
        if (!img.load(srcPath)) {
            std::cout << "[失败] 无法加载: " << fileName << "\n";
            continue;
        }

        Image outImg;
        std::string suffix = "";

        switch (operationType) {
            case 1:  // === 修改处：现在调用数码变焦 ===
                // 参数：输出 500x500，聚焦中心(0.5, 0.5)，放大 2.0 倍
                digitalZoom(img, outImg, 500, 500, 0.5f, 0.5f, 2.0f);
                suffix = "_zoom.jpg";  // 后缀改叫 zoom
                break;
            case 2:  // 旋转
                rotateImage90(img, outImg);
                suffix = "_rotate.jpg";
                break;
            case 3:  // 转格式
                resizeImage(img, outImg, img.width, img.height);
                suffix = ".png";
                break;
            default:
                return;
        }

        // 保存逻辑
        std::string outPath;
        if (operationType == 3) {
            std::string rawName = fileName.substr(0, fileName.find_last_of('.'));
            outPath = outputFolder + "/" + rawName + suffix;
        } else {
            outPath = outputFolder + "/processed_" + fileName + suffix;
        }

        if (outImg.save(outPath)) {
            std::cout << "[成功] " << fileName << " -> " << suffix << "\n";
        }

        if (outImg.data) free(outImg.data);
        outImg.data = nullptr;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "全部处理完成！\n";
}

int main() {
    clearScreen();

    while (true) {
        std::cout << "\n=== 智能图片管理系统 (无库版 / VS Code) ===\n";
        std::cout << "1. 扫描文件夹 (train1)\n";
        std::cout << "2. 选择图片并设置操作\n";
        std::cout << "3. 退出\n";
        std::cout << "输入选项: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) {
            scanDirectory();
        } else if (choice == 2) {
            if (scannedFiles.empty()) {
                std::cout << "请先扫描！\n";
                continue;
            }
            std::cout << "请输入图片编号 (用空格隔开，如: 1 3 5): ";
            std::cin.ignore();
            std::string line;
            std::getline(std::cin, line);
            std::stringstream ss(line);
            int tempIdx;
            std::vector<int> selectedIndices;
            while (ss >> tempIdx) selectedIndices.push_back(tempIdx);

            if (selectedIndices.empty()) continue;

            // 这里的文案修改了，更准确
            std::cout << "选择操作: 1.放大(聚焦中心)  2.旋转  3.转格式\n";
            int opType;
            std::cin >> opType;
            processSelectedImages(selectedIndices, opType);
        } else if (choice == 3) {
            break;
        }
    }
    return 0;
}