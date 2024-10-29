#include "Decoder.h"
#include <iostream>

// Decoder Implementation
Decoder::Decoder() 
    : format_context(nullptr), codec_context(nullptr), codec(nullptr), 
      video_stream_index(-1) {
}

Decoder::~Decoder() {
    close();
}

bool Decoder::open(const std::string& input_file) {
    if (avformat_open_input(&format_context, input_file.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Could not open input file" << std::endl;
        return false;
    }

    if (avformat_find_stream_info(format_context, nullptr) < 0) {
        std::cerr << "Could not find stream information" << std::endl;
        close();
        return false;
    }

    if (!findVideoStream()) {
        close();
        return false;
    }

    return true;
}

bool Decoder::findVideoStream() {
    for (unsigned int i = 0; i < format_context->nb_streams; i++) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            AVCodecParameters* codec_param = format_context->streams[i]->codecpar;
            codec = avcodec_find_decoder(codec_param->codec_id);
            
            if (!codec) {
                std::cerr << "Failed to find decoder" << std::endl;
                return false;
            }

            codec_context = avcodec_alloc_context3(codec);
            if (!codec_context) {
                std::cerr << "Could not allocate codec context" << std::endl;
                return false;
            }

            if (avcodec_parameters_to_context(codec_context, codec_param) < 0) {
                std::cerr << "Could not copy codec params" << std::endl;
                return false;
            }

            if (avcodec_open2(codec_context, codec, nullptr) < 0) {
                std::cerr << "Could not open codec" << std::endl;
                return false;
            }

            return true;
        }
    }
    
    std::cerr << "Could not find video stream" << std::endl;
    return false;
}

bool Decoder::readFrame(AVFrame* frame) {
    while (av_read_frame(format_context, &packet) >= 0) {
        if (packet.stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_context, &packet) == 0) {
                int ret = avcodec_receive_frame(codec_context, frame);
                av_packet_unref(&packet);
                if (ret == 0) {
                    return true;
                }
            }
        }
        av_packet_unref(&packet);
    }
    return false;
}

void Decoder::close() {
    if (codec_context) {
        avcodec_free_context(&codec_context);
    }
    if (format_context) {
        avformat_close_input(&format_context);
    }
}
