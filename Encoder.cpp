#include "Encoder.h"
Encoder::Encoder()
    : format_context(nullptr), codec_context(nullptr), stream(nullptr), codec(nullptr) {
}

Encoder::~Encoder() {
    close();
}

bool Encoder::initialize(const std::string& output_file, int width, int height,
                            AVRational time_base, AVRational framerate) {
    if (!setupCodec(width, height, time_base, framerate)) {
        return false;
    }

    if (!openOutputFile(output_file)) {
        return false;
    }

    return true;
}

bool Encoder::setupCodec(int width, int height, AVRational time_base, AVRational framerate) {
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        std::cerr << "Codec not found" << std::endl;
        return false;
    }

    codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        std::cerr << "Could not allocate codec context" << std::endl;
        return false;
    }

    codec_context->width = width;
    codec_context->height = height;
    codec_context->time_base = time_base;
    codec_context->framerate = framerate;
    codec_context->gop_size = 10;
    codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

    if (avcodec_open2(codec_context, codec, nullptr) < 0) {
        std::cerr << "Could not open codec" << std::endl;
        return false;
    }

    return true;
}

bool Encoder::openOutputFile(const std::string& output_file) {
    if (avformat_alloc_output_context2(&format_context, nullptr, nullptr, output_file.c_str()) < 0) {
        std::cerr << "Could not create output context" << std::endl;
        return false;
    }

    stream = avformat_new_stream(format_context, nullptr);
    if (!stream) {
        std::cerr << "Could not allocate output stream" << std::endl;
        return false;
    }

    if (avcodec_parameters_from_context(stream->codecpar, codec_context) < 0) {
        std::cerr << "Could not copy codec parameters to output stream" << std::endl;
        return false;
    }

    stream->time_base = codec_context->time_base;

    if (!(format_context->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&format_context->pb, output_file.c_str(), AVIO_FLAG_WRITE) < 0) {
            std::cerr << "Could not open output file" << std::endl;
            return false;
        }
    }

    if (avformat_write_header(format_context, nullptr) < 0) {
        std::cerr << "Error occurred when writing header" << std::endl;
        return false;
    }

    return true;
}

bool Encoder::writeFrame(AVFrame* frame) {
    if (avcodec_send_frame(codec_context, frame) < 0) {
        return false;
    }

    while (true) {
        AVPacket packet = {};
        int ret = avcodec_receive_packet(codec_context, &packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        }
        if (ret < 0) {
            return false;
        }

        packet.stream_index = stream->index;
        av_interleaved_write_frame(format_context, &packet);
        av_packet_unref(&packet);
    }

    return true;
}

void Encoder::close() {
    if (format_context && format_context->pb) {
        av_write_trailer(format_context);
        if (!(format_context->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&format_context->pb);
        }
    }
    if (codec_context) {
        avcodec_free_context(&codec_context);
    }
    if (format_context) {
        avformat_free_context(format_context);
    }
}