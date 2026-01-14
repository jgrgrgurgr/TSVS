#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DUST_VALUE_PIN 35  // 주석의 34번과 일치하는지 확인하세요
#define DUST_LED_PIN 32
#define ONE_WIRE_BUS 2

const int relayPin = 33;
int fanState = 0;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  pinMode(DUST_LED_PIN, OUTPUT);
  digitalWrite(DUST_LED_PIN, HIGH); // LED 초기 상태 OFF (GP2Y10은 LOW일 때 켜짐)
  sensors.begin();
  Serial.println("System Initialized...");
}

void loop() {
  // --- 1. 먼지 센서 측정 (타이밍이 매우 중요) ---
  digitalWrite(DUST_LED_PIN, LOW);    // LED ON
  delayMicroseconds(280);             // 샘플링 대기
  int dustValue = analogRead(DUST_VALUE_PIN); // 값 읽기
  delayMicroseconds(40);
  digitalWrite(DUST_LED_PIN, HIGH);   // LED OFF
  delayMicroseconds(9680);            // 나머지 시간 대기

  // --- 2. 온도 센서 측정 ---
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(0);

  // ---------------------------------
  if(dustValue >= 2000 && tempC >= 25){
    fanState = 0;
    digitalWrite(relayPin, LOW);
    Serial.println("팬 OFF");
  }else{
    fanState = 1;
    digitalWrite(relayPin, HIGH);
    Serial.println("팬 ON");
  }
  // ---------------------------------

  // --- 3. 시리얼 출력 (올바른 방식) ---
  Serial.print("DUST (Raw): ");
  Serial.print(dustValue);
  
  Serial.print(" | TEMP: ");
  if(tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor Error");
  } else {
    Serial.print(tempC);
    Serial.println(" °C");
  }

  delay(1000); // 전체 루프를 1초마다 반복
}