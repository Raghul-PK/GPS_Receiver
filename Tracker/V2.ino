// Using Software Serial for communicating with GPS and GSM modules ---------------------------
#include <SoftwareSerial.h>

// TinyGSM ------------------------------------------------------------------------------------
#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 256

#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM
#include <ArduinoHttpClient.h> //https://github.com/arduino-libraries/ArduinoHttpClient
 
SoftwareSerial sim800(4, 5); // (13, 15)
 
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
SoftwareSerial gps_ss(13, 12); // (0, 2)

TinyGPSCustom gmt_time(gps, "GNGGA", 1); // 
TinyGPSCustom gmt_date(gps, "GNRMC", 9); // 
TinyGPSCustom gps_lat(gps, "GNRMC", 3); // 
TinyGPSCustom gps_long(gps, "GNRMC", 5); // 
TinyGPSCustom snr(gps, "GPGSV", 7); // $GPGSV sentence, 7th element
TinyGPSCustom fix_status(gps, "GNGGA",6); // $GPGGA sentence, 6th element
TinyGPSCustom gps_speed(gps, "GNRMC", 7); // 

// -----------------------------------------------------------------------------------------
void hw_wdt_disable(){
  *((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
}

void hw_wdt_enable(){
  *((volatile uint32_t*) 0x60000900) |= 1; // Hardware WDT ON
}

int timeoutSuccessUpdateInSecs = 100;
unsigned long lastSuccessUpdate = 0;

void setup()
{  
  ESP.wdtDisable();
  hw_wdt_disable();
  
  Serial.begin(9600); // baud rate for displaying on monitor
  gps_ss.begin(9600); // baud rate for GPS module - Arduino comm
  sim800.begin(9600); // baud rate for GSM module - Arduino comm

  Serial.print("ESP Reset : ");
  Serial.println(ESP.getResetReason().c_str());
  
  Serial.println("Starting SIM800L modem");
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

  while (true)
  {
    Serial.println("Time passed since start = " + String(millis()/1000) + " secs");
    Serial.println("Connecting to DB ");
    http_client.connect(FIREBASE_HOST, SSL_PORT);

    get_data();

    http_client.stop();
  }
}
 
String cur_date, cur_time, lat_data, long_data, SNR_data, fix_data, speed_data;
void get_data()
{
  getGPSData(1000);
  Serial.println("Fix status : " + String(fix_status.value()));
  fix_data = String(fix_status.value())!="" ? String(fix_status.value()):"No GPS fix";
  cur_date = String(gmt_date.value())!="" ? String(gmt_date.value()):"0";
  cur_time = String(gmt_time.value())!="" ? String(gmt_time.value()).substring(0,6):"0";
  lat_data = String(gps_lat.value())!="" ? String(gps_lat.value()):"0";
  long_data = String(gps_long.value())!="" ? String(gps_long.value()):"0";
  SNR_data = String(snr.value())!="" ? String(snr.value()):"0";
  speed_data = String(gps_speed.value())!="" ? String(gps_speed.value()):"0";

  // vehicle speed
  float vehicle_speed = speed_data.toFloat();
 
  Serial.println("Lat = " + String(lat_data) + ", Long = " + String(long_data) + 
                ", SNR = " + String(SNR_data) + ", Speed = " + String(speed_data));
 
  String Data = "{";
  Data += "\"Lat\":" + String("\"") + lat_data + String("\"") + ",";
  Data += "\"Long\":" + String("\"") + long_data + String("\"") + ",";
  Data += "\"SNR\":" + String("\"") + SNR_data + String("\"") + "";
  Data += "}";
  
  String FIREBASE_PATH = "/" + cur_date + "/" + cur_time + "/";
  Serial.println("Posting to DB ");
  int timeGapInSecs = 10;
  PostToFirebase(FIREBASE_PATH, Data, &http_client, timeGapInSecs);
}

// Taken from "TinyGSM" examples
// This custom version of delay() ensures that the gps object is being "fed"
// with enough GPS data --> called as "smartDelay()"
static void getGPSData(unsigned long ms)
{
  Serial.println("Reading GPS data : ");
  unsigned long start = millis();
  do 
  {
    while (gps_ss.available())
    {
      byte gpsData = gps_ss.read();
      gps.encode(gpsData);
      Serial.write(gpsData);
    }
  } while (millis() - start < ms);
}

void PostToFirebase(const String & path , const String & data, HttpClient* http, int timeGap)
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
  Serial.println("HTTP Put request");
  http->put(url, contentType, data);

  Serial.println("Response");
  statusCode = http->responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.println("Time passed since start = " + String(millis()/1000) + " secs");
  Serial.println();

  // If we are not updating DB for a long time --> Restart the ESP
  if (statusCode==200)
  {
    lastSuccessUpdate = millis();
  }
  else
  {
    if (millis() - lastSuccessUpdate > timeoutSuccessUpdateInSecs*1000)
    {
      Serial.println("ESP not updating to DB for a long time --> " + String((millis() - lastSuccessUpdate)/1000));
      Serial.println("Restarting the board");
      ESP.restart();
    }
  }

  delay(timeGap*1000);
}