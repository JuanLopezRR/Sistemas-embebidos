#include <xc.h>

//=====================================================
// CONFIGURACIÓN
//=====================================================
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 4000000UL

//=====================================================
// VARIABLES
//=====================================================
volatile unsigned char unidades = 0;
volatile unsigned char decenas = 0;
volatile unsigned int contador_ms = 0;

volatile unsigned char funcionando = 0;
volatile unsigned char pausado = 0;
volatile unsigned char digito = 0;

//=====================================================
// TABLA DISPLAY 7 SEGMENTOS
//=====================================================
const unsigned char display[10] =
{
    0x3F,   // 0
    0x06,   // 1
    0x5B,   // 2
    0x4F,   // 3
    0x66,   // 4
    0x6D,   // 5
    0x7D,   // 6
    0x07,   // 7
    0x7F,   // 8
    0x6F    // 9
};

//=====================================================
// INTERRUPCIÓN
//=====================================================
void __interrupt() ISR(void)
{
    if (INTCONbits.TMR0IF)
    {
        TMR0 = 6;
        INTCONbits.TMR0IF = 0;

        // Apagar ambos dígitos
        PORTBbits.RB0 = 1;
        PORTBbits.RB1 = 1;

        // Multiplexación
        if (digito == 0)
        {
            PORTD = display[unidades];
            PORTBbits.RB0 = 0;
            digito = 1;
        }
        else
        {
            PORTD = display[decenas];
            PORTBbits.RB1 = 0;
            digito = 0;
        }

        // Cronómetro
        if (funcionando == 1 && pausado == 0)
        {
            contador_ms++;

            if (contador_ms >= 1000)
            {
                contador_ms = 0;

                unidades++;

                if (unidades >= 10)
                {
                    unidades = 0;
                    decenas++;

                    if (decenas >= 10)
                    {
                        decenas = 0;
                    }
                }
            }
        }
    }
}

//=====================================================
// TIMER0
//=====================================================
void Timer0_Init(void)
{
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;

    // Prescaler 1:4
    OPTION_REGbits.PS2 = 0;
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS0 = 1;

    TMR0 = 6;

    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    INTCONbits.GIE = 1;
}

//=====================================================
// PROGRAMA PRINCIPAL
//=====================================================
void main(void)
{
    // Pines digitales
    ANSEL = 0x00;
    ANSELH = 0x00;

    // RA0 = iniciar
    // RA1 = pausar
    // RA2 = reanudar
    // RA3 = cero
    TRISA = 0x0F;

    // RB0 y RB1 = selección de dígitos
    TRISB = 0x00;

    // RD0-RD7 = segmentos
    TRISD = 0x00;

    PORTA = 0x00;
    PORTB = 0xFF;
    PORTD = 0x00;

    Timer0_Init();

    while (1)
    {
        // INICIAR
        if (PORTAbits.RA0 == 0)
        {
            __delay_ms(30);

            if (PORTAbits.RA0 == 0)
            {
                funcionando = 1;
                pausado = 0;

                while (PORTAbits.RA0 == 0);
            }
        }

        // PAUSAR
        if (PORTAbits.RA1 == 0)
        {
            __delay_ms(30);

            if (PORTAbits.RA1 == 0)
            {
                pausado = 1;

                while (PORTAbits.RA1 == 0);
            }
        }

        // REANUDAR
        if (PORTAbits.RA2 == 0)
        {
            __delay_ms(30);

            if (PORTAbits.RA2 == 0)
            {
                pausado = 0;
                funcionando = 1;

                while (PORTAbits.RA2 == 0);
            }
        }

        // PONER EN CERO
        if (PORTAbits.RA3 == 0)
        {
            __delay_ms(30);

            if (PORTAbits.RA3 == 0)
            {
                funcionando = 0;
                pausado = 0;

                unidades = 0;
                decenas = 0;
                contador_ms = 0;

                while (PORTAbits.RA3 == 0);
            }
        }
    }
}
