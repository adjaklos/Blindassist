"""
Test script for BlindAssist API
Tests the API server with sample images
"""

import requests
import os
import sys

def test_api(image_path, api_url, api_key):
    """
    Test the BlindAssist API with an image
    
    Args:
        image_path: Path to test image
        api_url: API endpoint URL
        api_key: API key for authentication
    """
    print(f"Testing API at: {api_url}")
    print(f"Using image: {image_path}")
    
    # Check if image exists
    if not os.path.exists(image_path):
        print(f"Error: Image file not found: {image_path}")
        return False
    
    try:
        # Read image file
        with open(image_path, 'rb') as f:
            image_data = f.read()
        
        # Prepare request
        headers = {
            'Content-Type': 'application/octet-stream',
            'X-API-Key': api_key
        }
        
        # Send request
        print("Sending request...")
        response = requests.post(api_url, data=image_data, headers=headers, timeout=30)
        
        # Check response
        print(f"Status Code: {response.status_code}")
        
        if response.status_code == 200:
            result = response.json()
            print("\n✓ Success!")
            print(f"  Object: {result.get('object', 'Unknown')}")
            print(f"  Confidence: {result.get('confidence', 0):.2%}")
            return True
        else:
            print(f"\n✗ Error: {response.text}")
            return False
            
    except requests.exceptions.RequestException as e:
        print(f"\n✗ Request failed: {e}")
        return False
    except Exception as e:
        print(f"\n✗ Unexpected error: {e}")
        return False

def test_health(api_url):
    """Test the health endpoint"""
    try:
        health_url = api_url.replace('/identify', '/health')
        response = requests.get(health_url, timeout=10)
        
        if response.status_code == 200:
            print("✓ Health check passed")
            print(f"  {response.json()}")
            return True
        else:
            print(f"✗ Health check failed: {response.status_code}")
            return False
    except Exception as e:
        print(f"✗ Health check error: {e}")
        return False

if __name__ == '__main__':
    # Configuration
    API_URL = os.environ.get('API_URL', 'http://localhost:5000/identify')
    API_KEY = os.environ.get('API_KEY', 'YOUR_API_KEY')
    
    # Test image path
    if len(sys.argv) > 1:
        IMAGE_PATH = sys.argv[1]
    else:
        IMAGE_PATH = 'test_image.jpg'
    
    print("=" * 50)
    print("BlindAssist API Test")
    print("=" * 50)
    print()
    
    # Test health endpoint
    print("1. Testing health endpoint...")
    test_health(API_URL)
    print()
    
    # Test identification endpoint
    print("2. Testing identification endpoint...")
    test_api(IMAGE_PATH, API_URL, API_KEY)
    print()
    print("=" * 50)
