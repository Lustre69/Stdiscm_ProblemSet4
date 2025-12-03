# Distributed OCR System

A client-server distributed system for Optical Character Recognition (OCR) using Tesseract, gRPC, and Qt.

## Overview

This system implements a distributed OCR application where:
- **Client**: Qt-based UI for uploading images and displaying OCR results in real-time
- **Server**: Multithreaded gRPC server that processes images using Tesseract OCR

## Features

### Client
- Image upload interface with file selection
- Real-time result display as OCR completes
- Progress bar tracking processed images
- Support for multiple simultaneous uploads
- Automatic batch management (clears results when batch completes)

### Server
- Multithreaded processing pipeline
- gRPC-based communication
- Concurrent image processing
- Fault tolerance with connection handling

## Prerequisites

### Required Libraries
- **Qt6**: GUI framework
- **gRPC & Protobuf**: Interprocess communication
- **Tesseract OCR**: Text recognition engine
- **Leptonica**: Image processing library (used by Tesseract)
- **CMake**: Build system (version 3.15 or higher)
- **C++17**: Compiler support

### Installation (Ubuntu/Debian)

```bash
# Install Qt6
sudo apt-get install qt6-base-dev qt6-base-dev-tools

# Install gRPC and Protobuf
sudo apt-get install libgrpc++-dev libprotobuf-dev protobuf-compiler

# Install Tesseract and Leptonica
sudo apt-get install tesseract-ocr libtesseract-dev libleptonica-dev

# Install CMake
sudo apt-get install cmake
```

### Installation (macOS)

```bash
# Using Homebrew
brew install qt6
brew install grpc
brew install protobuf
brew install tesseract
brew install leptonica
brew install cmake
```

### Installation (Windows)

1. Install Qt6 from https://www.qt.io/download
2. Install vcpkg and use it to install:
   - grpc
   - protobuf
   - tesseract
   - leptonica
3. Install CMake from https://cmake.org/download/

## Building

1. Clone or download this repository

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Configure with CMake:
```bash
cmake ..
```

4. Build:
```bash
cmake --build .
```

## Running

### Starting the Server

Run the server on the machine/VM where processing will occur:

```bash
./ocr_server [address] [num_workers]
```

Arguments:
- `address`: Server address (default: `0.0.0.0:50051`)
- `num_workers`: Number of worker threads (default: 4)

Example:
```bash
./ocr_server 0.0.0.0:50051 4
```

### Starting the Client

Run the client on a different machine:

```bash
./ocr_client
```

**Note**: You may need to modify the server address in `client/mainwindow.cpp` (line 164) to point to your server's IP address.

## Architecture

### Client-Server Communication
- Uses gRPC for efficient, type-safe communication
- Supports streaming for multiple images
- Automatic reconnection on connection failures

### Multithreading
- Server: Worker thread pool processes images concurrently
- Client: Worker threads handle gRPC communication asynchronously

### Synchronization
- Thread-safe queues for task distribution
- Mutexes and condition variables for coordination
- Atomic operations for state tracking

### GUI Implementation
- Qt6 widgets for modern UI
- Custom card widgets for result display
- Real-time updates using Qt's signal-slot mechanism

### Fault Tolerance
- Connection state monitoring
- Automatic reconnection attempts
- Error handling and user feedback

## Project Structure

```
.
├── proto/
│   └── ocr.proto           # gRPC service definition
├── server/
│   └── main.cpp            # Server implementation
├── client/
│   ├── main.cpp            # Client entry point
│   ├── mainwindow.h/cpp    # Qt UI implementation
│   └── ocr_client.h/cpp    # gRPC client wrapper
├── CMakeLists.txt          # Build configuration
└── README.md               # This file
```

## Usage

1. Start the server on your processing machine
2. Start the client on your local machine
3. Click "Upload Images" to select image files
4. Watch as results appear in real-time
5. Progress bar shows completion status
6. Upload additional images while processing continues

## Notes for Development

- The server should run on a machine with sufficient CPU for OCR processing
- The client and server should be on different machines/VMs as specified
- Modify server address in client code or add configuration file for flexibility
- Consider adding authentication and encryption for production use

## Troubleshooting

### Connection Issues
- Ensure server is running before starting client
- Check firewall settings for port 50051
- Verify server address in client code matches server machine

### Build Issues
- Ensure all dependencies are installed
- Check CMake version (3.15+)
- Verify compiler supports C++17

### OCR Issues
- Ensure Tesseract language data is installed
- Check image format support (PNG, JPEG)
- Verify image quality and text clarity

## License

This is an academic project. Please refer to your course guidelines.

