// Using Software Serial for communicating with GPS and GSM modules ---------------------------
#include <SoftwareSerial.h>

// TinyGSM ------------------------------------------------------------------------------------
#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 256

#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM
#include <ArduinoHttpClient.h> //https://github.com/arduino-libraries/ArduinoHttpClient
 
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

// TinyGPS ------------------------------------------------------------------------------------
#include <TinyGPSPlus.h>

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial gps_ss(D3, D4);

TinyGPSCustom gmt_time(gps, "GPGGA", 1); // 
TinyGPSCustom gmt_date(gps, "GPRMC", 9); // 
TinyGPSCustom gps_lat(gps, "GPRMC", 3); // 
TinyGPSCustom gps_long(gps, "GPRMC", 5); // 
TinyGPSCustom snr(gps, "GPGSV", 7); // $GPGSV sentence, 7th element
TinyGPSCustom fix_status(gps, "GPGGA",6); // $GPGGA sentence, 6th element

// -----------------------------------------------------------------------------------------
  
void setup()
{
  Serial.begin(9600); // baud rate for displaying on monitor
  gps_ss.begin(9600); // baud rate for GPS module - Arduino comm
  sim800.begin(9600); // baud rate for GSM module - Arduino comm
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

  // Run this loop and start transferring GPS info to Firebase DB 
  // when http_client gets connected
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
      create_data();
    }
  }
}
 
 
void PostToFirebase(const String & path , const String & data, HttpClient* http)
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

  // url = "/.json?auth=zhw6Zkn3OQXjojhr8dDPn3OTJFdzhNk27c828puQ"
  
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
 
int count = 0;
String cur_date, cur_time, lat_data, long_data, SNR_data, fix_data;
void create_data()
{
  smartDelay(1000);
  Serial.println("Fix status : " + String(fix_status.value()));
  fix_data = String(fix_status.value())!="" ? String(fix_status.value()):"No GPS fix";
  cur_date = String(gmt_date.value())!="" ? String(gmt_date.value()):"0";
  cur_time = String(gmt_time.value())!="" ? String(gmt_time.value()).substring(0,6):"0";
  lat_data = String(gps_lat.value())!="" ? String(gps_lat.value()):"0";
  long_data = String(gps_long.value())!="" ? String(gps_long.value()):"0";
  SNR_data = String(snr.value())!="" ? String(snr.value()):"0";
  delay(100);
 
  Serial.print("Lat = " + String(lat_data));
  Serial.print("Long = " + String(long_data));
  Serial.print("SNR = " + String(SNR_data));
 
  String Data = "{";
  Data += "\"Lat\":" + String("\"") + lat_data + String("\"") + ",";
  Data += "\"Long\":" + String("\"") + long_data + String("\"") + ",";
  Data += "\"SNR\":" + String("\"") + SNR_data + String("\"") + "";
  Data += "}";

  // Data = "{\"Lat\":10,\"Long\":50,\"SNR\":25}"
  // Data in JSON format --> {"Lat":10,"Long":50,"SNR":25}

  count = count+1;
  String FIREBASE_PATH = "/" + cur_date + "/" + cur_time + "/";
  PostToFirebase(FIREBASE_PATH, Data, &http_client);
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gps_ss.available())
      gps.encode(gps_ss.read());
  } while (millis() - start < ms);
}
