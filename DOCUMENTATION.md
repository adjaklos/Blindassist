# BlindAssist - Object Identification System

A hardware module with **Seeed Xiao ESP32S3 Sense** to provide real-time object identification using API requests and machine learning algorithms, designed to assist visually impaired individuals.

## Features

- **Real-time Object Detection**: Captures images using the built-in camera and identifies objects
- **Dual Identification Methods**: 
  - Cloud-based API for accurate identification
  - On-device ML inference using TensorFlow Lite (optional)
- **Audio Feedback**: Provides audio cues through buzzer to indicate detection status
- **Visual Feedback**: LED indicators for system status
- **Manual & Automatic Capture**: Button trigger or automatic periodic capture
- **Low Power Design**: Optimized for battery operation

## Hardware Requirements

### Main Components
- **Seeed Xiao ESP32S3 Sense** (with built-in camera)
- Buzzer or small speaker (for audio feedback)
- Push button (for manual trigger)
- Optional: External power supply or battery pack

### Pin Connections

| Component | ESP32S3 Pin | Description |
|-----------|-------------|-------------|
| LED | GPIO 21 | Status indicator (built-in) |
| Button | GPIO 1 | Manual capture trigger |
| Buzzer | GPIO 2 | Audio feedback |
| Camera | Pre-wired | Built-in on ESP32S3 Sense |

## Software Requirements

### Arduino IDE Setup

1. **Install Arduino IDE** (version 2.0 or later recommended)
   - Download from: https://www.arduino.cc/en/software

2. **Install ESP32 Board Support**
   - Open Arduino IDE
   - Go to `File` → `Preferences`
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to `Tools` → `Board` → `Boards Manager`
   - Search for "esp32" and install "esp32 by Espressif Systems"

3. **Select Board**
   - Go to `Tools` → `Board` → `ESP32 Arduino`
   - Select **"XIAO_ESP32S3"**

### Required Libraries

Install the following libraries via Arduino Library Manager (`Tools` → `Manage Libraries`):

- **ArduinoJson** (by Benoit Blanchon) - for API response parsing
- **ESP32 Camera** (included with ESP32 board package)

### Optional Libraries (for on-device ML)

- **TensorFlow Lite Micro for ESP32** - for local inference
  - Install from: https://github.com/tensorflow/tflite-micro-arduino-examples

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/adjaklos/Blindassist.git
cd Blindassist
```

### 2. Configure the System

1. Copy `config.h.example` to `config.h`:
   ```bash
   cp config.h.example config.h
   ```

2. Edit `config.h` with your settings:
   ```cpp
   #define WIFI_SSID "Your_WiFi_Network"
   #define WIFI_PASSWORD "Your_WiFi_Password"
   #define API_ENDPOINT "https://your-api-endpoint.com/identify"
   #define API_KEY "your_api_key_here"
   ```

### 3. Upload to ESP32S3

1. Connect the Seeed Xiao ESP32S3 Sense to your computer via USB-C
2. Open `blindassist_esp32s3.ino` in Arduino IDE
3. Select the correct port: `Tools` → `Port` → Select your ESP32S3 port
4. Click **Upload** button (→)
5. Wait for compilation and upload to complete

### 4. Monitor Serial Output

1. Open Serial Monitor: `Tools` → `Serial Monitor`
2. Set baud rate to **115200**
3. You should see initialization messages and system status

## Usage

### Manual Mode
- Press the button connected to GPIO 1 to capture and identify an object
- System will provide audio feedback based on detection confidence
- Results are displayed in the Serial Monitor

### Automatic Mode
- System automatically captures images every 5 seconds (configurable)
- Identified objects trigger audio feedback
- LED blinks to indicate active detection

### Audio Feedback Signals

| Signal | Meaning |
|--------|---------|
| Short beep (1000Hz) | System starting |
| Two beeps (1000Hz, 1500Hz) | System ready |
| Double beep (2000Hz) | High confidence detection (>80%) |
| Single beep (1500Hz) | Medium confidence (60-80%) |
| Low beep (1000Hz) | Low confidence (50-60%) |

### LED Indicators

| Pattern | Meaning |
|---------|---------|
| Solid ON | System ready/idle |
| OFF | Capturing image |
| Triple blink | Object detected |
| Fast blinking | Error state |

## API Integration

### API Endpoint Format

The system expects the API endpoint to:
- Accept POST requests with image data in the body
- Accept `Content-Type: application/octet-stream`
- Return JSON response in the format:

```json
{
  "object": "cup",
  "confidence": 0.87
}
```

### Example API Services

You can integrate with various object detection APIs:

1. **Google Cloud Vision API**
   - Endpoint: `https://vision.googleapis.com/v1/images:annotate`
   - Requires API key setup

