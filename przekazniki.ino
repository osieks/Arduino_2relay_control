#include <math.h>

#include <Dusk2Dawn.h>

#include "DHT.h"          // biblioteka DHT
//wyswietlacz
#include <LiquidCrystal.h>

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//#define THICKS 1200 // 1 sec.
//#define THICKS 6000 // 1 sec.
//#define THICKS 9000 // około 8? target połowy żaluzji aby cykle się zgadzały
//#define THICKS 12000 //around 10 sec.
#define THICKS 24000 // 20sec.
//#define THICKS 36000 // 0.5 min.
//#define THICKS 72000 // 1 min. ///////DOMYŚLNIE
//#define THICKS 144000 // 2 min.
//#define THICKS 360000 // 5 min.

const int rs = 12, en=11, d4=5, d5=4, d6=3, d7=2;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

//definiowanie zmiennych
unsigned long czas_teraz,czas_poprzednio=THICKS - THICKS*0.5;
int aktywacja=20,warAktywacji=15;
unsigned int fotores,moi,x_times_up=0,x_times_down=0;
int procenty0,procenty1,on_off=0; 
bool zbocze[10];
bool semi_down=false,semi_up=false;
//DEBUG TUTAJ
bool na_zmiane=false,debug=true;
String str;
int buttonState=0,changeState=0;
float x,moi_proc;

//czujnik wilgoci i temperatury
#define DHTPIN 7          // numer pinu sygnałowego
#define DHTTYPE DHT11     // typ czujnika (DHT11)
#define Z_on_off 10 // on/off auto zaluzje
#define Zup 9      // zaluja
#define Zdown 8     
DHT dht(DHTPIN, DHTTYPE); // definicja czujnika

Dusk2Dawn Gliwice(50.2833, 18.6667, +2);

void setup() {
  //SERIAL SETUP
  Serial.begin(9600);
  Serial.print("\n");
  Serial.print("~~~~~~STARTING~~~~~~");
  Serial.print("\n");  Serial.print("\n");

  for(int i=0;i<10;i++){
    zbocze[i]=1;
    Serial.print(zbocze[i]);Serial.print(" ");
  }
  Serial.print("\n");
  
  //pin przekaznika
  pinMode(Zup,OUTPUT);
  pinMode(Zdown,OUTPUT);
  digitalWrite(Zup,HIGH); // HIGH oznacza wyłączone
  digitalWrite(Zdown,HIGH); // HIGH oznacza wyłączone
  pinMode(Z_on_off,INPUT_PULLUP);
  zbocze[0]=0;
  zbocze[1]=0;

  //pinMode(0, INPUT);
  //pinMode(1, INPUT);
  //pinMode(12, INPUT_PULLUP);
    
  dht.begin();            // inicjalizacja czujnika
  lcd.begin(16,2);
  
  //lcd.setCursor(0,0);
  //lcd.print("Made by Osiek");
  //lcd.setCursor(0,1);
  //lcd.print("Made by Osiek");
 

  pinMode(6, OUTPUT);
  analogWrite(6,100);

  // setup zegara!
    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
    }
    //rtc.adjust(DateTime(2021, 8, 30, 21, 23, 0));
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    na_zmiane=0;
}

bool check_if_halfs(bool array[], int n)
{   
    if(array[n/2 - 1] == array[n/2])return false;
    for (int i = 0; i < n/2 - 1; i++)      
    {         
        if (array[i] != 0)
            return false;  
        if(array[n/2 + i] != 1)
            return false; 
    }
    return true;
}

