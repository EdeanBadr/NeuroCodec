#include "Decoder.h"
#include "Encoder.h"
#include "Processor.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    Decoder decoder;
    Encoder encoder;
    Processor processor;

    if (!decoder.open(argv[1])) {
        return 1;
    }

    if (!encoder.initialize(argv[2], decoder.getWidth(), decoder.getHeight())) {
        return 1;
    }

    if (!processor.initialize(decoder.getWidth(), decoder.getHeight())) {
        return 1;
    }

    AVFrame* input_frame = av_frame_alloc();
    AVFrame* output_frame = av_frame_alloc();
    
    output_frame->format = AV_PIX_FMT_YUV420P;
    output_frame->width = decoder.getWidth();
    output_frame->height = decoder.getHeight();
    av_frame_get_buffer(output_frame, 0);

    while (decoder.readFrame(input_frame)) {
        if (processor.processFrame(input_frame, output_frame)) {
            encoder.writeFrame(output_frame);
        }
    }
    av_frame_free(&input_frame);
    av_frame_free(&output_frame);

    return 0;
}