#include <string>
#include <opencv2/opencv.hpp>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <iostream>
}
using std::string;
class Processor {
public:
    Processor();
    ~Processor();
    
    bool initialize(int width, int height);
    bool loadClassList(const std::string& filePath);
    void processImage();
    void drawBoundingBoxes(const std::vector<cv::Rect>& boxes, 
                                const std::vector<float>& confidences,
                                const std::vector<int>& class_ids,
                                const std::vector<int>& indices,
                                AVFrame* frame) ;
    bool convertToMat(AVFrame* input_frame, cv::Mat& output_mat);
    bool convertFromMat(cv::Mat& input_mat, AVFrame* output_frame);
    bool processFrame(AVFrame* input_frame, AVFrame* output_frame);
    
private: 
    cv::Mat cv_frame;
    SwsContext* sws_context;
    cv::dnn::Net net; // DNN model
    std::vector<string> class_list;
};
void drawBoxesAndSave(const cv::Mat& frame, const std::vector<cv::Rect>& boxes, const std::string& filename);