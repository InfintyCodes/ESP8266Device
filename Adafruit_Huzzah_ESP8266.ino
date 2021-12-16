#include <ESP8266WiFi.h>
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11

char *ssid = "dlink-95DB";
char *pass = "gwxyj36867";
char *conn = "HostName=IOTHUB2020.azure-devices.net;DeviceId=ESP8266;SharedAccessKey=TUTsp7fnS2E0EUGE1nG7o01e1bvITKnogXwhFvtReOQ=";

bool messagePending = false;
int interval = 1000;
unsigned long prevMillis = 0;


DHT dht(DHT_PIN, DHT_TYPE);
IOTHUB_CLIENT_LL_HANDLE deviceClient;

void setup() {
  initSerial();
  initWifi();
  initEpochTime();
  initDHT();
  initDevice();
}

time_t epochTime;
float PrevTemp;
float temperature;
float humidity;

void loop() {
  unsigned long currentMillis = millis();


  if (!messagePending) {
    if ((currentMillis - prevMillis) >= interval) {
      prevMillis = currentMillis;

      epochTime = time(NULL);
      Serial.printf("Current Time: %lu. ", epochTime);
      Serial.println("\n");

      temperature = dht.readTemperature();
      humidity = dht.readHumidity();

      if (!(std::isnan(temperature)) && !(std::isnan(humidity)))  {
        char payload[256];
        char epochTimeBuf[12];

        StaticJsonBuffer<sizeof(payload)> buf;
        JsonObject &root = buf.createObject();
        root["deviceId"] = WiFi.macAddress();
        root["temperature"] = temperature;
        root["humidity"] = humidity;
        root["epochTime"] = epochTime;
        root.printTo(payload, sizeof(payload));
        sendMessage(payload, itoa(epochTime, epochTimeBuf, 10));
        Serial.println("Message Sent...");
        delay(1000);
      }
    }
  }
  IoTHubClient_LL_DoWork(deviceClient);
  delay(1000);
}
