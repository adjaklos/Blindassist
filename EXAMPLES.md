# Examples and Use Cases

This document provides practical examples of using BlindAssist in various scenarios.

## Example 1: Kitchen Object Identification

### Scenario
A visually impaired user wants to identify items in their kitchen.

### Setup
```cpp
// config.h settings
#define CONFIDENCE_THRESHOLD 0.6  // Medium sensitivity
#define CAPTURE_INTERVAL_MS 3000  // Quick captures
#define USE_API true              // Use cloud API for accuracy
```

### Usage
1. Mount device on chest at comfortable height
2. Point toward kitchen counter or refrigerator
3. Press button or let auto-capture work
4. Listen for audio feedback:
   - High beep = recognized item (cup, bottle, etc.)
   - Medium beep = partial recognition
   - Low beep = unclear/low confidence

### Expected Objects
- Cups and mugs
- Bottles
- Plates and bowls
- Utensils
- Food packages
- Appliances

## Example 2: Office/Study Environment

### Scenario
Identifying office supplies and electronic devices.

### Setup
```cpp
// config.h settings
#define CONFIDENCE_THRESHOLD 0.5  // Standard sensitivity
#define CAPTURE_INTERVAL_MS 5000  // Normal interval
```

### Common Detections
- Laptop/computer
- Keyboard and mouse
- Books and notebooks
- Phone
- Pens and pencils
- Desk lamp

## Example 3: Outdoor Navigation Aid

### Scenario
Identifying obstacles and landmarks while walking.

### Setup
```cpp
// config.h settings
#define CONFIDENCE_THRESHOLD 0.7  // Higher threshold for reliability
#define CAPTURE_INTERVAL_MS 4000  // Regular scanning
#define USE_API false             // Use local ML for offline operation
```

### Typical Objects
- People
- Benches
- Signs
- Vehicles
- Trees and plants
- Doors and entryways

## Example 4: Shopping Assistant

### Scenario
Identifying products while shopping.

### Arduino Code Modification

Add product database lookup:

```cpp
// In blindassist_esp32s3.ino

struct ProductInfo {
  String name;
  String category;
  float price;
};

ProductInfo lookupProduct(String objectName) {
  // Connect to product database or use local cache
  ProductInfo info;
  info.name = objectName;
  info.category = "Unknown";
  info.price = 0.0;
  
  // Add your product lookup logic here
  
  return info;
}

void provideFeedback(String objectName, float confidence) {
  // ... existing feedback code ...
  
  // Add product information
  ProductInfo product = lookupProduct(objectName);
  Serial.printf("Product: %s, Category: %s\n", 
                product.name.c_str(), 
                product.category.c_str());
}
```

## Example 5: Reading Labels (Enhanced)

### Custom API for OCR Integration

```python
# Enhanced api_server.py with OCR

from PIL import Image
import pytesseract

@app.route('/read-text', methods=['POST'])
def read_text():
    """Extract text from image using OCR"""
    try:
        api_key = request.headers.get('X-API-Key')
        if api_key != API_KEY:
            return jsonify({'error': 'Unauthorized'}), 401
        
        image_data = request.get_data()
        image = Image.open(io.BytesIO(image_data))
        
        # Perform OCR
        text = pytesseract.image_to_string(image)
        
        return jsonify({
            'text': text.strip(),
            'status': 'success'
        })
    except Exception as e:
        return jsonify({'error': str(e)}), 500
```

### ESP32 Code to Use OCR

```cpp
String readTextFromAPI(uint8_t* imageBuffer, size_t imageSize) {
  HTTPClient http;
  http.begin(String(API_ENDPOINT) + "/read-text");
  http.addHeader("Content-Type", "application/octet-stream");
  http.addHeader("X-API-Key", API_KEY);
  
  int httpResponseCode = http.POST(imageBuffer, imageSize);
  
  String text = "";
  if (httpResponseCode > 0) {
    String response = http.getString();
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, response);
    text = doc["text"].as<String>();
  }
  
  http.end();
  return text;
}
```

## Example 6: Multiple Object Detection

### Enhanced Detection Function

```cpp
struct DetectedObject {
  String name;
  float confidence;
  int x, y, width, height;  // Bounding box (if API provides)
};

std::vector<DetectedObject> detectMultipleObjects(camera_fb_t* fb) {
  std::vector<DetectedObject> objects;
  
  // Send to API that supports multiple object detection
  HTTPClient http;
  http.begin(String(API_ENDPOINT) + "/detect-multiple");
  http.addHeader("Content-Type", "application/octet-stream");
  http.addHeader("X-API-Key", API_KEY);
  
  int httpResponseCode = http.POST(fb->buf, fb->len);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, response);
    
    JsonArray detections = doc["objects"];
    for (JsonObject obj : detections) {
      DetectedObject detected;
      detected.name = obj["name"].as<String>();
      detected.confidence = obj["confidence"];
      detected.x = obj["x"];
      detected.y = obj["y"];
      detected.width = obj["width"];
      detected.height = obj["height"];
      objects.push_back(detected);
    }
  }
  
  http.end();
  return objects;
}

void announceMultipleObjects(std::vector<DetectedObject>& objects) {
  Serial.printf("Found %d objects:\n", objects.size());
  
  for (auto& obj : objects) {
    Serial.printf("  - %s (%.2f)\n", obj.name.c_str(), obj.confidence);
    
    // Audio feedback for each object
    playTone(1500 + (obj.confidence * 500), 150);
    delay(100);
  }
}
```

