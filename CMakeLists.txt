cmake_minimum_required(VERSION 3.10)
project(VideoProcessingApp)
add_definitions(-D__STDC_CONSTANT_MACROS)

# Locate FFmpeg libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVUTIL REQUIRED libavutil)
pkg_check_modules(SWSCALE REQUIRED libswscale)

# Include directories for FFmpeg
include_directories(${AVCODEC_INCLUDE_DIRS}
                    ${AVFORMAT_INCLUDE_DIRS}
                    ${AVUTIL_INCLUDE_DIRS}
                    ${SWSCALE_INCLUDE_DIRS})

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories for OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Add source files
add_executable(VideoProcessingApp main.cpp Decoder.cpp Encoder.cpp Processor.cpp )

# Link FFmpeg and OpenCV libraries
target_link_libraries(VideoProcessingApp
    ${AVCODEC_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    ${SWSCALE_LIBRARIES}
    ${OpenCV_LIBS}
)