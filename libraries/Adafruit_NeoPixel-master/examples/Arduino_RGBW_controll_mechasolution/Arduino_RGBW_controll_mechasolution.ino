#include <Adafruit_NeoPixel.h>   // 네오픽셀 라이브러리를 불러옵니다.

#define PIN 6                      // 디지털핀 어디에 연결했는지 입력
#define LEDNUM 20                  // 연결된 네오픽셀의 숫자입력
#define BRIGHTNESS 50               // 네오픽셀의 밝기를 설정합니다. (0~255)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDNUM, PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  strip.setBrightness(BRIGHTNESS);
}

void loop() {                              // 이 안에 입력한 내용들이 반복 실행됩니다
  strip.begin();                           // 네오픽셀 제어시작
  strip.show();                            // 네오픽셀 초기화
  strip.setPixelColor(0, 255, 0, 255, 0);     // (A,R,G,B,W) A번째 LED를 RGBW (0~255) 만큼의 밝기로 켭니다.
}

