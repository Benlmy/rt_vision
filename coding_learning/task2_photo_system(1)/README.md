# 智能图片管理系统 (Photo System)

一个基于 C++17 的多线程图片批量处理系统，支持图片缩放、旋转和格式转换等操作。

## 📋 项目简介

本项目是一个控制台图片管理系统，采用**纯 C++ 标准库**实现核心框架（无 OpenCV 依赖），并通过调用 Python 脚本（使用 OpenCV）完成实际的图像处理任务。系统支持多线程并发处理，可高效处理大量图片。

## ✨ 功能特性

- **扫描文件夹** - 自动扫描指定文件夹中的图片文件（支持 jpg、png、bmp、jpeg 格式）
- **图片缩放 (Zoom)** - 对图片进行居中放大处理
- **图片旋转 (Rotate)** - 将图片旋转 90 度
- **格式转换** - 将图片转换为不同格式
- **多线程处理** - 使用线程池并发处理多张图片，提高处理效率

## 🏗️ 技术架构

### C++ 核心框架

| 模块 | 说明 |
|------|------|
| `Image` 基类 | 图片数据的抽象基类，定义加载和保存接口 |
| `StandardImage` | 标准图片实现类，使用 `std::ifstream/ofstream` 进行二进制读写 |
| `ImageProcessor` | 处理器模板基类，定义图片处理接口 |
| `ResizeProcessor` | 缩放处理器 |
| `RotateProcessor` | 旋转处理器 |
| `FormatConvertProcessor` | 格式转换处理器 |
| `ThreadSafeQueue` | 线程安全队列（使用 `mutex` 和 `condition_variable`） |
| `ImageProcessingManager` | 任务管理器，负责任务分发和线程池管理 |

### Python 插件 (plugin.py)

使用 OpenCV 实现实际的图像处理操作：
- `resize` - 按指定宽度等比缩放
- `zoom` - 居中放大裁剪
- `rotate` - 旋转 90 度

## 📦 依赖要求

### C++ 编译环境
- C++17 或更高版本（需要 `<filesystem>` 支持）
- Visual Studio 2017+ 或支持 C++17 的编译器

### Python 环境
- Python 3.x
- OpenCV (`pip install opencv-python`)

## 🚀 快速开始

### 1. 编译项目

使用 Visual Studio 打开 `photo_system.slnx` 解决方案文件进行编译。

### 2. 准备图片

在项目目录下创建 `train1` 文件夹，并放入需要处理的图片文件。

### 3. 运行程序

```bash
./photo_system
```

### 4. 使用菜单

```
=== 智能图片管理系统 (无库版) ===
1. 扫描文件夹 (train1)
2. 选择图片并设置操作
3. 退出
```

1. 首先选择 `1` 扫描文件夹
2. 选择 `2` 进行图片处理
   - 输入图片编号或输入 `a` 全选
   - 选择操作类型（缩放/旋转/转格式）
3. 处理完成后，结果保存在 `processed_images` 文件夹中

## 📁 项目结构

```
task2_photo_system(1)/
├── photo_system. cpp      # 主程序源码（包含所有 C++ 类定义）
├── photo_system.slnx     # Visual Studio 解决方案文件
├── photo_system.vcxproj  # Visual Studio 项目文件
├── plugin.py             # Python 图像处理插件
├── train1/               # 输入图片文件夹（需自行创建）
└── processed_images/     # 输出图片文件夹（自动创建）
```

## 💡 核心技术要点

- **面向对象设计** - 使用继承和多态实现可扩展的处理器架构
- **模板编程** - `ProcessingResult<T>` 和 `ThreadSafeQueue<T>` 泛型设计
- **并发编程** - 使用 `std::thread`、`std::mutex`、`std::atomic` 实现线程安全
- **C++17 特性** - 使用 `std::filesystem` 进行文件系统操作
- **跨语言调用** - 通过 `std::system()` 调用 Python 脚本

---
*Created by [李明远/Benlmy] for Coding Learning Task 2.*
