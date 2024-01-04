#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 256
 
#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM
#include <ArduinoHttpClient.h> //https://github.com/arduino-libraries/ArduinoHttpClient
#include <SoftwareSerial.h>
 
SoftwareSerial sim800(D7, D8);
 
const char FIREBASE_HOST[]  = "gps-tracker2-13e30-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH  = "zhw6Zkn3OQXjojhr8dDPn3OTJFdzhNk27c828puQ";
const String FIREBASE_PATH  = "/";
const int SSL_PORT          = 443;
 
char apn[]  = "airtelgprs.com";
char user[] = "";
char pass[] = "";
 
 
TinyGsm modem(sim800);
 
TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);
 
unsigned long previousMillis = 0;
 
 
void setup()
{
  Serial.begin(9600);
 
  sim800.begin(9600);
  Serial.println("SIM800L serial initialize");
 
  Serial.println("Initializing modem...");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
 
  http_client.setHttpResponseTimeout(10 * 1000); //^0 secs timeout
}
 
void loop()
{
 
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass))
  {
    Serial.println(" fail");
    delay(1000);
    return;
  }
  Serial.println(" OK");
 
  http_client.connect(FIREBASE_HOST, SSL_PORT);
 
  while (true) {
    if (!http_client.connected())
    {
      Serial.println();
      http_client.stop();// Shutdown
      Serial.println("HTTP  not connect");
      break;
    }
    else
    {
      dht_loop();
    }
 
  }
 
}
 
 
 
void PostToFirebase(const char* method, const String & path , const String & data, HttpClient* http)
{
  String response;
  int statusCode = 0;
  http->connectionKeepAlive(); // Currently, this is needed for HTTPS
 
  String url;
  if (path[0] != '/')
  {
    url = "/";
  }
  url += path + ".json";
  url += "?auth=" + FIREBASE_AUTH;
  Serial.print("POST:");
  Serial.println(url);
  Serial.print("Data:");
  Serial.println(data);
 
  String contentType = "application/json";
  http->put(url, contentType, data);
 
  statusCode = http->responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  response = http->responseBody();
  Serial.print("Response: ");
  Serial.println(response);
 
  if (!http->connected())
  {
    Serial.println();
    http->stop();// Shutdown
    Serial.println("HTTP POST disconnected");
  }
 
}
 
void dht_loop()
{
  String h = "1001";
  String t = "501";
  delay(100);
 
  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.println(" °C");
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.println(" %");
 
  String Data = "{";
  Data += "\"temp\":" + t + ",";
 
  Data += "\"humid\":" + h + "";
  Data += "}";
 
  PostToFirebase("PATCH", FIREBASE_PATH, Data, &http_client);
 
 
}
