# Distributed OCR System

A client-server distributed OCR (Optical Character Recognition) system built with gRPC, Qt, Tesseract, and OpenCV.

## Features

### Client Application
- Qt-based GUI for uploading images
- Real-time display of OCR results as they arrive
- Progress bar showing processing status
- Support for batch processing
- Automatic batch reset when processing completes
- Configurable server address
- Connection error handling

### Server Application
- Multi-threaded OCR processing pipeline
- Continuous operation for handling multiple requests
- Uses Tesseract OCR engine for text extraction
- OpenCV for image preprocessing
- gRPC-based communication

## Requirements

### Dependencies
- **CMake** (3.20 or higher)
- **C++20** compatible compiler
- **Qt6** (Core, Widgets)
- **OpenCV** (for image processing)
- **Tesseract OCR** (optional, falls back to mock OCR if not available)
- **gRPC** and **Protocol Buffers**
- **vcpkg** (recommended for Windows dependency management)

### Windows Setup with vcpkg

1. Install vcpkg if you haven't already:
   ```powershell
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. Install required packages:
   ```powershell
   .\vcpkg install qt6 opencv tesseract grpc protobuf --triplet x64-windows
   ```

3. Configure CMake with vcpkg toolchain:
   ```powershell
   cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake
   ```

## Building

### Generate gRPC Code

First, you need to generate the gRPC code from the proto file. The CMakeLists.txt should handle this automatically, but if you need to do it manually:

```bash
protoc --grpc_out=. --cpp_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ocr_service.proto
```

### Build with CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
```

## Running

### Start the Server

On the server machine/VM:

```bash
# Default: listen on 0.0.0.0:50051 with 4 worker threads
./ocr_server

# Custom address and worker threads
./ocr_server 0.0.0.0:50051 8
```

The server will continuously run and wait for client connections.

### Start the Client

On the client machine:

```bash
# Connect to localhost
./ocr_client

# Connect to remote server
./ocr_client 192.168.1.100:50051
```

Or use the GUI:
1. Click "Configure Server" to set the server address
2. Click "Upload Images" to select images
3. Results will appear as they are processed
4. Progress bar shows completion status

## Architecture

### Communication Protocol

The system uses gRPC with Protocol Buffers:
- **ImageRequest**: Contains image data (bytes), filename, format, and request ID
- **ImageResponse**: Contains OCR result, processing time, and status

### Server Architecture

```
gRPC Server Thread
    ↓
Request Handler (decodes image)
    ↓
Thread-Safe Queue
    ↓
Worker Thread Pool (4+ threads)
    ├─ Worker 1: Preprocess → OCR → Return
    ├─ Worker 2: Preprocess → OCR → Return
    └─ Worker N: Preprocess → OCR → Return
```

### Client Architecture

```
Qt GUI Thread
    ↓
User Upload Action
    ↓
Worker Threads (one per image)
    ├─ Worker 1: Send Request → Wait → Update UI
    ├─ Worker 2: Send Request → Wait → Update UI
    └─ Worker N: Send Request → Wait → Update UI
```

## Multithreading & Synchronization

- **Thread-Safe Queue**: Bounded queue for task distribution
- **Worker Thread Pool**: Multiple threads process images concurrently
- **Atomic Counters**: Track progress without blocking
- **Mutex Protection**: Thread-safe result storage
- **Promise/Future**: Synchronize async processing with gRPC responses

## Fault Tolerance

### Client-Side
- Connection timeout handling (60 seconds)
- Error messages for failed requests
- Server health check on startup
- Graceful handling of server disconnection

### Server-Side
- Exception handling in worker threads
- Graceful shutdown handling
- Error responses for invalid images
- Thread-safe queue prevents race conditions

## File Structure

```
.
├── client.cpp              # Qt client application
├── server.cpp              # gRPC server application
├── main.cpp                # Legacy local processor (for reference)
├── ocr_service.proto       # gRPC service definition
├── ThreadSafeQueue.hpp     # Thread-safe queue implementation
├── CMakeLists.txt          # Build configuration
└── README.md               # This file
```

## Testing

1. Start the server on one machine/VM
2. Start the client on another machine
3. Upload multiple images
4. Observe real-time result display
5. Upload more images during processing (should adjust progress bar)
6. Wait for 100% completion, then upload more (should clear and start new batch)

## Troubleshooting

### Server won't start
- Check if port 50051 is available
- Verify gRPC dependencies are installed
- Check firewall settings

### Client can't connect
- Verify server address is correct
- Check network connectivity
- Ensure server is running
- Check firewall allows connections on port 50051

### OCR not working
- Ensure Tesseract is installed
- Check Tesseract language data files are present
- Verify OpenCV can load images

### Build errors
- Verify all dependencies are installed via vcpkg
- Check CMake toolchain file path is correct
- Ensure C++20 compiler is used

## Notes

- The system is designed to run on different machines (client and server)
- For testing on the same machine, use `localhost:50051` as server address
- Tesseract language data files should be in the system path or Tesseract data directory
- Image formats supported: PNG, JPG, JPEG

