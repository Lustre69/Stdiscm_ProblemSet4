#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <grpcpp/grpcpp.h>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include "ocr.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using ocr::OCRService;
using ocr::ImageRequest;
using ocr::ImageResponse;

// Thread-safe queue for image processing tasks
template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;

public:
    void push(T item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(item);
        condition_.notify_one();
    }

    bool tryPop(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        item = queue_.front();
        queue_.pop();
        return true;
    }

    void waitAndPop(T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !queue_.empty(); });
        item = queue_.front();
        queue_.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

// OCR Worker class
class OCRWorker {
private:
    std::unique_ptr<tesseract::TessBaseAPI> tess_;
    bool initialized_;

public:
    OCRWorker() : initialized_(false) {
        tess_ = std::make_unique<tesseract::TessBaseAPI>();
        // Initialize Tesseract with English language
        if (tess_->Init(nullptr, "eng")) {
            std::cerr << "Could not initialize tesseract" << std::endl;
            initialized_ = false;
        } else {
            initialized_ = true;
        }
    }

    ~OCRWorker() {
        if (initialized_) {
            tess_->End();
        }
    }

    bool isInitialized() const {
        return initialized_;
    }

    std::string processImage(const std::string& imageData, const std::string& format) {
        if (!initialized_) {
            return "Error: OCR engine not initialized";
        }

        try {
            // Convert image data to PIX format
            PIX* pix = nullptr;
            if (format == "png") {
                pix = pixReadMemPng(reinterpret_cast<const l_uint8*>(imageData.data()), imageData.size());
            } else if (format == "jpg" || format == "jpeg") {
                pix = pixReadMemJpeg(reinterpret_cast<const l_uint8*>(imageData.data()), imageData.size(), 0);
            } else {
                return "Error: Unsupported image format";
            }

            if (!pix) {
                return "Error: Could not decode image";
            }

            // Set image for OCR
            tess_->SetImage(pix);

            // Perform OCR
            char* outText = tess_->GetUTF8Text();
            std::string result = outText ? outText : "";
            
            // Cleanup
            delete[] outText;
            pixDestroy(&pix);

            return result;
        } catch (const std::exception& e) {
            return std::string("Error: ") + e.what();
        }
    }
};

// Task structure for worker threads
struct ProcessingTask {
    std::string image_id;
    std::string image_data;
    std::string image_format;
    ServerReaderWriter<ImageResponse, ImageRequest>* stream;
    std::mutex* stream_mutex;  // Mutex for thread-safe stream writes
};

// OCR Service Implementation
class OCRServiceImpl final : public OCRService::Service {
private:
    ThreadSafeQueue<ProcessingTask> task_queue_;
    std::vector<std::thread> worker_threads_;
    std::vector<std::unique_ptr<OCRWorker>> workers_;
    std::atomic<bool> running_;
    int num_workers_;

    void workerThread(int worker_id) {
        while (running_) {
            ProcessingTask task;
            if (task_queue_.tryPop(task)) {
                // Process the image
                std::string extracted_text = workers_[worker_id]->processImage(
                    task.image_data, 
                    task.image_format
                );

                // Send response back through stream (thread-safe)
                ImageResponse response;
                response.set_image_id(task.image_id);
                response.set_extracted_text(extracted_text);
                response.set_success(!extracted_text.empty() && extracted_text.find("Error:") == std::string::npos);
                
                if (!response.success()) {
                    response.set_error_message(extracted_text);
                }

                // Thread-safe write to stream
                if (task.stream_mutex) {
                    std::lock_guard<std::mutex> lock(*task.stream_mutex);
                    task.stream->Write(response);
                } else {
                    task.stream->Write(response);
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

public:
    OCRServiceImpl(int num_workers = 4) : running_(true), num_workers_(num_workers) {
        // Initialize worker threads
        for (int i = 0; i < num_workers_; ++i) {
            workers_.push_back(std::make_unique<OCRWorker>());
            worker_threads_.emplace_back(&OCRServiceImpl::workerThread, this, i);
            std::cout << "Started worker thread " << i << std::endl;
        }
    }

    ~OCRServiceImpl() {
        running_ = false;
        for (auto& thread : worker_threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    Status ProcessImageStream(
        ServerContext* context,
        ServerReaderWriter<ImageResponse, ImageRequest>* stream
    ) override {
        // Mutex for thread-safe writes to this stream
        std::mutex stream_mutex;
        
        ImageRequest request;
        
        while (stream->Read(&request)) {
            // Create processing task
            ProcessingTask task;
            task.image_id = request.image_id();
            task.image_data = request.image_data();
            task.image_format = request.image_format();
            task.stream = stream;
            task.stream_mutex = &stream_mutex;

            // Add to queue for processing
            task_queue_.push(task);
        }

        return Status::OK;
    }

    Status ProcessImage(
        ServerContext* context,
        const ImageRequest* request,
        ImageResponse* response
    ) override {
        // Single image processing (non-streaming)
        OCRWorker worker;
        if (!worker.isInitialized()) {
            response->set_success(false);
            response->set_error_message("OCR engine not initialized");
            return Status::OK;
        }

        std::string extracted_text = worker.processImage(
            request->image_data(),
            request->image_format()
        );

        response->set_image_id(request->image_id());
        response->set_extracted_text(extracted_text);
        response->set_success(!extracted_text.empty() && extracted_text.find("Error:") == std::string::npos);
        
        if (!response->success()) {
            response->set_error_message(extracted_text);
        }

        return Status::OK;
    }
};

void RunServer(const std::string& server_address = "0.0.0.0:50051", int num_workers = 4) {
    OCRServiceImpl service(num_workers);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    std::string server_address = "0.0.0.0:50051";
    int num_workers = 4;

    if (argc > 1) {
        server_address = argv[1];
    }
    if (argc > 2) {
        num_workers = std::stoi(argv[2]);
    }

    std::cout << "Starting OCR Server..." << std::endl;
    std::cout << "Server address: " << server_address << std::endl;
    std::cout << "Number of workers: " << num_workers << std::endl;

    RunServer(server_address, num_workers);
    return 0;
}

