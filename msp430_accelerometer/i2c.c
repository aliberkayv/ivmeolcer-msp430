#include "genel.h"

// I2C için P1.6 Serial Clock(SCL) ve P1.7 Serial Data (SDA) pinlerini ayarla
void i2c_init()
{
    P1SEL |= BIT6 + BIT7;  // P1.6 ve P1.7 pinlerini I2C fonksiyonlarýna atadýk
    P1SEL2 |= BIT6 + BIT7;

    UCB0CTL1 |= UCSWRST;   // I2C modülünü yazýlým reset moduna al

    UCB0CTL0 = UCMST + UCSYNC + UCMODE_3;  // Master mod, senkron mod, I2C mod
    UCB0CTL1 |= UCSSEL_2;   // SMCLK (Sub Main Clock) seçimi
    UCB0BR0 = 80;           // Baud rate ayarý, I2C iletiþim hýzý 100kHz
    UCB0BR1 = 0;            // Baud rate yüksek byte sýfýrlama

    UCB0CTL1 &= ~UCSWRST;   // I2C modülünü reset modundan çýkar
    UCB0I2CSA = 0x68;       // Sensörün datasheetine göre adresi
}

// Bir byte'ý I2C üzerinden yaz, baþarýlý olursa false dan true'ya döner
bool i2c_write_byte(uint8_t byte)
{
    bool success = false;

    UCB0CTL1 |= UCTXSTT + UCTR;  // Master'ý TX moduna al ve baþlangýç koþulunu gönder

    UCB0TXBUF = byte;  // TX buffer'a gönderilecek byte'ý yükle

    while (UCB0CTL1 & UCTXSTT);  // Baþlangýç koþulunun gönderilmesini bekle
    success = !(UCB0STAT & UCNACKIFG);
    if (success)
    {
        while (!(IFG2 & UCB0TXIFG));  // Byte'ýn gönderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);
    }

    UCB0CTL1 |= UCTXSTP;  // Durdurma koþulunu gönder
    while (UCB0CTL1 & UCTXSTP);  // Durdurma koþulunun bitmesini bekle
    success = !(UCB0STAT & UCNACKIFG);
    return success;
}

// 8-bit adres ve 8-bit veri okuma, baþarýlý olursa true döner
bool i2c_read_addr8_data8(uint8_t addr, uint8_t *data)
{
    bool success = false;

    UCB0CTL1 |= UCTXSTT + UCTR;  // Master'ý TX moduna al ve baþlangýç koþulunu gönder
    UCB0TXBUF = addr;  // Adresi TX buffer'a yükle

    while (UCB0CTL1 & UCTXSTT);  // Baþlangýç koþulunun gönderilmesini bekle
    success = !(UCB0STAT & UCNACKIFG);
    if (success)
    {
        while (!(IFG2 & UCB0TXIFG));  // Byte'ýn gönderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);
    }

    // Adres gönderildikten sonra, þimdi alýcý moduna geç ve veriyi al
    if (success)
    {
        UCB0CTL1 &= ~UCTR;   // Alýcý modunu ayarla
        UCB0CTL1 |= UCTXSTT; // Tekrarlanan baþlangýç koþulunu gönder

        while (UCB0CTL1 & UCTXSTT); // Baþlangýç koþulunun gönderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);

        UCB0CTL1 |= UCTXSTP; // Sadece bir byte alacaðýmýzý belirten durdurma koþulunu gönder
        while (UCB0CTL1 & UCTXSTP); // Durdurma koþulunun bitmesini bekle

        success &= !(UCB0STAT & UCNACKIFG);
        if (success)
        {
            while (!(IFG2 & UCB0RXIFG)); // RX buffer'ýn dolmasýný bekle
            *data = UCB0RXBUF; // Veriyi RX buffer'dan oku
        }
    }
    else
    {
        // Baþarýsýzlýk durumunda durdurma koþulunu gönder
        UCB0CTL1 |= UCTXSTP;  // Durdurma koþulunu gönder
        while (UCB0CTL1 & UCTXSTP);  // Durdurma koþulunun bitmesini bekle
    }

    return success;
}

// 8-bit adres ve 8-bit veri yazma, baþarýlý olursa true döner
bool i2c_write_addr8_data8(uint8_t addr, uint8_t data)
{
    bool success = false;

    UCB0CTL1 |= UCTXSTT + UCTR;  // Master'ý TX moduna al ve baþlangýç koþulunu gönder
    UCB0TXBUF = addr;  // Adresi TX buffer'a yükle

    while (UCB0CTL1 & UCTXSTT);  // Baþlangýç koþulunun gönderilmesini bekle
    success = !(UCB0STAT & UCNACKIFG);
    if (success)
    {
        while (!(IFG2 & UCB0TXIFG));  // Byte'ýn gönderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);
    }

    if (success)
    {
        UCB0TXBUF = data;  // Gönderilecek veriyi TX buffer'a yükle
        while (!(IFG2 & UCB0TXIFG));  // Byte'ýn gönderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);
    }
    UCB0CTL1 |= UCTXSTP;  // Durdurma koþulunu gönder
    while (UCB0CTL1 & UCTXSTP);  // Durdurma koþulunun bitmesini bekle
    return success;
}

// 16-bit veri okuma, verilen 8-bit adresten baþlar ve 2 byte okur
uint16_t i2c_read_word(uint8_t addr)
{
    uint8_t reg_data;
    int16_t reg_data16;
    uint16_t result;

    i2c_read_addr8_data8(addr, &reg_data); // Verilen adresten 1 byte oku
    reg_data16 = reg_data << 8;            // 8 bit sola kaydýrarak 16-bit veri için hazýrlýk yap

    i2c_read_addr8_data8(addr + 1, &reg_data); // +1 byte adresinden 1 byte oku
    reg_data16 |= reg_data;               // Okunan byte'ý birleþtir

    result = reg_data16;                  // Sonucu döndür

    return result;
}
