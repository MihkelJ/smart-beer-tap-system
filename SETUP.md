# 🍺 Smart Beer Tap Setup Guide

This guide will walk you through setting up your Smart Beer Tap system step by step.

## 📋 Prerequisites

Before starting, make sure you have:

- [ ] ESP32 development board
- [ ] Arduino IDE installed
- [ ] Smartphone with Blynk app
- [ ] WiFi network (2.4GHz)
- [ ] 5-10 minutes for setup

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

### 3a. Blynk Credentials

**Get these from Blynk.Cloud:**

1. **Go to [blynk.cloud](https://blynk.cloud) and create account**
2. **Create new template:**
   - Device: ESP32
   - Name: "Beer Tap" (or whatever you prefer)
3. **Add these datastreams:**

   | Virtual Pin | Name         | Data Type | Min | Max  | Default |
   | ----------- | ------------ | --------- | --- | ---- | ------- |
   | V1          | Cup Size     | Integer   | 50  | 2000 | 0       |
   | V2          | Status       | String    | -   | -    | -       |
   | V3          | ML per Pulse | Double    | 0.5 | 10.0 | 2.222   |

4. **Copy your credentials:**
   ```cpp
   #define BLYNK_TEMPLATE_ID "TMPL4f7ASCKOI"        // ← Replace this
   #define BLYNK_TEMPLATE_NAME "beertap"             // ← Replace this
   #define BLYNK_AUTH_TOKEN "AbCdEf1234567890..."    // ← Replace this
   ```

### 3b. WiFi Credentials

**Use your 2.4GHz WiFi network:**

```cpp
#define WIFI_SSID "YourWiFiName"          // ← Replace this
#define WIFI_PASSWORD "YourWiFiPassword"  // ← Replace this
```

⚠️ **Important:** ESP32 only supports 2.4GHz WiFi, not 5GHz!

---

## 📱 Step 4: Create Blynk Mobile App

1. **Download Blynk IoT app** (iOS/Android)
2. **Login with same account** as web console
3. **Create new dashboard** from your template
4. **Add widgets:**

   | Widget Type | Virtual Pin | Settings                                         |
   | ----------- | ----------- | ------------------------------------------------ |
   | Slider      | V1          | Range: 50-2000, Step: 50, Title: "Cup Size (ml)" |
   | Label       | V2          | Title: "Status", Color: Auto                     |
   | Slider      | V3          | Range: 0.5-10.0, Step: 0.1, Title: "Calibration" |

---

## 🔧 Step 5: Upload Code to ESP32

1. **Open Arduino IDE**
2. **Install ESP32 board support** (if not done):
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install
3. **Install Blynk library**:
   - Tools → Manage Libraries → Search "Blynk" → Install latest
4. **Configure Arduino IDE**:
   - Tools → Board → "ESP32 Dev Module"
   - Tools → Port → Select your ESP32 port
5. **Open the project**:
   - File → Open → Select `beer-tap.ino`
6. **Compile and upload**:
   - Click Upload button (→)

---

## 🔍 Step 6: Verify Setup

### Watch the LED Patterns:

| Pattern                | Meaning                  | What to do                |
| ---------------------- | ------------------------ | ------------------------- |
| 🔴 **Fast blinking**   | Config error             | Check `src/config.h` file |
| 🟡 **Medium blinking** | Connecting to WiFi/Blynk | Wait 30 seconds           |
| 🟢 **4 quick blinks**  | ✅ System ready!         | Try the app               |
| 🔵 **Slow blink (2s)** | Ready for operation      | Set cup size in app       |

### Check Serial Monitor:

1. **Open Serial Monitor** (Tools → Serial Monitor)
2. **Set baud rate to 115200**
3. **Look for these messages:**
   ```
   ✅ WiFi connected
   ✅ Blynk connected
   ✅ System ready for operation
   ```

---

## 🚨 Troubleshooting

### Problem: Fast Red Blinking + "Config Error"

**Solution:** Check your `src/config.h` file

- Make sure all placeholders are replaced
- Check for typos in credentials
- Verify file is saved

### Problem: Stuck on "Connecting" (Medium Blink)

**Solutions:**

1. **WiFi Issues:**
   - Check WiFi name/password
   - Use 2.4GHz network only
   - Move ESP32 closer to router
2. **Blynk Issues:**
   - Verify auth token is correct
   - Check template ID matches
   - Ensure internet connection works

### Problem: Compiles but doesn't work

**Solutions:**

1. Check wiring connections
2. Verify power supply (5V for ESP32)
3. Test without hardware first

### Problem: "File not found: src/config.h"

**Solution:**

```bash
# Make sure you created the config file:
cp examples/config_template.h src/config.h
# Then edit it with your credentials
```

---

## 🧪 Step 7: Test Your Setup

1. **Power on ESP32**
2. **Wait for "System Ready" (4 quick blinks)**
3. **Open Blynk app**
4. **Test basic functions:**
   - Set cup size to 100ml
   - Watch status updates
   - Verify LED feedback

---

## 📞 Getting Help

If you're still having trouble:

1. **Check Serial Monitor output** at 115200 baud
2. **Take a photo of your LED pattern**
3. **Copy error messages** from Serial Monitor
4. **Create issue on GitHub** with:
   - Your LED pattern description
   - Serial Monitor output (remove credentials!)
   - Description of what's not working

---

## 🎉 Success!

Once you see the slow blink pattern (every 2 seconds), your Smart Beer Tap is ready to dispense the perfect pour! 🍺

**Next steps:**

- Test with water first
- Calibrate flow sensor if needed
- Enjoy your automated beer tap!
