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

// 핀 정의
const int LED_PIN = 25;        // LED 제어 핀
const int DUST_SENSOR_PIN = 34; // 아날로그 입력 핀 (ADC1)

// 타이밍 상수 (마이크로초)
const int SAMPLING_TIME = 280;
const int DELTA_TIME = 40;
const int SLEEP_TIME = 9680;

// 전압-먼지농도 변환 상수
const float V_REF = 3.3;        // ESP32 ADC 기준 전압
const int ADC_RESOLUTION = 4095; // 12-bit ADC

// 필터링
const int NUM_SAMPLES = 10;
float dustDensity = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  // ADC 설정
  analogReadResolution(12); // 12-bit 해상도
  analogSetAttenuation(ADC_11db); // 0-3.3V 범위
  
  Serial.println("GP2Y1014AU0F 먼지 센서 초기화 완료");
  Serial.println("온도: 약 10초 후 측정 시작...");
  delay(10000); // 센서 안정화 대기
}

void loop() {
  float voMeasured = 0;
  int minADC = 4095;
  int maxADC = 0;
  
  // 여러 번 샘플링하여 평균 계산
  for(int i = 0; i < NUM_SAMPLES; i++) {
    // LED 켜기 (HIGH로 시도 - 센서에 따라 다를 수 있음)
    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(SAMPLING_TIME);
    
    // 아날로그 값 읽기
    int adcValue = analogRead(DUST_SENSOR_PIN);
    
    // 디버깅용: 최소/최대값 추적
    if(adcValue < minADC) minADC = adcValue;
    if(adcValue > maxADC) maxADC = adcValue;
    
    delayMicroseconds(DELTA_TIME);
    
    // LED 끄기
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(SLEEP_TIME);
    
    // ADC 값을 전압으로 변환
    float voltage = adcValue * (V_REF / ADC_RESOLUTION);
    voMeasured += voltage;
    
    delay(10);
  }
  
  // 평균 전압 계산
  voMeasured = voMeasured / NUM_SAMPLES;
  
  // 전압 분배기 사용 시 아래 줄의 주석을 해제하세요
  // voMeasured = voMeasured * 2.0;
  
  // 전압을 먼지 농도로 변환 (mg/m³)
  // 공식: Dust Density (mg/m³) = 0.17 * Vo - 0.1
  float calcVoltage = voMeasured;
  
  if(calcVoltage >= 0.6) {
    dustDensity = 0.17 * calcVoltage - 0.1;
  } else {
    dustDensity = 0;
  }
  
  // 음수 방지
  if(dustDensity < 0) {
    dustDensity = 0;
  }
  
  // 결과 출력
  Serial.println("=========================");
  Serial.print("RAW ADC 최소값: ");
  Serial.println(minADC);
  Serial.print("RAW ADC 최대값: ");
  Serial.println(maxADC);
  Serial.print("RAW ADC 범위: ");
  Serial.println(maxADC - minADC);
  
  Serial.print("ADC 전압: ");
  Serial.print(voMeasured, 3);
  Serial.println(" V");
  
  Serial.print("먼지 농도: ");
  Serial.print(dustDensity, 2);
  Serial.println(" mg/m³");
  
  // 공기질 판단
  if(dustDensity < 0.05) {
    Serial.println("공기질: 매우 좋음");
  } else if(dustDensity < 0.15) {
    Serial.println("공기질: 좋음");
  } else if(dustDensity < 0.30) {
    Serial.println("공기질: 보통");
  } else if(dustDensity < 0.50) {
    Serial.println("공기질: 나쁨");
  } else {
    Serial.println("공기질: 매우 나쁨");
  }
  
  delay(1000); // 1초마다 측정
}