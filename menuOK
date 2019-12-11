#include <stdio.h>
#include "UART.h"
#include "Systick.h"
#include "int.h"
#include "input.h"

//CONSTANTES
#define FILAS 15
#define COLUMNAS 25
#define IZQUIERDA 7
#define DERECHA 8
#define ARRIBA 9
#define ABAJO 10
#define NADA 11
#define ESTADO1 1
#define ESTADO2 2
#define ESTADO3 3

//VARIABLES GLOBALES
char tablero [FILAS][COLUMNAS];
int contadorTiempo;
char tiempoAscii[11];
char opcion = 1;
char opcionMenu;
char teclaApretada;
char empiezaJuego=0;

struct serpiente {
	char x;
	char y;
	char direccion;
} typedef serpiente;

//snake windows
//Variables globales
//el maximo tamanho que podria tener la serpiente es de este segun las FILAS y COLUMNAS
//vector que contendra los datos de la posicion de la serpiente
serpiente snake [(FILAS-2)*(COLUMNAS-2)];
char tablero [FILAS][COLUMNAS];
int longitudSerpiente=3;
char cola_x,cola_y;
int murio=0;


//Funciones utilizadas
void creaTablero(void);
void imprimirTablero(void);
void jugar(void);
//char* mi_itoa(int num, char* str);
void Timer2_Init(int seconds);
void Timer1_Init(unsigned char freq);

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

void leeTecla(void){
	uint8_t tecla;
	tecla=(GPIOD->DATA << 1)+GPIOF->DATA;
	
	static uint32_t estado = ESTADO1;
	static uint8_t teclaAnt = 0;
	
//	// ejecutamos la máquina de estados
	
	switch (estado) {
		case ESTADO1:	// no hay tecla apretada
			if (tecla) {
				// una tecla se apretó por primera vez
				teclaAnt = tecla;
				estado = ESTADO2;
			}
			break;
		case ESTADO2:
			if (tecla == teclaAnt) {
				// se confirma la tecla apretada
				teclaApretada = tecla;
				estado = ESTADO3;
			} else if (tecla == 0) {
				// no hay tecla apretada
				teclaAnt = 0;
				estado = ESTADO1;
			} else {
				// se apretó otra tecla distinta, nos quedamos en este estado
				teclaAnt = tecla;
			}
			break;
		case ESTADO3:
			if (tecla != teclaAnt) {
				// se apretó otra tecla distinta o se soltó la tecla
				teclaAnt = tecla;
				estado = ESTADO2;
			} 
			// si se mantiene la misma tecla apretada no hacemos nada
			break;
	}
}

void imprimeMenu(){
	UART_OutString("\033\143");
	if (opcion == 1) {
		UART_OutString("\n\r...::BIENVENIDO A SNAKE::..\n\r--->1-Nueva Partida\n\r");
		UART_OutString("2-Cargar Partida\n\r3-Borrar partida\n\r");
		UART_OutString("4-Cambiar dificultad\n\r::");
	}
	else if (opcion == 2) {
		UART_OutString("\n\r...::BIENVENIDO A SNAKE::..\n\r1-Nueva Partida\n\r");
		UART_OutString("--->2-Cargar Partida\n\r3-Borrar partida\n\r");
		UART_OutString("4-Cambiar dificultad\n\r::");
	}
	else if (opcion == 3) {
		UART_OutString("\n\r...::BIENVENIDO A SNAKE::..\n\r1-Nueva Partida\n\r");
		UART_OutString("2-Cargar Partida\n\r--->3-Borrar partida\n\r");
		UART_OutString("4-Cambiar dificultad\n\r::");
	}
	else if (opcion == 4) {
		UART_OutString("\n\r...::BIENVENIDO A SNAKE::..\n\r1-Nueva Partida\n\r");
		UART_OutString("2-Cargar Partida\n\r3-Borrar partida\n\r");
		UART_OutString("--->4-Cambiar dificultad\n\r::");
	}
	else{
		UART_OutString("\n\r...::BIENVENIDO A SNAKE::..\n\r1-Nueva Partida\n\r");
		UART_OutString("2-Cargar Partida\n\r3-Borrar partida\n\r");
		UART_OutString("4-Cambiar dificultad\n\r::");
	}
}

int main(void) {
	
  DisableInterrupts();
	
  UART_Init();
  puertoD_init();
	puertoF_init ();
	Timer1_Init(100);//filtra tecla
	Timer2_Init(10);
	EnableInterrupts();
	
	//int SW1,SWRight,SWLeft;

	
		while(1)
		{
			WaitForInterrupt();
		}
		
		//imprimeMenu();
		//leeTecla();
		
		//opcion=UART_InChar();
		
		//UART_OutChar('\r');
	  //UART_OutChar('\n');
		
		/**switch(opcion){
			case 0x31:
			DisableInterrupts();
		
			Timer1_Init(50);
			Timer2_Init(1);							//imprime en pantalla el juego
			inicSysTickInt (16000000);	//cuenta el tiempo que transcurre el juego
			
			EnableInterrupts();
			
			//creamos el tablero inicial
			creaTablero();
			while(1){
				
			//jugar();
			}
				
				break;
			case 50:
				

				break;
			case 51:

				break;
			case 52:
				
				break;

			case 48:
				//Sale del while
				break;
			default:
			    UART_OutString("\nERROR:Opcion no valida\n");
		}**/
		

  return 0;
}

