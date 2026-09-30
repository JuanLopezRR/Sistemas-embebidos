/*
 * Ejercicio 5 - PIC16F887 + LCD 16x2
 * XC8
 *
 * Conexiones:
 * RB0 -> RS
 * RB1 -> E
 * RB4 -> D4
 * RB5 -> D5
 * RB6 -> D6
 * RB7 -> D7
 *
 * Cristal: 4 MHz
 * LCD: LM016L, modo 4 bits
 */

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000UL

// Configuracion del PIC16F887
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF
#pragma config BOR4V = BOR40V
#pragma config WRT = OFF

#define LCD_RS RB0
#define LCD_EN RB1

static void lcd_pulse_enable(void)
{
    LCD_EN = 1;
    __delay_us(2);
    LCD_EN = 0;
    __delay_us(50);
}

static void lcd_send_nibble(uint8_t nibble)
{
    PORTB = (PORTB & 0x0F) | (nibble & 0xF0);
    lcd_pulse_enable();
}

static void lcd_cmd(uint8_t cmd)
{
    LCD_RS = 0;

    lcd_send_nibble(cmd & 0xF0);
    lcd_send_nibble((uint8_t)(cmd << 4));

    if (cmd == 0x01 || cmd == 0x02)
        __delay_ms(2);
    else
        __delay_us(50);
}

static void lcd_data(uint8_t data)
{
    LCD_RS = 1;

    lcd_send_nibble(data & 0xF0);
    lcd_send_nibble((uint8_t)(data << 4));

    __delay_us(50);
}

static void lcd_init(void)
{
    __delay_ms(20);

    LCD_RS = 0;
    LCD_EN = 0;

    // Secuencia de inicializacion HD44780
    lcd_send_nibble(0x30);
    __delay_ms(5);

    lcd_send_nibble(0x30);
    __delay_us(150);

    lcd_send_nibble(0x30);
    __delay_us(150);

    lcd_send_nibble(0x20);
    __delay_us(150);

    lcd_cmd(0x28);   // 4 bits, 2 lineas, 5x8
    lcd_cmd(0x0C);   // Display ON, cursor OFF
    lcd_cmd(0x06);   // Incremento automatico
    lcd_cmd(0x01);   // Limpiar pantalla
}

static void lcd_print(const char *text)
{
    while (*text)
    {
        lcd_data((uint8_t)*text);
        text++;
    }
}

static void mostrar_dos_digitos(uint8_t valor)
{
    lcd_data((uint8_t)('0' + (valor / 10)));
    lcd_data((uint8_t)('0' + (valor % 10)));
}

static void mostrar_pantalla(uint8_t hora, uint8_t minuto)
{
    lcd_cmd(0x80);
    lcd_print("Santa Marta");

    lcd_cmd(0xC0);
    lcd_print("Hora: ");

    mostrar_dos_digitos(hora);

    lcd_data(':');

    mostrar_dos_digitos(minuto);

    lcd_print(" AM");
}

void main(void)
{
    uint8_t hora = 7;
    uint8_t minuto = 1;

    // Desactivar entradas analogicas
    ANSEL = 0x00;
    ANSELH = 0x00;

    // PORTB como salida
    TRISB = 0x00;
    PORTB = 0x00;

    lcd_init();

    mostrar_pantalla(hora, minuto);

    while (1)
    {
        // Para la simulacion, la hora avanza cada minuto real.
        for (uint8_t segundos = 0; segundos < 60; segundos++)
        {
            __delay_ms(1000);
        }

        minuto++;

        if (minuto >= 60)
        {
            minuto = 0;
            hora++;

            if (hora > 12)
            {
                hora = 1;
            }
        }

        mostrar_pantalla(hora, minuto);
    }
}
