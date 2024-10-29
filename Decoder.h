#pragma once
#include <string>
#include <opencv2/opencv.hpp>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

class Decoder {
public:
    Decoder();
    ~Decoder();
    bool open(const std::string& input_file);
    bool readFrame(AVFrame* frame);
    void close();
    
    int getWidth() const { return codec_context ? codec_context->width : 0; }
    int getHeight() const { return codec_context ? codec_context->height : 0; }
    AVRational getTimeBase() const { return codec_context ? codec_context->time_base : AVRational{0, 1}; }

private:
    AVFormatContext* format_context;
    AVCodecContext* codec_context;
    const AVCodec* codec;
    int video_stream_index;
    AVPacket packet;
    bool findVideoStream();
};


