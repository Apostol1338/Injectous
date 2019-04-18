
cmake_minimum_required(VERSION 2.8)
project(injectous)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

set(INCLUDE
lib
)

set(SOURCE
dev/main.c
)

set(LIBS
${CMAKE_SOURCE_DIR}/lib/openh264/libcommon.a
${CMAKE_SOURCE_DIR}/lib/openh264/libconsole_common.a
${CMAKE_SOURCE_DIR}/lib/openh264/libdecoder.a
${CMAKE_SOURCE_DIR}/lib/openh264/libencoder.a
${CMAKE_SOURCE_DIR}/lib/openh264/libopenh264.a
${CMAKE_SOURCE_DIR}/lib/openh264/libprocessing.a
)

include_directories(${INCLUDE})
add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${LIBS})
