#include "Arduino.h"
//Output Pins
const int oUMKA=D0;   //Output Relais Umkleite-Rollo AUF
const int oUMKZ=D1;   //Output Relais Umkleide-Rollo ZU
const int oSYSA=D2;   //Output Relais System-Rollo AUF
const int oSYSZ=D3;   //Output Relais System-Rollo ZU
const int oTA=D4;     //Output Türschloss AUF
const int oTZ=D5;     //Output Türschloss ZU
//Input Pins
const int iSYS_AZ=D6; //Input System-Signal AUF/ZU (LOW/HIGH)
const int iResv=D7;   //Input Reedsensor Systemtür OFFEN/ZU (HIGH/LOW)
const int iREED_AUF=D8; //Input Reedsensor system-Rollo AUF/ZU (LOW/HIGH)
const int iTAS_A=D9;    //Input Taster Umkleide AUF (LOW)
const int iClubh_AZ=D10;  //Input Schalter Clubhaus AUF/ZU (LOW/HIGH)
unsigned long rolloTime = 1000*60;  //Laufzeit Rollo = 60sek 
unsigned long rolloTimeStart; //Startzeit als Referenz zur Rollolaufzeit
unsigned long tasterTime = 1000*60 + rolloTime; 
unsigned long tasterTimeStart;
bool timerFl=false; 
bool tasterFl=false;
void RolloAuf()
{
   digitalWrite(oSYSZ, LOW);  //Relays Systemrollo ZU ausschalten
   digitalWrite(oUMKZ, LOW);  //Relays Umkleiderollo ZU ausschalten
   delay(500);
   digitalWrite(oSYSA, HIGH); //Relays Systemrollo AUF einschalten
   digitalWrite(oUMKA, HIGH); //Relays Umkleiderollo AUF einschalten
   digitalWrite(oTA, HIGH);  //Türimpuls aufschließen HIGH
   delay(1000);              //Impulslänge für Türschließer
   digitalWrite(oTA, LOW);  //Türimpuls aufschließen LOW
    rolloTimeStart=millis(); //Rollo Startzeit setzen Referenz zur Rollolaufzeit
   timerFl=true;    //Rollo läuft
}
void RolloZu()
{
   digitalWrite(oSYSA, LOW);  //Relays Sytemrollo AUF ausschalten
   digitalWrite(oUMKA, LOW);  //Relays Umkleiderollo AUF ausschalten
   delay(500);
   digitalWrite(oSYSZ, HIGH); //Relays Sytemrollo ZU einschalten
   digitalWrite(oUMKZ, HIGH); //Relays Umkleiderollo ZU einschalten
   digitalWrite(oTZ, HIGH);    //Türimpuls zuschließen HIGH
   delay(1000);             //Impulslänge für Türschließer
   digitalWrite(oTZ, LOW);  //Türimpuls zuschließen LOW
   rolloTimeStart=millis(); //Rollo Startzeit setzen Referenz zur Rollolaufzeit
   timerFl=true;    //Rollo läuft
}  

void setup() 
{
  Serial.begin(115200);
  //define for output and inital pins
  pinMode(oUMKA, OUTPUT);
  digitalWrite(oUMKA, LOW);
  pinMode(oUMKZ, OUTPUT);
  digitalWrite(oUMKZ, LOW);
  pinMode(oSYSA, OUTPUT);
  digitalWrite(oSYSA, LOW);
  pinMode(oSYSZ, OUTPUT);
  digitalWrite(oSYSZ, LOW);
  pinMode(oTA, OUTPUT);
  digitalWrite(oTA, LOW);
  pinMode(oTZ, OUTPUT);
  digitalWrite(oTZ, LOW);
  //define for input and inital pins 
  pinMode(iSYS_AZ, INPUT_PULLUP);
  pinMode(iResv, INPUT_PULLUP);
  pinMode(iREED_AUF, INPUT_PULLUP);
  pinMode(iClubh_AZ, INPUT_PULLUP);
  pinMode(iTAS_A, INPUT_PULLUP);
}

void loop() 
{
  // Serial.println(digitalRead(iSYS_AZ));
  // Serial.println(digitalRead(iResv));
  // Serial.println(digitalRead(iREED_AUF));
  // Serial.println(digitalRead(iTAS_A));
  // Serial.println(digitalRead(iClubh_AZ));
//  delay(500);
  if(digitalRead(iResv)==LOW)
  {
    if(digitalRead(iClubh_AZ)==LOW)  
    {
      if(digitalRead(oUMKA)==LOW && digitalRead(oSYSA)==LOW && digitalRead(iREED_AUF)==HIGH)
      {
        Serial.println("Clubhausschalter AUF ");
        RolloAuf();
      }
    }
    else 
    {
      if(digitalRead(iSYS_AZ)==LOW) 
      {
        if(digitalRead(oUMKA)==LOW && digitalRead(oSYSA)==LOW && digitalRead(iREED_AUF)==HIGH)
        {
          Serial.println("System AUF ");
          RolloAuf();
        }
      }
      else 
      {
        if(digitalRead(oUMKZ)==LOW && digitalRead(oSYSZ)==LOW && digitalRead(iREED_AUF)==LOW && tasterFl==false)
        {
          Serial.println("System ZU ");
          RolloZu();
        }
      }
    }

    if(digitalRead(iTAS_A)==LOW && digitalRead(oUMKA)==LOW && digitalRead(oSYSA)==LOW)
    {
      Serial.println("Taster AUF ");
      tasterFl=true;
      tasterTimeStart=millis();
      RolloAuf();
//          }
    }
    if(timerFl==true && millis()-rolloTimeStart > rolloTime)
    {
      Serial.println("Rollorelays AUS ");
      digitalWrite(oSYSA, LOW);
      digitalWrite(oUMKA, LOW);  
      digitalWrite(oSYSZ, LOW);
      digitalWrite(oUMKZ, LOW);
      timerFl=false; 
//      }
    }
    if(tasterFl==true && millis()-tasterTimeStart > tasterTime)
    {
      Serial.println("Taster Rollorelays AUS ");
      if(iClubh_AZ==HIGH && iSYS_AZ==HIGH)RolloZu();
      tasterFl=false; 
//      }
    }
  }
  else
  {
    Serial.println("Systemtür Auf ");
    digitalWrite(oSYSA, LOW);
    digitalWrite(oUMKA, LOW);  
    digitalWrite(oSYSZ, LOW);
    digitalWrite(oUMKZ, LOW);
    timerFl=false; 
    delay(1000);    
  }
}
