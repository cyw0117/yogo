#include <SoftwareSerial.h>

int fnd_pin_num[] = {4, 5, 6, 7, 8, 9}; // 배터리 세그먼트 핀번호

int battery_segment[6][6] = { // 배터리 세그먼트 입력 배열
  {1, 1, 1, 1, 1, 0},
  {0, 1, 1, 1, 1, 1},
  {0, 0, 1, 1, 1, 1},
  {0, 0, 0, 1, 1, 1},
  {0, 0, 0, 0, 1, 1},
  {0, 0, 0, 0, 0, 1},
};


int result = 0;
int check;
float vout = 0.0;
float vin = 0.0;
//float R1 = 100000.0; // resistance of R1 (100K) -see text!
//float R2 = 10000.0; // resistance of R2 (10K) - see text!
int volt_value = 0;


void setup() {
  check = 1;

  Serial.begin(9600);
  for (int i = 4; i < 10; i++) {
    pinMode(i, OUTPUT);
  }

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
  if (check == 1) {
    vin = readVcc(); // 전압

    if (8000 < vin) { // 8V 이상
      volt_value = 6;
    }
    else if (7000 < vin && vin <= 8000) { //7V~8V
      volt_value = 5;
    }
    else if (6500 < vin && vin <= 7000) { //6.5V~7V
      volt_value = 4;
    }
    else if (6000 < vin && vin <= 6500) { //6V~6.5V
      volt_value = 3;
    }
    else if (5000 < vin && vin <= 6000) { // 5V~6V
      volt_value = 2;
    }
    else if ( vin <= 5000) { //5V이하
      volt_value = 1;
    }

    Serial.println(volt_value);
    for ( int fnd_pin = 0; fnd_pin < 6; fnd_pin++) {
      digitalWrite(fnd_pin_num[fnd_pin], battery_segment[volt_value - 1][fnd_pin]); //세그먼트 표시
    }
  }


}
