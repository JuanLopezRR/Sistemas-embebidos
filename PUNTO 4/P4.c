#include <xc.h>

#define _XTAL_FREQ 4000000

//==================================================
// CONFIGURACION DEL PIC
//==================================================

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF


//==================================================
// TABLA DISPLAY ANODO COMUN
//
// RD7 RD6 RD5 RD4 RD3 RD2 RD1 RD0
// DP   G   F   E   D   C   B   A
//==================================================

const unsigned char numeros[10] =
{
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
};


//==================================================
// VARIABLES
//==================================================

volatile unsigned char segundos = 0;

volatile unsigned char decenas = 0;
volatile unsigned char unidades = 0;

volatile unsigned char digito = 0;

volatile unsigned int contador_timer = 0;

volatile unsigned char corriendo = 0;


//==================================================
// INTERRUPCION
//==================================================

void __interrupt() interrupcion(void)
{
    if (T0IF)
    {
        // Recargar Timer0
        TMR0 = 6;

        T0IF = 0;


        //==================================================
        // MULTIPLEXACION
        //==================================================

        // Apagar ambos displays
        RB0 = 0;
        RB1 = 0;


        if (digito == 0)
        {
            // DECENAS

            PORTD = numeros[decenas];

            RB0 = 1;

            digito = 1;
        }
        else
        {
            // UNIDADES

            PORTD = numeros[unidades];

            RB1 = 1;

            digito = 0;
        }


        //==================================================
        // CONTADOR DE TIEMPO
        //==================================================

        contador_timer++;


        // 500 interrupciones × 2 ms = 1 segundo

        if (contador_timer >= 500)
        {
            contador_timer = 0;


            if (corriendo == 1)
            {
                segundos++;


                // 99 ? 00
                if (segundos >= 100)
                {
                    segundos = 0;
                }


                // Actualizar display
                decenas = segundos / 10;
                unidades = segundos % 10;
            }
        }
    }
}


//==================================================
// PROGRAMA PRINCIPAL
//==================================================

void main(void)
{
    //==================================================
    // PUERTOS DIGITALES
    //==================================================

    ANSEL = 0x00;
    ANSELH = 0x00;


    //==================================================
    // CONFIGURAR PUERTOS
    //==================================================

    // PORTD = segmentos
    TRISD = 0x00;


    // RB0 = display 1
    // RB1 = display 2
    // RB2 = INICIAR
    // RB3 = PAUSAR
    // RB4 = REANUDAR
    // RB5 = RESET

    TRISB = 0b00111100;


    PORTD = 0xFF;
    PORTB = 0x00;


    //==================================================
    // ESTADO INICIAL
    //==================================================

    segundos = 0;

    decenas = 0;
    unidades = 0;

    digito = 0;

    contador_timer = 0;

    corriendo = 0;


    //==================================================
    // CONFIGURACION TIMER0
    //==================================================

    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE = 0;

    OPTION_REGbits.PSA = 0;

    // Prescaler 1:8
    OPTION_REGbits.PS2 = 0;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 0;

    TMR0 = 6;


    //==================================================
    // INTERRUPCIONES
    //==================================================

    T0IF = 0;
    T0IE = 1;

    PEIE = 1;
    GIE = 1;


    //==================================================
    // BUCLE PRINCIPAL
    //==================================================

    while(1)
    {

        //==================================================
        // INICIAR - RB2
        //==================================================

        if (RB2 == 0)
        {
            __delay_ms(30);

            if (RB2 == 0)
            {
                segundos = 0;

                decenas = 0;
                unidades = 0;

                corriendo = 1;

                // Esperar a soltar
                while (RB2 == 0);
            }
        }


        //==================================================
        // PAUSAR - RB3
        //==================================================

        if (RB3 == 0)
        {
            __delay_ms(30);

            if (RB3 == 0)
            {
                corriendo = 0;

                // Esperar a soltar
                while (RB3 == 0);
            }
        }


        //==================================================
        // REANUDAR - RB4
        //==================================================

        if (RB4 == 0)
        {
            __delay_ms(30);

            if (RB4 == 0)
            {
                corriendo = 1;

                // Esperar a soltar
                while (RB4 == 0);
            }
        }


        //==================================================
        // RESET - RB5
        //==================================================

        if (RB5 == 0)
        {
            __delay_ms(30);

            if (RB5 == 0)
            {
                segundos = 0;

                decenas = 0;
                unidades = 0;

                corriendo = 0;

                // Esperar a soltar
                while (RB5 == 0);
            }
        }
    }
}