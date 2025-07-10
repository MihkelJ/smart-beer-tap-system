# 🍺 Smart Beer Tap Setup Guide

This guide will walk you through setting up your Smart Beer Tap system step by step.

## 📋 Prerequisites

Before starting, make sure you have:

- [ ] ESP32 development board
- [ ] Arduino IDE installed
- [ ] ThingsBoard account or server access
- [ ] WiFi network (2.4GHz)
- [ ] 5–10 minutes for setup

---

## 🚀 Step 1: Download and Prepare Code

1. **Clone or download this repository**
2. **Navigate to the project folder:**
   ```bash
   cd beer-tap
   ```

---

## ⚙️ Step 2: Create Your Configuration File

### Option A: Copy Template (Recommended)

```bash
# Copy the template
cp examples/config_template.h src/config.h

# Edit with your favorite editor
nano src/config.h        # Linux/Mac
notepad src/config.h     # Windows
```

### Option B: Manual Creation

1. Create a new file: `src/config.h`
2. Copy the contents from `examples/config_template.h`
3. Replace placeholder values (see Step 3)

---

## 🔐 Step 3: Configure Your Credentials

Edit `src/config.h` and replace these values:

### 3a. ThingsBoard Credentials

**Get these from ThingsBoard:**

1. **Go to [thingsboard.io](https://thingsboard.io) and create an account** (or use demo.thingsboard.io)
2. **Create a new device:**
   - Navigate to Devices → Add Device
   - Name: "Beer Tap" (or whatever you prefer)
   - Device Type: Default
3. **Copy device access token:**
   - Click on your device → Copy access token
4. **Set up telemetry keys:**
   - cupSize (Integer)
   - ready (Integer, 0=BUSY, 1=READY)
   - mlPerPulse (Double)
5. **Set up RPC commands:**
   - setCupSize
   - setMlPerPulse
   - stopPour

6. **Copy your credentials:**
   ```cpp
   #define THINGSBOARD_SERVER "demo.thingsboard.io"     // ← Replace this
   #define THINGSBOARD_ACCESS_TOKEN "YOUR_ACCESS_TOKEN" // ← Replace this
   ```

### 3b. WiFi Credentials

**Use your 2.4GHz WiFi network:**

```cpp
#define WIFI_SSID "YourWiFiName"          // ← Replace this
#define WIFI_PASSWORD "YourWiFiPassword"  // ← Replace this
```

⚠️ **Important:** ESP32 only supports 2.4 GHz Wi-Fi, not 5 GHz!

---

## 📱 Step 4: Create ThingsBoard Dashboard

1. **Navigate to Dashboards** in ThingsBoard
2. **Create new dashboard** → Add title "Beer Tap Control"
3. **Add widgets:**

   | Widget Type   | Data Key      | Settings                                      |
   |---------------|---------------|-----------------------------------------------|
   | RPC Button    | setCupSize    | Method: setCupSize, Values: 100,200,330,500ml |
   | Latest Values | cupSize       | Title: "Current Cup Size (ml)"                |
   | Latest Values | ready         | Title: "System Status", 0=BUSY, 1=READY       |
   | RPC Button    | setMlPerPulse | Method: setMlPerPulse, Values: 1.0,2.0,3.0    |
   | RPC Button    | stopPour      | Method: stopPour, Value: 1                    |
   | Latest Values | mlPerPulse    | Title: "Calibration (ml/pulse)"               |

---

## 🔧 Step 5: Upload Code to ESP32

1. **Open Arduino IDE**
2. **Install ESP32 board support** (if not done):
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install
3. **Install required libraries**:
   - Tools → Manage Libraries → Search "ThingsBoard" → Install latest
   - Search "Arduino_MQTT_Client" → Install
4. **Configure Arduino IDE**:
   - Tools → Board → "ESP32 Dev Module"
   - Tools → Port → Select your ESP32 port
5. **Open the project**:
   - File → Open → Select `beer-tap.ino`
6. **Compile and upload**:
   - Click the Upload button (→)

---

## 🔍 Step 6: Verify Setup

### Check Serial Monitor:

1. **Open Serial Monitor** (Tools → Serial Monitor)
2. **Set baud rate to 115,200**
3. **Look for these messages:**
   ```
   🍺 Smart Beer Tap System Starting...
   ✅ WiFi connected!
   IP address: xxx.xxx.xxx.xxx
   ✅ ThingsBoard connected!
   ✅ RPC subscriptions successful!
   ✅ SETUP COMPLETE!
   🍺 Smart Beer Tap ready for operation
   ```

### Successful Startup Sequence:

You should see this progression in the serial monitor:

1. **System Start**: Configuration validation
2. **Wi-Fi Connection**: Network connection and IP assignment
3. **ThingsBoard Connection**: IoT platform connection
4. **RPC Setup**: Command subscription
5. **Ready State**: System operational

**Total startup time**: Usually 10–30 seconds depending on network conditions.

---

## 🚨 Troubleshooting

### Problem: "Config Error" in Serial Monitor

**Solution:** Check your `src/config.h` file

- Make sure all placeholders are replaced
- Check for typos in credentials
- Verify the file is saved
- Remove any template comments or example text

### Problem: Wi-Fi Connection Fails

**Solutions:**

1. **WiFi Issues:**
   - Check Wi-Fi name/password spelling
   - Use 2.4 GHz network only (ESP32 limitation)
   - Move ESP32 closer to the router
   - Check if the network requires special authentication

### Problem: ThingsBoard Connection Fails

**Solutions:**

1. **ThingsBoard Issues:**
   - Verify the device access token is correct
   - Check server URL is accessible (try in browser)
   - Ensure a device exists in ThingsBoard
   - Check firewall allows MQTT traffic (port 1883)

### Problem: Compilation Errors

**Solutions:**

1. **Missing Libraries:**
   - Install ThingsBoard library
   - Install Arduino_MQTT_Client library
   - Update ESP32 board package

2. **Missing Config File:**
   ```bash
   # Make sure you created the config file:
   cp examples/config_template.h src/config.h
   # Then edit it with your credentials
   ```

### Problem: Hardware Not Responding

**Solutions:**

1. Check wiring connections
2. Verify power supply (5 V for ESP32)
3. Test ESP32 with a simple blink sketch first
4. Check relay and flow sensor connections

### Problem: RPC Commands Not Working

**Solutions:**

1. Verify RPC methods are set up correctly in ThingsBoard
2. Check the device is online in ThingsBoard
3. Try sending commands from ThingsBoard rule chains for testing

---

## 🧪 Step 7: Test Your Setup

1. **Power on ESP32**
2. **Wait for "SETUP COMPLETE" message in serial monitor**
3. **Open ThingsBoard dashboard**
4. **Test basic functions:**
   - Send setCupSize RPC with value 100
   - Check serial monitor for confirmation
   - Send stopPour RPC to test emergency stop
   - Verify telemetry updates in dashboard

### Testing Without Hardware:

The system will work without flow sensors or relays connected:
- RPC commands will be received and acknowledged
- Serial monitor will show all operations
- System operates in "simulation mode"

---

## 📞 Getting Help

If you're still having trouble:

1. **Check Serial Monitor output** at 115,200 baud rate
2. **Copy complete error messages** from Serial Monitor
3. **Check ThingsBoard device status** (online/offline)
4. **Create an issue on GitHub** with:
   - Complete Serial Monitor output (remove credentials!)
   - Description of what's not working
   - ThingsBoard server you're using
   - ESP32 board model

---

## 🎉 Success!

Once you see "Smart Beer Tap ready for operation" in the serial monitor, your system is ready to dispense! 🍺

**Access your dashboard at:**
- Demo server: https://demo.thingsboard.io
- Your server: https://your-server.com

**Next steps:**

- Test with water first before connecting to beer lines
- Calibrate flow sensor with `setMlPerPulse` RPC commands
- Set up payment integration (optional)
- Enjoy your automated beer tap!

**Safety reminders:**
- System includes automatic shutoffs (90 second timeout, 2L max volume)
- Always test thoroughly before production use
- Monitor serial output during initial testing