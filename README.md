# BlindAssist 🦯

A hardware module with **Seeed Xiao ESP32S3 Sense** to provide real-time object identification using API requests and machine learning algorithms, designed to assist visually impaired individuals.

## Overview

BlindAssist is an assistive technology device that helps visually impaired users identify objects in their environment through:
- **Real-time image capture** using ESP32S3's built-in camera
- **Cloud-based identification** via REST API
- **On-device ML inference** using TensorFlow Lite (optional)
- **Audio feedback** through buzzer/speaker
- **Simple button interface** for manual triggering

## Features

✨ **Dual Identification Modes**
- Cloud API for high accuracy
- Local ML for offline operation

🎯 **Real-time Processing**
- Automatic periodic capture (configurable)
- Manual trigger via button
- ~2-3 second response time

🔊 **Audio Feedback**
- Different tones for confidence levels
- Status beeps for system events

📱 **Easy Configuration**
- WiFi setup via config file
- Adjustable sensitivity thresholds
- Multiple API backend support

## Quick Start

### Hardware Required
- Seeed Xiao ESP32S3 Sense
- Push button
- Buzzer (3-5V)
- USB-C cable
- Optional: Battery pack for portability

### Software Setup
1. Install Arduino IDE 2.0+
2. Add ESP32 board support
3. Install required libraries (ArduinoJson, ESP32 Camera)
4. Configure WiFi and API settings in `config.h`
5. Upload `blindassist_esp32s3.ino` to your board

### Basic Usage
1. Power on the device
2. Wait for two startup beeps
3. Press button or wait for auto-capture
4. Listen for detection audio feedback
5. Check Serial Monitor for object names

## Documentation

📖 **[Complete Documentation](DOCUMENTATION.md)** - Full setup, configuration, and usage guide

🔧 **[Hardware Setup Guide](HARDWARE_SETUP.md)** - Wiring diagrams, pinouts, and assembly instructions

## Project Structure

```
Blindassist/
├── blindassist_esp32s3.ino    # Main Arduino sketch for ESP32S3
├── config.h                    # Configuration file (WiFi, API, settings)
├── config.h.example            # Example configuration template
├── ml_model.h                  # TensorFlow Lite ML model handler
├── api_server.py               # Example Python API server
├── requirements.txt            # Python dependencies for API server
├── DOCUMENTATION.md            # Complete user guide
├── HARDWARE_SETUP.md           # Hardware assembly guide
└── README.md                   # This file
```

## API Server

The repository includes an example Python Flask API server (`api_server.py`) that can be deployed to process object identification requests. It uses TensorFlow and MobileNetV2 for demonstration purposes.

### Running the API Server

```bash
# Install dependencies
pip install -r requirements.txt

# Start the server
python api_server.py
```

The server will be available at `http://localhost:5000/identify`

## Configuration

Edit `config.h` with your settings:

```cpp
// WiFi credentials
#define WIFI_SSID "Your_Network"
#define WIFI_PASSWORD "Your_Password"

// API endpoint
#define API_ENDPOINT "https://your-api.com/identify"
#define API_KEY "your_api_key"

// Detection settings
#define CONFIDENCE_THRESHOLD 0.5
#define USE_API true
```

## Pin Configuration

| Component | GPIO Pin | Description |
|-----------|----------|-------------|
| LED | 21 | Status indicator |
| Button | 1 | Manual trigger |
| Buzzer | 2 | Audio feedback |
| Camera | Built-in | Image capture |

See [HARDWARE_SETUP.md](HARDWARE_SETUP.md) for detailed wiring diagrams.

## Audio Feedback Guide

| Pattern | Meaning |
|---------|---------|
| Two beeps (startup) | System ready |
| Single beep | Button pressed |
| Double beep | High confidence detection |
| Single tone | Medium confidence |
| Low tone | Low confidence |

## Requirements

### Hardware
- Seeed Xiao ESP32S3 Sense (with camera)
- 5V Buzzer
- Push button (normally open)
- USB-C cable or 5V power supply

### Software (Arduino)
- Arduino IDE 2.0+
- ESP32 board package v2.0.0+
- ArduinoJson library v6.0+
- ESP32 Camera (included with board package)

### Software (API Server - Optional)
- Python 3.7+
- Flask
- TensorFlow 2.8+
- PIL, NumPy

## Contributing

Contributions are welcome! Please feel free to submit pull requests or create issues for bugs and feature requests.

## Use Cases

- 🏠 **Home Navigation**: Identify household objects
- 🛒 **Shopping**: Read product labels and identify items
- 🍽️ **Dining**: Identify food items and utensils
- 🚶 **Outdoor**: Recognize obstacles and landmarks
- 📚 **Reading**: Identify books, documents, and signs

## Safety Notice

⚠️ **Important**: This device is an assistive aid and should not be used as the sole means of navigation or object identification. Always use in conjunction with traditional mobility aids and techniques.

## License

This project is open source and available for educational and assistive technology purposes.

## Support

- 📝 Create an issue for bug reports
- 💡 Request features through GitHub issues
- 📖 Check the documentation for troubleshooting

## Credits

- Built for Seeed Xiao ESP32S3 Sense
- ESP32 Arduino Framework
- TensorFlow Lite for Microcontrollers

## Version

**v1.0.0** - Initial release with camera capture, API integration, and audio feedback

---

Made with ❤️ for accessibility
