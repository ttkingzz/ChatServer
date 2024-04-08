cmake_minimum_required(VERSION 3.0)  #最小名字
project(main)   #工程名字

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)  # -g  加上gdb

# 配置 头文件搜索路径
# include_directories()

# 生产可执行文件server
set(SRC_LIST ./muduo_test.cpp)

# 可执行文件 放在工程根目录的bin下
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


# 设置可执行文件存储路径
add_executable(server ${SRC_LIST})

# 需要连接的文件 libmuduo_base.so   libmuduo_net.so libpthread.so
# 注意 库依赖有顺序  
target_link_libraries(server muduo_net muduo_base pthread)