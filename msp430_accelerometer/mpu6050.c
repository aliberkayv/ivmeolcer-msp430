#include "genel.h"
/*Tüm flag, set etme ve deðer okuma iþlemleri, MPU6050 datasheetindeki deðerlere göre ve önerilere göre yapýlmýþtýr.*/
// MPU6050 sensörünü baþlatma fonksiyonu
void mpu_init(void)
{
    i2c_write_addr8_data8(0x6B, 0x80); // MPU6050'yi resetle
    __delay_cycles(100000);            // Önerilen beklemeyi yap  8Mhz ise 0.0125 saniye

    i2c_write_addr8_data8(0x6B, 0x00); // MPU6050'yi uyku durumundan uyandýr

    i2c_write_addr8_data8(0x68, 0x07); // Sinyal yollarýný(datasheete göre) resetle
    __delay_cycles(100000); // Önerilen beklemeyi yap

    i2c_write_addr8_data8(0x6A, 0x00); // I2C'yi FIFO olarak ayarla,veri kaybýný önlemek ve verimli þekilde yönetmek için
    i2c_write_addr8_data8(0x6B, 0x00); // Dahili 8MHz osilatörü kullanarak CPU zamanlayýcýyý ayarla
    i2c_write_addr8_data8(0x19, 0x07); // Zamanlayýcý bölücüsünü 8 olarak ayarla
    i2c_write_addr8_data8(0x1A, 0x00); // DLPF'yi 260Hz olarak ayarla, dijital alçak geçirgen filtre ile yüksek frekansý keisp gürültü engelleme
    i2c_write_addr8_data8(0x1B, 0x00); // Gyroscope Config'u 250 d/s olarak ayarla
    i2c_write_addr8_data8(0x1C, 0x00); // Accelerometer Config'u 2g olarak ayarla
    i2c_write_addr8_data8(0x1B, 0x00);
}
// MPU6050'den veri okuma fonksiyonu
void get_mpu_data(int16_t acceleration[3], int16_t gyro[3], int16_t *temp)
{acceleration[0] = i2c_read_word(0x3B);  // Acceleration X verisini oku
 acceleration[1] = i2c_read_word(0x3D);  // Acceleration Y verisini oku
 acceleration[2] = i2c_read_word(0x3F);  // Acceleration Z verisini oku
gyro[0] = i2c_read_word(0x43);          //gyroscope verilerine ihtiyaç duymuyoruz 3 eksende ivme ölçtüðümüz için
gyro[1] = i2c_read_word(0x45);
*temp = i2c_read_word(0x41);}            // Temperature verisini okumak için ama sýcaklýða da ihtiyacýmýz olmadýðý için gerek duymuyoruz.
