# BlindAssist - Quick Start Guide

Get your BlindAssist system up and running in 15 minutes!

## What You Need

### Hardware (5 minutes to assemble)
- [ ] Seeed Xiao ESP32S3 Sense
- [ ] Push button
- [ ] Buzzer (3-5V)
- [ ] Breadboard
- [ ] 5 jumper wires
- [ ] USB-C cable

### Software (10 minutes to setup)
- [ ] Arduino IDE 2.0+
- [ ] ESP32 board support
- [ ] ArduinoJson library

## Step 1: Wire the Hardware (3 minutes)

```
Connect these 4 wires:
1. Button → GPIO1 and GND
2. Buzzer (+) → GPIO2
3. Buzzer (-) → GND
```

That's it! The camera is built-in.

## Step 2: Install Arduino IDE (2 minutes)

1. Download from: https://www.arduino.cc/en/software
2. Install and open Arduino IDE

## Step 3: Add ESP32 Support (3 minutes)

1. Go to `File → Preferences`
2. In "Additional Board Manager URLs", paste:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to `Tools → Board → Boards Manager`
4. Search "esp32" and install "esp32 by Espressif Systems"

## Step 4: Install Library (1 minute)

1. Go to `Tools → Manage Libraries`
2. Search "ArduinoJson"
3. Install "ArduinoJson by Benoit Blanchon"

## Step 5: Configure (2 minutes)

1. Copy `config.h.example` to `config.h`
2. Edit `config.h` with your WiFi details:
   ```cpp
   #define WIFI_SSID "YourWiFiName"
   #define WIFI_PASSWORD "YourPassword"
   ```

## Step 6: Upload (4 minutes)

1. Connect ESP32S3 via USB-C
2. Select `Tools → Board → ESP32 Arduino → XIAO_ESP32S3`
3. Select `Tools → Port → [Your ESP32 Port]`
4. Open `blindassist_esp32s3.ino`
5. Click **Upload** (→ button)
6. Wait for "Done uploading"

## Step 7: Test! (1 minute)

1. Open `Tools → Serial Monitor`
2. Set baud rate to **115200**
3. You should hear **two beeps** - system is ready!
4. Press the button
5. Point camera at an object
6. Wait for audio feedback

## Troubleshooting (If needed)

### "Camera initialization failed"
- Check you have ESP32S3 **Sense** variant (with camera)
- Try different USB cable/port

### "WiFi initialization failed"
- Verify SSID and password in config.h
- Ensure 2.4GHz network (not 5GHz)
- Move closer to router

### "No audio feedback"
- Check buzzer is connected to GPIO2 and GND
- Verify buzzer polarity (+/-)
- Try different buzzer

### "Compilation error"
- Verify ArduinoJson library is installed
- Check correct board is selected (XIAO_ESP32S3)
- Ensure config.h exists

## What's Next?

### Use without API (offline mode)
Set in `config.h`:
```cpp
#define USE_API false
```

### Adjust sensitivity
Lower value = more sensitive:
```cpp
#define CONFIDENCE_THRESHOLD 0.3
```

### Change capture interval
Time in milliseconds:
```cpp
#define CAPTURE_INTERVAL_MS 3000  // 3 seconds
```

### Deploy your own API
See [API_DEPLOYMENT.md](API_DEPLOYMENT.md) for cloud deployment

### Advanced features
See [EXAMPLES.md](EXAMPLES.md) for:
- Voice output
- Battery operation
- SD card logging
- Web configuration
- And more!

## Complete Documentation

- 📖 [DOCUMENTATION.md](DOCUMENTATION.md) - Full user manual
- 🔧 [HARDWARE_SETUP.md](HARDWARE_SETUP.md) - Detailed wiring
- ☁️ [API_DEPLOYMENT.md](API_DEPLOYMENT.md) - Deploy API server
- 💡 [EXAMPLES.md](EXAMPLES.md) - Advanced examples

## Need Help?

1. Check the troubleshooting section above
2. Review [DOCUMENTATION.md](DOCUMENTATION.md)
3. Create an issue on GitHub

## Safety Reminder

⚠️ This device is an assistive aid. Always use with traditional mobility aids and techniques.

---

**Congratulations!** You now have a working object identification system. 🎉
