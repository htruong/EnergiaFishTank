void setup() {
  Serial.begin(115200);    
  
  Serial.println("~~~ Main program starting up ~~~");  

  pinMode(D1_LED, OUTPUT);
  pinMode(D2_LED, OUTPUT);
  pinMode(PUSH1, INPUT_PULLUP); // released = HIGH, pressed = LOW
  pinMode(PUSH2, INPUT_PULLUP);
  
  
  Serial.println("~~~ Communication starting ~~~");  
  
  setupCommunication();
  
  Serial.println("~~~ Sensors starting ~~~");  
  setupTempReader();
  
  
  Serial.println("~~~ Main program started ~~~");  
}

void loop() {
  loopCommunication();
  loopTempReader();
}