2. **AWS Rekognition**
   - Endpoint: Your AWS endpoint
   - Requires AWS credentials

3. **Custom API**
   - Deploy your own object detection model
   - Use frameworks like TensorFlow Serving or FastAPI

## Machine Learning Integration

### Using On-Device ML (Optional)

The system supports TensorFlow Lite models for on-device inference:

1. **Train or obtain a model**
   - Use MobileNet or EfficientNet for best performance
   - Input size: 96x96 or 128x128 pixels recommended

2. **Convert to TensorFlow Lite**
   ```python
   import tensorflow as tf
   
   converter = tf.lite.TFLiteConverter.from_keras_model(model)
   converter.optimizations = [tf.lite.Optimize.DEFAULT]
   tflite_model = converter.convert()
   
   with open('model.tflite', 'wb') as f:
       f.write(tflite_model)
   ```

3. **Convert to C array**
   ```bash
   xxd -i model.tflite > model.h
   ```

4. **Update ml_model.h** with your model data

## Configuration Options

Edit `config.h` to customize behavior:

```cpp
// WiFi settings
#define WIFI_SSID "your_network"
#define WIFI_PASSWORD "your_password"

// API settings
#define API_ENDPOINT "https://api.example.com/identify"
#define API_KEY "your_key"
#define USE_API true  // false to use only local ML

// Detection settings
#define CONFIDENCE_THRESHOLD 0.5  // Minimum confidence (0.0-1.0)
#define AUTO_CAPTURE_ENABLED true
#define CAPTURE_INTERVAL_MS 5000  // Time between captures

// Performance
#define IMAGE_QUALITY 12  // JPEG quality (0-63, lower=better)
#define FRAME_SIZE FRAMESIZE_QVGA  // 320x240 pixels
```

## Troubleshooting

### Camera Initialization Failed
- Check that you're using the Seeed Xiao ESP32S3 **Sense** variant (with camera)
- Verify camera ribbon cable is properly connected
- Try power cycling the device

### WiFi Connection Failed
- Verify SSID and password in config.h
- Check that WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Ensure you're in range of the WiFi router

### API Errors
- Verify API endpoint URL is correct
- Check API key is valid
- Ensure API service is accessible from your network
- Monitor Serial output for HTTP error codes

### No Audio Feedback
- Check buzzer is connected to correct pin (GPIO 2)
- Verify buzzer polarity
- Try different buzzer type (active vs passive)

### Low Detection Accuracy
- Improve lighting conditions
- Adjust camera position
- Lower CONFIDENCE_THRESHOLD in config.h
- Try different API service

## Power Consumption

- Active (capturing): ~150-200mA
- Idle: ~80-100mA
- Deep sleep (future feature): ~10mA

For battery operation, consider:
- 1000mAh battery: ~5-10 hours
- 2000mAh battery: ~10-20 hours
- Power bank: Extended operation

## Future Enhancements

- [ ] Battery level monitoring
- [ ] Deep sleep mode for power saving
- [ ] Voice output via I2S audio
- [ ] SD card logging
- [ ] BLE connectivity for smartphone app
- [ ] Multi-language support
- [ ] Object tracking across frames
- [ ] Distance estimation

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## License

This project is provided as-is for educational and assistive technology purposes.

## Safety Notice

This device is intended as an assistive aid and should not be relied upon as the sole means of navigation or object identification for visually impaired users. Always use in conjunction with traditional mobility aids and techniques.

## Support

For questions and support:
- Create an issue on GitHub
- Check the troubleshooting section
- Review Arduino ESP32 documentation

## Credits

- Built for the Seeed Xiao ESP32S3 Sense
- Uses ESP32 Arduino framework
- TensorFlow Lite for Microcontrollers

## Version History

- v1.0.0 (2025-10-23): Initial release
  - Basic camera capture
  - API integration
  - Audio feedback
  - Auto and manual modes
