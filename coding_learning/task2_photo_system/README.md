# Task 2: 智能图片管理系统 (Photo System - CMake/Linux Ver.)

这是一个基于 C++17 和 CMake 构建的模块化图像处理系统。本项目严格遵循工业级 C++ 工程结构规范，实现了业务逻辑（App）、核心算法（Src）与接口定义（Include）的彻底分离。

## ✨ 项目亮点

* **标准 CMake 构建**：摒弃 IDE 依赖，采用跨平台的 CMake 构建系统，支持在 Linux (Ubuntu/WSL) 及 Windows 环境下编译。
* **模块化架构**：
    * `include/`：对外暴露的头文件接口。
    * `src/`：隐藏的算法实现细节（Pimpl 思想雏形）。
    * `app/`：独立的业务逻辑入口。
* **轻量级依赖**：仅使用 `stb_image` 单头文件库，无繁重的第三方依赖。
* **交互式终端**：提供友好的命令行菜单，支持批量操作。

## 🛠️ 功能特性 (Features)

1.  **自动扫描 (Auto Scan)**：
    * 自动遍历 `train1` 目录，识别并列出所有支持的图片格式 (.jpg, .png, .bmp)。
2.  **数码变焦 (Digital Zoom)**：
    * 支持 ROI (Region of Interest) 裁切并放大。
    * 默认聚焦图片中心区域并放大 2.0 倍。
3.  **图像旋转 (Rotation)**：
    * 支持顺时针 90 度旋转算法。
4.  **格式转换 (Format Conversion)**：
    * 支持 JPG 与 PNG 格式的互相转换与另存为。

## 📂 项目结构 (Project Structure)

```text
task2_photo_system/
├── CMakeLists.txt          # 项目核心构建脚本
├── app/
│   └── main.cpp            # 主程序入口 (菜单交互逻辑)
├── src/
│   └── image_system.cpp    # 图像处理算法具体实现
├── include/
│   └── rt_vision/
│       └── image_system.h  # 头文件接口声明
├── external/               # 第三方库
│   ├── stb_image.h
│   └── stb_image_write.h
└── train1/                 # (需自行放入) 测试图片文件夹
```
## 🚀 快速开始 (Build & Run)

本项目在 WSL: Ubuntu 环境下开发，推荐使用 GCC 编译器。

### 1. 编译项目
在项目根目录下打开终端：

```bash
# 1. 创建并进入构建目录
mkdir build && cd build

# 2. 生成 Makefile (配置 CMake)
cmake ..

# 3. 编译可执行文件
make
```
### 2. 运行程序
编译成功后，在 build 目录下运行：
```bash
./demo_app
```
(注：Windows 环境下为 .\Debug\demo_app.exe 或 .\demo_app.exe)
## 🎮 使用指南

1.  确保项目根目录下有名为 `train1` 的文件夹，并放入测试图片。
2.  启动程序后，选择 **`1. 扫描文件夹`** 加载图片列表。
3.  选择 **`2. 选择图片`**，输入图片编号（支持多选，空格隔开，如 `1 3 5`）。
4.  选择具体的处理算法（缩放/旋转/转格式）。
5.  处理后的结果将自动保存至 `processed_images/` 文件夹。

## 📦 依赖说明

* **stb_image**:用于图像的解码与编码 (Public Domain)。
* **C++ Standard**: C++17

---
*Created by [李明远/Benlmy] for Coding Learning Task 2.*
