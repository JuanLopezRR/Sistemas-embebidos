#include <xc.h>
#pragma config FOSC=HS, WDTE=OFF, PWRTE=OFF, MCLRE=ON
#pragma config CP=OFF, CPD=OFF, BOREN=OFF, IESO=OFF
#pragma config FCMEN=OFF, LVP=OFF, DEBUG=OFF
#define _XTAL_FREQ 4000000UL

const unsigned char DIGITOS[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
unsigned char u=0,d=0,c=0,m=0,t=0;

void mostrar(unsigned char n,unsigned char sel){
    PORTD=0x0F;
    PORTC=DIGITOS[n];
    PORTD=sel;
    __delay_ms(1);
}
void main(void){
    TRISC=0x00; TRISD=0x00;
    PORTC=0x00; PORTD=0x0F;
    while(1){
        mostrar(m,0x0E); mostrar(c,0x0D);
        mostrar(d,0x0B); mostrar(u,0x07);
        if(++t>=250){
            t=0;
            if(++u>=10){u=0;
                if(++d>=10){d=0;
                    if(++c>=10){c=0;
                        if(++m>=10)m=0;
                    }
                }
            }
        }
    }
}
