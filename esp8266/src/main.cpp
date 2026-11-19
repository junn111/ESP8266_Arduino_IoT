
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Wi-Fi 이름 및 비밀번호 설정
const char* ssid = "ESP8266_TEST";
const char* password = "********";

// 웹 서버 설정
ESP8266WebServer server(80);

// 웹페이지 접속 처리
void handleRoot() {
  server.send(200, "text/html; charset=utf-8",
              "<h1>ESP8266 Wi-Fi 통신 성공</h1>"
              "<p>웹페이지 접속 확인</p>");
}

void setup() {
  Serial.begin(9600);

  // ESP8266 자체 Wi-Fi 생성
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  // 웹페이지 주소 설정
  server.on("/", handleRoot);

  // 웹 서버 시작
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // 웹페이지 접속 요청 처리
  server.handleClient();
}
