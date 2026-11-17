# UART 통신 프로토콜 (과제 요구사항, 미구현)

- 9600bps, 8N1, ASCII, LF(`\n`) 종료
- ESP → Arduino: `GET_SENSOR`, `LED:128`
- Arduino → ESP: `TEMP:25.0,HUM:60.0`, `LED_OK:128`
- 오류: `ERROR:INVALID_CMD`, `ERROR:INVALID_VALUE`, `ERROR:SENSOR_FAIL`

현재 Arduino 코드는 위 규격을 아직 구현하지 않았습니다.
