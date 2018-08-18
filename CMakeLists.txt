cmake_minimum_required(VERSION 3.12)
project(file_reader)

#set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_FLAGS "-Wall -pthread -fPIC")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pthread -fPIC")

link_directories(${PROJECT_SOURCE_DIR}/libs)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(test_scan
        src/hbase/hbase_types.cpp
        src/hbase/hbase_constants.cpp
        src/hbase/THBaseService.cpp
        src/test_hbase.h
        src/test_hbase.cc
        src/hbase/proto/HBase.pb.cc
        src/hbase/proto/HFile.pb.cc
        src/hfile.cc
        src/hbase/util/bytes.h src/hfile_reader.h)


target_link_libraries(test_scan gtest gtest_main thrift protobuf)