
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

 // Wi-Fi 이름 및 비밀번호 설정
const char* ssid = "ESP8266_SETUP";
const char* password = "01234567";

 // 웹 서버 설정
ESP8266WebServer server(80);
float temperature = 0.0;
float humidity = 0.0;

 // 웹페이지 접속 처리
void handleRoot() {
  File file = LittleFS.open("/index.html", "r");

  if (!file) {
    server.send(404, "text/plain", "index.html not found");
    return;
  }

  server.streamFile(file, "text/html; charset=utf-8");
  file.close();
}

// 온습도 데이터 전송
void handleTemperature() {
  String json = "{\"temperature\":" + String(temperature) +
                ",\"humidity\":" + String(humidity) + "}";

  server.send(200, "application/json", json); // JSON 형식으로 온습도 데이터 전송
}

// LED 밝기 제어 요청 처리
void handleLED() {
  if (server.hasArg("value")) { // LED 밝기 값이 있는 경우
    int brightness = server.arg("value").toInt(); // LED 밝기 값 추출
    brightness = constrain(brightness, 0, 255); // 밝기 범위 제한
    Serial.println(brightness); // Arduino로 LED 밝기값 UART 전송
    server.send(200, "text/plain", "LED 밝기 설정 완료"); // 성공 응답
  } else {
    server.send(400, "text/plain", "잘못된 요청"); // 실패 응답
  }
}

void setup() {

  Serial.begin(9600);
  LittleFS.begin(); // LittleFS 초기화

   // ESP8266 자체 Wi-Fi 생성
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

   // 웹페이지 주소 설정
  server.on("/", handleRoot); // 루트 경로 접속 시 handleRoot() 함수 호출
  server.on("/temperature", handleTemperature); // 온습도 데이터 전송 요청 처리
  server.on("/led", handleLED); // LED 밝기 제어 요청 처리

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

