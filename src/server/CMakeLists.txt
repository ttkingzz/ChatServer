# 定义了SRC_LIST变量 包含目录下所有源文件
# 不会搜索子目录
aux_source_directory(. SRC_LIST)  
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./redis REDIS_LIST)
# 指定生成可执行文件
add_executable(chatserver ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})

# 指定依赖库 muduo_net muduo_base pthread
target_link_libraries(chatserver muduo_net muduo_base mysqlclient hiredis pthread)