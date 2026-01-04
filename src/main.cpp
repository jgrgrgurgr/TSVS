#include <OneWire.h>
#include <DallasTemperature.h>

// ------------------- 핀 설정 -------------------
const int oneWireBus = 4;
#define DUST_VALUE_PIN 35
#define DUST_LED_PIN 32

// ------------------- 설정 값 -------------------
const unsigned long PRINT_INTERVAL = 1000; // 출력 주기 (1초)
const int SAMPLE_COUNT = 10;               // 먼지 센서 평균 낼 샘플 수

// ------------------- 객체 생성 -------------------
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// ------------------- 변수 선언 -------------------
unsigned long lastPrintTime = 0;   // 마지막 출력 시간 저장
float lastTempC = 0.0;             // 마지막 측정된 온도값 저장

void setup() {
  Serial.begin(115200);

  sensors.begin();
  
  // [핵심 최적화 1] 온도 변환 대기 시간을 기다리지 않도록 설정 (비동기 모드)
  sensors.setWaitForConversion(false);
  
  // 첫 번째 온도 요청 (다음 루프에서 읽기 위함)
  sensors.requestTemperatures();

  pinMode(DUST_LED_PIN, OUTPUT);
}

void loop() {
  // ==========================================
  // 1. 먼지 센서 측정 (함수로 분리하여 깔끔하게 관리)
  // ==========================================
  // 여러 번 측정하여 평균값을 냅니다 (노이즈 감소)
  long dustSum = 0;
  for(int i = 0; i < SAMPLE_COUNT; i++) {
    dustSum += readDustSensor();
    delay(10); // 샘플링 간격 (짧게 대기)
  }
  int dustAvg = dustSum / SAMPLE_COUNT;

  // ==========================================
  // 2. 결과 출력 및 온도 갱신 (1초마다 실행)
  // ==========================================
  if (millis() - lastPrintTime >= PRINT_INTERVAL) {
    lastPrintTime = millis();

    // 2-1. 온도 값 읽기 (이미 요청해둔 값이 변환 완료되었을 시점)
    // DS18B20은 변환에 최대 750ms가 걸리는데, 
    // 우리는 1초마다 실행하므로 이미 완료되어 있습니다.
    float tempC = sensors.getTempCByIndex(0);
    
    // 에러 값(-127) 필터링: 센서 연결 불량 시 이전 값 유지
    if(tempC != DEVICE_DISCONNECTED_C) {
        lastTempC = tempC;
    }

    // 2-2. 다음 온도 변환 요청 (다음 1초 뒤에 읽을 예정)
    sensors.requestTemperatures();

    // 2-3. 시리얼 출력
    printResult(dustAvg, lastTempC);
  }
}

// 먼지 센서 단일 측정 함수
int readDustSensor() {
  digitalWrite(DUST_LED_PIN, LOW);     // LED ON
  delayMicroseconds(280);
  
  int value = analogRead(DUST_VALUE_PIN);
  
  delayMicroseconds(40);
  digitalWrite(DUST_LED_PIN, HIGH);    // LED OFF
  delayMicroseconds(9680);
  
  return value;
}

// 결과 출력 함수
void printResult(int dust, float temp) {
  Serial.print("Dust(Avg): ");
  Serial.print(dust);
  Serial.print("\t | \t");
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println(" ºC");
}