  #include <dht.h>

#define DHT_PIN PE_4

const long dhtInterval = 60000; 
unsigned long dhtPreviousMillis;

void getDHTReadings();

void setupTempReader() {
  pinMode(DHT_PIN, INPUT_PULLUP);
  dhtPreviousMillis = millis();
  Serial.println("DHT Reader thread started!");
}

void loopTempReader() {

  unsigned long currentMillis = millis();

  if (currentMillis - dhtPreviousMillis >= dhtInterval) {
    // save the last time you took the readings
    dhtPreviousMillis = currentMillis;
    
   // read the value from the sensor:
   getDHTReadings();
   
   announceDHTReadings();
  }
  
  

}

void getDHTReadings()
{
 int tries = 0;
 //Being a timed single wire protocol, errors do happen.
 //But by comparing the result of getData to 0 you can discard bad reads using the CRC system.
  while ((dht::readFloatData(DHT_PIN, &temperature, &humidity, true) != 0) && (tries < 10)) {
    //Serial.println("Temperature and Humidity reading data corrupted. Trying again... ");
    tries++;
  }
  
  if (tries < 10) {
    
    Serial.print("\tTemperature: ");
    Serial.print(temperature);
    Serial.print(" Humidity: ");
    Serial.println(humidity);
    
  } else {
    Serial.println("Temperature and Humidity reading data is unavailable.");
  }
}
