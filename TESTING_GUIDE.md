# Testing Guide for 2 Devices

## Overview

This guide will help you test the Distributed OCR System on **two separate devices** (as required by the project specifications). One device runs the server, the other runs the client.

## ⚠️ IMPORTANT: Network Requirements

**YES, both devices MUST be on the same WiFi network!**

- ✅ **Same WiFi/LAN**: Works perfectly - devices can communicate directly
- ❌ **Different networks**: Requires complex setup (port forwarding, VPN, etc.) - NOT recommended

**Best Approach:** Meet up with your partner and connect both devices to the same WiFi network (home WiFi, university WiFi, mobile hotspot, etc.)

## Prerequisites

### Both Devices Need:
- All dependencies installed (Qt6, gRPC, Tesseract, etc.)
- Project built successfully
- **Connected to the same WiFi network** (or same LAN)

### Server Device:
- Tesseract OCR installed and working
- Port 50051 accessible (firewall configured)

### Client Device:
- Qt6 GUI support
- Network access to server device

---

## Step-by-Step Testing Setup

### Step 1: Build the Project (Both Devices)

**On both server and client devices:**

```bash
cd Stdiscm_ProblemSet4
mkdir build
cd build
cmake ..
cmake --build .
```

Or use the build script:
- **Linux/macOS**: `./build.sh`
- **Windows**: `build.bat`

---

### Step 2: Find Server Device IP Address

**On the SERVER device:**

#### Windows:
```cmd
ipconfig
```
Look for "IPv4 Address" under your network adapter (e.g., `192.168.1.100`)

#### Linux/macOS:
```bash
hostname -I        # Linux
ifconfig           # macOS/Linux  
ip addr            # Linux
```
Look for the IPv4 address (e.g., `192.168.1.100`)

**Write down this IP address!** You'll need it for the client.

---

### Step 3: Configure Firewall (Server Device)

**On the SERVER device**, allow incoming connections on port 50051:

#### Windows:
1. Open Windows Defender Firewall
2. Click "Advanced settings"
3. Click "Inbound Rules" → "New Rule"
4. Select "Port" → Next
5. Select "TCP" and enter port `50051`
6. Select "Allow the connection"
7. Apply to all profiles
8. Name it "OCR Server" → Finish

#### Linux (UFW):
```bash
sudo ufw allow 50051/tcp
sudo ufw status  # Verify it's added
```

#### Linux (firewalld):
```bash
sudo firewall-cmd --add-port=50051/tcp --permanent
sudo firewall-cmd --reload
```

---

### Step 4: Configure Client to Connect to Server

**On the CLIENT device:**

You need to change the server address in the code. Edit `client/mainwindow.cpp`:

**Find line 180:**
```cpp
serverAddress_("localhost:50051")
```

**Change it to your server's IP:**
```cpp
serverAddress_("192.168.1.100:50051")  // Replace with your server's IP
```

**Rebuild the client:**
```bash
cd build
cmake --build . --target ocr_client
```

---

### Step 5: Start the Server

**On the SERVER device:**

```bash
cd Stdiscm_ProblemSet4/build
./ocr_server 0.0.0.0:50051 4
```

**Expected output:**
```
Starting OCR Server...
Server address: 0.0.0.0:50051
Number of workers: 4
Started worker thread 0
Started worker thread 1
Started worker thread 2
Started worker thread 3
Server listening on 0.0.0.0:50051
Press Ctrl+C to stop the server
```

**Keep this terminal open!** The server must keep running.

---

### Step 6: Start the Client

**On the CLIENT device:**

```bash
cd Stdiscm_ProblemSet4/build
./ocr_client
```

The GUI window should open. You should see:
- "Upload Images" button
- Progress bar
- Empty grid area for results

**Check the console/terminal** for connection messages:
- Success: `Connected to server at 192.168.1.100:50051`
- Failure: `Failed to connect to server...` (see troubleshooting)

---

### Step 7: Test Basic Functionality

**On the CLIENT device:**

1. **Click "Upload Images"**
2. **Select test images** (you can use images from `dataset/` folder)
3. **Observe:**
   - Cards appear with "In progress"
   - Progress bar updates
   - Results appear as they complete (real-time)
   - Different images complete at different times

---

### Step 8: Test Advanced Scenarios

#### Scenario A: Upload Multiple Images
1. Upload 5-10 images at once
2. Watch progress bar increment as each completes
3. Verify results appear asynchronously (not all at once)

