#include <genel.h>

#define LED_X_PIN BIT0 // X ekseni için LED pin tanýmý
#define LED_Y_PIN BIT3 // Y ekseni için LED pin tanýmý
#define LED_Z_PIN BIT4 // Z ekseni için LED pin tanýmý

void setup() {
    WDTCTL = WDTPW + WDTHOLD;   // Watchdog Timer'ý durdur
    P1DIR |= (LED_X_PIN | LED_Y_PIN | LED_Z_PIN);  // LED pinlerini çýkýþ olarak ayarla
    P1OUT &= ~(LED_X_PIN | LED_Y_PIN | LED_Z_PIN); // LED'leri baþlangýçta kapalý tutuyoruz.

    i2c_init();                 // I2C iletiþimini baþlatma fonksiyonumuz.
    //uart_init();                // UART iletiþimini baþlat (sadece verileri kontrol etmek için kullanýldý.)
    mpu_init();                 // MPU6050 sensörünü baþlatan fonksiyonumuz.

    __bis_SR_register(GIE);     // Global kesmeleri etkinleþtir,sensörden veri alýrken hýzlý tepki veya veri alabilmek için.
}

void loop() {
    int16_t acceleration[3], gyro[3], temp;
    get_mpu_data(acceleration, gyro, &temp); // MPU6050'den veri oku

    // X ekseni için 90 derecelik açý kontrolü ve LED kontrolü
    if (acceleration[0] >= 16000 || acceleration[0] <= -16000) {
        P1OUT |= LED_X_PIN;    // X LED'ini aç
    } else {
        P1OUT &= ~LED_X_PIN;   // X LED'ini kapat
    }

    // Y ekseni için 90 derecelik açý kontrolü ve LED kontrolü
    if (acceleration[1] >= 16000 || acceleration[1] <= -16000) {
        P1OUT |= LED_Y_PIN;    // Y LED'ini aç
    } else {
        P1OUT &= ~LED_Y_PIN;   // Y LED'ini kapat
    }

    // Z ekseni için aþaðý veyahut yukarý hareket için LED kontrolü
    int zesik_yukari = 20000; // Yukarý hareket için eþik deðeri
    int zesik_asagi = -18000; // Aþaðý hareket için eþik deðeri
    if (acceleration[2] >= zesik_yukari || acceleration[2] <= zesik_asagi) {
        P1OUT |= LED_Z_PIN;    // Z LED'ini aç
    } else {
        P1OUT &= ~LED_Z_PIN;   // Z LED'ini kapat
    }

}

int main(void) {
    setup();                   // Donanýmý baþlat ve ayarlarý yap
    while (1) {
        loop();                // Sürekli olarak sensör verilerini okumasý ve LED'leri kontrol etmesi için sonsuz bi loop döngüsü.
    }
}
