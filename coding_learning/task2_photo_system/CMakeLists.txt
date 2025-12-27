cmake_minimum_required(VERSION 3.10)

# 项目名称
project(PhotoSystem_Standard)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)

# 解决中文乱码
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")

# === 核心变化 1: 头文件路径 ===
# 告诉编译器去 "include" 文件夹找头文件
# 这样我们在代码里就可以写 #include "rt_vision/image_system.h"
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external)

# === 核心变化 2: 源文件扫描 ===
# 扫描 src 下的所有实现代码
file(GLOB SRC_FILES "src/*.cpp")

# === 核心变化 3: 生成可执行文件 ===
# 我们的入口在 app/main.cpp，同时要把 src 里的代码编译进去
add_executable(demo_app app/main.cpp ${SRC_FILES})