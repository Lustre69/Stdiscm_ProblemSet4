# Quick Start Guide

## Prerequisites Check

Before starting, ensure you have installed:
- [ ] Qt6 development libraries
- [ ] gRPC and Protocol Buffers
- [ ] Tesseract OCR and Leptonica
- [ ] CMake (3.15+)
- [ ] C++17 compatible compiler

## Building the Project

### Linux/macOS
```bash
chmod +x build.sh
./build.sh
```

### Windows
```batch
build.bat
```

### Manual Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Running the System

### Step 1: Start the Server

On your server machine/VM:

```bash
cd build
./ocr_server 0.0.0.0:50051 4
```

This starts the server:
- Listening on all interfaces (`0.0.0.0:50051`)
- With 4 worker threads

**Note**: Note the server's IP address for client configuration.

### Step 2: Configure Client (if needed)

Edit `client/mainwindow.cpp` line 164 to change server address:

```cpp
serverAddress_("your-server-ip:50051")  // Change localhost to actual server IP
```

Or modify the code to accept command-line arguments or a config file.

### Step 3: Start the Client

On your local machine:

```bash
cd build
./ocr_client
```

## Testing

1. Click "Upload Images" button
2. Select one or more image files with text
3. Watch the progress bar and see results appear in real-time
4. Try uploading more images while others are processing
5. Wait for progress bar to reach 100% to see batch completion

## Troubleshooting

### Server won't start
- Check if port 50051 is available: `netstat -an | grep 50051`
- Ensure Tesseract is installed: `tesseract --version`
- Check firewall settings

### Client can't connect
- Verify server is running: `ps aux | grep ocr_server`
- Check server IP address is correct
- Test network connectivity: `ping server-ip`
- Check firewall allows port 50051

### Build errors
- Verify all dependencies are installed
- Check CMake version: `cmake --version`
- Review CMake output for missing packages

### OCR not working
- Ensure Tesseract language data is installed
- Test Tesseract: `tesseract image.png output`
- Check image format is supported (PNG, JPEG)

## Next Steps

1. Review the code structure
2. Customize server address configuration
3. Add more error handling if needed
4. Prepare presentation materials using PRESENTATION_OUTLINE.md
5. Record demo video showing various scenarios

## Development Tips

- Server logs show worker thread activity
- Use multiple test images for demo
- Test connection failure scenarios
- Document any customizations you make

