#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secret.h"

#define TIME_ZONE -5

#define TRIG_PIN_1 D7     // Digital pin connected to the ultrasonic sensor trigger
#define ECHO_PIN_1 D8   // Digital pin connected to the ultrasonic sensor echo
#define TRIG_PIN_2 D6     // Digital pin connected to the ultrasonic sensor trigger
#define ECHO_PIN_2 D5
#define TRIG_PIN_3 D1    // Digital pin connected to the ultrasonic sensor trigger
#define ECHO_PIN_3 D0
float distance1;  // Variable to store the distance measured by the ultrasonic sensor
float distance2;
float distance3;
unsigned long lastMillis = 0;
const long interval = 500;
const long delayBetweenReadings = 1000;  // Adjust this delay as needed

#define AWS_IOT_PUBLISH_TOPIC_1   "esp8266/pub/m1/dist"
#define AWS_IOT_PUBLISH_TOPIC_2   "esp8266/pub/m2/dist"
#define AWS_IOT_PUBLISH_TOPIC_3   "esp8266/pub/m3/dist"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266/sub"

WiFiClientSecure net;

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

PubSubClient client(net);

time_t now;
time_t nowish = 1510592825;

void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  NTPConnect();

  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage(float distance, const char *topic)
{
  struct tm *timeinfo;
  now = time(nullptr);
  timeinfo = gmtime(&now);

  char timestampBuffer[20];
  strftime(timestampBuffer, sizeof(timestampBuffer), "%Y-%m-%dT%H:%M:%SZ", timeinfo);

  StaticJsonDocument<200> doc;
  //doc["time"] = timestampBuffer;
  doc["distance"] = distance; // Add the distance to the JSON message
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  client.publish(topic, jsonBuffer);
}

void setup()
{
  Serial.begin(115200);
  connectAWS();
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
  pinMode(TRIG_PIN_3, OUTPUT);
  pinMode(ECHO_PIN_3, INPUT);
}

void loop()
{
  // Ultrasonic sensor reading for Sensor 1
  digitalWrite(TRIG_PIN_1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_1, LOW);
  distance1 = pulseIn(ECHO_PIN_1, HIGH) * 0.034 / 2;

  Serial.print(F("Distance 1: "));
  Serial.print(distance1);
  Serial.println(F(" cm"));

  delay(delayBetweenReadings);

  // Ultrasonic sensor reading for Sensor 2
  digitalWrite(TRIG_PIN_2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_2, LOW);
  distance2 = pulseIn(ECHO_PIN_2, HIGH) * 0.034 / 2;

  Serial.print(F("Distance 2: "));
  Serial.print(distance2);
  Serial.println(F(" cm"));

  delay(delayBetweenReadings);

  // Ultrasonic sensor reading for Sensor 3
  digitalWrite(TRIG_PIN_3, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_3, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_3, LOW);
  distance3 = pulseIn(ECHO_PIN_3, HIGH) * 0.034 / 2;

  Serial.print(F("Distance 3: "));
  Serial.print(distance3);
  Serial.println(F(" cm"));

  now = time(nullptr);

  if (!client.connected())
  {
    connectAWS();
  }
  else
  {
    client.loop();
    if (millis() - lastMillis > interval)
    {
      lastMillis = millis();
      publishMessage(distance1, AWS_IOT_PUBLISH_TOPIC_1);
      publishMessage(distance2, AWS_IOT_PUBLISH_TOPIC_2);
      publishMessage(distance3, AWS_IOT_PUBLISH_TOPIC_3);
    }
  }
}
