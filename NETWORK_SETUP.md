# Network Setup Guide

## 🔌 Same WiFi Required!

**YES, both devices need to be on the same WiFi network for easy testing.**

---

## ✅ Option 1: Same WiFi Network (RECOMMENDED)

### How to Set Up:

1. **Meet up with your partner** (or coordinate remotely)
2. **Connect both devices to the same WiFi network:**
   - Home WiFi
   - University WiFi
   - Library WiFi
   - Mobile hotspot (one person creates hotspot, other connects)
   - Coffee shop WiFi
   - Any shared WiFi network

3. **Verify connection:**
   - Check WiFi icon on both devices
   - Both should show connected to the same network name

4. **Find server IP and configure client** (see TESTING_GUIDE.md)

### ✅ Advantages:
- **Easy setup** - no configuration needed
- **Works immediately** - just connect and go
- **No firewall issues** - local network is open
- **Fast connection** - low latency

### 📱 Using Mobile Hotspot:

If you can't access the same WiFi, one person can create a mobile hotspot:

1. **Person A (with hotspot):**
   - Enable mobile hotspot on phone
   - Connect their laptop to the hotspot
   - Note: Some carriers limit hotspot connections

2. **Person B:**
   - Connect laptop to Person A's hotspot
   - Make sure you're both on the same hotspot network

3. **Run server on one device, client on other**

---

## ⚠️ Option 2: Different Networks (NOT RECOMMENDED)

If devices are on **different networks**, you have these complex options:

### Option 2A: Port Forwarding
- Requires router access
- Security concerns
- Complex setup
- **Not recommended for testing**

### Option 2B: VPN
- Both connect to same VPN
- Additional software needed
- **More complex than necessary**

### Option 2C: Public IP + Firewall
- Requires public IP
- Complex firewall rules
- Security risks
- **Not recommended**

---

## 🎯 Best Practice: Meet Up!

**The easiest solution:**

1. **Meet in person** (library, coffee shop, university lab, etc.)
2. **Connect both laptops to same WiFi**
3. **Test your system** together
4. **Much easier** than trying to coordinate remotely on different networks

---

## 📋 Network Verification Steps

### Step 1: Verify Both Devices on Same Network

**On both devices, check:**

**Windows:**
```cmd
ipconfig
```
Look at "IPv4 Address" - should be similar ranges:
- `192.168.x.x` (both should be 192.168.x.x)
- `10.0.x.x` (both should be 10.0.x.x)

**Linux/macOS:**
```bash
hostname -I
ifconfig
```
Same subnet ranges should match.

### Step 2: Test Connectivity

**From CLIENT device, ping SERVER device:**

```bash
ping 192.168.1.100  # Replace with server's actual IP
```

**Expected:** Should receive replies. If you get "Request timeout" or "Destination host unreachable", devices aren't on same network.

### Step 3: Test Port Connectivity

**From CLIENT device:**

```bash
# Windows PowerShell:
Test-NetConnection -ComputerName 192.168.1.100 -Port 50051

# Linux/macOS:
nc -zv 192.168.1.100 50051
telnet 192.168.1.100 50051
```

**Expected:** Connection should succeed (or at least port should be reachable)

---

## 🔥 Troubleshooting Network Issues

### Problem: Can't see each other on network

**Solution:**
- Verify both on same WiFi SSID (network name)
- Check WiFi passwords are correct
- Restart WiFi on both devices
- Try different WiFi network

### Problem: Ping works but can't connect to server

**Solution:**
- Check firewall settings (allow port 50051)
- Verify server is actually running
- Check server IP address is correct

### Problem: Different IP ranges

**Example:**
- Device 1: `192.168.1.100`
- Device 2: `10.0.0.50`

**This means they're on different networks!**

**Solution:**
- Connect both to same WiFi network
- Check WiFi network names match exactly

---

## 📱 Alternative: Use Virtual Machines

If meeting in person isn't possible:

1. **One person creates 2 VMs** on their machine
2. **Configure VMs with bridged networking** (not NAT)
3. **Each VM gets its own IP** on the network
4. **Run server on VM1, client on VM2**
5. **This still requires** both VMs on same network, but can be done on one computer

---

## ✅ Quick Checklist

Before starting testing:

- [ ] Both devices connected to same WiFi network
- [ ] WiFi connection is active (not disconnected)
- [ ] Can ping between devices (or at least see same network)
- [ ] Server IP address found and noted
- [ ] Client configured with correct server IP
- [ ] Firewall allows port 50051 (if needed)

---

## 💡 Pro Tips

1. **University WiFi:** Often works well, just make sure both connected
2. **Home WiFi:** Easiest option if meeting at someone's place
3. **Mobile Hotspot:** Good backup if no shared WiFi available
4. **Coffee Shop:** Public WiFi works, but check if they allow device-to-device communication
5. **Test connectivity first:** Ping before trying to run the application

---

## 🎯 Summary

**For easiest testing:**
- ✅ Meet up in person
- ✅ Connect to same WiFi
- ✅ Start testing immediately

**For remote testing:**
- ⚠️ Much more complex
- ⚠️ Requires port forwarding or VPN
- ⚠️ Not recommended for this project

**Bottom line:** Same WiFi = Easy. Different networks = Hard. Meet up if possible! 🚀

