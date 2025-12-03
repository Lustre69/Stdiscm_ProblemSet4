#ifndef OCR_CLIENT_H
#define OCR_CLIENT_H

#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "ocr.grpc.pb.h"

class OCRClient {
public:
    OCRClient(const std::string& server_address);
    ~OCRClient();

    // Process a single image (blocking)
    bool processImage(const std::string& image_id,
                     const std::string& image_data,
                     const std::string& image_format,
                     std::string& extracted_text);

    // Check if client is connected
    bool isConnected() const;

private:
    std::unique_ptr<ocr::OCRService::Stub> stub_;
    std::shared_ptr<grpc::Channel> channel_;
    std::string server_address_;
    bool connected_;

    void reconnect();
};

#endif // OCR_CLIENT_H