/*
* Funcion para jugar hasta que se pierda (por ahora sera infinito nunca perdera)
*/
void jugar(){

	//while(1){
		
	//}
}

/*
* Funcion que simplemente imprime el tablero
*/
void imprimirTablero (){
	
	UART_OutString("\033\143");
	
  int i,j;
  for(i=0;i<FILAS;i++){
    for(j=0; j<COLUMNAS;j++){
	    UART_OutChar( tablero[i][j]);
    }
	UART_OutChar('\r');
	UART_OutChar('\n');
  }
  UART_OutString("Tiempo en segundos: ");
  UART_OutString(tiempoAscii);
	UART_OutChar('\r');
  UART_OutChar('\n');
	
 
}

/*
* Funcion que simplemente crea el tablero del juego
*/
void creaTablero (){
  int i,j;
  for(i=0;i<FILAS;i++){
    for(j=0; j<COLUMNAS;j++){
      if (i==0) 
          tablero[i][j]='#';
      else if (j==0)
        tablero[i][j]='#';
      else if(j==COLUMNAS-1)
        tablero[i][j]='#';
      else if (i==FILAS-1)
        tablero[i][j]='#';
      else
        tablero[i][j]=' ';
    }
  }
}
/**
char* mi_itoa(int num, char* str){
	
	int i;
	int sig = -1;
	if(num<0){
		num*=-1;
		if(num<10){
			str[0]='-'; str[1]='0'+num; str[2]=0; return str;
		}else{
			sig=1;
		}
	}
	else if(num==0){
		str[0]='0'; str[1]='0'; str[2]=0; return str;
	} 
	else{
		if(num<10){
			str[0]='0'; str[1]='0'+num; str[2]=0; return str;
		}else{
			sig=0;
		}
	}
	if(sig!=-1){
		int copia=num, m=1, cifras=1;
		for(;copia>=10;copia/=10) cifras++;
		for(int x=0;x<(cifras-1);x++) m*=10;
		float v1=num;
		int v2=0, v3=num;
		if(sig) str[0]='-';
		for(i=0; i<cifras; i++){//Descompone en factores
			v1/=m;
			v2=(int)v1*m;
			v3-=v2;
			m/=10;
			str[i+sig]=48+(int)v1;
			v1=v3;
		} 
		str[i+sig]=0;//Si str está a 0 no es necesario..
	}
	return str;
}**/


void SysTick_Handler () {
	contadorTiempo++;
	//mi_itoa(contadorTiempo, tiempoAscii);
}

void TIMER1A_Handler(void){
	TIMER1->ICR = 0x00000001;
	leeTecla();
	//contadorTiempo++;
	//mi_itoa(contadorTiempo, tiempoAscii);
	

}

void TIMER2A_Handler(void){
	TIMER2->ICR = 0x00000001;
  //imprimirTablero();
		switch (teclaApretada){
		case 0x2E: //switch 2, enter
			if (opcion==1){
				empiezaJuego=1;
			}
			break;
			
		case 0x1F://switch 1
			break;
			
		case 0x2D://boton izquierda, boton 3
			break;
			
		case 0x2B: //boton derecha, boton 4
			opcion++;
			if(opcion > 4)
				opcion=1;
			break;
	
		default:
			break;
			
	}
	teclaApretada=0;
	imprimeMenu();
	}


void Timer1_Init(unsigned char freq){ 
	
// Initalise Timer1 to trigger every specified seconds(assuming 880Mhz PLL)
  unsigned long volatile delay;
	SYSCTL->RCGCTIMER |= 0x02;
	delay = SYSCTL->RCGCTIMER;
	TIMER1->CTL = 0x00;
	TIMER1->CFG = 0x00;
	TIMER1->TAMR = 0x00000002;
	TIMER1->TAILR =  (16000000 / freq)  - 1;
	TIMER1->TAPR = 0;
	TIMER1->ICR = 0x01;
	TIMER1->IMR = 0x01;
	NVIC->IP[5] = (NVIC->IP[5]& 0xFFFF0FFF)|(0x02 <<13);
	//NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF0FFF)|(0x02 <<13)
	NVIC->ISER[0] |= 1 << 21;
	TIMER1->CTL = 0x01;
	


}

void Timer2_Init(int fps){ 
// Initalise Timer2 to trigger every few seconds(assuming 880Mhz PLL)
  unsigned long volatile delay;
  SYSCTL->RCGCTIMER |= 0x04;
  delay = SYSCTL->RCGCTIMER;
  TIMER2->CTL = 0x00000000;
  TIMER2->CFG = 0x00000000; // 32-bit mode
  TIMER2->TAMR = 0x00000002; // periodic mode, default down-count
  TIMER2->TAILR = (16000000 / fps)  - 1; 
  TIMER2->TAPR = 0; // bus clock resolution
  TIMER2->ICR = 0x00000001;
  TIMER2->IMR = 0x00000001;
	NVIC->IP[5] = (NVIC->IP[5] &0x0FFFFFFF)|(0x03 << 29);
	NVIC->ISER[0] |= 1<<23;
	TIMER2->CTL = 0x00000001; 
}


