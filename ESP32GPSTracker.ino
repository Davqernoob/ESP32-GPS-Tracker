#include <WiFi.h>
#include <WebServer.h>
#include <TinyGPSPlus.h>

const char* ssid = "ESP32_GPS_Tracker";
const char* password = ""; 

#define RXD2 16
#define TXD2 17

WebServer server(80);
TinyGPSPlus gps;

String rawData = "";

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>GPS Telemetry</title>
    <style>
        body {
            background-color: #0d1117;
            color: #58a6ff;
            font-family: 'Courier New', Courier, monospace;
            margin: 0;
            padding: 20px;
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        h1 { color: #c9d1d9; }
        .dashboard {
            background-color: #161b22;
            border: 1px solid #30363d;
            border-radius: 8px;
            padding: 20px;
            width: 100%;
            max-width: 600px;
            margin-bottom: 20px;
            box-shadow: 0 4px 10px rgba(0,0,0,0.5);
        }
        .data-row {
            display: flex;
            justify-content: space-between;
            font-size: 1.2em;
            margin: 10px 0;
            border-bottom: 1px dashed #30363d;
            padding-bottom: 5px;
        }
        .label { color: #8b949e; }
        .value { font-weight: bold; color: #7ee787; }
        
        .raw-data-container {
            background-color: #010409;
            border: 1px solid #30363d;
            border-radius: 8px;
            padding: 15px;
            width: 100%;
            max-width: 600px;
            height: 200px;
            overflow-y: auto;
            color: #8b949e;
            font-size: 0.9em;
            white-space: pre-wrap;
        }
    </style>
</head>
<body>

    <h1>🛰️ ESP32 GPS Tracker</h1>
    
    <div class="dashboard">
        <div class="data-row"><span class="label">Широта:</span> <span class="value" id="lat">Поиск спутников...</span></div>
        <div class="data-row"><span class="label">Долгота:</span> <span class="value" id="lon">Поиск спутников...</span></div>
        <div class="data-row"><span class="label">Высота (м):</span> <span class="value" id="alt">0.0</span></div>
        <div class="data-row"><span class="label">Спутники:</span> <span class="value" id="sats">0</span></div>
        <div class="data-row"><span class="label">Время (UTC):</span> <span class="value" id="time">--:--:--</span></div>
    </div>

    <h3 style="color: #c9d1d9;">Raw data (NMEA)</h3>
    <div class="raw-data-container" id="raw-log">
        Waiting for data from the module NEO-6M...
    </div>

    <script>
        setInterval(function() {
            fetch('/data')
            .then(response => response.json())
            .then(data => {
                if(data.valid) {
                    document.getElementById('lat').innerText = data.lat;
                    document.getElementById('lon').innerText = data.lon;
                    document.getElementById('alt').innerText = data.alt;
                    document.getElementById('sats').innerText = data.sats;
                    document.getElementById('time').innerText = data.time;
                }
                
                if(data.raw.length > 0) {
                    let rawBox = document.getElementById('raw-log');
                    rawBox.innerText = data.raw;
                    rawBox.scrollTop = rawBox.scrollHeight;
                }
            });
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Serial.println("Setting up a Wi-Fi hotspot...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access point created! IP address: ");
  Serial.println(IP); 

  server.on("/", []() {
    server.send(200, "text/html", htmlPage);
  });

  server.on("/data", []() {
    String json = "{";
    json += "\"valid\":" + String(gps.location.isValid() ? "true" : "false") + ",";
    json += "\"lat\":\"" + String(gps.location.lat(), 6) + "\",";
    json += "\"lon\":\"" + String(gps.location.lng(), 6) + "\",";
    json += "\"alt\":\"" + String(gps.altitude.meters(), 1) + "\",";
    json += "\"sats\":\"" + String(gps.satellites.value()) + "\",";
    
    char timeStr[10];
    sprintf(timeStr, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    json += "\"time\":\"" + String(timeStr) + "\",";
    
    String safeRaw = rawData;
    safeRaw.replace("\n", "\\n");
    safeRaw.replace("\r", "\\r");
    json += "\"raw\":\"" + safeRaw + "\"";
    json += "}";
    
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("The web server is running!");
}

void loop() {
  server.handleClient(); 

  while (Serial2.available() > 0) {
    char c = Serial2.read();
    gps.encode(c); 
    
    rawData += c;
  }

  if (rawData.length() > 800) {
    rawData = rawData.substring(rawData.length() - 800);
  }
}