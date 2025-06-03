#include "genel.h"
/*Sensörden aldýgýmýz verileri ekranda görebilmek ve eþik deðerlerimizi
ona göre ayarlayabilmek için kullanýldý*/

unsigned char rxBuffer[128];      // rxBuffer arrayi

volatile unsigned int idx=0;            // RX buffer indexi
volatile unsigned int rxFlag=0;           // receiver flagini reset degerinde set etme

void uart_init( void )
{
    P1SEL |= (BIT1 + BIT2);//P1.1 = RXD P1.2 = TXD
    P1SEL2 |= (BIT1 + BIT2);//P1.1 = RXD P1.2 = TXD
    UCA0CTL1 = UCSSEL_2; // clock source SMCLK
    //UCOS16 = 1 icin Baudrate ayarlari
    UCA0BR0 = 52;
    UCA0BR1 = 0;
    //Modulator ayari
    UCA0MCTL = 0x11;//formule gore bkz.

    UCA0CTL1 &= ~UCSWRST; // USCI islemleri icin UCSWRST registeri temizlendi

    IE2 |= UCA0RXIE; // RXD kesmesi aktif edildi;
}

void uart_send_byte(uint8_t data)
{
    while(!(IFG2 & UCA0TXIFG)); // tx flaginin set oldugunu kontrol etme ve bekleme
    UCA0TXBUF = data; // tx icin datayi buffera set etme
}

void uart_send_string(uint8_t * cmdstr, uint8_t nstr)    // uart_send_byte kullanilarak direk string gonderme fonksiyonu
{
    uint16_t i, c;

    for(i=0; i < nstr; i++)
    {
        c=(uint8_t)*cmdstr++;
        uart_send_byte((uint8_t) c);
    }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    uint8_t c;
    c=UCA0RXBUF; // gelen rx buffer i degere atama
    rxBuffer[idx++]=c;
    // uart_send_byte(c); // eko modu icin kullanilabilir.(ack icin)
    if(c == '\n') // gelen veri bitis kontrol kosulu
    {
        rxFlag=1; // flag i gelen veri var diye set etme
        __bic_SR_register_on_exit(LPM3_bits); /* low power moddan cikis
        (eger main icerisinde giris yapilmissa kullanilabilir, yoksa kaldirilabilir) */
    }
}
