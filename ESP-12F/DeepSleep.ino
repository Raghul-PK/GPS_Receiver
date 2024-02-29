// Hardware configuration
// Connect the RST pin with D0 (Wakeup) pin

#define ledPin 2 /**/

void setup()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

void loop()
{
  digitalWrite(ledPin, LOW);
  delay(5000);
  ESP.deepSleep(5e6); /* Sleep for 5 seconds (5*10^6 microseconds) --> Power consumption drops to ~10uA in this time */
}
