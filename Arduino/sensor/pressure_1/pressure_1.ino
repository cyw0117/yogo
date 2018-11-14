#include <SPI.h>

int Emergency = 0 ;

void setup() {
  SPI.begin();//SPI 통신 초기화
  digitalWrite(SS,HIGH);//슬레이브가 선택되지 않은 상태로 유지 
  SPI.setClockDivider(SPI_CLOCK_DIV16);//분주비를 높여 전송 속도를 낮춤
  // 센서값을 측정하기위해 시리얼통신을 준비합니다.
  Serial.begin(9600); 
}

void loop() {
  //아날로그 0번 핀에 압력센서를 연결합니다.
  int pressure_sensor = analogRead(A0);
 
  if(Emergency == 0 && pressure_sensor > 400){
    Emergency = 1; 
    digitalWrite(SS,LOW); //슬레이브를 선택한다.
    SPI.transfer('E');
  }

  if(Emergency == 1 && pressure_sensor == 0 ){
    digitalWrite(SS,LOW); //슬레이브를 선택한다.
    SPI.transfer('S');
    Emergency = 0;
  }
  

  //시리얼 모니터를 통해 센서값과 LED의 밝기를 표기합니다. 
  Serial.println("Sensorvalue : " +String(pressure_sensor));
  
  //100ms동안 대기합니다. 
}
