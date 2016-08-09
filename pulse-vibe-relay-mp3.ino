
/*  Pulse Sensor Amped 1.4    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com
  ----------------------  Notes ----------------------  ----------------------
  This code:
  1) Blinks an LED to User's Live Heartbeat   PIN 13
  2) Fades an LED to User's Live HeartBeat
  3) Determines BPM
  4) Prints All of the Above to Serial
  Read Me:
  https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md
  ----------------------       ----------------------  ----------------------
*/
#include <SoftwareSerial.h>

#define ARDUINO_RX 5//MP3 should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 6//MP3 connect to RX of the module
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);//MP3

static int8_t Send_buf[8] = {0} ;// MP3

//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin
int relay = 8;                    //relay pin for peltier

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = false;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse


void setup() {

  mySerial.begin(9600); //MP3
  delay(500);// MP3 Wait chip initialization is complete
  sendCommand(0X09, 0X02);//MP3 select the TF card
  delay(200);// MP3 wait for 200ms
  sendCommand(0X22, 0X1E01);//MP3 play the first song with volume 30 class

  pinMode (relay, OUTPUT);               // pin for relay on peltier
  pinMode(12, OUTPUT);               //pin for vibration motors
  pinMode(blinkPin, OUTPUT);        // pin that will blink to your heartbeat!
  pinMode(fadePin, OUTPUT);         // pin that will fade to your heartbeat!
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
  // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
  // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
  //   analogReference(EXTERNAL);


}


//  Where the Magic Happens
void loop() {


  serialOutput() ;

  if (QS == true) {    // A Heartbeat Was Found
    // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat
    fadeRate = 255;         // Makes the LED Fade Effect Happen
    // Set 'fadeRate' Variable to 255 to fade LED with pulse
    serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
    if (IBI >= 1000) {             //IBI for slow heart rate
      slowbeat();                 //call slowbeat subroutine
    }
    if (IBI < 1000 && IBI > 706) { //IBI for normal heart rate
      normalbeat();                //call normalbeat routine
    }
    if (IBI <= 706) {                //IBI for fast heart rate
      fastbeat();                   //call for fast beat
    }
    QS = false;                      // reset the Quantified Self flag for next time
  }
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);
  // ledFadeToBeat();                      // Makes the LED Fade Effect Happen
  delay(100);                             //  take a break
  //  peltier();
}

void sendCommand(int8_t command, int16_t dat)//MP3
{
  delay(20); //MP3
  Send_buf[0] = 0x7e; //MP3 starting byte
  Send_buf[1] = 0xff; //MP3 version
  Send_buf[2] = 0x06; //MP3 the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //MP3
  Send_buf[4] = 0x00;//MP3 0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = (int8_t)(dat >> 8);//MP3 datah
  Send_buf[6] = (int8_t)(dat); //MP3 datal
  Send_buf[7] = 0xef; //MP3 ending byte
  for (uint8_t i = 0; i < 8; i++) //MP3
  {
    mySerial.write(Send_buf[i]) ;//MP3
  }
}
  void ledFadeToBeat() {
    fadeRate -= 15;                         //  set LED fade value
    fadeRate = constrain(fadeRate, 0, 255); //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin, fadeRate);         //  fade LED
  }

  void slowbeat() {
    digitalWrite(12, HIGH);   // turn the Vibration Motor on (HIGH is the voltage level)
    delay(300);              
    digitalWrite(12, LOW);    // turn the Vibration Motor off by making the voltage LOW
    delay(200);
    digitalWrite(12, HIGH);   // turn the Vibration Motor on (HIGH is the voltage level)
    delay(300);              // wait for a second
    digitalWrite(12, LOW);    // turn the Vibration Motor off by making the voltage LOW
    delay(800);
  }

  void normalbeat() {
    digitalWrite(12, HIGH);   // turn the Vibration Motor on (HIGH is the voltage level)
    delay(200);              // wait for a second
    digitalWrite(12, LOW);    // turn the Vibration Motor off by making the voltage LOW
    delay(100);
    digitalWrite(12, HIGH);   // turn the Vibration Motor on (HIGH is the voltage level)
    delay(200);              // wait for a second
    digitalWrite(12, LOW);    // turn the Vibration Motor off by making the voltage LOW
    delay(500);
  }

  void fastbeat() {
    digitalWrite(12, HIGH);   // turn the Vibration Motor on (HIGH is the voltage level)
    delay(100);              // wait for a second
    digitalWrite(12, LOW);    // turn the Vibration Motor off by making the voltage LOW
    delay(50);
    digitalWrite(12, HIGH);   // turn the Vibration Motor on (HIGH is the voltage level)
    delay(100);              // wait for a second
    digitalWrite(12, LOW);    // turn the Vibration Motor off by making the voltage LOW
    delay(200);
  }

  /*void peltier() {
     digitalWrite (relay, LOW);
    delay (5000);                     //delay for the relay which is the peltier
    digitalWrite (relay, HIGH);
    delay (5000);
    digitalWrite (relay, LOW);
    delay (5000);                     //delay for the relay which is the peltier
    digitalWrite (relay, HIGH);
    delay (5000);
    digitalWrite (relay, LOW);
    delay (5000);                     //delay for the relay which is the peltier
    digitalWrite (relay, HIGH);
    loop();
    } */
