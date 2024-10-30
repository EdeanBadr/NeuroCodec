#include "Processor.h"
#include <opencv2/dnn.hpp>
#include <fstream>
#include <iostream>

Processor::Processor() : sws_context(nullptr) {
    std::string modelPath = "../Model/yolov5s.onnx";
    try {
        net = cv::dnn::readNet(modelPath);
        if (net.empty()) {
            std::cerr << "Failed to load model: " << modelPath << std::endl;
            throw std::runtime_error("Model loading failed");
        }
        std::cout << "Model loaded successfully" << std::endl;
        loadClassList("../Model/coco.names");
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error during model loading: " << e.what() << std::endl;
        throw;
    }
}

Processor::~Processor() {
    if (sws_context) {
        sws_freeContext(sws_context);
    }
}

bool Processor::loadClassList(const std::string& filePath) {
    std::ifstream ifs(filePath);
    std::string line;
    while (std::getline(ifs, line)) {
        class_list.push_back(line);
        std::cout<<line;
    }

    ifs.close();
    return true;
}

bool Processor::initialize(int width, int height) {
    cv_frame = cv::Mat(height, width, CV_8UC3);
    
    return true;
}
bool Processor::convertToMat(AVFrame* input_frame, cv::Mat& output_mat) {
    if (input_frame->format == AV_PIX_FMT_YUV420P) {
        int width = input_frame->width;
        int height = input_frame->height;
        
        // Create a properly sized YUV420p Mat
        cv::Mat yuv420(height * 3/2, width, CV_8UC1);
        
        // Copy Y plane
        std::memcpy(yuv420.data, 
                   input_frame->data[0], 
                   width * height);
        
        // Copy U plane
        std::memcpy(yuv420.data + width * height, 
                   input_frame->data[1], 
                   width * height / 4);
                   
        // Copy V plane
        std::memcpy(yuv420.data + width * height * 5/4, 
                   input_frame->data[2], 
                   width * height / 4);
        
        // Convert YUV420p to BGR
        cv::cvtColor(yuv420, output_mat, cv::COLOR_YUV2BGR_I420);
        return true;
    }
    std::cerr << "Unsupported frame format: " << input_frame->format << std::endl;
    return false;
}

bool Processor::convertFromMat(cv::Mat& input_mat, AVFrame* output_frame) {
    if (!output_frame || output_frame->data[0] == nullptr) {
        std::cerr << "Output frame is not allocated properly." << std::endl;
        return false;
    }

    int width = output_frame->width;
    int height = output_frame->height;
    
    // Convert BGR to YUV420p
    cv::Mat yuv420;
    cv::cvtColor(input_mat, yuv420, cv::COLOR_BGR2YUV_I420);
    
    // Copy Y plane
    std::memcpy(output_frame->data[0], 
                yuv420.data, 
                width * height);
    
    // Copy U plane
    std::memcpy(output_frame->data[1], 
                yuv420.data + width * height, 
                width * height / 4);
    
    // Copy V plane
    std::memcpy(output_frame->data[2], 
                yuv420.data + width * height * 5/4, 
                width * height / 4);

    return true;
}


bool Processor::processFrame(AVFrame* input_frame, AVFrame* output_frame) {
    // Convert AVFrame to cv::Mat
    cv::Mat cv_frame;
    if (!convertToMat(input_frame, cv_frame)) {
        std::cerr << "Failed to convert frame to Mat." << std::endl;
        return false;
    }

    int original_height = cv_frame.rows;
    int original_width = cv_frame.cols;

    // Prepare the input blob
    const int model_width = 640;
    const int model_height = 640;
    cv::Mat blob;
    cv::dnn::blobFromImage(cv_frame, blob, 1/255.0, cv::Size(model_width, model_height), cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    // Calculate scaling factors
    float x_factor = static_cast<float>(original_width) / model_width;
    float y_factor = static_cast<float>(original_height) / model_height;

    // Process each output layer and draw bounding boxes
    const float conf_threshold = 0.25f;
    const float nms_threshold = 0.45f;
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;
    std::vector<int> class_ids;
    for (const auto& output : outputs) {
        const int dimensions = output.size[2];
        const int rows = output.size[1]; 
        float* data = (float*)output.ptr<float>();
        // Loop over each detection row.
    for (int i = 0; i < rows; ++i) 
    {
        float confidence = data[4];  // Extract confidence score.

        if (confidence >= conf_threshold) 
        {
            float* class_scores = data + 5;
            cv::Mat scores(1, class_list.size(), CV_32FC1, class_scores);

            cv::Point class_id_point;
            double max_class_score;
            cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id_point);

            if (max_class_score > conf_threshold) 
            {
                // Store confidence and class ID.
                confidences.push_back(confidence);
                class_ids.push_back(class_id_point.x);

                // Extract and scale bounding box coordinates.
                float cx = data[0];
                float cy = data[1];
                float w = data[2];
                float h = data[3];
                int left = int((cx - 0.5 * w) * x_factor);
                int top = int((cy - 0.5 * h) * y_factor);
                int width = int(w * x_factor);
                int height = int(h * y_factor);

                // Store bounding box.
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        data += dimensions;  // Move to the next detection.
    }
    }
    // Apply NMS
    
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, conf_threshold, nms_threshold, indices);
    int i=0;
    // Draw boxes directly on cv_frame
    for (int idx : indices) {
        cv::Rect box = boxes[idx];
        int left = box.x;
        int top = box.y;
        int width = box.width;
        int height = box.height;
        cv::rectangle(cv_frame, cv::Point(left, top), cv::Point(left + width, top + height), cv::Scalar(255, 0, 0), 3);
        std::string label = class_list[class_ids[idx]] + ": " + std::to_string(confidences[idx]);
        cv::putText(cv_frame, label, boxes[idx].tl(), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);         
    }

    // Convert back to AVFrame
    if (!convertFromMat(cv_frame, output_frame)) {
        std::cerr << "Failed to convert processed frame back to AVFrame" << std::endl;
        return false;
    }

    output_frame->pts = input_frame->pts;
    return true;
}

void drawBoxesAndSave(const cv::Mat& frame, const std::vector<cv::Rect>& boxes, const std::string& filename) {
    // Create a copy of the frame to avoid modifying the original
    cv::Mat frameWithBoxes = frame.clone();

    // Draw each box on the frame
    for (const auto& box : boxes) {
        cv::rectangle(frameWithBoxes, box, cv::Scalar(0, 255, 0), 2);  // Green boxes with thickness 2
    }

    // Save the frame with boxes for debugging
    std::vector<int> compression_params = { cv::IMWRITE_JPEG_QUALITY, 90 };  // JPEG quality 90
    try {
        bool success = cv::imwrite(filename, frameWithBoxes, compression_params);
        if (success) {
            std::cout << "Debug frame saved successfully: " << filename << std::endl;
        } else {
            std::cerr << "Failed to save debug frame: " << filename << std::endl;
        }
    } catch (const cv::Exception& ex) {
        std::cerr << "Exception occurred while saving debug frame: " << ex.what() << std::endl;
    }
}
