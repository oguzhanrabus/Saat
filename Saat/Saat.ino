//----------Kütüphaneler----------
#include "RTClib.h"  //Gerçek Zaman Saati Kütüphanesi
#include "DHT.h" //Isı ve Nem Sensörü 
#include "Wire.h" //I2C Haberleşme kütüphanesi (A4 - SDA , A5 - SCL)
#include "SevSeg.h" //7 Segment Display Kütüphanesi

//----------Arduino pin Ayarları----------
int Led = 9; //Maketi Aydınlatan ledler
int Led_S1 = A2; //Saniye ledi-1
int Led_S2 = A3; //Saniye ledi-2

//----------7Seg Display tanımlamaları----------
SevSeg display7seg; //Display ismi
int digit1 = 10;
int digit2 = 11;
int digit3 = 12;
int digit4 = 13;

int segA = 2;
int segB = 3;
int segC = 4;
int segD = 5;
int segE = 6;
int segF = 7;
int segG = 8;
int segDP;
int hardwareConfig;
int numDigits;

//----------I2C ayarları----------
int valor = 0;
byte bilgiyaz = 0x00; //DS3231 yazma adresi

//----------RTC Ayarları----------
#define DS3231_ADDRESS 0x68
RTC_DS3231 RTC;

//---------- DHT11 Ayarları----------
#define DHTPIN A1 //Isı ve nem sensörünün bağlandığı pin
#define DHTTYPE DHT11 //Isı ve nem sensörünün ismi
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Wire.begin(); // I2C'yi başlat
  RTC.begin(); // RTC'yi başlat
  dht.begin(); //DHT'yi başlat

  //----------RTC Ayarlama----------
  //yıl , ay , gün , saat , dakika , saniye
  //  RTC.adjust(DateTime(2019,10,16,11,45,30));

  //----------Display Ayarları----------
  int displayType = COMMON_ANODE; //Display ortak anot ve katot olarak seç.
  int numberOfDigits = 4; //Kaç display kullanılacak.
  display7seg.SetBrightness(255);//Display Parlaklığı
  display7seg.Begin(displayType, numberOfDigits, digit1, digit2, digit3, digit4, segA, segB, segC, segD, segE, segF, segG, segDP);

  //----------Pin Ayarları----------
  pinMode(Led, OUTPUT);
  pinMode(Led_S1, OUTPUT);
  pinMode(Led_S2, OUTPUT);

  //Açılış Testi
  digitalWrite(digit1, HIGH);
  digitalWrite(digit2, HIGH);
  digitalWrite(digit3, HIGH);
  digitalWrite(digit4, HIGH);
  delay(1000);
  digitalWrite(segA, LOW);
  delay(1000);
  digitalWrite(segB, LOW);
  delay(1000);
  digitalWrite(segC, LOW);
  delay(1000);
  digitalWrite(segD, LOW);
  delay(1000);
  digitalWrite(segE, LOW);
  delay(1000);
  digitalWrite(segF, LOW);
  delay(1000);
  digitalWrite(segG, LOW);
  delay(1000);
  digitalWrite(Led_S1, HIGH);
  delay(1000);
  digitalWrite(Led_S2, HIGH);
  delay(1000);
}
void loop() {
  DateTime now = RTC.now(); //Zaman bilgilerini al
  char tempString[10]; //Dize Hızı
  Wire.beginTransmission(DS3231_ADDRESS); //İletişime başla
  Wire.write(bilgiyaz); //bilgiyaz değişkenine yaz
  Wire.endTransmission(); //İletişimi durdur
  Wire.requestFrom(DS3231_ADDRESS, 7); //Bilgi alma isteği gönder

  // DS3231'den gelen bilgi ikilik yani Binary (0-1)sayı sisteminde gelir.Displaya yazdırmak için ikilik sayı sistemini onluk(decimal)sayı sistemine çeviriyoruz.Bu bilgileri sıralamayı değiştirmeden alıyoruz.
  int saniye = ConvertToDecimal(Wire.read()); //saniye bilgisi
  int dakika = ConvertToDecimal(Wire.read()); //dakika bilgisi
  int saat = ConvertToDecimal(Wire.read() & 0b111111); //saat bilgisi ve (0b111111) 24 saat bilgisi
  int dow  = ConvertToDecimal(Wire.read());
  int gun = ConvertToDecimal(Wire.read()); //gün bilgisi
  int ay = ConvertToDecimal(Wire.read()); //ay bilgisi
  int yil = ConvertToDecimal(Wire.read() ); //yıl bilgisi

  //----------sıcaklık----------
  if (saniye == 21 || saniye == 22 || saniye == 50 || saniye == 51)
  {
    byte s = dht.readTemperature();
    sprintf(tempString, "%02d%01cC%", s, '"');
    display7seg.DisplayString(tempString, 0); //DP ayarlama
    digitalWrite(Led_S1, LOW);
    digitalWrite(Led_S2, LOW);
  }
  //----------nem----------
  else if ( saniye == 23 || saniye == 24 || saniye == 52 || saniye == 53)
  {
    byte n = dht.readHumidity();
    sprintf(tempString, "%01cH%02d%", 'r', n);
  display7seg.DisplayString(tempString, 0); //DP ayarlama
    digitalWrite(Led_S1, HIGH);
    digitalWrite(Led_S2, HIGH);
  }
  //----------gün ay----------
  else if (saniye == 25 || saniye == 26 || saniye == 54 || saniye == 55)
  {
    sprintf(tempString, "%02d%02d", gun, ay);
    display7seg.DisplayString(tempString, 0); //DP ayarlama
    digitalWrite(Led_S1, LOW);
    digitalWrite(Led_S2, HIGH);
  }
  //----------yıl----------
  else if (saniye == 27 || saniye == 28 || saniye == 56  || saniye == 57)
  {
    sprintf(tempString, "%02d%02d", 20, yil);
    display7seg.DisplayString(tempString, 0); //DP ayarlama
    digitalWrite(Led_S1, LOW);
    digitalWrite(Led_S2, LOW);
  }
  //----------saat dakika----------
  else
    sprintf(tempString, "%02d%02d", saat, dakika);
  display7seg.DisplayString(tempString, 0); //DP ayarlama
  //----------Saniye Ledleri----------
  if (saniye == 0 || saniye == 2 || saniye == 4 || saniye == 6 || saniye == 8 || saniye == 10 || saniye == 12 || saniye == 14 || saniye == 16 || saniye == 18 || saniye == 20 || saniye == 30 || saniye == 32 || saniye == 34 || saniye == 36 || saniye == 38 || saniye == 40 || saniye == 42 || saniye == 44 || saniye == 46 || saniye == 48 || saniye ==  58)
  {
    digitalWrite(Led_S1, HIGH);
    digitalWrite(Led_S2, HIGH);
  }
  else
  {
    digitalWrite(Led_S1, LOW);
    digitalWrite(Led_S2, LOW);
  }

  //----------Maket Aydınlatma----------
  {
    int deger = analogRead(A0);
    deger = map(deger, 0, 1023, 255, 0);
    if (deger <= 65)
      analogWrite(Led, 0);
    else
      analogWrite(Led, deger);
  }
}
byte ConvertToDecimal(byte val)
{
  //İkilik sayı sisteminden onluk sayı sistemine çevirme
  return ( (val / 16 * 10) + (val % 16) );
}
