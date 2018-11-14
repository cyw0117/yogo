#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define PIN 11
#define LEDNUM 50

//네오픽셀을 사용하기 위해 객체 하나를 생성한다.
//첫번째 인자값은 네오픽셀의 LED의 개수
//두번째 인자값은 네오픽셀이 연결된 아두이노의 핀번호
//세번째 인자값은 네오픽셀의 타입에 따라 바뀌는 flag
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDNUM, PIN, NEO_GRB + NEO_KHZ800);


SoftwareSerial btSerial(2, 3); //Tx = 2, Rx = 3
const int ledPin = 13;
char input;
char temp;

//시간 측정을 위한 변수
unsigned long previousMillis = 0;
const long interval = 3000; // 3초로 설정
unsigned long ledpreMillis = 0;
unsigned long offMillis = 0;


void setup() {
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);

  strip.begin(); //네오픽셀을 초기화하기 위해 모든LED를 off시킨다
  strip.show();

  while (!Serial) {
    ; //시리얼 통신이 연결되어 있지 않다면 코드 실행을 멈추고 무한 반복.
  }

  //블루투스와 아두이노 통신속도 설정
  btSerial.begin(9600);
}



void loop() {


  if (btSerial.available()) { //블루투스에서 넘어온 데이터가 있다면

    input = btSerial.read();

read :
    unsigned long currentMillis = millis();

    unsigned long startTime = 0;
    long interval = 3000;
    unsigned long currentTime = 0;

    if (input == 'L') {


      //함수내용
      currentTime = millis();
      startTime = 0;
      startTime = currentTime;

      for (int x = 0; x < 5 ; x++) {
        Serial.println("x문안");
        temp = btSerial.read();
        if (temp == 'E' || temp == 'R') { //다른 입력 check
          Serial.println("gotoRead");
          input = temp;
          goto read;
        }

        for (uint16_t i = 0; i < 25; i++) {

          Serial.println("Lets setPixel!");
          strip.setPixelColor(i, strip.Color(255, 0, 0));
          strip.show();
        }
        delay(250);

        for (uint16_t i = 0; i < 25; i++) {
          Serial.println("Set");
          strip.setPixelColor(i, 0);
          strip.show();
        }
      }

      Serial.println("LEFT TURN!!");
      //delay(500);

      colorWipe(strip.Color(0, 0, 0), 50);
    }


    if (input == 'R') {

      previousMillis = currentMillis;

      //함수내용
      currentTime = millis();
      startTime = 0;
      startTime = currentTime;
      for ( int x = 0 ; x < 5; x++) {
        temp = btSerial.read();
        if (temp == 'L' || temp == 'E') {
          input = temp;
          goto read;
        }
        for (uint16_t i = strip.numPixels()-1; i > 25; i--) {
          Serial.println("Set");
          strip.setPixelColor(i, strip.Color(255, 0, 0));
          strip.show();
        }
        delay(250);
        for (uint16_t i = strip.numPixels()-1; i > 25; i--) {
          strip.setPixelColor(i, 0);
          strip.show();
        }

      }




      Serial.println("RIGHT TURN!!");
     //delay (500);


      colorWipe(strip.Color(0, 0, 0), 50);
    }

    if (input == 'E') {
      previousMillis = currentMillis;


      //함수
      currentTime = millis();

      startTime = currentTime;


      for (int x = 0; x < 5 ; x++) {
        temp = btSerial.read();
        if (temp == 'L' || temp == 'R') {
          input = temp;
          goto read;
        }
        for (uint16_t i = 0; i < strip.numPixels(); i++) {
          Serial.println("Set");
          strip.setPixelColor(i, strip.Color(255, 0, 0));
          strip.show();
        }
        delay(250);
        for (uint16_t i = 0; i < strip.numPixels(); i++) {
          strip.setPixelColor(i, 0);
          strip.show();
        }

      }







      Serial.println("EMERGENCY!!");
      //delay(500);


      colorWipe(strip.Color(0, 0, 0), 50);

    }
  }
}


//전체 깜빡이 끄기
void colorWipe(uint32_t c, uint8_t wait) {

  unsigned long startTime = 0;
  long interval = 2000;
  unsigned long currentTime = millis();

  startTime = currentTime;

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    Serial.println("Set");
    strip.setPixelColor(i, c);
    strip.show();
  }
  delay(200);
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
    strip.show();
  }





}

/*
  // 왼쪽 깜빡이
  void colorWipeLeft(uint32_t c, uint8_t wait) {

  unsigned long startTime = 0;
  long interval = 3000;
  unsigned long currentTime = millis();

  if ((startTime == 0) && (currentTime - startTime < 3000)) {
    startTime = currentTime;


    for (int x = 0; x < 15 ; x++) {
      if (btSerial.read() == 'E' || btSerial.read() == 'R') {
        input = btSerial.read();
        //goto read;
      }
      for (uint16_t i = 0; i < 25; i++) {
        strip.setPixelColor(i, c);
        strip.show();
      }
      delay(250);

      for (uint16_t i = 0; i < 25; i++) {
        strip.setPixelColor(i, 0);
        strip.show();
      }
    }
    if (currentTime - startTime > interval) {
      startTime = 0;
      strip.show();
    }
  }
  }


  // 오른쪽 깜빡이
  void colorWipeRight(uint32_t c, uint8_t wait) {
  unsigned long startTime = 0;
  long interval = 2000;
  unsigned long currentTime = millis();

  if ((startTime == 0) && (currentTime - startTime < 3000)) {
    startTime = currentTime;
    for ( int x = 0 ; x < 15; x++) {
      if (btSerial.read() == 'L' || btSerial.read() == 'E') {
        input = btSerial.read();
        //goto read;
      }
      for (uint16_t i = 25; i <  strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        // delay(wait);
      }
      delay(250);
      for (uint16_t i = 25; i <  strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);
        strip.show();
      }

      if (currentTime - startTime > interval) {
        startTime = 0;
        strip.show();
      }
    }
  }

  }*/
