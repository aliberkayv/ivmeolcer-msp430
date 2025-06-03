
#ifndef genel_H_
#define genel_H_

#include <msp430g2553.h> //mikrodenetleyicinin k�t�phanesi
#include <stdlib.h>// standart c k�t�phanesi
#include <stdio.h> // standart input/output k�t�phanesi
#include "types.h" // �zel t�rk tan�mlad�g�m�z k�t�phane
#include "i2c.h"// I2C haberle�me i�in olan k�t�phane
#include "mpu6050.h" //sens�r�n k�t�phanesi
extern volatile unsigned int idx;
extern volatile unsigned int rxFlag;
extern unsigned char rxBuffer[128];
#endif /* genel_H_ */ // header dosyas�n� kapsayan korumay� sonland�rma
