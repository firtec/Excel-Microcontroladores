/* ***************************************************************************
**  File Name    : EXCEL.ino
**  Descripción  : Envía datos del conversor en 8 bit (A0) al EXCEL
**                 Pin 2 Inicia Excel - Pin3 Inicia Hoja
**                 Se comunica por la USART a 9600 baudios.
**  Autor        : www.firtec.com.ar
**  Target       : Arduino Nano
** ***************************************************************************/
#define NOP __asm__ __volatile__ ("nop\n\t") // 62.5ns en 16MHz 
#define excel 2   // Pin del boton para activar Excel
#define hoja 3    // Pin para activar la hoja de trabajo
#define F_CPU 16000000
#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

//---- Variables del programa
unsigned int conversion =0;
byte muestras =0;
unsigned int M0=0;
unsigned int contador =0;

byte bandera_1 = 0;
volatile byte dato = 0;

void Leer_Conversor(void){   
do{ 
  M0 += analogRead(A0)>>2;    // Lee el A/D y acumula el dato en M0
  NOP;
  NOP;
  NOP;
  muestras++;         // Incrementa el contador de muestras
}while(muestras <=4);    // Se tomaron  muestras ??
  conversion = M0/5;    // Se busca el promedio de las 5 muestras
  M0 =0;
  muestras =0;
 }

//------- Funcion de configuracion del Hardaware ---------- 
 void setup(){ 
  DDRB = 0b00100000;   // Pin 13 (PB5) como salida.
   UCSR0A = 0x00; // Limpia las banderas de la UART
   UBRR0H = (uint8_t)(UBRR_VALUE >> 8); // Configura baudios
   UBRR0L = (uint8_t)UBRR_VALUE;
   UCSR0C |= (1 << UCSZ00) | (1 << USBS0); // Datos en 8-bits con 1 bit de stop
   UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);   // Activa TX y la RX por interrupcion      
  interrupts();
  pinMode(excel,INPUT_PULLUP);
  pinMode(hoja,INPUT_PULLUP);
  
  pinMode(A0, INPUT);
 
} 
//----------------------------------------
void loop() {
  if (digitalRead(excel) == LOW && bandera_1 == 0) {  
      UDR0 = 0x01;	// Comando para iniciar Excel
      bandera_1 = 1;
  }
  if (digitalRead(hoja) == LOW && bandera_1 == 1) {  
      UDR0 = 0x02;	// Comando para iniciar hoja de trabajo
      bandera_1 = 2;
  }
   contador++;
    if(contador == 10000){
        Leer_Conversor();
        contador = 0;
    }
  } 

ISR(USART_RX_vect){  
  
  byte dato = UDR0; 
  if(dato == 0x01)	// Excel pide datos?
      UDR0 = conversion;
  PORTB ^=  0b00100000;  // Cambia estado del pin 13
 }
//*************** Fin del archivo Firtec Argentina ********************
