
void handleWiFi()
{
  File file = LittleFS.open("/wifi.html", "r");

  if (!file) {
    server.send(404, "text/plain; charset=utf-8", "Wi-Fi 설정 페이지를 찾을 수 없습니다.");
    return;
  }

  server.streamFile(file, "text/html; charset=utf-8");
  file.close();
}

// Wi-Fi 연결 상태 확인
bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
  
}

void handleWiFiStatus() {
  String wifiStatus = "연결 되지 않음";
  String ipAddress = "N/A";
  String WiFiSSID = "N/A";

  if (isWiFiConnected()) {
    wifiStatus = "연결됨";
    ipAddress = WiFi.localIP().toString();
    WiFiSSID = WiFi.SSID();
  }
  String json = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + ",\"wifiStatus\":\"" + wifiStatus + "\",\"ipAddress\":\"" + ipAddress + "\",\"WiFiSSID\":\"" + WiFiSSID + "\"}";
  server.send(200, "application/json; charset=utf-8", json);
}

void setup() {

  Serial.begin(9600);
  LittleFS.begin(); // LittleFS 초기화

   // ESP8266 자체 Wi-Fi 생성
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);

   // 웹페이지 주소 설정
  server.on("/", handleRoot); // 루트 경로 접속 시 handleRoot() 함수 호출
  server.on("/temperature", handleTemperature); // 온습도 데이터 전송 요청 처리
  server.on("/led", handleLED); // LED 밝기 제어 요청 처리
  server.on("/wifi", handleWiFi); // Wi-Fi 설정 페이지 접속 처리

   // 웹 서버 시작
  server.begin();
  Serial.println("Web server started"); // 웹 서버 시작 메시지 출력
}

void loop() {

  // 웹 서버 요청 처리
  server.handleClient();

  // Arduino에서 전송한 DHT22 온습도 데이터 수신
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    data.trim();

    if (data.startsWith("TEMP:")) {
      temperature = data.substring(5).toFloat(); // 온도 값 추출
    }
    else if (data.startsWith("HUM:")) {
      humidity = data.substring(4).toFloat(); // 습도 값 추출
    }
  }

}

