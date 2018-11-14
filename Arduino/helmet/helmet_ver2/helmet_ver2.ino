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
char E_temp;

int result = 0;
String myString = "";//받는 문자열

const long interval = 3000; // 3초로 설정

//스피커 변수
int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
 


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


  long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
  }

void loop() {
  
    result = readVcc();
    btSerial.println(result);
    Serial.println(result);
    delay(100);

  if (btSerial.available()) { //버튼에 장착된 블루투스에서 넘어온 값이 있다면
    input = btSerial.read();
    
read :
    //왼쪽깜빡이
    if (input == 'L') {
      for (int x = 0; x < 5 ; x++) { //5번 깜빡임
        temp = btSerial.read();//현재 들어오는 버튼의 값 저장
        if (temp == 'E' || temp == 'R') { //다른 버튼인지 check
          input = temp;
          goto read;//다른 버튼이면 다시 조건문으로
        }

        for (uint16_t i = 0; i < 25; i++) {
          Serial.println("LLLLLLL ");
          strip.setPixelColor(i, strip.Color(255, 0, 0));
          strip.show();
        }
        delay(250);
        for (uint16_t i = 0; i < 25; i++) {
          strip.setPixelColor(i, 0);
          strip.show();
        }
      }
      colorWipe(strip.Color(0, 0, 0), 50);
    }

    //오른쪽 깜빡이
    if (input == 'R') {
      for ( int x = 0 ; x < 5; x++) {
        temp = btSerial.read();
        if (temp == 'L' || temp == 'E') {
          input = temp;
          goto read;
        }
        for (uint16_t i = strip.numPixels() - 1; i >= 25; i--) {
          strip.setPixelColor(i, strip.Color(255, 0, 0));
          strip.show();
        }
        delay(250);
        for (uint16_t i = strip.numPixels() - 1; i >= 25; i--) {
          strip.setPixelColor(i, 0);
          strip.show();
        }
      }
      colorWipe(strip.Color(0, 0, 0), 50);
    }

    //비상등 켯을 때
    if (input == 'E') {
      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 0));
        strip.show();
      }
    }
    
    if (input == 'S') {
      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);
        strip.show();
      }
    }
  }



}

//전체 조명 끄는 함수
void colorWipe(uint32_t c, uint8_t wait) {

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
  }
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
    strip.show();
  }
}

