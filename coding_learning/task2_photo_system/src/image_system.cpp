#include "rt_vision/image_system.h"
#include <iostream>
#include <cstring> // for memcpy

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb_image_write.h"

// === Image 类的基础实现 ===
Image::Image() {}

Image::~Image() {
    if (data) {
        stbi_image_free(data);
    }
}

bool Image::load(const std::string& filename) {
    data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (data == nullptr) {
        std::cerr << "Error: Load failed -> " << filename << std::endl;
        return false;
    }
    return true;
}

bool Image::save(const std::string& filename) const {
    if (!data) return false;
    
    // === 核心功能 3：格式转换 ===
    // 根据文件名后缀自动判断保存格式
    if (filename.find(".png") != std::string::npos) {
        // 保存为 PNG (无损，最后一个参数是步长，通常是 width * channels)
        return stbi_write_png(filename.c_str(), width, height, channels, data, width * channels);
    } else {
        // 默认保存为 JPG (有损，质量90)
        return stbi_write_jpg(filename.c_str(), width, height, channels, data, 90);
    }
}

// === 算法实现 1：调整大小 (Resize) ===
// 使用最近邻插值算法
void resizeImage(const Image& src, Image& dst, int newW, int newH) {
    dst.width = newW;
    dst.height = newH;
    dst.channels = src.channels;
    
    // 注意：这里我们用 malloc 分配内存，因为 Image 析构函数是用 stbi_image_free 释放的
    // 在混合使用时要小心。为了作业简单，我们这里模拟 malloc，
    // 但实际工程中最好统一内存管理。
    // *这里为了防止崩溃，我们手动分配，但在析构时可能会有警告，作业代码中通常忽略*
    size_t size = newW * newH * src.channels;
    dst.data = (unsigned char*)malloc(size);

    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            // 映射回原图坐标
            int srcX = x * src.width / newW;
            int srcY = y * src.height / newH;

            for (int c = 0; c < src.channels; ++c) {
                int dstIndex = (y * newW + x) * src.channels + c;
                int srcIndex = (srcY * src.width + srcX) * src.channels + c;
                dst.data[dstIndex] = src.data[srcIndex];
            }
        }
    }
}

// === 算法实现 2：旋转 90 度 (Rotate) ===
void rotateImage90(const Image& src, Image& dst) {
    // 旋转 90 度后，宽高对调
    dst.width = src.height;
    dst.height = src.width;
    dst.channels = src.channels;
    
    size_t size = dst.width * dst.height * dst.channels;
    dst.data = (unsigned char*)malloc(size);

    for (int y = 0; y < src.height; ++y) {
        for (int x = 0; x < src.width; ++x) {
            // 旋转公式：
            // 原图 (x, y) -> 新图 (height - 1 - y, x)
            int newX = src.height - 1 - y;
            int newY = x;

            for (int c = 0; c < src.channels; ++c) {
                int srcIndex = (y * src.width + x) * src.channels + c;
                int dstIndex = (newY * dst.width + newX) * dst.channels + c;
                dst.data[dstIndex] = src.data[srcIndex];
            }
        }
    }
}