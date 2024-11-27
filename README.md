# NeuroCodec
## Overview
The ** NeuroCodec ** is a C++ project that implements a complete video processing workflow using FFmpeg and OpenCV libraries. This project features three primary classes: 
- **Decoder**: Responsible for decoding video files and frames.
- **Processor**: Handles object detection and draws bounding boxes around detected objects.
- **Encoder**: Re-encodes the processed video frames back into a video file. This pipeline allows users to efficiently process video files and perform object detection in real-time.
  
## Features
- Decode video files using FFmpeg. - Perform object detection with bounding box visualization using OpenCV. - Re-encode processed frames back to a video format. - Supports multiple video formats and resolutions.
## Requirements
- C++11 or later
- OpenCV (version 4.6.0)
- FFmpeg libraries (libavcodec, libavformat, libavutil)
- CMake (for building the project)
## Installation
1. Clone the repository:
```
 git clone https://github.com/EdeanBadr/VideoObjectDetectionPipeline.git
```
``` cd VideoObjectDetectionPipeline ``` 
3. Install the required libraries:
   - Install FFmpeg:
     ``` sudo apt install ffmpeg libavcodec-dev libavformat-dev libavutil-dev ```
   - Install OpenCV:
     ``` sudo apt install libopencv-dev ```
5. Build the project:
   ``` mkdir```
   ``` build cd ```
   ``` build cmake ..```
   ``` make ```
## Usage
To use the video processing pipeline, you can run the executable with the input video file as follows: 
  ``` ./VideoProcessingPipeline input_video.mp4 output_video.mp4 ``` Replace `input_video.mp4` with the path to your input video file and `output_video.mp4` with the desired output filename.
## Example
A basic example of how to utilize the classes in the pipeline can be found in the `main.cpp` file. You can modify this file to suit your specific use case and input files.
## Contributing
Contributions are welcome! Please feel free to submit a pull request or open an issue for any bugs or feature requests.
## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
