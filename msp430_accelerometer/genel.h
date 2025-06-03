
#ifndef genel_H_
#define genel_H_

#include <msp430g2553.h> //mikrodenetleyicinin kütüphanesi
#include <stdlib.h>// standart c kütüphanesi
#include <stdio.h> // standart input/output kütüphanesi
#include "types.h" // özel türk tanýmladýgýmýz kütüphane
#include "i2c.h"// I2C haberleþme için olan kütüphane
#include "mpu6050.h" //sensörün kütüphanesi
extern volatile unsigned int idx;
extern volatile unsigned int rxFlag;
extern unsigned char rxBuffer[128];
#endif /* genel_H_ */ // header dosyasýný kapsayan korumayý sonlandýrma
