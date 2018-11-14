#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define PIN 11
#define LEDNUM 50

//부저 변수
const int BUZZER_PIN = 12;

const int C = 262;    //도
const int D = 294;    //레
const int E = 330;    //미
const int F = 349;    //파
const int G = 392;    //솔
const int A = 440;    //라
const int B = 493;    //시

typedef struct  {
  int tone;
  unsigned long delay;
} TAD; //Tone And Delay

//학교종이 땡땡땡
TAD music[] =
{
  {G, 100}, {G, 100}, {A, 100}, {A, 100}, {G, 100}, {G, 100}, {E, 200},
  {G, 100}, {G, 100}, {E, 100}, {E, 100}, {D, 200}, {G, 100}, {G, 100},
  {A, 100}, {A, 100}, {G, 100}, {G, 100}, {E, 200}, {G, 100}, {E, 100},
  {D, 100}, {E, 100}, {C, 200}
};

int musicLen;


//배터리 세그먼트 변수
int fnd_pin_num[] = {4, 5, 6, 7, 8, 9}; // 배터리 세그먼트 핀번호
int battery_segment[6][6] = { // 배터리 세그먼트 입력 배열
  {1, 1, 1, 1, 1, 0},
  {0, 1, 1, 1, 1, 1},
  {0, 0, 1, 1, 1, 1},
  {0, 0, 0, 1, 1, 1},
  {0, 0, 0, 0, 1, 1},
  {0, 0, 0, 0, 0, 1},
};


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

String myString = "";//받는 문자열

const long interval = 3000; // 3초로 설정


int result = 0;
float vout = 0.0;
float vin = 0.0;
//float R1 = 100000.0; // resistance of R1 (100K) -see text!
//float R2 = 10000.0; // resistance of R2 (10K) - see text!
int volt_value = 0;



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

  //4V 이하

  delay(4000);
    pinMode(BUZZER_PIN, OUTPUT);
    musicLen = sizeof(music) / sizeof(TAD);

    for (int i = 0; i < musicLen; i++) {
      tone(BUZZER_PIN, music[i].tone);
      delay(music[i].delay * 5);

      noTone(BUZZER_PIN);
      delay(30);
    }
    noTone(BUZZER_PIN);
    delay(1000);
 

}

//배터리 표시 함수
void battery() {
  readVcc(); // 전압
  vin = readVcc();//두번째 체크

  if (5000 < vin) { // 5V 이상
    volt_value = 6;
  }
  else if (4700 < vin && vin <= 5000) {//4.7V~5V
    volt_value = 5;
  }
  else if (4500 < vin && vin <= 4700) { //4.5V~4.7V
    volt_value = 4;
  }
  else if (4000 < vin && vin <= 4500) { //4V~4.5V
    volt_value = 3;
  }
  else if (3500 < vin && vin <= 4000) { // 3.5V~4V
    volt_value = 2;
  }
  else if ( vin <= 3500) { //3.5V이하
    volt_value = 1;
  }


  for ( int fnd_pin = 0; fnd_pin < 6; fnd_pin++) {
    digitalWrite(fnd_pin_num[fnd_pin], battery_segment[volt_value - 1][fnd_pin]); //세그먼트 표시
  }

  Serial.println(volt_value);


  //4V 이하
  if (volt_value <= 2) {
    pinMode(BUZZER_PIN, OUTPUT);
    musicLen = sizeof(music) / sizeof(TAD);

    for (int i = 0; i < musicLen; i++) {
      tone(BUZZER_PIN, music[i].tone);
      delay(music[i].delay * 5);

      noTone(BUZZER_PIN);
      delay(30);
    }
    noTone(BUZZER_PIN);
    delay(1000);
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

void setup() {


  Serial.begin(9600);

  strip.begin(); //네오픽셀을 초기화하기 위해 모든LED를 off시킨다
  strip.setBrightness(30);
  strip.show();

  while (!Serial) {
    ; //시리얼 통신이 연결되어 있지 않다면 코드 실행을 멈추고 무한 반복.
  }
  //블루투스와 아두이노 통신속도 설정
  btSerial.begin(9600);

  //배터리 세그먼트 on
  for (int i = 4; i < 10; i++) {
    pinMode(i, OUTPUT);
  }

  battery();

}


