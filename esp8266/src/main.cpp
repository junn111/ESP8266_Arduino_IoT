#include <Arduino.h> // Arduino 라이브러리 포함
#include <ESP8266WiFi.h> // ESP8266 Wi-Fi 라이브러리 포함 
#include <ESP8266WebServer.h> // ESP8266 웹 서버 라이브러리 포함
#include <LittleFS.h> // LittleFS 파일 시스템 라이브러리 포함
#include <ArduinoJson.h> // ArduinoJson 라이브러리 포함
#include <EEPROM.h> // EEPROM 라이브러리 포함

// 웹 서버 설정
ESP8266WebServer server(80);

// 온습도 데이터 저장
float temperature = 0.0;
float humidity = 0.0;

// AP 이름 비밀번호 설정
const char* ssid = "ESP8266_TEST";
const char* password = "01234567";

 // 연결 정보 저장
String sta_password = "";
String sta_ssid = "";

bool wifiConfigChanged = false; // Wi-Fi 설정 변경 여부 확인 변수
unsigned long lastWiFiAttempt = 0; // 마지막 Wi-Fi 연결 시도 시간 저장

 // Wi-Fi 설정 
void handleWiFi()
{
  File file = LittleFS.open("/wifi.html", "r"); // LittleFS에서 Wi-Fi 설정 페이지 파일 열기

   // 파일이 존재하지 않으면 404 오류 응답 전송
  if (!file) {
    server.send(404, "text/plain; charset=utf-8", "Wi-Fi 설정 페이지를 찾을 수 없습니다.");
    return;
  }

  server.streamFile(file, "text/html; charset=utf-8"); // Wi-Fi 설정 페이지 전송
  file.close();
}

// Wi-Fi 연결 상태 확인
bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
  
}

 // Wi-Fi 연결 상태 확인 및 JSON 형식으로 응답 전송
void handleWiFiStatus() {

  String wifiStatus = "연결 되지 않음";
  String ipAddress = "N/A";
  String WiFiSSID = "N/A";

   // Wi-Fi 연결 상태 확인
  if (isWiFiConnected()) {
    wifiStatus = "연결됨";
    ipAddress = WiFi.localIP().toString();
    WiFiSSID = WiFi.SSID();

  }

   // JSON 형식으로 Wi-Fi 상태 정보 생성
  String json = "{";
  json += "\"status\": \"" + wifiStatus + "\",";
  json += "\"ip\": \"" + ipAddress + "\",";
  json += "\"ssid\": \"" + WiFiSSID + "\"";
  json += "}";

  server.send(200, "application/json; charset=utf-8", json); // Wi-Fi 상태 응답 전송

}

 // 웹페이지에서 전송한 SSID 및 비밀번호 수신 및 처리
void handleWiFiSettings() {
  String json = server.arg("plain"); // JSON 형식의 Wi-Fi 설정 데이터 수신
  JsonDocument doc; // ArduinoJson 라이브러리의 JsonDocument 객체 생성
  DeserializationError error = deserializeJson(doc, json); // 수신된 JSON 데이터 파싱

   // JSON 파싱 오류 처리
  if (error) {
    server.send(400, "application/json; charset=utf-8", "{\"message\": \"잘못된 JSON 형식입니다.\"}");
    return;
  }

   // SSID가 비어 있는 경우 처리
  if (doc["ssid"].as<String>().isEmpty()) {
    server.send(400, "application/json; charset=utf-8", "{\"message\": \"SSID가 비어 있습니다.\"}");
    return; 
  }

   // SSID 및 비밀번호 추출
  sta_ssid = doc["ssid"].as<String>();
  sta_password = doc["password"].as<String>();

  wifiConfigChanged = true; // Wi-Fi 설정 변경 여부 표시

  server.send(200, "application/json; charset=utf-8", "{\"message\": \"Wi-Fi 설정 정보가 접수되었습니다.\"}");

}

void setup() {

  Serial.begin(9600);
  LittleFS.begin(); // LittleFS 초기화

   // ESP8266 자체 Wi-Fi 생성
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password); // Wi-Fi AP 모드 활성화 및 SSID, 비밀번호 설정
/*
   // 웹페이지 주소 설정
  server.on("/", handleRoot); // 루트 경로 접속 시 handleRoot() 함수 호출
  server.on("/temperature", handleTemperature); // 온습도 데이터 전송 요청 처리
  server.on("/led", handleLED); // LED 밝기 제어 요청 처리  

     Wi-Fi 연결 및 자동 재연결 기능 검증을 위한 임시 주석 처리
   */
  server.on("/wifi", handleWiFi); // Wi-Fi 설정 페이지 접속 처리
  server.on("/wifi_status", handleWiFiStatus); // Wi-Fi 연결 상태 확인 요청 처리

  server.on("/api/wifi", HTTP_POST, handleWiFiSettings); // Wi-Fi 설정 정보 수신 처리

   // 웹 서버 시작
  server.begin();
  Serial.println("Web server started"); // 웹 서버 시작 메시지 출력
}

void loop() {

  // 웹 서버 요청 처리
  server.handleClient();

   // Wi-Fi 설정 변경 시 처리
  if (wifiConfigChanged == true) {
    WiFi.disconnect(); // 기존 Wi-Fi 연결 해제
    WiFi.begin(sta_ssid.c_str(), sta_password.c_str()); // 새로운 Wi-Fi 연결 시도
    wifiConfigChanged = false; // 설정 변경 여부 초기화
    lastWiFiAttempt = millis(); // 마지막 연결 시도 시간 갱신

  }

   // 마지막 연결 시도 후 10초 경과 및 Wi-Fi 미연결 시 재연결
  if (millis()- lastWiFiAttempt >= 10000 && !isWiFiConnected() && !sta_ssid.isEmpty()) {
    WiFi.begin(sta_ssid.c_str(), sta_password.c_str()); // Wi-Fi 재연결 시도

    lastWiFiAttempt = millis(); // 마지막 연결 시도 시간 갱신
  }
  

  // Arduino에서 전송한 DHT22 온습도 데이터 수신
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n'); // 수신된 데이터 읽기
    data.trim();

    if (data.startsWith("TEMP:")) {
      temperature = data.substring(5).toFloat(); // 온도 값 추출
    }
    else if (data.startsWith("HUM:")) {
      humidity = data.substring(4).toFloat(); // 습도 값 추출
    }
  }
}


