
cmake_minimum_required(VERSION 2.8)
project(injectous)

set(INCLUDE
)

set(SOURCE
dev/main.c
)

find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})
list(APPEND LIBS ${JPEG_LIBRARIES}) 

include_directories(${INCLUDE})
add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${LIBS})
