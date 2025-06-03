#include <genel.h>

#define LED_X_PIN BIT0 // X ekseni i�in LED pin tan�m�
#define LED_Y_PIN BIT3 // Y ekseni i�in LED pin tan�m�
#define LED_Z_PIN BIT4 // Z ekseni i�in LED pin tan�m�

void setup() {
    WDTCTL = WDTPW + WDTHOLD;   // Watchdog Timer'� durdur
    P1DIR |= (LED_X_PIN | LED_Y_PIN | LED_Z_PIN);  // LED pinlerini ��k�� olarak ayarla
    P1OUT &= ~(LED_X_PIN | LED_Y_PIN | LED_Z_PIN); // LED'leri ba�lang��ta kapal� tutuyoruz.

    i2c_init();                 // I2C ileti�imini ba�latma fonksiyonumuz.
    //uart_init();                // UART ileti�imini ba�lat (sadece verileri kontrol etmek i�in kullan�ld�.)
    mpu_init();                 // MPU6050 sens�r�n� ba�latan fonksiyonumuz.

    __bis_SR_register(GIE);     // Global kesmeleri etkinle�tir,sens�rden veri al�rken h�zl� tepki veya veri alabilmek i�in.
}

void loop() {
    int16_t acceleration[3], gyro[3], temp;
    get_mpu_data(acceleration, gyro, &temp); // MPU6050'den veri oku

    // X ekseni i�in 90 derecelik a�� kontrol� ve LED kontrol�
    if (acceleration[0] >= 16000 || acceleration[0] <= -16000) {
        P1OUT |= LED_X_PIN;    // X LED'ini a�
    } else {
        P1OUT &= ~LED_X_PIN;   // X LED'ini kapat
    }

    // Y ekseni i�in 90 derecelik a�� kontrol� ve LED kontrol�
    if (acceleration[1] >= 16000 || acceleration[1] <= -16000) {
        P1OUT |= LED_Y_PIN;    // Y LED'ini a�
    } else {
        P1OUT &= ~LED_Y_PIN;   // Y LED'ini kapat
    }

    // Z ekseni i�in a�a�� veyahut yukar� hareket i�in LED kontrol�
    int zesik_yukari = 20000; // Yukar� hareket i�in e�ik de�eri
    int zesik_asagi = -18000; // A�a�� hareket i�in e�ik de�eri
    if (acceleration[2] >= zesik_yukari || acceleration[2] <= zesik_asagi) {
        P1OUT |= LED_Z_PIN;    // Z LED'ini a�
    } else {
        P1OUT &= ~LED_Z_PIN;   // Z LED'ini kapat
    }

}

int main(void) {
    setup();                   // Donan�m� ba�lat ve ayarlar� yap
    while (1) {
        loop();                // S�rekli olarak sens�r verilerini okumas� ve LED'leri kontrol etmesi i�in sonsuz bi loop d�ng�s�.
    }
}
