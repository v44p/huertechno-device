#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Thing.h>
#include <WebThingAdapter.h>
#define DHTPIN 2    
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

int TIME_DELAY=500;
const char *ssid = "";
const char *password = "";

WebThingAdapter* adapter = NULL;

const char* deviceTypes[] = {"MultiLevelSensor", "Sensor", nullptr};
ThingDevice dht22t("dht22T", "temperature-sensor", deviceTypes);
ThingProperty dht22tLevel("level", "Analog Input pin", NUMBER, "LevelProperty");
ThingDevice dht22h("dht22H", "humidity-sensor", deviceTypes);
ThingProperty dht22hLevel("level", "Analog Input pin", NUMBER, "LevelProperty");

double lastValueDht22t = 0;
double lastValueDht22h = 0;
const int ledPin = 13;  // manully configure LED pin

void setup() {
  Serial.begin(9600);
  // setting wifi
  Serial.println(F("DHTxx test!"));
  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(ssid);
  Serial.println("\"");
  #if defined(ESP8266) || defined(ESP32)
    WiFi.mode(WIFI_STA);
  #endif
  WiFi.begin(ssid, password);
  Serial.println("");
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(ledPin, blink ? LOW : HIGH); // active low led
    blink = !blink;
  }
  digitalWrite(ledPin, HIGH); // active low led
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
// end wifi

  adapter = new WebThingAdapter("analog-sensor", WiFi.localIP());
  dht22t.addProperty(&dht22tLevel);
  adapter->addDevice(&dht22t);
  dht22h.addProperty(&dht22hLevel);
  adapter->addDevice(&dht22h);
  adapter->begin();

  Serial.println("HTTP server started");

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(dht22t.id);

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(dht22h.id);

  dht.begin(); 
}

void loop() {
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

    ThingPropertyValue levelDht22t;
    levelDht22t.number = t;
    dht22tLevel.setValue(levelDht22t);
    lastValueDht22t = t;
    
    Serial.print("log: Value dht22t: ");
    Serial.print(t);
    Serial.println("%");


    ThingPropertyValue levelDht22h;
    levelDht22h.number = h;
    dht22hLevel.setValue(levelDht22h);
    lastValueDht22h = h;
    
    Serial.print("log: Value dht22h: ");
    Serial.print(h);
    Serial.println("%");

    Serial.print("log: Value dht22t: ");
    Serial.print(t);
    Serial.println("%");

    adapter->update();
    delay(TIME_DELAY);
}