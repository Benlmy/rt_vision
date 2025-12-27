// photo_system.cpp : 定义控制台应用程序的入口点。
// 【无OpenCV纯标准库版】
// 包含了 main 函数，可以直接运行。

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <filesystem> // C++17 标准库
#include <fstream>    // 用于文件读写
#include <sstream>

// 使用 C++17 文件系统命名空间
namespace fs = std::filesystem;

// ==========================================
// 1. image_base.hpp - 图片基类
// ==========================================
class Image {
protected:
    std::string filename_;
    std::string format_;
    // 我们用二进制 buffer 来模拟存储图片数据，不进行真实解码
    std::vector<char> rawData_;
    bool loaded_ = false;

public:
    Image(const std::string& filename) : filename_(filename) {
        size_t dotPos = filename.find_last_of(".");
        if (dotPos != std::string::npos) {
            format_ = filename.substr(dotPos);
        }
    }
    virtual ~Image() = default;

    // 纯虚函数
    virtual bool load() = 0;
    virtual bool save(const std::string& outputPath) const = 0;

    std::string getFilename() const { return filename_; }
    // 这里的 buffer 暴露给处理器
    std::vector<char>& getData() { return rawData_; }
    const std::vector<char>& getData() const { return rawData_; }
};

// ==========================================
// 2. image_formats.hpp - 具体格式类
// ==========================================
class StandardImage : public Image {
public:
    using Image::Image;

    bool load() override {
        // 使用标准库 ifstream 以二进制模式读取文件
        std::ifstream file(filename_, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "无法打开文件: " << filename_ << std::endl;
            loaded_ = false;
            return false;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        rawData_.resize(size);
        if (file.read(rawData_.data(), size)) {
            loaded_ = true;
            return true;
        }
        return false;
    }

    bool save(const std::string& outputPath) const override {
        if (!loaded_ || rawData_.empty()) return false;
        // 使用 standard ofstream 写入文件
        std::ofstream file(outputPath, std::ios::binary);
        if (file.write(rawData_.data(), rawData_.size())) {
            return true;
        }
        return false;
    }
};

// ==========================================
// 3. processors_base.hpp - 处理器
// ==========================================

template<typename T>
struct ProcessingResult {
    std::string operation;
    std::string inputFile;
    std::string outputFile;
    bool success;
    T additionalInfo;
};

template<typename ImageType>
class ImageProcessor {
protected:
    std::string processorName_;
public:
    ImageProcessor(const std::string& name) : processorName_(name) {}
    virtual ~ImageProcessor() = default;
    virtual ProcessingResult<std::string> process(ImageType& image, const std::string& outputPath) = 0;
};

// --- 具体处理器（模拟操作） ---

// 3.1 模拟缩放
class ResizeProcessor : public ImageProcessor<Image> {
public:
    ResizeProcessor() : ImageProcessor("Resize") {}

    ProcessingResult<std::string> process(Image& image, const std::string& outputPath) override {
        // === 修改这里 ===
        // 我们把操作码从 "resize" 改成 "zoom"
        // 最后一个参数 "2.0" 意思是：放大 2 倍 (聚焦中间)
        std::string cmd = "python plugin.py zoom \"" + image.getFilename() + "\" \"" + outputPath + "\" 2.0";

        int ret = std::system(cmd.c_str());

        bool success = (ret == 0);
        // 修改一下返回的提示信息
        return { "Zoom", image.getFilename(), outputPath, success, success ? "Zoom 2x Done" : "Failed" };
    }
};

// 3.2 模拟旋转
class RotateProcessor : public ImageProcessor<Image> {
public:
    RotateProcessor() : ImageProcessor("Rotate") {}

    ProcessingResult<std::string> process(Image& image, const std::string& outputPath) override {
        // 构建 Python 调用命令: python plugin.py rotate "输入路径" "输出路径" 90
        std::string cmd = "python plugin.py rotate \"" + image.getFilename() + "\" \"" + outputPath + "\" 90";

        int ret = std::system(cmd.c_str());

        bool success = (ret == 0);
        return { "Rotate", image.getFilename(), outputPath, success, success ? "Real Rotate Done" : "Python Call Failed" };
    }
};

// 3.3 模拟格式转换
class FormatConvertProcessor : public ImageProcessor<Image> {
public:
    FormatConvertProcessor() : ImageProcessor("FormatConvert") {}
    ProcessingResult<std::string> process(Image& image, const std::string& outputPath) override {
        // 即使是二进制复制，只要扩展名变了，在文件系统层面就是完成了“任务”
        bool success = image.save(outputPath);
        return { "FormatConvert", image.getFilename(), outputPath, success, success ? "Simulated Convert Done" : "Failed" };
    }
};

// ==========================================
// 4. concurrent_base.hpp - 并发管理 (核心考点)
// ==========================================

template<typename T>
class ThreadSafeQueue {
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
        cond_.notify_one();
    }
    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

struct Task {
    std::shared_ptr<Image> img;
    std::shared_ptr<ImageProcessor<Image>> processor;
    std::string outputPath;
};

class ImageProcessingManager {
    ThreadSafeQueue<Task> taskQueue_;
    std::vector<std::thread> workers_;
    std::atomic<bool> stop_ = false;
    std::mutex outputMutex_;

public:
    ImageProcessingManager() {}
    ~ImageProcessingManager() { stopProcessing(); }

