#pragma once
#include <string>

class Image {
public:
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = nullptr;

    Image();
    ~Image();

    bool load(const std::string& filename);
    bool save(const std::string& filename) const;
};

// === 新增：图像处理算法声明 ===
// 1. 调整大小 (Resize)
void resizeImage(const Image& src, Image& dst, int newW, int newH);

// 2. 旋转 90 度 (Rotate)
void rotateImage90(const Image& src, Image& dst);