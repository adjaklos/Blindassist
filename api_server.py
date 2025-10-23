"""
BlindAssist API Server Example
This is a simple Flask-based API server for object identification
that can be used with the BlindAssist ESP32S3 hardware module.

Requirements:
- Python 3.7+
- Flask
- TensorFlow or PyTorch
- PIL (Python Imaging Library)
- numpy

Install dependencies:
pip install flask tensorflow pillow numpy

Usage:
python api_server.py

The server will start on http://localhost:5000
"""

from flask import Flask, request, jsonify
import io
import numpy as np
from PIL import Image
import tensorflow as tf
from tensorflow import keras
import logging

app = Flask(__name__)
logging.basicConfig(level=logging.INFO)

# Load a pre-trained model (MobileNetV2 for example)
# In production, use your custom trained model
model = None
labels = []

def load_model():
    """Load the object detection model"""
    global model, labels
    try:
        # Load MobileNetV2 pre-trained on ImageNet as an example
        model = keras.applications.MobileNetV2(
            weights='imagenet',
            include_top=True,
            input_shape=(224, 224, 3)
        )
        
        # Load ImageNet labels
        # In production, use your custom labels
        labels = load_imagenet_labels()
        
        logging.info("Model loaded successfully")
        return True
    except Exception as e:
        logging.error(f"Error loading model: {e}")
        return False

def load_imagenet_labels():
    """Load ImageNet class labels"""
    # Simplified label list - in production use complete imagenet_classes.json
    return [
        'background', 'person', 'bicycle', 'car', 'motorcycle', 'airplane',
        'bus', 'train', 'truck', 'boat', 'traffic light', 'fire hydrant',
        'stop sign', 'parking meter', 'bench', 'bird', 'cat', 'dog', 'horse',
        'sheep', 'cow', 'elephant', 'bear', 'zebra', 'giraffe', 'backpack',
        'umbrella', 'handbag', 'tie', 'suitcase', 'frisbee', 'skis',
        'snowboard', 'sports ball', 'kite', 'baseball bat', 'baseball glove',
        'skateboard', 'surfboard', 'tennis racket', 'bottle', 'wine glass',
        'cup', 'fork', 'knife', 'spoon', 'bowl', 'banana', 'apple',
        'sandwich', 'orange', 'broccoli', 'carrot', 'hot dog', 'pizza',
        'donut', 'cake', 'chair', 'couch', 'potted plant', 'bed',
        'dining table', 'toilet', 'tv', 'laptop', 'mouse', 'remote',
        'keyboard', 'cell phone', 'microwave', 'oven', 'toaster', 'sink',
        'refrigerator', 'book', 'clock', 'vase', 'scissors', 'teddy bear',
        'hair drier', 'toothbrush'
    ]

def preprocess_image(image_bytes):
    """Preprocess image for model input"""
    try:
        # Load image from bytes
        image = Image.open(io.BytesIO(image_bytes))
        
        # Convert to RGB if needed
        if image.mode != 'RGB':
            image = image.convert('RGB')
        
        # Resize to model input size
        image = image.resize((224, 224))
        
        # Convert to numpy array and normalize
        img_array = keras.preprocessing.image.img_to_array(image)
        img_array = np.expand_dims(img_array, axis=0)
        img_array = keras.applications.mobilenet_v2.preprocess_input(img_array)
        
        return img_array
    except Exception as e:
        logging.error(f"Error preprocessing image: {e}")
        return None

@app.route('/identify', methods=['POST'])
def identify_object():
    """
    Main endpoint for object identification
    Expects: Raw image bytes in request body
    Returns: JSON with detected object and confidence
    """
    try:
        # Check authentication (simple API key check)
        api_key = request.headers.get('X-API-Key')
        if not api_key or api_key != 'YOUR_API_KEY':
            return jsonify({'error': 'Invalid API key'}), 401
        
        # Get image data from request
        image_data = request.get_data()
        if not image_data:
            return jsonify({'error': 'No image data provided'}), 400
        
        logging.info(f"Received image data: {len(image_data)} bytes")
        
        # Preprocess image
        processed_image = preprocess_image(image_data)
        if processed_image is None:
            return jsonify({'error': 'Failed to process image'}), 400
        
        # Run inference
        predictions = model.predict(processed_image)
        
        # Decode predictions
        decoded = keras.applications.mobilenet_v2.decode_predictions(predictions, top=1)[0]
        
        # Get top prediction
        _, object_name, confidence = decoded[0]
        
        # Simplify object name (remove underscores, make lowercase)
        object_name = object_name.replace('_', ' ').lower()
        
        logging.info(f"Detected: {object_name} with confidence {confidence:.2f}")
        
        # Return result
        return jsonify({
            'object': object_name,
            'confidence': float(confidence),
            'status': 'success'
        })
        
    except Exception as e:
        logging.error(f"Error during identification: {e}")
        return jsonify({'error': 'Internal server error', 'details': str(e)}), 500

@app.route('/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return jsonify({
        'status': 'healthy',
        'model_loaded': model is not None
    })

@app.route('/', methods=['GET'])
def index():
    """API information endpoint"""
    return jsonify({
        'name': 'BlindAssist API',
        'version': '1.0.0',
        'endpoints': {
            '/identify': 'POST - Identify objects in images',
            '/health': 'GET - Health check',
        },
        'usage': {
            'method': 'POST',
            'url': '/identify',
            'headers': {
                'Content-Type': 'application/octet-stream',
                'X-API-Key': 'YOUR_API_KEY'
            },
            'body': 'Raw image bytes (JPEG)'
        }
    })

if __name__ == '__main__':
    # Load the model on startup
    if load_model():
        # Start the Flask server
        app.run(host='0.0.0.0', port=5000, debug=False)
    else:
        logging.error("Failed to load model. Exiting.")
