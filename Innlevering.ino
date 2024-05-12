#include <Wire.h> 
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <Adafruit_NeoPixel.h>

const int rod = 10;
const int bla = 7;
const int lys_rod = 13;
const int lys_bla = 6;

const int LED_PIN_1 = 8;
const int LED_PIN_2 = 9;

const int rod_dioder = 12;
const int bla_dioder = 4;

#define LED_COUNT 20

int i = 0;
int x = 0;

float highscore = 100.00; //Setter en high score som er veldig lett å slå
float highscore_tid = 0.00;

hd44780_I2Cexp lcd_1(0x27);
hd44780_I2Cexp lcd_2(0x26);
hd44780_I2Cexp lcd_H(0x25);

Adafruit_NeoPixel strip_rod(LED_COUNT, LED_PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_bla(LED_COUNT, LED_PIN_2, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(rod, INPUT_PULLUP);
  pinMode(bla, INPUT_PULLUP);
  pinMode(lys_rod, OUTPUT);
  pinMode(lys_bla, OUTPUT);
  pinMode(rod_dioder, OUTPUT);
  pinMode(bla_dioder, OUTPUT);

  strip_rod.begin();
  strip_bla.begin();
  strip_rod.show();
  strip_bla.show();

  lcd_1.begin(16, 2);
  lcd_2.begin(16, 2);
  lcd_H.begin(20, 4);

  lcd_H.clear(); //Starter med dagens beste på 0,00 sekunder
  lcd_H.setCursor(3,1);
  lcd_H.print("DAGENS BESTE:");
  lcd_H.setCursor(8,2);
  lcd_H.print(highscore_tid);
  lcd_H.setCursor(6,3);
  lcd_H.print("SEKUNDER");
}

void blinkBlue(Adafruit_NeoPixel &strip, Adafruit_NeoPixel &otherStrip) {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < strip.numPixels(); j++) {
      strip.setPixelColor(j, 0, 0, 255); // Blå
    }
    strip.show();
    digitalWrite(lys_bla, HIGH);
    digitalWrite(bla_dioder, HIGH);
    delay(250); 
    strip.clear(); 
    digitalWrite(lys_bla, LOW);
    digitalWrite(bla_dioder, LOW);
    strip.show();
    delay(250);
  }
  otherStrip.clear(); 
  otherStrip.show();
}

void blinkRed(Adafruit_NeoPixel &strip, Adafruit_NeoPixel &otherStrip) {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < strip.numPixels(); j++) {
      strip.setPixelColor(j, 255, 0, 0); // Rød
    }
    strip.show();
    digitalWrite(lys_rod, HIGH);
    digitalWrite(rod_dioder, HIGH);
    delay(250);
    strip.clear();
    digitalWrite(lys_rod, LOW);
    digitalWrite(rod_dioder, LOW);
    strip.show();
    delay(250); 
  }
  otherStrip.clear();
  otherStrip.show();
}

void resetLEDs(Adafruit_NeoPixel *strip_rod, Adafruit_NeoPixel *strip_bla) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip_rod->setPixelColor(i, 0, 0, 0); // Svart
    strip_bla->setPixelColor(i, 0, 0, 0); // Svart
  }
  strip_rod->show();
  strip_bla->show();
}

void sjekkeHighscore(float poeng_rod, float poeng_bla, float sekunder_rod, float sekunder_bla) { //Sjekker og oppdaterer high score
  if (abs(poeng_rod) < abs(highscore)) {
    highscore = poeng_rod; //Oppdaterer high scoren
    highscore_tid = sekunder_rod;
    lcd_H.clear();
    lcd_H.setCursor(3,1);
    lcd_H.print("GRATULERER MED");
    lcd_H.setCursor(5,2);
    lcd_H.print("NY REKORD!");
  }
  if (abs(poeng_bla) < abs(highscore)) {
    highscore = poeng_bla; //Oppdaterer highscoren
    highscore_tid = sekunder_bla;
    lcd_H.clear();
    lcd_H.setCursor(3,1);
    lcd_H.print("GRATULERER MED");
    lcd_H.setCursor(5,2);
    lcd_H.print("NY REKORD!");
  }
}