    void addTask(std::shared_ptr<Image> img, std::shared_ptr<ImageProcessor<Image>> proc, const std::string& outPath) {
        taskQueue_.push({ img, proc, outPath });
    }

    void startProcessing(int numThreads = 4) {
        stop_ = false;
        for (int i = 0; i < numThreads; ++i) {
            workers_.emplace_back(&ImageProcessingManager::workerThread, this);
        }
    }

    void stopProcessing() {
        stop_ = true;
        for (auto& t : workers_) {
            if (t.joinable()) t.detach();
        }
    }

    void waitAll() {
        while (!taskQueue_.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 等待最后一点任务收尾
    }

private:
    void workerThread() {
        while (!stop_) {
            Task task;
            if (taskQueue_.pop(task)) {
                if (!task.img->load()) continue; // 读取文件
                auto result = task.processor->process(*task.img, task.outputPath);

                // 线程安全输出日志
                std::lock_guard<std::mutex> lock(outputMutex_);
                std::cout << "[线程 " << std::this_thread::get_id() << "] "
                    << (result.success ? "成功: " : "失败: ")
                    << fs::path(result.inputFile).filename().string()
                    << " -> " << result.operation
                    << std::endl;
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
};

// ==========================================
// 5. Main Logic
// ==========================================

void printMenu() {
    std::cout << "\n=== 智能图片管理系统 (无库版) ===\n";
    std::cout << "1. 扫描文件夹 (train1)\n";
    std::cout << "2. 选择图片并设置操作\n";
    std::cout << "3. 退出\n";
    std::cout << "输入选项: ";
}

int main() {

    std::string inputFolder = "train1";
    std::string outputFolder = "processed_images";
    if (!fs::exists(outputFolder)) fs::create_directory(outputFolder);

    std::vector<std::string> fileList;
    ImageProcessingManager manager;
    manager.startProcessing(4); // 启动4个线程

    while (true) {
        printMenu();
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            fileList.clear();
            if (!fs::exists(inputFolder)) {
                std::cerr << "错误: 找不到文件夹 " << inputFolder << "，请确保它存在！" << std::endl;
                continue;
            }
            int idx = 0;
            for (const auto& entry : fs::directory_iterator(inputFolder)) {
                auto ext = entry.path().extension().string();
                if (ext == ".jpg" || ext == ".png" || ext == ".bmp" || ext == ".jpeg") {
                    fileList.push_back(entry.path().string());
                    std::cout << "[" << idx++ << "] " << entry.path().filename().string() << std::endl;
                }
            }
            std::cout << "共扫描到 " << fileList.size() << " 张图片。\n";
        }
        else if (choice == 2) {
            if (fileList.empty()) {
                std::cout << "请先执行扫描 (选项1)。\n";
                continue;
            }
            std::cout << "请输入图片编号 (输入 'a' 全选): ";
            std::string line;
            std::cin >> line; // 简单起见，这里简化输入

            std::vector<int> selectedIndices;
            if (line == "a") {
                for (size_t i = 0; i < fileList.size(); ++i) selectedIndices.push_back(i);
            }
            else {
                try {
                    int idx = std::stoi(line);
                    if (idx >= 0 && idx < fileList.size()) selectedIndices.push_back(idx);
                }
                catch (...) {}
            }

            std::cout << "选择操作: 1.缩放 2.旋转 3.转格式\n";
            int op;
            std::cin >> op;

            std::shared_ptr<ImageProcessor<Image>> processor;
            std::string suffix = "";

            if (op == 1) { processor = std::make_shared<ResizeProcessor>(); suffix = "_resized.jpg"; }
            else if (op == 2) { processor = std::make_shared<RotateProcessor>(); suffix = "_rotated.jpg"; }
            else if (op == 3) { processor = std::make_shared<FormatConvertProcessor>(); suffix = ".png"; }

            if (processor) {
                for (int idx : selectedIndices) {
                    auto img = std::make_shared<StandardImage>(fileList[idx]);
                    fs::path p(fileList[idx]);
                    std::string outName = p.stem().string() + suffix;
                    fs::path outPath = fs::path(outputFolder) / outName;
                    manager.addTask(img, processor, outPath.string());
                }
                std::cout << "任务已分发到后台线程...\n";
                manager.waitAll();
                std::cout << "全部处理完成！\n";
            }
        }
        else if (choice == 3) break;
    }
    return 0;
}