## Example 7: Battery-Powered Operation

### Power Management Code

```cpp
#include "esp_sleep.h"

#define BATTERY_PIN 4  // ADC pin for battery monitoring
#define LOW_BATTERY_THRESHOLD 3.3  // Volts

float getBatteryVoltage() {
  int rawValue = analogRead(BATTERY_PIN);
  // Convert to voltage (adjust based on voltage divider)
  return (rawValue / 4095.0) * 3.3 * 2.0;  // Assuming 1:1 divider
}

void checkBattery() {
  float voltage = getBatteryVoltage();
  Serial.printf("Battery: %.2fV\n", voltage);
  
  if (voltage < LOW_BATTERY_THRESHOLD) {
    // Low battery warning
    for (int i = 0; i < 5; i++) {
      playTone(800, 100);
      delay(100);
    }
    
    // Enter deep sleep to preserve battery
    Serial.println("Low battery - entering sleep mode");
    esp_deep_sleep_start();
  }
}

void enablePowerSaving() {
  // Reduce CPU frequency
  setCpuFrequencyMhz(80);  // Down from 240MHz
  
  // Reduce capture frequency when on battery
  CAPTURE_INTERVAL = 10000;  // 10 seconds instead of 5
  
  // Lower camera quality
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QQVGA);  // 160x120
  s->set_quality(s, 20);  // Lower quality
}
```

## Example 8: Voice Output (Text-to-Speech)

### Using DFPlayer Mini for Audio

```cpp
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mySoftwareSerial(16, 17);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setupAudio() {
  mySoftwareSerial.begin(9600);
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("DFPlayer initialization failed");
  }
  
  myDFPlayer.volume(20);  // 0-30
}

void speakObject(String objectName) {
  // Pre-recorded audio files named: 001.mp3 = "cup", 002.mp3 = "bottle", etc.
  int trackNumber = getTrackNumber(objectName);
  
  if (trackNumber > 0) {
    myDFPlayer.play(trackNumber);
  }
}

int getTrackNumber(String objectName) {
  // Map object names to track numbers
  if (objectName == "cup") return 1;
  if (objectName == "bottle") return 2;
  if (objectName == "chair") return 3;
  // ... add more mappings
  return 0;
}
```

## Example 9: Data Logging to SD Card

```cpp
#include "SD.h"
#include "SPI.h"

#define SD_CS 21

void setupSDCard() {
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card initialization failed");
    return;
  }
  Serial.println("SD Card initialized");
}

void logDetection(String objectName, float confidence) {
  File logFile = SD.open("/detections.csv", FILE_APPEND);
  
  if (logFile) {
    // Format: timestamp, object, confidence
    logFile.printf("%lu,%s,%.2f\n", 
                   millis(), 
                   objectName.c_str(), 
                   confidence);
    logFile.close();
  }
}

void saveImage(camera_fb_t* fb, String filename) {
  File imageFile = SD.open("/" + filename, FILE_WRITE);
  
  if (imageFile) {
    imageFile.write(fb->buf, fb->len);
    imageFile.close();
    Serial.println("Image saved: " + filename);
  }
}
```

## Example 10: Web Interface for Configuration

### ESP32 Web Server

```cpp
#include <WebServer.h>

WebServer server(80);

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/config", HTTP_POST, handleConfig);
  server.begin();
  Serial.println("Web server started");
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>BlindAssist Configuration</h1>";
  html += "<form action='/config' method='POST'>";
  html += "Confidence Threshold: <input type='number' name='threshold' step='0.1' value='" + String(CONFIDENCE_THRESHOLD) + "'><br>";
  html += "Capture Interval (ms): <input type='number' name='interval' value='" + String(CAPTURE_INTERVAL) + "'><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form></body></html>";
  
  server.send(200, "text/html", html);
}

void handleConfig() {
  if (server.hasArg("threshold")) {
    CONFIDENCE_THRESHOLD = server.arg("threshold").toFloat();
  }
  if (server.hasArg("interval")) {
    CAPTURE_INTERVAL = server.arg("interval").toInt();
  }
  
  server.send(200, "text/html", "<html><body>Configuration saved! <a href='/'>Back</a></body></html>");
}

void loop() {
  server.handleClient();
  // ... rest of loop code
}
```

## Integration Tips

### Best Practices
1. **Lighting**: Ensure adequate lighting for better recognition
2. **Distance**: Keep objects 30-100cm from camera
3. **Angle**: Point camera perpendicular to object
4. **Stability**: Minimize camera shake during capture
5. **Testing**: Test in actual use environments

### Performance Optimization
1. Use appropriate image resolution
2. Implement result caching for repeated objects
3. Adjust capture intervals based on scenario
4. Use local ML for common objects, API for complex ones

### User Feedback
1. Provide clear audio cues
2. Minimize delays between capture and feedback
3. Offer multiple confidence levels
4. Allow customization of feedback patterns

## Troubleshooting Examples

### Low Accuracy in Specific Scenario
- Increase lighting
- Adjust camera angle
- Lower movement speed
- Increase confidence threshold
- Train custom model for specific objects

### Battery Drains Quickly
- Reduce capture frequency
- Lower image quality
- Implement sleep mode
- Use larger battery
- Optimize WiFi usage

### Missing Common Objects
- Retrain model with specific objects
- Adjust confidence threshold
- Check API endpoint capabilities
- Verify camera focus and clarity
