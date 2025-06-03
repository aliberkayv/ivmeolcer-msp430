//MPU dosyamýzýn header dosyasý, fonksiyon prototipleri için
#ifndef MPU6050_H_
#define MPU6050_H_

void mpu_init(void);
void get_mpu_data(int16_t acceleration[3], int16_t gyro[3], int16_t *temp);

#endif /* MPU6050_H_ */
