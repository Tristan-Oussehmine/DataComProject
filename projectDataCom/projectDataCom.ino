#include <ArduinoBLE.h>
BLEService newService("180A"); // creating the service

BLEUnsignedCharCharacteristic randomReading("2A58", BLERead | BLENotify); // creating the Analog Value characteristic
BLEByteCharacteristic switchChar("2A57", BLERead | BLEWrite); // creating the LED characteristic
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int tiltPin = 9;  // the number of the pushbutton pin
const int pressurePin = 10;  // the number of the pushbutton pin
const int ledPin1 = 4;    // the number of the LED pin
const int ledPin2 = 5;    // the number of the LED pin
const int ledPin3 = 6;    // the number of the LED pin
const int ledPin4 = 7;    // the number of the LED pin
const int ledPin5 = 8;    // the number of the LED pin

// variables will change:
int tiltState = 0;  // variable for reading the pushbutton status
int pressureState = 0;
int switchvalue;
bool tiltFelt = false;
bool pressureFelt = false;
bool circuitStatus = false;
bool switchcharwritten = false;
void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
      pinMode(ledPin3, OUTPUT);
        pinMode(ledPin4, OUTPUT);
          pinMode(ledPin5, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(tiltPin, INPUT_PULLUP);
    pinMode(pressurePin, INPUT_PULLUP);
  Serial.begin(9600);
   while (!Serial);       //starts the program if we open the serial monitor.

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected
    if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }

  BLE.setLocalName("ArduinoTristan"); //Setting a name that will appear when scanning for Bluetooth® devices
  BLE.setAdvertisedService(newService);

  newService.addCharacteristic(switchChar); //add characteristics to a service
  newService.addCharacteristic(randomReading);

  BLE.addService(newService);  // adding the service

  switchChar.writeValue(0); //set initial value for characteristics
  randomReading.writeValue(0);

  BLE.advertise(); //start advertising the service
  Serial.println(" Bluetooth® device active, waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central(); // wait for a Bluetooth® Low Energy central

  if (central) {  // if a central is connected to the peripheral
    Serial.print("Connected to central: ");
    
    Serial.println(central.address()); // print the central's BT address
    
    digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection
    while (central.connected()) {
      if (switchChar.written()) { //leest meegegeven hex-waarden in
         switchvalue = switchChar.value();
         switchcharwritten = true; //Start de vlag
        }
        if(switchcharwritten){
         if ( switchvalue >= '0' && switchvalue <= '9' ) //conversie van serieel naar hex
        {
          switchvalue = switchvalue - '0';
        }
        else if ( switchvalue >= 'A' && switchvalue <= 'F' ) //conversie van serieel naar hex 2
        {
          switchvalue = switchvalue + 10 - 'A';  //  text 'A' = hex A = decimal 10
        }
          if (switchvalue == 1 || switchvalue > 4) {   // elke  value anders than 0, 2, 3, 4
            Serial.println("Circuit On");
            circuitStatus = true;
            digitalWrite(ledPin1, LOW);
            digitalWrite(ledPin2, LOW);
            digitalWrite(ledPin3, LOW);
            digitalWrite(ledPin4, LOW);
            digitalWrite(ledPin5, LOW);
          } else if (switchvalue == 2){
            Serial.println("Left-To-Right On"); //links naar rechts leds laten blinken
            circuitStatus = false;
            digitalWrite(ledPin1, LOW);
            digitalWrite(ledPin2, LOW);
            digitalWrite(ledPin3, LOW);
            digitalWrite(ledPin4, LOW);
            digitalWrite(ledPin5, LOW);
          } else if (switchvalue == 3){ //rechts naar links leds laten blinken
            Serial.println("Right-to-left On");
            circuitStatus = false;
            digitalWrite(ledPin1, LOW);
            digitalWrite(ledPin2, LOW);
            digitalWrite(ledPin3, LOW);
            digitalWrite(ledPin4, LOW);
            digitalWrite(ledPin5, LOW);
          } else if(switchvalue == 4){ 
            Serial.println("leds on"); //leds aanhouden
            circuitStatus = false;
            digitalWrite(ledPin1, HIGH);
            digitalWrite(ledPin2, HIGH);
            digitalWrite(ledPin3, HIGH);
            digitalWrite(ledPin4, HIGH);
            digitalWrite(ledPin5, HIGH);
          } else if(switchvalue == 0){   //Alles stopzetten
            Serial.println("Circuit Off");
            circuitStatus = false;
            digitalWrite(ledPin1, LOW);
            digitalWrite(ledPin2, LOW);
            digitalWrite(ledPin3, LOW);
            digitalWrite(ledPin4, LOW);
            digitalWrite(ledPin5, LOW);
          }
          switchcharwritten = false; //stopt de vlag
        }
      if(circuitStatus == true){ //Laat de leds en sensoren werken als circuitStatus aan staat.
        tiltState = digitalRead(tiltPin);
        pressureState = digitalRead(pressurePin);

        if (!tiltState == HIGH) { //safety om leds uit te houden als sensoren niet werken.
          digitalWrite(ledPin1, LOW);
          digitalWrite(ledPin2, LOW);
          digitalWrite(ledPin3, LOW);
          digitalWrite(ledPin4, LOW);
          digitalWrite(ledPin5, LOW);
          tiltFelt = true;
          pressureFelt = false;
        } else if(!pressureState ==HIGH){
          digitalWrite(ledPin1, LOW);
          digitalWrite(ledPin2, LOW);
          digitalWrite(ledPin3, LOW);
          digitalWrite(ledPin4, LOW);
          digitalWrite(ledPin5, LOW);
          pressureFelt = true;
          tiltFelt = false;
        }
        if(tiltFelt == true){ //Als tiltsensor gebruikt wordt
          digitalWrite(ledPin1, HIGH);
          delay(600);
          digitalWrite(ledPin2, HIGH);
          delay(600);
          digitalWrite(ledPin3, HIGH);
          delay(600);
          digitalWrite(ledPin4, HIGH);
          delay(600);
          digitalWrite(ledPin5, HIGH);
          Serial.println("Done1");
          delay(10000);
          digitalWrite(ledPin1, LOW);
          digitalWrite(ledPin2, LOW);
          digitalWrite(ledPin3, LOW);
          digitalWrite(ledPin4, LOW);
          digitalWrite(ledPin5, LOW);
          tiltFelt = false;
          
        }else if(pressureFelt == true){ //Als druksensor gebruikt wordt
          digitalWrite(ledPin5, HIGH);
          delay(600);
          digitalWrite(ledPin4, HIGH);
          delay(600);
          digitalWrite(ledPin3, HIGH);
          delay(600);
          digitalWrite(ledPin2, HIGH);
          delay(600);
          digitalWrite(ledPin1, HIGH);
          Serial.println("Done2");
          delay(10000);
          digitalWrite(ledPin1, LOW);
          digitalWrite(ledPin2, LOW);
          digitalWrite(ledPin3, LOW);
          digitalWrite(ledPin4, LOW);
          digitalWrite(ledPin5, LOW);
          pressureFelt = false;
        }
      }
      if(switchvalue == 2){ //Als switchvalue 2 (links naar rechts) wordt opgevraagd
          digitalWrite(ledPin1, HIGH);
          digitalWrite(ledPin5, LOW);
          delay(500);
          digitalWrite(ledPin1, LOW);
          digitalWrite(ledPin2, HIGH);
          delay(500);
          digitalWrite(ledPin2, LOW);
          digitalWrite(ledPin3, HIGH);
          delay(500);
          digitalWrite(ledPin3, LOW);
          digitalWrite(ledPin4, HIGH);
          delay(500);
          digitalWrite(ledPin4, LOW);
          digitalWrite(ledPin5, HIGH);
          delay(500);
      } else if(switchvalue ==3){ //Als switchvalue 3 (rechts naar links) wordt opgevraagd
          digitalWrite(ledPin1, LOW);
          digitalWrite(ledPin5, HIGH);
          delay(500);
          digitalWrite(ledPin5, LOW);
          digitalWrite(ledPin4, HIGH);
          delay(500);
          digitalWrite(ledPin4, LOW);
          digitalWrite(ledPin3, HIGH);
          delay(500);
          digitalWrite(ledPin3, LOW);
          digitalWrite(ledPin2, HIGH);
          delay(500);
          digitalWrite(ledPin2, LOW);
          digitalWrite(ledPin1, HIGH);
          delay(500);
      }
    }
  }
}
