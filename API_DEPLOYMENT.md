# API Deployment Guide

This guide covers deploying the BlindAssist API server to various platforms for production use.

## Prerequisites

- Python 3.7 or higher
- Git
- Your trained ML model (or use the provided example)

## Local Deployment

### 1. Setup Environment

```bash
# Clone the repository
git clone https://github.com/adjaklos/Blindassist.git
cd Blindassist

# Create virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt
```

### 2. Run the Server

```bash
python api_server.py
```

The server will be available at `http://localhost:5000`

## Cloud Deployment Options

### Option 1: Heroku

1. **Create Heroku App**
```bash
heroku create blindassist-api
```

2. **Create Procfile**
```
web: gunicorn api_server:app
```

3. **Deploy**
```bash
git add .
git commit -m "Deploy to Heroku"
git push heroku main
```

4. **Set Environment Variables**
```bash
heroku config:set API_KEY=your_secure_api_key
```

### Option 2: AWS EC2

1. **Launch EC2 Instance**
   - Amazon Linux 2 or Ubuntu 20.04
   - t2.small or larger (for ML models)
   - Open port 5000 in security group

2. **Connect and Setup**
```bash
ssh -i your-key.pem ec2-user@your-instance-ip

# Install Python and dependencies
sudo yum update -y
sudo yum install python3 git -y
git clone https://github.com/adjaklos/Blindassist.git
cd Blindassist
pip3 install -r requirements.txt
```

3. **Run with Supervisor (Process Manager)**
```bash
sudo yum install supervisor -y

# Create supervisor config
sudo nano /etc/supervisord.d/blindassist.ini
```

Add:
```ini
[program:blindassist]
command=/usr/bin/python3 /home/ec2-user/Blindassist/api_server.py
directory=/home/ec2-user/Blindassist
user=ec2-user
autostart=true
autorestart=true
stderr_logfile=/var/log/blindassist.err.log
stdout_logfile=/var/log/blindassist.out.log
```

4. **Start Service**
```bash
sudo supervisorctl reread
sudo supervisorctl update
sudo supervisorctl start blindassist
```

### Option 3: Google Cloud Platform (Cloud Run)

1. **Create Dockerfile**
```dockerfile
FROM python:3.9-slim

WORKDIR /app
COPY requirements.txt .
RUN pip install -r requirements.txt

COPY api_server.py .
EXPOSE 8080

CMD ["gunicorn", "--bind", "0.0.0.0:8080", "api_server:app"]
```

2. **Build and Deploy**
```bash
gcloud builds submit --tag gcr.io/your-project/blindassist-api
gcloud run deploy blindassist-api --image gcr.io/your-project/blindassist-api --platform managed
```

### Option 4: DigitalOcean App Platform

1. **Create app.yaml**
```yaml
name: blindassist-api
services:
- name: api
  github:
    repo: adjaklos/Blindassist
    branch: main
  run_command: gunicorn --bind 0.0.0.0:8080 api_server:app
  envs:
  - key: API_KEY
    value: ${API_KEY}
  http_port: 8080
```

2. **Deploy via Dashboard**
   - Connect GitHub repository
   - Select branch
   - Configure environment variables
   - Deploy

## Production Considerations

### 1. Security

**API Key Management**
```python
# Use environment variables
import os
API_KEY = os.environ.get('API_KEY', 'default-key')

@app.route('/identify', methods=['POST'])
def identify_object():
    api_key = request.headers.get('X-API-Key')
    if api_key != API_KEY:
        return jsonify({'error': 'Unauthorized'}), 401
```

**HTTPS/SSL**
- Use reverse proxy (Nginx) with SSL certificate
- Or use cloud platform's built-in SSL

**Rate Limiting**
```bash
pip install flask-limiter
```

```python
from flask_limiter import Limiter

limiter = Limiter(
    app,
    key_func=lambda: request.headers.get('X-API-Key'),
    default_limits=["100 per hour"]
)
```

### 2. Performance

**Gunicorn Configuration** (production.py)
```python
import multiprocessing

bind = "0.0.0.0:5000"
workers = multiprocessing.cpu_count() * 2 + 1
worker_class = "sync"
timeout = 30
keepalive = 5
```

Run with:
```bash
gunicorn -c production.py api_server:app
```

**Model Optimization**
- Use TensorFlow Lite for smaller models
- Implement model caching
- Use GPU instances for heavy models

### 3. Monitoring

**Logging**
```python
import logging
from logging.handlers import RotatingFileHandler

handler = RotatingFileHandler('app.log', maxBytes=10000000, backupCount=3)
handler.setLevel(logging.INFO)
app.logger.addHandler(handler)
```

**Health Checks**
```python
@app.route('/health')
def health():
    return jsonify({
        'status': 'healthy',
        'timestamp': datetime.now().isoformat()
    })
```

**Metrics** (with Prometheus)
```bash
pip install prometheus-flask-exporter
```

```python
from prometheus_flask_exporter import PrometheusMetrics
metrics = PrometheusMetrics(app)
```

### 4. Scalability

**Load Balancing**
- Use Nginx as reverse proxy
- Deploy multiple instances
- Implement auto-scaling

**Caching**
```python
from flask_caching import Cache

cache = Cache(app, config={'CACHE_TYPE': 'simple'})

@app.route('/identify', methods=['POST'])
@cache.cached(timeout=300, key_prefix='identify')
def identify_object():
    # ... identification logic
```

## Nginx Configuration

```nginx
upstream blindassist_api {
    server 127.0.0.1:5000;
    server 127.0.0.1:5001;
}

server {
    listen 80;
    server_name api.blindassist.com;

    location / {
        proxy_pass http://blindassist_api;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        client_max_body_size 10M;
    }
}
```

## Custom Model Deployment

If you have a custom trained model:

1. **Export Model**
```python
# TensorFlow
model.save('custom_model.h5')

# Or TensorFlow Lite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()
```

2. **Update api_server.py**
```python
def load_model():
    global model
    model = keras.models.load_model('custom_model.h5')
    return True
```

3. **Deploy Model File**
- Include in repository (if small)
- Or download from cloud storage on startup
- Or mount as volume in container

## Testing the Deployment

```bash
# Test health endpoint
curl https://your-api-domain.com/health

# Test identification with image
curl -X POST \
  -H "Content-Type: application/octet-stream" \
  -H "X-API-Key: your_api_key" \
  --data-binary @test_image.jpg \
  https://your-api-domain.com/identify
```

## Cost Estimates

| Platform | Estimated Monthly Cost |
|----------|----------------------|
| Heroku (Hobby) | $7 |
| AWS EC2 (t2.small) | $17 |
| Google Cloud Run | $5-20 (usage-based) |
| DigitalOcean (Basic) | $5 |

## Troubleshooting

### Issue: Model Loading Fails
- Check model file path
- Verify TensorFlow version compatibility
- Ensure sufficient memory

### Issue: High Latency
- Use GPU instance for inference
- Implement model caching
- Optimize image preprocessing

### Issue: Out of Memory
- Reduce batch size
- Use quantized models
- Increase instance size

## Additional Resources

- [Flask Documentation](https://flask.palletsprojects.com/)
- [Gunicorn Documentation](https://docs.gunicorn.org/)
- [TensorFlow Serving](https://www.tensorflow.org/tfx/guide/serving)
- [Docker Best Practices](https://docs.docker.com/develop/dev-best-practices/)

## Support

For deployment issues:
1. Check server logs
2. Review cloud platform documentation
3. Create GitHub issue with deployment details
