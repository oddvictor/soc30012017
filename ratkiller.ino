#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer;

LiquidCrystal_I2C lcd(0x27, 2,1,0,4,5,6,7,3, POSITIVE);

const byte rotter = 3;
int antall=0;
int tid = 0;
int driftled = 8;
int resetled = 9;
int coilled = 10;
int falluke = 7;
int gammelverdi =0;
int instilling =1;   //Instilling for coil. Satt til 1.(Kill) default
String inData;

void setup()        //Her gjennomfører vi alt som bare skal gjøres EN gang før vi gå inn i programløkke.
{
  pinMode(driftled, OUTPUT);// Drifts-led
  pinMode(resetled, OUTPUT);// Reset-led
  pinMode(coilled, OUTPUT); // PWM utgang til høyspenningscoil
  pinMode(falluke, OUTPUT); // Falluke
  lcd.begin(16,2);
  Serial.begin(9600);       //Initialiserer seriel kommunikasjon
  lcd.setCursor(4,0);
  lcd.print("RAT KILL");
  lcd.setCursor(6,1);
  lcd.print("2017");
  delay(3000);
  lcd.clear();
  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  sensors.setResolution(insideThermometer, 9);

  lcd.print("Rotter: ");
  
  lcd.setCursor(0,1);
  lcd.print("Temp  : ");
  pinMode(rotter, INPUT);
  attachInterrupt(digitalPinToInterrupt(rotter), rotteteller, FALLING);
  
}


void loop(){
  
    while(Serial.available() > 0)     // Sjekker om det er data tilgjengelig i serial-buffer
                                                 
  {
    inData=Serial.readString();
    instilling = inData.toInt();
  }
  
  int status=0;
  
while(gammelverdi<antall){            //Hvis en ny rotte står på fallemmen får den strøm utifra hvilken innstilling som er valgt.
      switch(instilling) {
        case 0:
          analogWrite(coilled,10);
          delay(500);
          digitalWrite(falluke, HIGH); //Åpne Falluke
          analogWrite(coilled,0);    
          delay(25);
          digitalWrite(falluke, LOW);
          
          break;
        case 1:
          analogWrite(10,100);
          delay(500);
          digitalWrite(falluke, HIGH);
          analogWrite(10,0);
          delay(25);
          digitalWrite(falluke, LOW);
          break;
      }
      gammelverdi = antall;
}

  sensors.requestTemperatures();                      //Be om sensor data
  float tempC = sensors.getTempC(insideThermometer);  //Legg data i flytvariablen tempC
 
if(tempC>29 || antall>=5)
  {
    status++;                     // Endre status hvis temp overstiger gitt antall grader eller hvis maks antall rotter er fanget
  }
  if(status==0){
  digitalWrite(driftled, HIGH);   //Hvis status viser normaldrift setter vi arduinoen til å printe dataen 
  lcd.setCursor(11,0);            //Printer til LCD
  lcd.print(antall);
  lcd.setCursor(10,1);
  lcd.print(tempC,1);
  lcd.print(char(223));
  Serial.print("$Data,");         //De neste linjene printer all data til seriell utgang
  Serial.print(tempC,1);
  Serial.print(",");
  Serial.print(antall);
  Serial.print(",");
  Serial.print(instilling);
  Serial.print("\r\n");
  delay(1000); 
  }
  else{
    lcd.clear();
    digitalWrite(resetled, HIGH);   //Sett Reset-led Høy
    digitalWrite(driftled, LOW);    //Sett Normaldrift-led Lav
    lcd.setCursor(6,0);
    lcd.print("EMPTY");
    lcd.setCursor(4,1);
    lcd.print("AND RESET");
    Serial.print("$Data,");         //De neste linjene printer all data til seriell utgang
    Serial.print(tempC,1);
    Serial.print(",");
    Serial.print(antall);
    Serial.print(",");
    Serial.print(tid);
    Serial.print(",");
    Serial.print(instilling);
    Serial.print("\r\n");
    
    delay(99999999);
  }
}
void rotteteller()                  //Mekanisk bryter med interrupt for å detektere rotte i sonen
{
  static unsigned long last_interrupt_time =0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 500) 
  {
    antall++;
  }
  
}

