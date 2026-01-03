#include <OneWire.h>
#include <DallasTemperature.h>

// ------------------- 핀 설정 -------------------
// DS18B20 온도 센서 핀
const int oneWireBus = 4;

// 먼지 센서 핀
#define DUST_VALUE_PIN 35
#define DUST_LED_PIN 32

// ------------------- 객체 생성 -------------------
// OneWire 및 DallasTemperature 인스턴스 설정
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void setup() {
  // 시리얼 통신 시작
  Serial.begin(115200);

  // 온도 센서 시작
  sensors.begin();

  // 먼지 센서 LED 핀 설정
  pinMode(DUST_LED_PIN, OUTPUT);
}

void loop() {
  // ==========================================
  // 1. 먼지 센서 측정 (시간에 민감한 작업 우선)
  // ==========================================
  digitalWrite(DUST_LED_PIN, LOW);    // 적외선 LED ON
  delayMicroseconds(280);             // 280마이크로초 대기
  
  int dustValue = analogRead(DUST_VALUE_PIN); // 먼지 데이터 읽기
  
  delayMicroseconds(40);              // 대기
  digitalWrite(DUST_LED_PIN, HIGH);   // 적외선 LED OFF
  delayMicroseconds(9680);            // OFF TIME (펄스 주기 맞춤)

  // ==========================================
  // 2. 온도 센서 측정
  // ==========================================
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0); // 섭씨 온도 가져오기

  // ==========================================
  // 3. 결과 출력 (한 줄에 표시)
  // ==========================================
  
  // 먼지 센서 값 출력 (Raw ADC 값)
  Serial.print("Dust Raw: ");
  Serial.print(dustValue);
  
  // 탭으로 구분
  Serial.print("\t | \t"); 

  // 온도 값 출력
  Serial.print("Temp: ");
  Serial.print(temperatureC);
  Serial.println(" ºC");

  // ==========================================
  // 4. 대기 (너무 빠른 출력을 방지하기 위해 1초 대기)
  // ==========================================
  delay(1000); 
}