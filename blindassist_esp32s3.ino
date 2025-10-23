/*
 * BlindAssist - Object Identification System
 * Hardware: Seeed Xiao ESP32S3 Sense
 * Features: Real-time object identification using camera, API calls, and ML
 * 
 * This sketch provides object identification assistance for visually impaired users
 * using both cloud-based API and on-device ML models.
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "config.h"

// Camera pins for Seeed Xiao ESP32S3 Sense
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39
#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

// Pin definitions
#define LED_PIN           21  // Built-in LED for status
#define BUTTON_PIN        1   // Button to trigger capture
#define BUZZER_PIN        2   // Buzzer for audio feedback

// System states
enum SystemState {
  STATE_IDLE,
  STATE_CAPTURING,
  STATE_PROCESSING,
  STATE_API_CALL,
  STATE_ML_INFERENCE,
  STATE_RESULT
};

SystemState currentState = STATE_IDLE;
String lastDetectedObject = "";
float lastConfidence = 0.0;
unsigned long lastCaptureTime = 0;
const unsigned long CAPTURE_INTERVAL = 5000; // 5 seconds between auto-captures

// Function declarations
bool initCamera();
bool initWiFi();
bool captureAndIdentifyObject();
String sendToAPI(uint8_t* imageBuffer, size_t imageSize);
void provideFeedback(String objectName, float confidence);
void playTone(int frequency, int duration);
void handleButton();

void setup() {
  Serial.begin(115200);
  Serial.println("BlindAssist - Starting...");
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Welcome tone
  playTone(1000, 200);
  delay(100);
  playTone(1500, 200);
  
  // Initialize camera
  if (!initCamera()) {
    Serial.println("Camera initialization failed!");
    while (1) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      delay(200);
    }
  }
  Serial.println("Camera initialized successfully");
  
  // Initialize WiFi
  if (!initWiFi()) {
    Serial.println("WiFi initialization failed!");
    // Continue without WiFi - can still use local ML
  } else {
    Serial.println("WiFi connected successfully");
  }
  
  digitalWrite(LED_PIN, HIGH);
  Serial.println("System ready!");
  playTone(2000, 300);
}

void loop() {
  // Handle button press for manual capture
  handleButton();
  
  // Auto-capture at intervals when idle
  if (currentState == STATE_IDLE) {
    unsigned long currentTime = millis();
    if (currentTime - lastCaptureTime >= CAPTURE_INTERVAL) {
      captureAndIdentifyObject();
      lastCaptureTime = currentTime;
    }
  }
  
  delay(100);
}

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA; // 320x240
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return false;
  }
  
  // Adjust sensor settings
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect)
    s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
    s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
    s->set_wb_mode(s, 0);        // 0 to 4
    s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
    s->set_aec2(s, 0);           // 0 = disable , 1 = enable
    s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
    s->set_agc_gain(s, 0);       // 0 to 30
    s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
    s->set_bpc(s, 0);            // 0 = disable , 1 = enable
    s->set_wpc(s, 1);            // 0 = disable , 1 = enable
    s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
    s->set_lenc(s, 1);           // 0 = disable , 1 = enable
    s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
    s->set_vflip(s, 0);          // 0 = disable , 1 = enable
    s->set_dcw(s, 1);            // 0 = disable , 1 = enable
    s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
  }
  
  return true;
}

bool initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  }
  
  return false;
}

bool captureAndIdentifyObject() {
  currentState = STATE_CAPTURING;
  digitalWrite(LED_PIN, LOW); // LED on during capture
  
  Serial.println("Capturing image...");
  camera_fb_t * fb = esp_camera_fb_get();
  
  if (!fb) {
    Serial.println("Camera capture failed");
    currentState = STATE_IDLE;
    digitalWrite(LED_PIN, HIGH);
    return false;
  }
  
  Serial.printf("Image captured: %d bytes\n", fb->len);
  
  // Try API identification if WiFi is connected
  if (WiFi.status() == WL_CONNECTED && USE_API) {
    currentState = STATE_API_CALL;
    String result = sendToAPI(fb->buf, fb->len);
    
    if (result.length() > 0) {
      Serial.println("Object identified via API: " + result);
      provideFeedback(result, 0.85);
      lastDetectedObject = result;
    }
  }
  
  // Note: Local ML inference would go here
  // For now, we focus on API-based identification
  // Future enhancement: Add TensorFlow Lite Micro for on-device inference
  
  esp_camera_fb_return(fb);
  currentState = STATE_IDLE;
  digitalWrite(LED_PIN, HIGH);
  
  return true;
}

String sendToAPI(uint8_t* imageBuffer, size_t imageSize) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return "";
  }
  
  HTTPClient http;
  http.begin(API_ENDPOINT);
  http.addHeader("Content-Type", "application/octet-stream");
  http.addHeader("X-API-Key", API_KEY);
  http.setTimeout(10000); // 10 second timeout
  
  Serial.println("Sending image to API...");
  int httpResponseCode = http.POST(imageBuffer, imageSize);
  
  String result = "";
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    Serial.println("Response: " + response);
    
    // Parse JSON response
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
      const char* objectName = doc["object"];
      float confidence = doc["confidence"];
      
      if (objectName != nullptr && confidence > CONFIDENCE_THRESHOLD) {
        result = String(objectName);
        lastConfidence = confidence;
      }
    }
  } else {
    Serial.printf("Error on HTTP request: %d\n", httpResponseCode);
  }
  
  http.end();
  return result;
}

void provideFeedback(String objectName, float confidence) {
  currentState = STATE_RESULT;
  
  // Print to serial
  Serial.printf("Detected: %s (confidence: %.2f)\n", objectName.c_str(), confidence);
  
  // Audio feedback - different tones for different confidence levels
  if (confidence > 0.8) {
    playTone(2000, 100);
    delay(50);
    playTone(2000, 100);
  } else if (confidence > 0.6) {
    playTone(1500, 200);
  } else {
    playTone(1000, 200);
  }
  
  // Blink LED to indicate detection
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
  }
}

void playTone(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
  delay(duration);
  noTone(BUZZER_PIN);
}

void handleButton() {
  static unsigned long lastButtonPress = 0;
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(BUTTON_PIN);
  
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress > 500) { // Debounce
      Serial.println("Button pressed - triggering capture");
      playTone(1500, 100);
      captureAndIdentifyObject();
      lastButtonPress = currentTime;
    }
  }
  
  lastButtonState = currentButtonState;
}
