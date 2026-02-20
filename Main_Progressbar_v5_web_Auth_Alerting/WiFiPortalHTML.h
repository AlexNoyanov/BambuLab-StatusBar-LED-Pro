#ifndef WIFI_PORTAL_HTML_H
#define WIFI_PORTAL_HTML_H

#include "config.h"

String getConfigPageHTML() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>StatusBarPro WiFi Setup</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    .container {
      background: white;
      border-radius: 20px;
      box-shadow: 0 20px 60px rgba(0,0,0,0.3);
      padding: 40px;
      max-width: 450px;
      width: 100%;
    }
    h1 {
      color: #333;
      margin-bottom: 10px;
      font-size: 28px;
      text-align: center;
    }
    .subtitle {
      color: #666;
      text-align: center;
      margin-bottom: 30px;
      font-size: 14px;
    }
    .form-group {
      margin-bottom: 20px;
    }
    label {
      display: block;
      color: #333;
      margin-bottom: 8px;
      font-weight: 500;
      font-size: 14px;
    }
    input {
      width: 100%;
      padding: 12px 16px;
      border: 2px solid #e0e0e0;
      border-radius: 10px;
      font-size: 16px;
      transition: border-color 0.3s;
    }
    input:focus {
      outline: none;
      border-color: #667eea;
    }
    button {
      width: 100%;
      padding: 14px;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      color: white;
      border: none;
      border-radius: 10px;
      font-size: 16px;
      font-weight: 600;
      cursor: pointer;
      transition: transform 0.2s, box-shadow 0.2s;
      margin-top: 10px;
    }
    button:hover {
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
    }
    button:active {
      transform: translateY(0);
    }
    .status {
      margin-top: 20px;
      padding: 12px;
      border-radius: 8px;
      text-align: center;
      font-size: 14px;
      display: none;
    }
    .status.success {
      background: #d4edda;
      color: #155724;
      border: 1px solid #c3e6cb;
    }
    .status.error {
      background: #f8d7da;
      color: #721c24;
      border: 1px solid #f5c6cb;
    }
    .info {
      background: #e7f3ff;
      border: 1px solid #b3d9ff;
      border-radius: 8px;
      padding: 12px;
      margin-bottom: 20px;
      font-size: 13px;
      color: #004085;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🔧 WiFi Setup</h1>
    <p class="subtitle">Configure your StatusBarPro device</p>
    
    <div class="info">
      <strong>Connect to:</strong> )";
  html += String(AP_SSID);
  html += R"(<br>
      <strong>Password:</strong> )";
  html += String(AP_PASSWORD);
  html += R"(
    </div>

    <form id="wifiForm">
      <div class="form-group">
        <label for="ssid">WiFi Network Name (SSID)</label>
        <input type="text" id="ssid" name="ssid" required autocomplete="off">
      </div>
      
      <div class="form-group">
        <label for="password">WiFi Password</label>
        <input type="password" id="password" name="password" autocomplete="off">
      </div>
      
      <button type="submit">Connect & Save</button>
    </form>
    
    <div id="status" class="status"></div>
  </div>

  <script>
    document.getElementById('wifiForm').addEventListener('submit', async function(e) {
      e.preventDefault();
      const ssid = document.getElementById('ssid').value;
      const password = document.getElementById('password').value;
      const statusDiv = document.getElementById('status');
      
      statusDiv.style.display = 'block';
      statusDiv.className = 'status';
      statusDiv.textContent = 'Connecting...';
      
      try {
        const response = await fetch('/save', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
          body: 'ssid=' + encodeURIComponent(ssid) + '&password=' + encodeURIComponent(password)
        });
        
        const text = await response.text();
        if (response.ok) {
          statusDiv.className = 'status success';
          statusDiv.textContent = '✓ WiFi configured! Device will restart and connect...';
          setTimeout(() => {
            statusDiv.textContent = 'Please wait while device connects to your network...';
          }, 2000);
        } else {
          throw new Error(text);
        }
      } catch (error) {
        statusDiv.className = 'status error';
        statusDiv.textContent = '✗ Error: ' + error.message;
      }
    });
  </script>
</body>
</html>
)";
  return html;
}

#endif // WIFI_PORTAL_HTML_H
