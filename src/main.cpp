#include <Arduino.h>
#include <DHT.h>

const int LED_PIN = 9;
const int DHT22_PIN = 2;
DHT dht(DHT22_PIN, DHT22);
unsigned long time_a = 0;
int LED_light = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
}

void loop() {

 // 수신 버퍼에 도착해 읽을 준비가 된 데이터의 바이트 수 반환
if (Serial.available() > 0) {
  if (Serial.peek() == '\n' || Serial.peek() == '\r') {
    Serial.read(); // 남아 있는 줄바꿈 문자 제거
  } else {
    LED_light = Serial.parseInt(); // 정수값 추출
    LED_light = constrain(LED_light, 0, 255); // 밝기 범위 제한
    analogWrite(LED_PIN, LED_light); // PWM 밝기 적용
    Serial.println(LED_light); // 적용된 밝기값 반환
  }
}

  // put your main code here, to run repeatedly:
  // 온도 : temp / 습도 : hum
  if (millis() - time_a >= 2000) {
    time_a = millis();

  // 섭씨 온도 측정 후 float 값 반환 
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("온도 측정 실패");
    return;
  }

  // 상대습도 측정 후 float 값 반환 
  float hum = dht.readHumidity();

  if (isnan(hum)) {
    Serial.println("습도 측정 실패");
    return;
  }

  // 결과 출력
  Serial.println(temp); // 온도 

  Serial.println(hum); // 습도 

  }
}
