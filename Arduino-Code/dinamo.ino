#include <HX711.h>
#include <ArduinoBLE.h>
 
BLEService dinamoB("A200");
BLEFloatCharacteristic loadChar("A201", BLERead | BLENotify);

// Pin de datos y de reloj
byte pinData = 6;
byte pinClk = 9;
byte R = 18;
byte G = 19;
byte B = 20;


HX711 dinamo;

int count;
bool ledOn = false;

float factor_calibracion = 20780.0; //Temporal, falta calibrar bien de verdad

void setup() {
  count = 0;
  Serial.begin(9600);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("BLE failed");
    while (1) {}
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("dinamo");
  BLE.setDeviceName("dinamo");
  BLE.setAdvertisedService(dinamoB);
  dinamoB.addCharacteristic(loadChar);
  BLE.addService(dinamoB);

  loadChar.writeValue(0);
  BLE.advertise();


  dinamo.begin(pinData, pinClk); //Inicia el HX711
  dinamo.set_scale();
  dinamo.tare(); //Tara
  long zero_factor = dinamo.read_average(50); //Referencia del 0

}

void loop() {
  count+=1;
  // Aplicar calibración
  dinamo.set_scale(factor_calibracion);
  BLEDevice central = BLE.central();


 if(count%20000 == 0){
    if(ledOn){
      setOff();
      ledOn = false;
    }
    else{
      setBlue();
      ledOn = true;
    }
  }

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    while (central.connected()) {
      if (1) {
        setBlue();
        float load = dinamo.get_units();
        if(load >0.05){
          //float send = 43.25;
          loadChar.writeValue(load);
          Serial.print("sent:");
          Serial.println(load, 4);
          //delay(10000);
        }
      }
    }
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
  

}

void setOff(){
  analogWrite(R,0);
  analogWrite(G,0);
  analogWrite(B,0);
}
void setBlue(){
  analogWrite(R,0);
  analogWrite(G,0);
  analogWrite(B,255);
 
  
}
