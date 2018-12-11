#include "Wire.h"
#include "Kalman.h"
#include "I2Cdev.h"
#include "mthread.h"
#include <SPI.h>

/* TURN */
const int XPrevious = 168; //168~170
const int YPrevious = 141; //144~146
const int XRBound = 161; //우회전 boundary
const int YRBound = 140;
const int XLBound = 173; //좌회전 boundary
const int YLBound = 145;
int XState = 0;
int YState = 0;
int XDiff = 0;
int YDiff = 0;
int Dflag = 0; //회전했다!
boolean QFlag = false;
unsigned long Start = 0; //회전시작
unsigned long End = 0; //회전끝

/* PRESS */
boolean Emergency = 0;
const int ELOW = 5; //안눌렸을때
const int EHIGH = 190; //다눌렸을때
const int LOCK = 4;
int LState = 0;

/* Kalman Filter */
Kalman kalmanX; // Create the Kalman instances
Kalman kalmanY;

/* IMU Data */
int16_t accX, accY, accZ;
int16_t tempRaw;
int16_t gyroX, gyroY, gyroZ;

double accXangle, accYangle; // Angle calculate using the accelerometer
double temp; // Temperature
double gyroXangle, gyroYangle; // Angle calculate using the gyro
double compAngleX, compAngleY; // Calculate the angle using a complementary filter
double kalAngleX, kalAngleY; // Calculate the angle using a Kalman filter

uint32_t timer;
uint8_t i2cData[14]; // Buffer for I2C data

void angleValue() {
  /* Update all the values */
  while (i2cRead(0x3B, i2cData, 14));
  accX = ((i2cData[0] << 8) | i2cData[1]);
  accY = ((i2cData[2] << 8) | i2cData[3]);
  accZ = ((i2cData[4] << 8) | i2cData[5]);
  tempRaw = ((i2cData[6] << 8) | i2cData[7]);
  gyroX = ((i2cData[8] << 8) | i2cData[9]);
  gyroY = ((i2cData[10] << 8) | i2cData[11]);
  gyroZ = ((i2cData[12] << 8) | i2cData[13]);

  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // We then convert it to 0 to 2π and then from radians to degrees
  accXangle = (atan2(accY, accZ) + PI) * RAD_TO_DEG;
  accYangle = (atan2(accX, accZ) + PI) * RAD_TO_DEG;

  double gyroXrate = (double)gyroX / 131.0;
  double gyroYrate = -((double)gyroY / 131.0);
  gyroXangle += gyroXrate * ((double)(micros() - timer) / 1000000); // Calculate gyro angle without any filter
  gyroYangle += gyroYrate * ((double)(micros() - timer) / 1000000);


  compAngleX = (0.93 * (compAngleX + (gyroXrate * (double)(micros() - timer) / 1000000))) + (0.07 * accXangle); // Calculate the angle using a Complimentary filter
  compAngleY = (0.93 * (compAngleY + (gyroYrate * (double)(micros() - timer) / 1000000))) + (0.07 * accYangle);

  kalAngleX = kalmanX.getAngle(accXangle, gyroXrate, (double)(micros() - timer) / 1000000); // Calculate the angle using a Kalman filter
  kalAngleY = kalmanY.getAngle(accYangle, gyroYrate, (double)(micros() - timer) / 1000000);
  timer = micros();

  temp = ((double)tempRaw + 12412.0) / 340.0;


}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================
class MThread : public Thread {
  public:
    MThread(int id);
  protected:
    bool loop();
  private:
    int id;
};

MThread::MThread(int id) {
  this->id = id;
}

bool MThread::loop() {
  if (id == 1) {
    LState = digitalRead(LOCK);
    angleValue();
    XState = kalAngleX;
    YState = kalAngleY;

    
    Serial.print(kalAngleX);
    Serial.print("    ");
    Serial.println(kalAngleY);
    
    
    if (LState == LOW) {
      /*자동방향등 알고리즘*/
      if (Dflag == 0) {
        if (XState < XRBound && YState < YRBound) { //우회전
          Dflag = 1;
          Start = millis();
          Serial.println("우회전!!!!!!!!!!!!!!!!!!!!!!!");
        } else if (XState > XLBound && YState > YLBound) { //좌회전
          Dflag = 2;
          Start = millis();
          Serial.println("좌회전!!!!!!!!!!!!!!!!!!!!!!!!");
        }
      } else if (Dflag == 1) { //우회전이 끝남
        if (166 < XState && 139 < YState) {
          End = millis();
          if ((End - Start) > 3750) {
            Dflag = 0;
            digitalWrite(SS, LOW);
            SPI.transfer('S');
            Serial.println("끝났다!!!!!!!!!!!!!!!!!");
          }
        }
      } else if (Dflag == 2) {
        if (XState < 171 && YState < 145) {//좌회전이 끝남
          End = millis();
          if ((End - Start) > 3750) {
            Dflag = 0;
            digitalWrite(SS, LOW);
            SPI.transfer('S');
            Serial.println("끝났다!");
          }
        }
      }
    } else if (LState == HIGH) {
      /*도난방지 알고리즘*/
      if (XState < XRBound || XState > XLBound) {
        digitalWrite(SS, LOW);
        SPI.transfer('Q');
        Serial.println("도난당했어!");
        delay(300);
        //Serial.println(XState);
      }
    }
    delay(20);
    return true;
  }//TURN THREAD


  if (id == 2) {
    LState = digitalRead(LOCK);
    if (LState == LOW) {
      int pressure = analogRead(A0);

      if (Emergency == false && pressure > EHIGH) {
        Emergency = true;
        digitalWrite(SS, LOW);
        SPI.transfer('E');
      }

      if (Emergency == true && pressure < ELOW) {
        Emergency = false;
        digitalWrite(SS, LOW);
        SPI.transfer('S');
      }
      //Serial.println("Sensorvalue : " + String(pressure));
    }
    return true;
  }//press sensor
}






// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
void setup() {
  Wire.begin();
  pinMode(LOCK, INPUT);
  SPI.begin(); //SPI통신 초기화
  digitalWrite(SS, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  Serial.begin(4800);

  /* sensor */
  Wire.begin();
  i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
  i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cData[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
  i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
  while (i2cWrite(0x19, i2cData, 4, false)); // Write to all four registers at once
  while (i2cWrite(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode
  while (i2cRead(0x75, i2cData, 1));

  delay(500); // Wait for sensor to stabilize

  /* Set kalman and gyro starting angle */
  while (i2cRead(0x3B, i2cData, 6));
  accX = ((i2cData[0] << 8) | i2cData[1]);
  accY = ((i2cData[2] << 8) | i2cData[3]);
  accZ = ((i2cData[4] << 8) | i2cData[5]);
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // We then convert it to 0 to 2π and then from radians to degrees
  accYangle = (atan2(accX, accZ) + PI) * RAD_TO_DEG;
  accXangle = (atan2(accY, accZ) + PI) * RAD_TO_DEG;

  kalmanX.setAngle(accXangle); // Set starting angle
  kalmanY.setAngle(accYangle);
  gyroXangle = accXangle;
  gyroYangle = accYangle;
  compAngleX = accXangle;
  compAngleY = accYangle;

  timer = micros();

  delay(500);

  main_thread_list->add_thread(new MThread(1));
  main_thread_list->add_thread(new MThread(2));
}