void loop() {
  int knapp_rod = digitalRead(rod);
  int knapp_bla = digitalRead(bla);
  float sekunder_rod = 0;
  float sekunder_bla = 0;

  while (knapp_rod == 0 || knapp_bla == 0) { //Tidtakingen skjer hele tiden mens én eller begge trykker ned knappen
  
    if (knapp_rod == 0 && knapp_bla == 1) { //Hvis kun rød trykker
      sekunder_rod += 0.005;
      digitalWrite(lys_rod, HIGH);
      digitalWrite(lys_bla, LOW);
    }

    if (knapp_rod == 0 && knapp_bla == 0) { //Hvis begge trykker
      sekunder_rod += 0.005;
      sekunder_bla += 0.005;
      digitalWrite(lys_rod, HIGH);
      digitalWrite(lys_bla, HIGH);
    }

    if (knapp_rod == 1 && knapp_bla == 0) { //Hvis kun blå trykker
      sekunder_bla += 0.005;
      digitalWrite(lys_bla, HIGH);
      digitalWrite(lys_rod, LOW);
    }

    delay(5);

    knapp_rod = digitalRead(rod); //Sjekker om knappen fortsatt blir trykket på
    knapp_bla = digitalRead(bla);
    
  }
  digitalWrite(lys_rod, LOW);
  digitalWrite(lys_bla, LOW);

  if ((sekunder_rod > 0.1 || sekunder_bla > 0.1) && (knapp_rod == 1 && knapp_bla == 1)) { //Poengene kommer når begge har sluppet knappen

    int heltall_rod = floor(sekunder_rod); // Runder av til nærmeste heltall
    int heltall_bla = floor(sekunder_bla);

  if (heltall_rod >= 0 && heltall_rod <= 20 || heltall_bla >= 0 && heltall_bla <= 20) {
    int maxNum = max(heltall_rod, heltall_bla);
    resetLEDs(&strip_rod, &strip_bla);

    strip_rod.clear();
    strip_bla.clear();
    strip_rod.show();
    strip_bla.show();

    for (int i = 0; i < maxNum; i++) {
      if (i < heltall_rod) {
        strip_rod.setPixelColor(i, 255, 0, 0); //Rød
      }
      if (i < heltall_bla) {
        strip_bla.setPixelColor(i, 0, 0, 255); //Blå
      }
      strip_rod.show();
      strip_bla.show();
      delay(400);
    }

    delay(500);

    lcd_1.setCursor(6,0); // Resultatet på skjermen kommer opp ETTER resultatet på led-stripene
    lcd_1.print(sekunder_rod);
    lcd_1.setCursor(4,1);
    lcd_1.print("sekunder");

    lcd_2.setCursor(6,0);
    lcd_2.print(sekunder_bla);
    lcd_2.setCursor(4,1);
    lcd_2.print("sekunder");

    float poeng_rod = sekunder_rod - 10; //Regner ut poengene
    float poeng_bla = sekunder_bla - 10;

    if (abs(poeng_rod) > abs(poeng_bla)) { //Den som har "minst" poeng, altså den som kommer nærmest 10, vinner
      delay(500);
      blinkBlue(strip_bla, strip_rod); // Blå blinker, den andre blir svart
    } else {
      delay(500);
      blinkRed(strip_rod, strip_bla); // Rød blinker, den andre blir svart
    }
    sjekkeHighscore(poeng_rod, poeng_bla, sekunder_rod, sekunder_bla); //Sjekker og evetuelt oppdaterer high score
  }

  delay(2000); //2 sekunder delay før spillet reset-es
  resetLEDs(&strip_rod, &strip_bla);
  lcd_1.clear();
  lcd_2.clear();

  lcd_H.clear(); //Skriver ut den nye (eller gamle) high scoren
  lcd_H.setCursor(3,1);
  lcd_H.print("DAGENS BESTE:");
  lcd_H.setCursor(8,2);
  lcd_H.print(highscore_tid);
  lcd_H.setCursor(6,3);
  lcd_H.print("SEKUNDER");
  }
}

/*

Bibliotek:
Adafruit NeoPixel   av Adafruit                                       https://github.com/adafruit/Adafruit_LiquidCrystal
hd44780             av Bill Perry       bperrybap@opensource.bi       https://github.com/duinoWitchery/hd44780

Gruppemedlemmer:
Erlend Andersen
Markus Vold Aspenes
Oda Hegedatter Makumbi Bentzen
Åshild Hellerslia Akselsen

*/