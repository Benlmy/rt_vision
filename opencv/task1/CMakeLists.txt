cmake_minimum_required(VERSION 3.0)
project(task1_project)

# 1. 寻找 OpenCV 库
find_package(OpenCV REQUIRED)

# 2. 包含 OpenCV 的头文件目录
include_directories(${OpenCV_INCLUDE_DIRS})

# 3. 定义我们要生成的可执行文件名为 task1，源文件是 main.cpp
add_executable(task1 main.cpp)

# 4. 将 OpenCV 库链接到我们的程序上
target_link_libraries(task1 ${OpenCV_LIBS})