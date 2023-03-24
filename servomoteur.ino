#include <ArduinoBLE.h>
#include <Servo.h>
#include "Ultrasonic.h"


//actionneurs
int voyant = D10;
int voyantVert = D8;
int voyantRouge = D9;
int buzzer= D11;
Servo servoMoteur;
int boutton = A2; 
int setupdone=1;

int switchBLEWork=1;



int dernierPassage=0;

//capteurs
Ultrasonic coupDePiedCapteur(14);
Ultrasonic nourritureCapteur(15);
int lumiere = A7;
int poid=A6;

int co = 0;

int masseGamelle=0;
int DernierMiamMiam=0;

int secondes=0;
int minutes=0;
int heure=0;


int nbportions=1;
int nbrepas=2;

float heureDernierRepas=0;


int state = 0;

/*Ultrasonic NourritureCapteur(14);
Ultrasonic CoupdepiedCapteur(15);
*/


BLEService ledService("180A"); 
BLEByteCharacteristic IntervalleNourriture("2B0B", BLERead | BLEWrite);//nbrepas
BLEByteCharacteristic NombredePortions("2A59", BLERead | BLEWrite);//nbportions
BLEByteCharacteristic QteMangee("2A98", BLERead);//capteur de pression
BLEByteCharacteristic DernierRepas("2AED", BLERead);//capteur externe 
BLEByteCharacteristic Niveaudenourriture("2AE2", BLERead);//capteur interne 


void connexionBLE(){
  BLEDevice central = BLE.central();

if (central) {
    Serial.print("Connected to central: ");

    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);           


    while (central.connected()) {

       
        
      if (IntervalleNourriture.written()) {
        Serial.println(" time changé");
        nbrepas=IntervalleNourriture.value();
        
      }

      if (NombredePortions.written()) {
        Serial.println("nombre de portions changé");
        nbportions=NombredePortions.value();
        
      }
     





    }

   
   Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);  
              
  }
  

  

}

void jouerunson(int pin){
digitalWrite (pin, HIGH);
delay (200);
digitalWrite (pin, LOW);
delay (400);
}



void verifstock(){


 long RangeInCentimeters;
 RangeInCentimeters = nourritureCapteur.MeasureInCentimeters();

//Serial.println(RangeInCentimeters);



if(RangeInCentimeters<9)
{digitalWrite(voyantVert, HIGH);
digitalWrite (voyantRouge, LOW);}
else
{digitalWrite(voyantVert, LOW);
digitalWrite (voyantRouge, HIGH);}

/*int valeur = analogRead(A0);
Serial.println(valeur);*/

}


void nourrir(){
  jouerunson(buzzer);
  servoMoteur.write(90);
  delay(400);
  servoMoteur.write(180);
  delay(400);


}

void coupDePied(){
int valeur = analogRead(lumiere);
long RangeInCentimeters;

RangeInCentimeters = coupDePiedCapteur.MeasureInCentimeters();

 //Serial.println(valeur);
 //Serial.println(RangeInCentimeters);
if(RangeInCentimeters<10)
    {
      heureDernierRepas=minutes;
      heureDernierRepas=heureDernierRepas/100;
      heureDernierRepas==heureDernierRepas+heure;
     // Serial.println(heureDernierRepas);
    }

if(valeur<1 && RangeInCentimeters<30)
  {digitalWrite (voyant, HIGH);
   jouerunson(buzzer); 
  }
else{
digitalWrite (voyant, LOW);

}



}


void stockPoid()
{
masseGamelle=analogRead(poid);
}


void mettreHeureAJour(){
  
  secondes++;
if(secondes>=60)
  {secondes=0;
  minutes++;}
if(minutes>=60)
  {minutes=0;
  heure++;}
if(heure>=24)
  {heure=0;}

/*Serial.println(heure);
Serial.println(" : ");
Serial.println(minutes);
Serial.println(" : ");
Serial.println(secondes);*/

}


void checkHeure()
{
  int ji;
  int jii;
if (nbrepas>=1)
  {
    int val=24/nbrepas;
    for(ji=0;ji<nbrepas;ji++)
      {if(heure==ji*val && secondes==0 && minutes==0)
        {
          for(jii=0;jii<nbportions;jii++)
          {
            nourrir();
            delay(100);
          }
        }

      }
  }

}
void TOTO()
{
state=!state;
}

void setup() {

 pinMode(2, INPUT_PULLUP);
  attachInterrupt(2, TOTO, CHANGE);

  servoMoteur.attach(12);
  servoMoteur.write(179);

  pinMode(buzzer, OUTPUT);
 // pinMode(voyantRouge, OUTPUT);

  pinMode(A7, INPUT);
  pinMode(A6, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(boutton, INPUT_PULLUP);






  Serial.begin(9600);
  


  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);       
  digitalWrite(LEDR, HIGH);            
  digitalWrite(LEDG, HIGH);              
  digitalWrite(LEDB, HIGH);                


  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);

    
  }

  BLE.setLocalName("NUTRITOR2I");
  BLE.setAdvertisedService(ledService);


ledService.addCharacteristic(IntervalleNourriture);
ledService.addCharacteristic(QteMangee);
ledService.addCharacteristic(DernierRepas);
ledService.addCharacteristic(NombredePortions);
ledService.addCharacteristic(Niveaudenourriture);
BLE.addService(ledService);



IntervalleNourriture.writeValue(nbrepas);
NombredePortions.writeValue(nbportions);
QteMangee.writeValue(0);
Niveaudenourriture.writeValue(0);
DernierRepas.writeValue(dernierPassage);

  BLE.advertise();

  Serial.println("BLE LED Peripheral");


  //connexionBLE();
}



void loop() {
//Serial.println(state);

while (state==0){
  connexionBLE();
/*BLEDevice central = BLE.central();

if (central) {


  
    Serial.print("Connected to central: ");

    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);           


    while (central.connected()) {

       
        
      if (IntervalleNourriture.written()) {
        Serial.println(" time changé");
        nbrepas=IntervalleNourriture.value();
        
      }

      if (NombredePortions.written()) {
        Serial.println("nombre de portions changé");
        nbportions=NombredePortions.value();
        
      }
      long RangeInCentimeters;

      RangeInCentimeters = coupDePiedCapteur.MeasureInCentimeters();    

      
      if(RangeInCentimeters<10)
          DernierRepas.writeValue(RangeInCentimeters);



    }

   
   Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);  
              
  }

  */
}


if(state==1)
{

  verifstock();
        coupDePied();
        stockPoid();
        checkHeure();
        mettreHeureAJour();
        //nourrir();
        delay(1000);
}


  







}








