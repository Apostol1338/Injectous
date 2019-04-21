
cmake_minimum_required(VERSION 2.8)
project(injectous)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

set(INCLUDE
openh264/codec/api/svc
openh264/codec/common/inc
openh264/codec/console/common/inc
openh264/codec/console/common/dec/inc
openh264/codec/decoder/core/inc
openh264/codec/decoder/plus/inc
openh264/codec/encoder/core/inc
openh264/codec/encoder/plus/inc
openh264/codec/processing/interface
openh264/codec/processing/src/adaptivequantization
openh264/codec/processing/src/backgrounddetection
openh264/codec/processing/src/common
openh264/codec/processing/src/complexityanalysis
openh264/codec/processing/src/denoise
openh264/codec/processing/src/downsample
openh264/codec/processing/src/imagerotate
openh264/codec/processing/src/scenechangedetection
openh264/codec/processing/src/scrolldetection
openh264/codec/processing/src/vaacalc
openh264/codec/module
)

set(SOURCE
dev/main.cpp
)

if(WIN32)

endif(WIN32)

if(UNIX)
message("[Setting up openh264 for Linux]")
set(LIBS
${CMAKE_SOURCE_DIR}/openh264/libcommon.a
${CMAKE_SOURCE_DIR}/openh264/libconsole_common.a
${CMAKE_SOURCE_DIR}/openh264/libdecoder.a
${CMAKE_SOURCE_DIR}/openh264/libencoder.a
${CMAKE_SOURCE_DIR}/openh264/libopenh264.a
${CMAKE_SOURCE_DIR}/openh264/libprocessing.a
)
endif(UNIX)

include_directories(${INCLUDE})
add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${LIBS})
