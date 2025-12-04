# Quick Reference - Testing on 2 Devices

## ⚠️ IMPORTANT: Same WiFi Required!

**Both devices MUST be on the same WiFi network!**
- ✅ Meet up and connect to same WiFi
- ✅ Home WiFi, university WiFi, mobile hotspot all work
- ❌ Different networks won't work without complex setup

## TL;DR - Quick Setup

### On SERVER Device:

1. **Find IP address:**
   ```bash
   ipconfig        # Windows
   hostname -I     # Linux
   ifconfig        # macOS
   ```
   Example: `192.168.1.100`

2. **Configure firewall** (allow port 50051)

3. **Start server:**
   ```bash
   cd build
   ./ocr_server 0.0.0.0:50051 4
   ```

### On CLIENT Device:

1. **Edit `client/mainwindow.cpp` line 180:**
   ```cpp
   serverAddress_("192.168.1.100:50051")  // Use server's IP
   ```

2. **Rebuild client:**
   ```bash
   cd build
   cmake --build . --target ocr_client
   ```

3. **Run client:**
   ```bash
   ./ocr_client
   ```

---

## Testing Checklist

- [ ] Server running and listening
- [ ] Client connects successfully
- [ ] Upload images works
- [ ] Results appear in real-time
- [ ] Progress bar updates
- [ ] Can upload more images during processing
- [ ] Batch clears when progress reaches 100%

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Can't connect | Check IP address, firewall, ping server |
| Server won't start | Check port availability, Tesseract installed |
| No results | Check server logs, image format |
| Build errors | Check dependencies installed |

---

## Network Commands

```bash
# Test connectivity
ping 192.168.1.100

# Test port
telnet 192.168.1.100 50051
nc -zv 192.168.1.100 50051

# Check firewall (Windows)
netsh advfirewall firewall add rule name="OCR Server" dir=in action=allow protocol=TCP localport=50051
```

---

For detailed instructions, see **TESTING_GUIDE.md**