void loop() {
  zbocze[0]= digitalRead(Z_on_off);

  /*
   * AUTO       - A -   on_off == 0
   * MANUAL     - M -   on_off == 1
   * SEMI-AUTO  - S -   on_off == 2
   */
  // jesli nacisniety przycisk
  if(check_if_halfs(zbocze,10)){
    x_times_up=0;
    x_times_down=0;
    semi_down=false;
    semi_up=false;
    analogWrite(6,100);

    if(debug==1){
      Serial.print("\nZMIANA TRYBU \n");
      Serial.print(on_off);
      Serial.print(" -> ");
    }
    if(on_off==0){
      Serial.print("1");
      on_off=1;
      lcd.setCursor(0,0);lcd.print("MANU");
    }
    else if(on_off==1){
      Serial.print("2");
      on_off=2;
      lcd.setCursor(0,0);lcd.print("SEMI");
    }
    else{
      Serial.print("0");
      on_off=0;
      lcd.setCursor(0,0);lcd.print("AUTO");
      
    }
    delay(500);
    Serial.print("\n");
  }
  for(int i=9;i>0;i--){
    zbocze[i]=zbocze[i-1];
    //Serial.print(zbocze[i]);Serial.print(" ");
  }
  //Serial.print(zbocze[0]);
  //Serial.print("\n");
  
      
  czas_teraz = millis();
  if(czas_teraz - czas_poprzednio > THICKS){// warunek cyklicznosci
    lcd.begin(16,2);
    if(debug==1){
      Serial.print("\n\n## NOWY CYKL ##\n");
      Serial.print("czas teraz: ");
      Serial.print(czas_teraz);
      Serial.print(" czas poprzednio: ");
      Serial.print(czas_poprzednio);
      Serial.print(" THICKS: ");
      Serial.print(THICKS);
      Serial.print("\n");
    }
    czas_poprzednio = czas_teraz;

      //testy z czasem
  DateTime now = rtc.now();
  if(debug==1){
     Serial.print(now.year(), DEC);
     Serial.print('/');
     Serial.print(now.month(), DEC);
     Serial.print('/');
     Serial.print(now.day(), DEC);
     Serial.print(" (");
     Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
     Serial.print(") ");
     Serial.print(now.hour(), DEC);
     Serial.print(':');
     Serial.print(now.minute(), DEC);
     Serial.print(':');
     Serial.print(now.second(), DEC);
     Serial.println();
     Serial.print(" since midnight 1/1/1970 = ");
     Serial.print(now.unixtime());
     Serial.print("s = ");
     Serial.print(now.unixtime() / 86400L);
     Serial.println("d");
  }
  //testy z DUSK 2 DAWN
  // tutaj warunki Day Light Saving
  int Sunrise  = Gliwice.sunrise(now.year(), now.month(), now.day(), false);
  int Sunset   = Gliwice.sunset(now.year(), now.month(), now.day(), false);
  
  int SunriseHour = Sunrise/60;
  int SunriseMinute = Sunrise%60;
  int SunsetHour = Sunset/60;
  int SunsetMinute = Sunset%60;

  if(debug==1){
    Serial.println();
    Serial.println("Dusk2Dawn");
    Serial.print(Sunrise);  // 418
    Serial.print(" z tego godzina = ");
    Serial.print(SunriseHour);  
    Serial.print(":");
    Serial.print(SunriseMinute);
    Serial.println();
    Serial.print(Sunset);
    Serial.print(" z tego godzina = ");
    Serial.print(SunsetHour);
    Serial.print(":");
    Serial.print(SunsetMinute);
    Serial.println();
    Serial.print("teraz minut = ");
    Serial.print((now.hour()*60 + now.minute()));
    Serial.println();
  }
  
    // TRYB PRACY PRZEKAŹNIKÓW
    if(on_off==1){
      lcd.setCursor(0,0);lcd.print("MANU");
    }
    else if( on_off == 0){
      lcd.setCursor(0,0);lcd.print("AUTO");
    } else{
      lcd.setCursor(0,0);lcd.print("SEMI");
    }
    
    delay(2000);
    //FOTOREZ CZYTANIE;
    //fotores = analogRead(A5); // dane z fotorezystora
    
    //fotores =(fotores/1024.0)*100;

    fotores=100;

/* for debugging */
    //fotores = 80;
    
    //CZYSZCZENIE WYSWIETLACZA
    //lcd.setCursor(0,0);lcd.print("                 ");
    //lcd.setCursor(1,0);lcd.print("                 ");
    // TRYB W ROGU

    //FOTOREZYSTOR
    /*
    Serial.print("fotores: ");
    Serial.print(fotores);
    Serial.print("%");
    Serial.print("\n");
    str = "swi:";
    str += fotores;
    str += "%";
    lcd.setCursor(2,0);lcd.print(str);

    delay(2000);
    */
    if(debug==1){
      Serial.print("wyswietlanie wschodu i zachodu na zmiane = ");
      Serial.println(na_zmiane);
    }
    if(na_zmiane==0){
      str = "Wschod";
      if(SunriseHour<10)str+=" ";
      str += SunriseHour;
      str += ":";
      if(SunriseMinute<10)str+="0";
      str += SunriseMinute;
      na_zmiane=1;
    }else{
      str = "Zachod";
      if(SunsetHour<10)str+=" ";
      str += SunsetHour;
      str += ":";
      if(SunsetMinute<10)str+="0";
      str += SunsetMinute;
      na_zmiane=0;
    }
    lcd.setCursor(5,0);lcd.print(str);
    
    // moisture sesnor 
    /*
    moi = analogRead(A6);
    // powietrze: 670
    // woda: 296

    //nowe przy przekaznikach
    // powietrze: ~71
    // woda:
    moi_proc = (moi*1.0 - 300)/(7000-300) * 100;
    Serial.print("moisture: ");
    Serial.print(moi);
    Serial.print(" proc: ");
    Serial.print(moi_proc);
    Serial.print("%");
    Serial.print("\n");
    str = "wk:";
    str += moi;
    str += "";
    lcd.setCursor(10,0);lcd.print(str);
    */
    //aktywacja = 11;
    //STEROWANIE PRZEKAŹNIKAMI
    if(debug==1){
      Serial.print("aktywacja zaluzji =(aktywacja przy ");
      Serial.print(warAktywacji);
      Serial.print(") = ");
      Serial.print(aktywacja);
      Serial.print("\n");
      Serial.print("x_times_up(4) ");
      Serial.print(x_times_up);
      Serial.print("   x_times_down(2) ");
      Serial.print(x_times_down);
      Serial.print("\n");
      Serial.print("semi_up ");
      Serial.print(semi_up);
      Serial.print("   semi_down ");
      Serial.print(semi_down);
      Serial.print("\n");
  
      Serial.println("warunek na otwieranie ");
      Serial.print((Sunrise));
      Serial.print(" =< ");
      Serial.print(now.hour()*60+now.minute());
      Serial.print(" < ");
      Serial.print((Sunset));
      Serial.print("\n");
  
      
      Serial.println("warunek na zamykanie ");
      Serial.print(now.hour()*60+now.minute());
      Serial.print(" >= ");
      Serial.print((Sunset));
      Serial.print("\n");

    }
    // ogólnie to zamiana 50% fotores na now.hour +1
    
    // AKTYWACJA GDY SEMI-AUTO
    if(aktywacja>=warAktywacji && on_off==2){
        aktywacja=0;
        if(debug==1){
        Serial.print("AKTYWACJA SEMI i fotores = ");
        Serial.print(fotores);
        Serial.print("\n");
        }
        if((now.hour()*60 + now.minute())>=(Sunrise/*+60*/) && (now.hour()*60 + now.minute())<(Sunset/*-60*/+15) && semi_up==false){
          Serial.print("UP \n");
          delay(2000);
          digitalWrite(Zup,LOW);
          delay(3500);
          digitalWrite(Zup,HIGH);
          delay(2000);
          semi_up = true;
          semi_down=false;
        }
        else if((now.hour()*60 + now.minute())>=(Sunset/*-60*/+15) && semi_down==false){
          Serial.print("down \n");
          delay(2000);
          digitalWrite(Zdown,LOW);
          delay(3300);
          digitalWrite(Zdown,HIGH);
          delay(2000);
          semi_down=true;
          semi_up = false;
        }

    }
    // AKTYWAXJA GDY AUTO
    else if(aktywacja>=warAktywacji && on_off==0){
      aktywacja=0;
      Serial.print("AKTYWACJA i fotores = ");
      Serial.print(fotores);
      Serial.print("\n");

      /*
      //tylko gdy mocno słońce otworzy bardziej niż połowa
        if(fotores>=95  && x_times_up<8){//wynik fotores w %
        Serial.print("UP ponad polowe \n");
        delay(2000);
        digitalWrite(Zup,LOW);
        delay(2500);
        digitalWrite(Zup,HIGH);
        delay(2000);
        x_times_up++;
        x_times_down=0;
      */
      
      // powinno otworzyć do prawie połowy
      //}else 
      if((now.hour()*60 + now.minute())>=(Sunrise/*+60*/) && (now.hour()*60 + now.minute())<(Sunset/*-60*/+15) && x_times_up<10){
        Serial.print("UP \n");
        delay(2000);
        digitalWrite(Zup,LOW);
        delay(2000);
        digitalWrite(Zup,HIGH);
        delay(2000);
        x_times_up++;
        x_times_down=0;

        // zamknięcie gdy ciemno
      }else if((now.hour()*60 + now.minute())>=(Sunset/*-60*/+15) && x_times_down<5){
        Serial.print("DOWN \n");
        delay(2000);
        digitalWrite(8,LOW);
        delay(4000);
        digitalWrite(8,HIGH);
        delay(2000);
        x_times_down++;
        x_times_up=0;
      }
    }else{
      aktywacja++;
      if(aktywacja>100)aktywacja=100;
    }
    /*
    if(changeState==0){
      Serial.print("HIGH \n");
      digitalWrite(8,HIGH);
      /digitalWrite(9,HIGH);
      changeState=1;
    }else{  
      Serial.print("LOW \n");
      digitalWrite(8,LOW);
      digitalWrite(9,LOW);
      changeState=0;
    }
    */    
    //USTAWIANIE JASNOSCI LCD
    /*
    if(fotores>=75){
      analogWrite(6,100);
    }else if(fotores>=50){
      analogWrite(6,80);
    }else if(fotores>=25){
      analogWrite(6,60);
    }else if(fotores>=10){
      analogWrite(6,10);
    }else{
      //analogWrite(6,7);
      analogWrite(6,5);
    }
    */
    // USTAWIENIA JASNOŚCI LCD W ZALEŻNOŚCI OD GODZINY

    if((now.hour()>(SunriseHour+1) && now.hour()<(SunsetHour-1)) || debug==1){
      analogWrite(6,100);
    }else 
    {
      if(now.hour()>(SunriseHour) && now.hour()<(SunsetHour)){
        analogWrite(6,50);
      }else{
        if(now.hour()>(SunriseHour-2) && now.hour()<(SunsetHour+2)){
          analogWrite(6,250);
        }else{
          analogWrite(6,0);
        }
      }
    }
    
    /*
    x = fotores *(10.0/8.0);
    if(x>100)x=100;
    analogWrite(6,x);
    Serial.print(" | jasnosc ");
    Serial.print(x);
    Serial.print("\n");
    */
    // TUTAJ WYSWIETLANIE Z CZUJNIKA WILGOCI I TEMPERATURY
    // tutaj DHT11
    //delay(10);
    float t = dht.readTemperature();
    //delay(10);
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h))
    {
    Serial.println("Blad odczytu danych z czujnika");
    }else
    {
      float hic = dht.computeHeatIndex(t, h, false);
      if(debug==1){
      Serial.print("Wilgotnosc: ");
      Serial.print(h);
      Serial.print("% Temperatura: ");
      Serial.print(t);
      Serial.print("C Index: ");
      Serial.print(hic);
      Serial.println("C ");
      }
      if(na_zmiane==0){
        str = "wp:";
        str += (int)h; 
        str+="%";
        str+=" ";
        //str+=t;
        str+=(int)hic;
        str+="C ";
      } else {
        str = now.year();
        str += "/";
        str += now.month();
        str += "/";
        str += now.day();
        str+= " ";
      }
      str+=now.hour();
      str+=":";
      if(now.minute()<10) str+="0";
      str+=now.minute();
      lcd.setCursor(0,1);lcd.print(str);
    }
  }
}