#### Scenario B: Upload During Processing
1. Upload some images
2. While they're processing, upload MORE images
3. Verify progress bar adjusts dynamically
4. Verify all images eventually complete

#### Scenario C: Batch Completion
1. Upload images until progress reaches 100%
2. Wait for all to complete
3. Upload MORE images
4. **Expected:** Previous results should be cleared and new batch starts

#### Scenario D: Connection Testing
1. Stop the server (Ctrl+C on server device)
2. Try uploading an image (should show error)
3. Restart the server
4. Try uploading again (should work)

---

## Testing Checklist

Use this checklist to ensure all requirements are met:

### Client Requirements
- [ ] UI allows uploading images ✓
- [ ] Images are sent to server ✓
- [ ] OCR results are received and displayed ✓
- [ ] Results show as soon as they're ready (not waiting for all) ✓
- [ ] Progress bar shows processed count ✓
- [ ] Can upload additional images while processing ✓
- [ ] Progress bar adjusts when new images added during processing ✓
- [ ] When progress reaches 100%, new upload clears previous results ✓

### Server Requirements
- [ ] Server runs continuously ✓
- [ ] Server receives images from client ✓
- [ ] Server processes images using multiple threads ✓
- [ ] Server returns results immediately after processing ✓
- [ ] Server runs on different machine/VM ✓

---

## Troubleshooting Connection Issues

### Problem: Client can't connect to server

**Checklist:**

1. **Is server running?**
   - Look at server terminal - should show "Server listening..."
   - If not, start the server

2. **Is IP address correct?**
   - Verify server IP hasn't changed (check with `ipconfig` or `ifconfig`)
   - Verify client code has correct IP

3. **Can you ping the server?**
   - **From client device:**
     ```bash
     ping 192.168.1.100  # Replace with server IP
     ```
   - If ping fails, network connectivity issue

4. **Is firewall blocking?**
   - Check server firewall settings
   - Temporarily disable firewall to test
   - Make sure port 50051 is open

5. **Test port connectivity:**
   - **From client device:**
     ```bash
     # Windows PowerShell:
     Test-NetConnection -ComputerName 192.168.1.100 -Port 50051
     
     # Linux/macOS:
     nc -zv 192.168.1.100 50051
     telnet 192.168.1.100 50051
     ```

6. **Are devices on same network?**
   - Both should be on same WiFi/LAN
   - Check subnet masks match

### Problem: Server won't start

1. **Port already in use:**
   ```bash
   # Check what's using port 50051
   netstat -ano | findstr 50051  # Windows
   lsof -i :50051                # Linux/macOS
   ```

2. **Tesseract not found:**
   ```bash
   tesseract --version  # Should show version
   ```

3. **Dependencies missing:**
   - Check build errors
   - Verify all libraries installed

### Problem: Images process but show errors

1. **Tesseract language data missing:**
   - Install language data: `sudo apt-get install tesseract-ocr-eng` (Linux)
   
2. **Image format not supported:**
   - Currently supports: PNG, JPG, JPEG
   - Check file extension matches format

3. **Image corrupted:**
   - Try with different images
   - Use images from `dataset/` folder

---

## Quick Testing on Same Machine (Development)

If you want to test on the same machine first (before using 2 devices):

1. Start server:
   ```bash
   ./ocr_server localhost:50051 4
   ```

2. Start client (server address is already `localhost:50051` by default)

3. Test functionality

4. **Remember:** For final testing, you MUST use 2 separate devices as per requirements!

---

## Alternative: Using Virtual Machines

If you don't have 2 physical devices, you can use VMs:

### Setup:
1. Create 2 VMs on the same host
2. Configure VM network as "Bridged" (not NAT)
3. Each VM gets its own IP address
4. Follow steps above using VM IPs

### Benefits:
- Test true distributed setup
- Easier to reset/restart
- Can run both on same computer

---

## Demo Preparation

For your video demonstration:

1. **Prepare test images** in advance
2. **Have server already running** before recording
3. **Show these scenarios:**
   - Single image upload → result appears
   - Multiple images → results appear asynchronously
   - Upload during processing → progress adjusts
   - Batch completion → new batch clears old results
   - Connection error handling (optional)

4. **Show server terminal** to demonstrate it's running on different machine

---

## Notes

- **Server must keep running** while client is active
- **Client can be closed and reopened** - server stays running
- **Network latency** may affect processing time
- **Multiple clients** can connect to same server (if needed)

Good luck with your testing! 🚀

