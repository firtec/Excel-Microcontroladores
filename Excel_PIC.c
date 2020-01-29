/* ***************************************************************************
**  File Name    : EXCEL.c
**  Descripción  : Envía datos del conversor en 8 bit (RA0) al EXCEL
**                 RA1 Inicia Excel - RA2 Inicia Hoja - RA3 Reset del sistema
**                 Se comunica por la USART a 9600 baudios.
**  Autor        : www.firtec.com.ar
**  Target       : 40PIN PIC18F4620
**  Compilador   : Microchip C18
**  IDE          : Microchip MPLAB IDE
**  XTAL         : 20MHZ  
** ***************************************************************************/
#include <p18f4620.h>

#pragma config OSC=HS,PWRT=ON,MCLRE=ON,LVP=OFF,WDT=OFF
#include <stdio.h>
#include "lcd_c18.h"
#include <adc.h> 
#include <usart.h> 
//*********** Macros************************************
#define bit_set(bit,ubicacion)	(ubicacion |= 1<<bit)
#define bit_clr(bit,ubicacion)	(ubicacion &= ~(1<<bit))
//******** Funciones Propias ***************************
void isr_conversor(void);
void verifica_isr(void);
//******** Variables del Sistema ***********************
volatile float voltaje = 0.00;
volatile unsigned int conversion=0;
volatile unsigned char conversiones=0;
volatile unsigned int M0=0;
volatile char Data;
volatile unsigned char Hi=0;
unsigned char bandera = 0;
#define led   PORTBbits.RB0 // No se usa

//************ Configura el vector de interrupción LOW**********************
#pragma code prioridad_nomal = 0X0008
 void ISR(void){
 _asm GOTO verifica_isr _endasm
 }
#pragma code
//****************** Se discrimina el origen de la interrupción*************
#pragma interrupt verifica_isr
void verifica_isr(void){
  if(PIR1bits.ADIF){   // Verifica la bandera de inte. del conversor A/D
     isr_conversor(); // Salta a la rutina correspondiente
  }
if (PIR1bits.RCIF==1){ // Discrimina la inte del receptor
Data=getcUSART(); // Se lee dato recibido
if(Data == 0x01){
bit_clr(6,PIE1);  // Desactiva la INTE del conversor
putcUSART (Hi);  // TX la parte baja de los  bit´s
bit_set(6,PIE1);
 }
PIR1bits.RCIF=0; 		// Borra bandera de interrupción
 } 
}

//**************************************************************************/

void main(void){
ADCON1 = 0X0E;  // Configura los pines del PORT A como I/O Digitales
TRISA=0X0F;		// Configura el pin 2,3 y 7 como entradas digitales
TRISBbits.TRISB0=0; // No se usa
PORTA=0;	// Puerto A inicia en cero
lcd_init();	 //Inicializa el LCD
OpenADC(ADC_FOSC_RC & ADC_RIGHT_JUST & ADC_16_TAD, 
ADC_CH0 & ADC_REF_VDD_VSS & ADC_INT_ON,ADC_1ANA);								   

OpenUSART (USART_TX_INT_OFF & 	// TX sin interrupción
           USART_RX_INT_ON &	// RX con interrupciónes
           USART_ASYNCH_MODE &	// Modo asincrónico
           USART_EIGHT_BIT &	// Modo alta velocidad
           USART_CONT_RX &		// Recepción continua
           USART_BRGH_HIGH,129);	// 9600 baudios a 20Mhz  
RCONbits.IPEN = 0; // Deshabilitamos prioridades
INTCONbits.PEIE=1; // Habilitamos la interrupcion de perifericos

lcd_gotoxy(2,1); 
lcd_putrs("Excel -> Boton_RA1");
lcd_gotoxy(2,2); 
lcd_putrs("Hoja -> Boton_RA2");
INTCONbits.GIEH = 1; //Habilita las interrupciones, por defecto
                     //el vector es 0x008 (sin prioridad)
//******************************************************
while(1){// Bucle donde se leen los botones de funciones para Excel
if(PORTAbits.RA1!= 1 && bandera == 0){ 
lcd_gotoxy(2,1); 
lcd_putrs("Realizado.........");
putcUSART (0x01); // Inicia el Excel
bandera =1;
}
if(PORTAbits.RA2!=1 && bandera == 1){ 
lcd_gotoxy(2,2);
lcd_putrs("Realizado.........");
Delay10KTCYx(250);
Delay10KTCYx(250);
lcd_gotoxy(2,1);
lcd_putrs("  EXCEL EN LINEA  ");
lcd_gotoxy(1,2); 
lcd_putrs("  Dato Enviado:         ");
putcUSART (0x02); 	// Inicia la hoja de calculo Controlador.xls
bandera =3;
}
//*********************************************************
if(bandera ==3){
while(1){		// Bucle infinito para medir y efectuar el reset
if(PORTAbits.RA3!= 1 && bandera == 3){ // Boton RA3 reset del sistema
 Nop();
   Nop();
     Reset();
     }
    ConvertADC(); 
   } 
  }  
 }
}
//**************************************************************************
void isr_conversor(void){   
conversion= ReadADC(); 	// Guarda el estado del conversor en conversion
M0 += ReadADC()>>2;		// Pasa a 8 bits
if(49==conversiones++){	// 50 Conversiones
conversion = M0/50;    
Hi=conversion;       	// Salvo el dato del conversor
conversiones=0;         // Limpia registros            
M0=0;					// para la próxima medición
lcd_gotoxy(16,2); 
stdout= _H_USER;		// Necesario para usar printf re-dirigido al LCD
printf("%03u",conversion); // Muestro el valor en LCD				  
 }
PIR1bits.ADIF=0; // Borra la bandera de Interrupcion del conversor A/D
}
