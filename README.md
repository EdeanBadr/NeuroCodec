# ObjectDetectionPipeline
A C++ project that builds a complete video processing pipeline using FFmpeg and OpenCV to detect objects in video frames. The project decodes a video stream, processes frames to detect and annotate objects with bounding boxes, and re-encodes the video with detections applied.
# Video Object Detection Pipeline

## Overview
The **Video Object Detection Pipeline** is a C++ project that implements a complete video processing workflow using FFmpeg and OpenCV libraries. This project features three primary classes:
- **Decoder**: Responsible for decoding video files and frames.
- **Processor**: Handles object detection and draws bounding boxes around detected objects.
- **Encoder**: Re-encodes the processed video frames back into a video file.

This pipeline allows users to efficiently process video files and perform object detection in real-time.

## Features
- Decode video files using FFmpeg.
- Perform object detection with bounding box visualization using OpenCV.
- Re-encode processed frames back to a video format.
- Supports multiple video formats and resolutions.

## Requirements
- C++11 or later
- OpenCV (version x.x.x)
- FFmpeg libraries (libavcodec, libavformat, libavutil)
- CMake (for building the project)

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/EdeanBadr/VideoObjectDetectionPipeline.git
   cd VideoObjectDetectionPipeline
   sudo apt update
   sudo apt install ffmpeg libavcodec-dev libavformat-dev libavutil-dev
   sudo apt install libopencv-dev

