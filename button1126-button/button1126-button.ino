 #include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <SPI.h>

/* NeoPixel */
#define LPIN 9
#define RPIN 8
#define LEDNUM 16

Adafruit_NeoPixel Lhand = Adafruit_NeoPixel(LEDNUM, LPIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Rhand = Adafruit_NeoPixel(LEDNUM, RPIN, NEO_GRB + NEO_KHZ800);


/* button */
const int LEFT = 6;
const int EMER = 5;
const int RIGHT = 4;

int LState = 0;
int EState = 0;
int RState = 0;
int LPrevious = LOW;
int EPrevious = LOW;
int RPrevious = LOW;
boolean IFlag = false;

long current = 0;       //
long debounce = 100; // Debounce 타임 설정


SoftwareSerial btSerial(2, 3); //Tx = 2, Rx = 3
char input;
char temp;

void setup()
{
  /* button */
  pinMode(LEFT, INPUT);
  pinMode(EMER, INPUT);
  pinMode(RIGHT, INPUT);
  /* SPI통신 */
  pinMode(MISO, OUTPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);
  pinMode(SS, INPUT);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPCR |= _BV(SPE);
  SPCR &= ~_BV(MSTR);
  SPCR |= _BV(SPIE);

  Serial.begin(9600); 
  Lhand.begin(); Rhand.begin(); //네오픽셀 초기화
  Lhand.setBrightness(30); Rhand.setBrightness(30);
  Lhand.show(); Rhand.show();

  while (!Serial) {
    ; //시리얼 통신이 연결되어 있지 않다면 코드 실행을 멈추고 무한 반복.
  }
  //블루투스와 아두이노 통신속도 설정
  btSerial.begin(9600);
}

ISR(SPI_STC_vect)
{
  byte c = SPDR;
  if (c == 'E') {
    IFlag = true;
    btSerial.print("E");
    Serial.println("E");
    for (uint16_t i = 0; i < LEDNUM; i++) {
      Lhand.setPixelColor(i, Lhand.Color(255, 255, 0));
      Rhand.setPixelColor(i, Rhand.Color(255, 255, 0));
      Lhand.show(); Rhand.show();
    }
    current = millis();
  }

  if (c == 'S') {
    IFlag = true;
    btSerial.print("F");
    Serial.println("S");
    for (uint16_t i = 0; i < LEDNUM; i++) {
      Lhand.setPixelColor(i, Lhand.Color(0,0,0));
      Rhand.setPixelColor(i, Rhand.Color(0,0,0));
      Lhand.show(); Rhand.show();
      Serial.println("꺼져라~!!!!!!");
    }
    current = millis();
  }

  if (c == 'Q') {
    btSerial.print("Q");
    Serial.println("Q");
  }
}


void loop()
{
  //Serial.println("처음");
  LState = digitalRead(LEFT);
  EState = digitalRead(EMER);
  RState = digitalRead(RIGHT);


read :
  /* LEFT 버튼 눌렸을 때 */
  if (LState == HIGH && LPrevious == LOW && millis() - current > debounce) {
    btSerial.print("L");
    Serial.println("L");
    /* Turn On */
    for (int x = 0; x < 50; x++) {
      /* Interrupt */
      if (digitalRead(RIGHT) == HIGH) {
        LState = LOW; RState = HIGH;
        goto read;
      }
      if (digitalRead(EMER) == HIGH) {
        LState = LOW; EState = HIGH;
        goto read;
      }

      /* Light On */
      for (uint16_t i = 0; i < LEDNUM; i++) {
        if(IFlag == true){
          IFlag = false;
          goto read;
        }
        Lhand.setPixelColor(i, Lhand.Color(255, 255, 0));
        Lhand.show();
      }
      delay(310);
      for (uint16_t i = 0; i < LEDNUM; i++) {
        Lhand.setPixelColor(i, 0);
        Lhand.show();
      }
    }
    Lhand.clear();
    current = millis();
  }

  /* RIGHT 버튼 눌렸을 때 */
  if (RState == HIGH && RPrevious == LOW && millis() - current > debounce) {
    btSerial.print("R");
    Serial.println("R");
    /* Turn On */
    for (int x = 0; x < 50; x++) {
      /* Interrupt */
      if (digitalRead(LEFT) == HIGH) {
        RState = LOW; LState = HIGH; //LPrevious = LOW;
        goto read;
      }
      if (digitalRead(EMER) == HIGH) {
        RState = LOW; EState = HIGH;
        goto read;
      }


      /* Light On */
      for (uint16_t i = 0; i < LEDNUM; i++) {
        if(IFlag == true){
          IFlag = false;
          goto read;
        }
        Rhand.setPixelColor(i, Rhand.Color(255, 255, 0));
        Rhand.show();
      }
      delay(310);
      for (uint16_t i = 0; i < LEDNUM; i++) {
        Rhand.setPixelColor(i, 0);
        Rhand.show();
      }
    }
    Rhand.clear();
    current = millis();
  }

  /* EMER 버튼 켰을 때 */
  if (EState == HIGH && EPrevious == LOW && millis() - current > debounce) {
    btSerial.print("E");
    Serial.println("E");
    for (uint16_t i = 0; i < LEDNUM; i++) {
      if(IFlag == true){
          IFlag = false;
          goto read;
        }
      Lhand.setPixelColor(i, Lhand.Color(255, 255, 0));
      Rhand.setPixelColor(i, Rhand.Color(255, 255, 0));
      Lhand.show(); Rhand.show();
    }
    current = millis();
  }

  /* EMER 버튼 껐을 때 */
  if (EState == LOW && EPrevious == HIGH && millis() - current > debounce / 2) {
    btSerial.print("S");
    Serial.println("S");
    for (uint16_t i = 0; i < LEDNUM; i++) {
      Lhand.setPixelColor(i, 0);
      Rhand.setPixelColor(i, 0);
      Lhand.show(); Rhand.show();
    }
    current = millis();
  }

  LPrevious = LState;
  RPrevious = RState;
  EPrevious = EState;

}

