#include "genel.h"
/*T�m flag, set etme ve de�er okuma i�lemleri, MPU6050 datasheetindeki de�erlere g�re ve �nerilere g�re yap�lm��t�r.*/
// MPU6050 sens�r�n� ba�latma fonksiyonu
void mpu_init(void)
{
    i2c_write_addr8_data8(0x6B, 0x80); // MPU6050'yi resetle
    __delay_cycles(100000);            // �nerilen beklemeyi yap  8Mhz ise 0.0125 saniye

    i2c_write_addr8_data8(0x6B, 0x00); // MPU6050'yi uyku durumundan uyand�r

    i2c_write_addr8_data8(0x68, 0x07); // Sinyal yollar�n�(datasheete g�re) resetle
    __delay_cycles(100000); // �nerilen beklemeyi yap

    i2c_write_addr8_data8(0x6A, 0x00); // I2C'yi FIFO olarak ayarla,veri kayb�n� �nlemek ve verimli �ekilde y�netmek i�in
    i2c_write_addr8_data8(0x6B, 0x00); // Dahili 8MHz osilat�r� kullanarak CPU zamanlay�c�y� ayarla
    i2c_write_addr8_data8(0x19, 0x07); // Zamanlay�c� b�l�c�s�n� 8 olarak ayarla
    i2c_write_addr8_data8(0x1A, 0x00); // DLPF'yi 260Hz olarak ayarla, dijital al�ak ge�irgen filtre ile y�ksek frekans� keisp g�r�lt� engelleme
    i2c_write_addr8_data8(0x1B, 0x00); // Gyroscope Config'u 250 d/s olarak ayarla
    i2c_write_addr8_data8(0x1C, 0x00); // Accelerometer Config'u 2g olarak ayarla
    i2c_write_addr8_data8(0x1B, 0x00);
}
// MPU6050'den veri okuma fonksiyonu
void get_mpu_data(int16_t acceleration[3], int16_t gyro[3], int16_t *temp)
{acceleration[0] = i2c_read_word(0x3B);  // Acceleration X verisini oku
 acceleration[1] = i2c_read_word(0x3D);  // Acceleration Y verisini oku
 acceleration[2] = i2c_read_word(0x3F);  // Acceleration Z verisini oku
gyro[0] = i2c_read_word(0x43);          //gyroscope verilerine ihtiya� duymuyoruz 3 eksende ivme �l�t���m�z i�in
gyro[1] = i2c_read_word(0x45);
*temp = i2c_read_word(0x41);}            // Temperature verisini okumak i�in ama s�cakl��a da ihtiyac�m�z olmad��� i�in gerek duymuyoruz.
