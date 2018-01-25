#include <Wire.h>
#include <Servo.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "MS5837.h"

static unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ; 

static char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 
0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 
0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;

#define ADDRESS   0x02
#define RS485PIN  2

#define BAUD_RATE	19200
#define TIMEOUT		5

#define PRESSURE_INTERVAL	500
#define IMU_INTERVAL	40

// register list
struct {
  short raxHi;
  short raxLo;
  
  short rayHi;
  short rayLo;
  
  short razHi;
  short razLo;

  short rgxHi;
  short rgxLo;
  
  short rgyHi;
  short rgyLo;
  
  short rgzHi;
  short rgzLo;
  
  short mx;
  short my;
  short mz;

  short pressureHi;
  short pressureLo;

  short depthHi;
  short depthLo;
  
} Data ;

// 16 bit CRC value for modbus
unsigned short CRC16(
  unsigned char *puchMsg,           /* message to calculate CRC upon */
  unsigned short usDataLen          /* quantity of bytes in message */
){
  unsigned char uchCRCHi = 0xFF ;   /* high byte of CRC initialized */
  unsigned char uchCRCLo = 0xFF ;   /* low byte of CRC initialized */
  unsigned uIndex ;                 /* will index into CRC lookup table */
  
  while (usDataLen--)
  {
    uIndex = uchCRCHi ^ *puchMsg++; /* calculate the CRC */
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
    uchCRCLo = auchCRCLo[uIndex];
  }
  
  return (uchCRCHi << 8 | uchCRCLo);
}

// convert float to 4 byte format
union {
  unsigned long bite;
  float data;
} extract;

// store float data to registers
void storeFloat(float input, short *pointer){
  extract.data = input;
  *pointer = extract.bite >> 16;
  pointer++;
  *pointer = extract.bite & 0x0FFFF;
}

// convert accel reading to true value
float convertRawAccel(int16_t raw) {
  return 2.0f*(float)raw/32768.0f;
}
// conver gyro reading to true value
float convertRawGyr(int16_t raw) {
  return 500.0f*(float)raw/32768.0f;
}

// initialisation
HMC5883L mag;
MPU6050 accelgyro;
MS5837 sensor;

unsigned long PressCheck, IMUCheck;

void setup() {
	// join I2C bus (I2Cdev library doesn't do this automatically)
	#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	Wire.begin();
	#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
	Fastwire::setup(400, true);
	#endif
	
	Serial.begin(BAUD_RATE);
	Serial.setTimeout(TIMEOUT);

	pinMode(RS485PIN, OUTPUT);
	digitalWrite(RS485PIN, LOW);

	// initialize device
	mag.initialize();
	accelgyro.initialize();
	accelgyro.setI2CBypassEnabled(true);

	//Set the scale on the accelerometer and gyroscope
	accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_500);
	accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

	IMUCheck = millis();
	PressCheck = millis();

	sensor.init();
	sensor.setFluidDensity(997); // kg/m^3 (997 freshwater, 1029 for seawater)
}

void loop(){
  //=========
  //   IMU 
  //=========
	unsigned long milliNow = millis();
	if (milliNow - IMUCheck >= IMU_INTERVAL) {
		short ax, ay, az, gx, gy, gz;
		accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
		mag.getHeading(&Data.mx, &Data.my, &Data.mz);

		storeFloat(convertRawAccel(ax), &Data.raxHi);    
		storeFloat(convertRawAccel(ay), &Data.rayHi);
		storeFloat(convertRawAccel(az), &Data.razHi);  

		storeFloat(convertRawGyr(gx), &Data.rgxHi);
		storeFloat(convertRawGyr(gy), &Data.rgyHi);
		storeFloat(convertRawGyr(gz), &Data.rgzHi);

		//increment previous time, so we keep proper pace
		IMUCheck += IMU_INTERVAL;  
	}

  //==================
  // Pressure Sensor  
  //==================
  if((milliNow - PressCheck) > PRESSURE_INTERVAL){
    float vOut = 3.3 * analogRead(A0) / 1023;
    float vIn = 3.3 * analogRead(A1) / 1023;
    float pressure = 0.01 * (vOut / vIn + 0.48571) / 0.00876;

    storeFloat(pressure, &Data.pressureHi);
    PressCheck += PRESSURE_INTERVAL;
  } 
} // end of main loop

// serial for modbus
void serialEvent() {
	uint8_t buf[50];
	uint8_t len = Serial.readBytes(buf, 50);
	short *pointer = &Data.raxHi;

	byte addrBuf = buf[0];

	//correct address
	if(addrBuf == ADDRESS){
		short crcBuf = (buf[len - 2] << 8 | buf[len - 1]);
		short crcData = CRC16(buf, len - 2);

		//correct CRC
		if(crcBuf == crcData){
			//function code
			byte funcBuf = buf[1];

			//register
			short regBuf = buf[2] << 8 | buf[3];

			if(funcBuf == 0x03){ // read function
				pointer += regBuf;
				short lenBuf = buf[5]*2;
				
				uint8_t outBuf[50];
				outBuf[0] = buf[0];
				outBuf[1] = buf[1];
				outBuf[2] = lenBuf;

				for(int i=0; i < lenBuf; i += 2){
					outBuf[3+i] = *pointer >> 8;
					outBuf[4+i] = *pointer & 0x0FF;
					pointer++;
				}

				crcData = CRC16(outBuf, lenBuf+3);
				outBuf[3+lenBuf] = crcData >> 8;
				outBuf[4+lenBuf] = crcData & 0x0FF;

				digitalWrite(RS485PIN, HIGH);
						
				//return frame
				Serial.write(outBuf, 5+lenBuf);
				Serial.flush();

				digitalWrite(RS485PIN, LOW);
			}        
		}
	}
}
