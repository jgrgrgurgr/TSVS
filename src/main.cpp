/*
 * GP2Y1014AU0F 먼지 센서 with ESP32
 * 
 * ⚠️ 중요: 센서는 반드시 5V 전원 필요!
 * 
 * 연결:
 * - Vcc (핀 6) -> ESP32 5V (VIN 핀)
 * - GND (핀 1, 2) -> ESP32 GND
 * - LED (핀 3) -> ESP32 GPIO 25
 * - Vo (핀 5) -> ESP32 GPIO 34
 * 
 * 센서 핀 배치 (정면에서 볼 때):
 * 1: GND
 * 2: GND  
 * 3: LED (제어 핀)
 * 4: NC
 * 5: Vo (아날로그 출력)
 * 6: Vcc (5V 필수!)
 */

#include <Arduino.h>

#define DUST_VALUE_PIN 35
#define DUST_LED_PIN 32

void setup()
{
  Serial.begin(115200); // 통신 속도 115200bps로 시리얼 통신 시작
  pinMode(DUST_LED_PIN, OUTPUT); // 적외선 led 출력으로 설정
}

void loop()
{
  digitalWrite(DUST_LED_PIN, LOW); // 적외선 LED ON
  delayMicroseconds(280); // 280마이크로초 대기
  int value = analogRead(DUST_VALUE_PIN); // 데이터를 읽음
  delayMicroseconds(40); // 대기
  digitalWrite(DUST_LED_PIN, HIGH); // 적외선 LED OFF
  delayMicroseconds(9680); // OFF TIME
  Serial.println(value);
  delay(100);
}
