/*
  Basic Solitude Vibration pattern
  Lorum Ipsum


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
   pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(13, HIGH);   // turn the Vibration Motor on (HIGH is the voltage level)
  delay(10000);              // wait for a second
  digitalWrite(12, HIGH);    // turn the Vibration Motor off by making the voltage LOW
  delay(10000);              // wait for a second

    digitalWrite(13, LOW);   // turn the Vibration Motor on (HIGH is the voltage level)
  delay(100);              // wait for a second
  digitalWrite(12, LOW);    // turn the Vibration Motor off by making the voltage LOW
  delay(100);              // wait for a second
}
