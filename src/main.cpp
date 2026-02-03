#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>

const char* ssid = "";
const char* password = "";

#define DUST_VALUE_PIN 35
#define DUST_LED_PIN 32
#define ONE_WIRE_BUS 13

const int relayPin = 33;
int fanState = 0;

WiFiServer server(80);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  pinMode(DUST_LED_PIN, OUTPUT);
  digitalWrite(DUST_LED_PIN, HIGH);
  sensors.begin();

  Serial.println();
  Serial.println("Connecting to " + String(ssid));

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  // --- 1. 먼지 센서 측정 ---
  digitalWrite(DUST_LED_PIN, LOW);
  delayMicroseconds(280);
  int dustValue = analogRead(DUST_VALUE_PIN);
  delayMicroseconds(40);
  digitalWrite(DUST_LED_PIN, HIGH);
  delayMicroseconds(9680);

  // --- 2. 온도 센서 측정 ---
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(0);

  // --- 3. 팬 제어 로직 (조건에 따라 수정 가능) ---
  if(dustValue >= 2000 && tempC >= 25){
    fanState = 0;
    digitalWrite(relayPin, LOW);
  } else {
    fanState = 1;
    digitalWrite(relayPin, HIGH);
  }

  // --- 4. 웹 서버 로직 (HTML 출력) ---
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Request.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // HTTP 응답 헤더
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=UTF-8");
            client.println("Connection: close");
            client.println("Refresh: 5");
            client.println();
            
            client.println("<!DOCTYPE HTML><html><head>");
            client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
            client.println("<style>body { font-family: sans-serif; text-align: center; }");
            client.println(".data { font-size: 24px; padding: 10px; border: 1px solid #ccc; display: inline-block; margin: 10px; }</style>");
            client.println("</head><body>");
            
            client.println("<h1>ESP32 센서 모니터링</h1>");
            
            // 먼지 데이터 표시
            client.print("<div class='data'><b>먼지 수치(Raw): </b>");
            client.print(dustValue);
            client.println("</div><br>");
            
            // 온도 데이터 표시
            client.print("<div class='data'><b>현재 온도: </b>");
            if(tempC == DEVICE_DISCONNECTED_C) client.print("연결 오류");
            else { client.print(tempC); client.print(" °C"); }
            client.println("</div><br>");
            
            // 팬 상태 표시
            client.print("<div class='data'><b>팬 상태: </b>");
            client.print(fanState ? "<span style='color:red'>OFF</span>" : "<span style='color:green'>ON</span>");
            client.println("</div>");
            
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
  
  // 시리얼 모니터 출력
  Serial.printf("Dust: %d | Temp: %.2f | Fan: %s\n", dustValue, tempC, fanState ? "OFF" : "ON");
  
  delay(100); // 웹 서버 응답성을 위해 delay를 줄임
}