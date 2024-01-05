// Ref : https://github.com/mikalhart/TinyGPSPlus/blob/master/examples/UsingCustomFields/UsingCustomFields.ino

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial gps_ss(D3, D4);

// Refer NMEA-0183 Docs for parsing terms from NMEA sentences : https://www.tronico.fi/OH6NT/docs/NMEA0183.pdf
TinyGPSCustom gmt_time(gps, "GPGGA", 1); // $GPGGAV entence, 1st element
TinyGPSCustom gmt_date(gps, "GPRMC", 9); // 
TinyGPSCustom gps_lat(gps, "GPRMC", 3); // 
TinyGPSCustom gps_long(gps, "GPRMC", 5); // 
TinyGPSCustom snr(gps, "GPGSV", 7); // $GPGSV sentence, 7th element

void setup() 
{
  Serial.begin(9600); // baud rate for displaying on monitor
  gps_ss.begin(9600); // baud rate for GPS module - Arduino comm
}

void loop() 
{
  Serial.println("Date : " + String(gmt_date.value()));
  Serial.println("Time : " + String(gmt_time.value()));
  Serial.println("SNR : " + String(snr.value()));
  Serial.println("Lat : " + String(gps_lat.value())); // Extracting Lat, Long from NMEA string
  Serial.println("Long : " + String(gps_long.value()));
  Serial.println("Lat : " + String(gps.location.lat())); // Inbuilt methods for Lat, Long extraction
  Serial.println("Long : " + String(gps.location.lng()));
  Serial.println();
  smartDelay(1000);
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
