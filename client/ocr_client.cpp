#include "ocr_client.h"
#include <iostream>

OCRClient::OCRClient(const std::string& server_address)
    : server_address_(server_address), connected_(false)
{
    reconnect(); // sdad
}

OCRClient::~OCRClient() {
}

void OCRClient::reconnect() {
    try {
        channel_ = grpc::CreateChannel(server_address_, grpc::InsecureChannelCredentials());
        stub_ = ocr::OCRService::NewStub(channel_);
        
        // Check connection state
        grpc::ConnectivityState state = channel_->GetState(true);
        connected_ = (state != GRPC_CHANNEL_SHUTDOWN);
        
        if (connected_) {
            std::cout << "Connected to server at " << server_address_ << std::endl;
        } else {
            std::cerr << "Failed to connect to server at " << server_address_ << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating gRPC channel: " << e.what() << std::endl;
        connected_ = false;
    }
}

bool OCRClient::isConnected() const {
    if (channel_) {
        grpc::ConnectivityState state = channel_->GetState(false);
        return (state == GRPC_CHANNEL_READY || state == GRPC_CHANNEL_IDLE);
    }
    return false;
}

bool OCRClient::processImage(const std::string& image_id,
                            const std::string& image_data,
                            const std::string& image_format,
                            std::string& extracted_text) {
    // Check connection and reconnect if necessary
    if (!isConnected()) {
        reconnect();
        if (!isConnected()) {
            extracted_text = "Error: Cannot connect to server";
            return false;
        }
    }

    ocr::ImageRequest request;
    request.set_image_id(image_id);
    request.set_image_data(image_data);
    request.set_image_format(image_format);

    ocr::ImageResponse response;
    grpc::ClientContext context;

    // Set timeout
    std::chrono::system_clock::time_point deadline = 
        std::chrono::system_clock::now() + std::chrono::seconds(60);
    context.set_deadline(deadline);

    grpc::Status status = stub_->ProcessImage(&context, request, &response);

    if (status.ok()) {
        extracted_text = response.extracted_text();
        return response.success();
    } else {
        // Handle connection errors - attempt reconnect
        if (status.error_code() == grpc::StatusCode::UNAVAILABLE ||
            status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
            reconnect();
        }
        extracted_text = "Error: " + status.error_message();
        return false;
    }
}

