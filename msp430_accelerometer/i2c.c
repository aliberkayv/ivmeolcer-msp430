#include "genel.h"

// I2C i�in P1.6 Serial Clock(SCL) ve P1.7 Serial Data (SDA) pinlerini ayarla
void i2c_init()
{
    P1SEL |= BIT6 + BIT7;  // P1.6 ve P1.7 pinlerini I2C fonksiyonlar�na atad�k
    P1SEL2 |= BIT6 + BIT7;

    UCB0CTL1 |= UCSWRST;   // I2C mod�l�n� yaz�l�m reset moduna al

    UCB0CTL0 = UCMST + UCSYNC + UCMODE_3;  // Master mod, senkron mod, I2C mod
    UCB0CTL1 |= UCSSEL_2;   // SMCLK (Sub Main Clock) se�imi
    UCB0BR0 = 80;           // Baud rate ayar�, I2C ileti�im h�z� 100kHz
    UCB0BR1 = 0;            // Baud rate y�ksek byte s�f�rlama

    UCB0CTL1 &= ~UCSWRST;   // I2C mod�l�n� reset modundan ��kar
    UCB0I2CSA = 0x68;       // Sens�r�n datasheetine g�re adresi
}

// Bir byte'� I2C �zerinden yaz, ba�ar�l� olursa false dan true'ya d�ner
bool i2c_write_byte(uint8_t byte)
{
    bool success = false;

    UCB0CTL1 |= UCTXSTT + UCTR;  // Master'� TX moduna al ve ba�lang�� ko�ulunu g�nder

    UCB0TXBUF = byte;  // TX buffer'a g�nderilecek byte'� y�kle

    while (UCB0CTL1 & UCTXSTT);  // Ba�lang�� ko�ulunun g�nderilmesini bekle
    success = !(UCB0STAT & UCNACKIFG);
    if (success)
    {
        while (!(IFG2 & UCB0TXIFG));  // Byte'�n g�nderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);
    }

    UCB0CTL1 |= UCTXSTP;  // Durdurma ko�ulunu g�nder
    while (UCB0CTL1 & UCTXSTP);  // Durdurma ko�ulunun bitmesini bekle
    success = !(UCB0STAT & UCNACKIFG);
    return success;
}

// 8-bit adres ve 8-bit veri okuma, ba�ar�l� olursa true d�ner
bool i2c_read_addr8_data8(uint8_t addr, uint8_t *data)
{
    bool success = false;

    UCB0CTL1 |= UCTXSTT + UCTR;  // Master'� TX moduna al ve ba�lang�� ko�ulunu g�nder
    UCB0TXBUF = addr;  // Adresi TX buffer'a y�kle

    while (UCB0CTL1 & UCTXSTT);  // Ba�lang�� ko�ulunun g�nderilmesini bekle
    success = !(UCB0STAT & UCNACKIFG);
    if (success)
    {
        while (!(IFG2 & UCB0TXIFG));  // Byte'�n g�nderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);
    }

    // Adres g�nderildikten sonra, �imdi al�c� moduna ge� ve veriyi al
    if (success)
    {
        UCB0CTL1 &= ~UCTR;   // Al�c� modunu ayarla
        UCB0CTL1 |= UCTXSTT; // Tekrarlanan ba�lang�� ko�ulunu g�nder

        while (UCB0CTL1 & UCTXSTT); // Ba�lang�� ko�ulunun g�nderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);

        UCB0CTL1 |= UCTXSTP; // Sadece bir byte alaca��m�z� belirten durdurma ko�ulunu g�nder
        while (UCB0CTL1 & UCTXSTP); // Durdurma ko�ulunun bitmesini bekle

        success &= !(UCB0STAT & UCNACKIFG);
        if (success)
        {
            while (!(IFG2 & UCB0RXIFG)); // RX buffer'�n dolmas�n� bekle
            *data = UCB0RXBUF; // Veriyi RX buffer'dan oku
        }
    }
    else
    {
        // Ba�ar�s�zl�k durumunda durdurma ko�ulunu g�nder
        UCB0CTL1 |= UCTXSTP;  // Durdurma ko�ulunu g�nder
        while (UCB0CTL1 & UCTXSTP);  // Durdurma ko�ulunun bitmesini bekle
    }

    return success;
}

// 8-bit adres ve 8-bit veri yazma, ba�ar�l� olursa true d�ner
bool i2c_write_addr8_data8(uint8_t addr, uint8_t data)
{
    bool success = false;

    UCB0CTL1 |= UCTXSTT + UCTR;  // Master'� TX moduna al ve ba�lang�� ko�ulunu g�nder
    UCB0TXBUF = addr;  // Adresi TX buffer'a y�kle

    while (UCB0CTL1 & UCTXSTT);  // Ba�lang�� ko�ulunun g�nderilmesini bekle
    success = !(UCB0STAT & UCNACKIFG);
    if (success)
    {
        while (!(IFG2 & UCB0TXIFG));  // Byte'�n g�nderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);
    }

    if (success)
    {
        UCB0TXBUF = data;  // G�nderilecek veriyi TX buffer'a y�kle
        while (!(IFG2 & UCB0TXIFG));  // Byte'�n g�nderilmesini bekle
        success = !(UCB0STAT & UCNACKIFG);
    }
    UCB0CTL1 |= UCTXSTP;  // Durdurma ko�ulunu g�nder
    while (UCB0CTL1 & UCTXSTP);  // Durdurma ko�ulunun bitmesini bekle
    return success;
}

// 16-bit veri okuma, verilen 8-bit adresten ba�lar ve 2 byte okur
uint16_t i2c_read_word(uint8_t addr)
{
    uint8_t reg_data;
    int16_t reg_data16;
    uint16_t result;

    i2c_read_addr8_data8(addr, &reg_data); // Verilen adresten 1 byte oku
    reg_data16 = reg_data << 8;            // 8 bit sola kayd�rarak 16-bit veri i�in haz�rl�k yap

    i2c_read_addr8_data8(addr + 1, &reg_data); // +1 byte adresinden 1 byte oku
    reg_data16 |= reg_data;               // Okunan byte'� birle�tir

    result = reg_data16;                  // Sonucu d�nd�r

    return result;
}
