# Configuration Guide

## Server Address Configuration

The client needs to know where the server is running. Currently, the server address is set in the client code.

### Option 1: Hardcoded (Current)

Edit `client/mainwindow.cpp`, line 164:

```cpp
serverAddress_("localhost:50051")  // Change to your server's IP
```

Change to:
```cpp
serverAddress_("192.168.1.100:50051")  // Your server's IP address
```

### Option 2: Command-Line Argument (Future Enhancement)

You could modify the client to accept command-line arguments:

```cpp
// In main.cpp
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QString serverAddress = "localhost:50051";
    if (argc > 1) {
        serverAddress = argv[1];
    }
    
    MainWindow window(serverAddress);
    window.show();
    return app.exec();
}
```

### Option 3: Configuration File (Future Enhancement)

Create a simple config file (e.g., `config.txt`) and read it at startup.

## Server Configuration

### Port Number

Default port is `50051`. To change:

**Server** - Edit `server/main.cpp`, line 262:
```cpp
void RunServer(const std::string& server_address = "0.0.0.0:50051", int num_workers = 4)
```

Or pass as command-line argument:
```bash
./ocr_server 0.0.0.0:50052 4  # Use port 50052
```

**Client** - Update server address in client code accordingly.

### Number of Worker Threads

Default is 4 workers. Adjust based on your server's CPU:

```bash
./ocr_server 0.0.0.0:50051 8  # Use 8 worker threads
```

### Tesseract Language

Currently set to English. To change, edit `server/main.cpp`, line 71:

```cpp
if (tess_->Init(nullptr, "eng")) {  // Change "eng" to other language codes
```

## Finding Server IP Address

### Linux/macOS
```bash
hostname -I        # Linux
ifconfig           # macOS/Linux
ip addr            # Linux
```

### Windows
```cmd
ipconfig
```

Look for IPv4 address (e.g., `192.168.1.100`)

## Firewall Configuration

Ensure port 50051 is open:

### Linux (UFW)
```bash
sudo ufw allow 50051
```

### Linux (firewalld)
```bash
sudo firewall-cmd --add-port=50051/tcp --permanent
sudo firewall-cmd --reload
```

### Windows Firewall
1. Open Windows Defender Firewall
2. Advanced settings
3. Inbound Rules → New Rule
4. Port → TCP → 50051
5. Allow connection

## Testing Connection

Before running the client, test server connectivity:

### From Client Machine
```bash
telnet server-ip 50051
# or
nc -zv server-ip 50051
```

If connection succeeds, you're good to go!

