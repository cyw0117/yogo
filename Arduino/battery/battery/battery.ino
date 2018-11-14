#include <SoftwareSerial.h>

int result = 0;
String myString = "";//받는 문자열

SoftwareSerial btSerial(2,3); //Tx = 2, Rx = 3

void setup(){
  Serial.begin(9600);

  while(!Serial){
    ; //시리얼 통신이 연결되어 있지 않다면 코드 실행을 멈추고 무한 반복
  }

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
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

void loop(){
  result = readVcc();
  btSerial.println(result);
  delay(1000);
  
  /*
  if(Serial.available()){ //시리얼 모니터에 입력된 값이 있다면
    btSerial.write(Serial.read()); //블루투스를 통해 시리얼 모니터에 입력된 값 전달.
    //내 입력을 넘겨주는거
  }

  while(btSerial.available()){ //입력받은거 출력하는 곳
    char myChar = (char)btSerial.read();
    myString += myChar;
    delay(5);
  }
  if(!myString.equals("")){
    Serial.println(myString);
    myString = "";
  }
  */
}
