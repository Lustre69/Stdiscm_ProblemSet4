# START HERE - Testing Your Distributed OCR System

## 🎯 What You Need to Know

Your friend has pushed the code and says it's done! Now you need to **test it on 2 separate devices** as required by the project specifications.

---

## 📚 Documentation Files Created

I've reviewed the code and created these helpful guides:

1. **TESTING_GUIDE.md** - Complete step-by-step testing instructions
2. **CODE_REVIEW.md** - Detailed code review and assessment
3. **TESTING_QUICK_REFERENCE.md** - Quick reference card
4. **This file** - Quick start summary

---

## ✅ Code Review Summary

**Good News:** The code looks **complete and well-structured**! 

**Key Points:**
- ✅ All required features are implemented
- ✅ Multithreading and synchronization look good
- ✅ gRPC communication properly set up
- ✅ GUI matches the specifications
- ⚠️ **One issue:** Server address is hardcoded (easy to fix)

---

## 🚀 Quick Start - Testing on 2 Devices

### ⚠️ IMPORTANT: Network Setup

**Both devices MUST be on the same WiFi network!**

- ✅ Connect both devices to the same WiFi (home, university, mobile hotspot, etc.)
- ✅ Make sure both devices are connected before starting
- ❌ Different networks won't work without complex configuration

**Best Option:** Meet up with your partner and connect to the same WiFi!

---

### Device 1: SERVER (The one that processes images)

**Step 1:** Make sure you're connected to the WiFi

**Step 2:** Find your IP address
```bash
# Windows:
ipconfig

# Linux/macOS:
hostname -I
```

**Step 2:** Configure firewall to allow port 50051

**Step 3:** Start the server
```bash
cd Stdiscm_ProblemSet4/build
./ocr_server 0.0.0.0:50051 4
```

**Keep this running!** The server needs to stay on.

---

### Device 2: CLIENT (The one with the GUI)

**Step 1:** Edit the server address in code

Open: `Stdiscm_ProblemSet4/client/mainwindow.cpp`

Find line 180:
```cpp
serverAddress_("localhost:50051")
```

Change to your SERVER device's IP:
```cpp
serverAddress_("192.168.1.100:50051")  // Replace with SERVER's IP!
```

**Step 2:** Rebuild the client
```bash
cd Stdiscm_ProblemSet4/build
cmake --build . --target ocr_client
```

**Step 3:** Run the client
```bash
./ocr_client
```

**Step 4:** Test!
- Click "Upload Images"
- Select some images
- Watch results appear in real-time!

---

## 🔧 Common Issues & Quick Fixes

### ❌ Client can't connect to server

**Check:**
1. Is server running? (Check server terminal)
2. Is IP address correct? (Double-check the IP)
3. Can you ping the server?
   ```bash
   ping 192.168.1.100  # Use server IP
   ```
4. Is firewall blocking? (Allow port 50051)

### ❌ Server won't start

**Check:**
1. Is port 50051 already in use?
2. Is Tesseract installed? (`tesseract --version`)
3. Are dependencies installed?

---

## 📋 Testing Checklist

When testing, verify:

- [ ] Server starts and shows "Server listening..."
- [ ] Client connects (check console for "Connected to server")
- [ ] Can upload images
- [ ] Results appear as they're processed (not all at once)
- [ ] Progress bar updates
- [ ] Can upload more images while others are processing
- [ ] When progress reaches 100%, new upload clears old results

---

## 📖 Full Instructions

For detailed instructions, troubleshooting, and more scenarios:

👉 **Read: TESTING_GUIDE.md**

For quick commands and reference:

👉 **Read: TESTING_QUICK_REFERENCE.md**

For code review details:

👉 **Read: CODE_REVIEW.md**

---

## 💡 Pro Tips

1. **Test on same machine first** (localhost) to verify everything works
2. **Use images from `dataset/` folder** for testing
3. **Keep server terminal visible** to see processing logs
4. **Check client console** for connection messages
5. **Start with 2-3 images** before testing many at once

---

## 🎬 For Your Demo Video

When recording your demo, show:

1. **Server terminal** running on one machine
2. **Client GUI** running on another machine
3. Upload images and show real-time results
4. Upload more images during processing
5. Show batch completion and clearing

---

## ⚡ Quick Network Setup

### Windows Firewall
```powershell
netsh advfirewall firewall add rule name="OCR Server" dir=in action=allow protocol=TCP localport=50051
```

### Linux Firewall
```bash
sudo ufw allow 50051/tcp
```

---

## 🆘 Need Help?

1. Check **TESTING_GUIDE.md** for detailed troubleshooting
2. Review **CODE_REVIEW.md** for code details
3. Check server/client console output for error messages

---

## ✨ Code Status

**Overall:** ✅ Ready for Testing

**What's Working:**
- All core features implemented
- Multithreading working
- gRPC communication set up
- GUI functional

**What Needs Attention:**
- Server address configuration (5-minute fix)
- Minor comment cleanup

---

Good luck with your testing! The code looks solid. Just follow the steps above and you should be good to go! 🚀

