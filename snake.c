#include <stdio.h>
#include <stdlib.h>
#include "UART.h"
#include "Systick.h"
#include "int.h"
#include "input.h"
#include "SensorTemperatura.h"

//CONSTANTES
#define FILAS 30
#define COLUMNAS 30
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
int temperatura=0;
int tiempoVelocidad=9;

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
int input;
//variables creadas para la partida guardada
serpiente snakeGuardada [(FILAS-2)*(COLUMNAS-2)];
int longitudSerpienteGuardada;
char cola_xGuardada,cola_yGuardada;

//Funciones utilizadas
void Timer2_Init(int seconds);
void Timer1_Init(unsigned char freq);
//FUNCIONES
void swap(void);
void mueve(void);
void acomoda_cabeza(char movimiento);
void crece(void);
void crea_fruta(void);
void cargaSerpienteEnTablero (void);
void imprimirTablero(void);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
void cerar(void);
int finJuego(void);
void borrarGuardado(void);
void guardaSerpiente(void);
void cargaSerpienteGuardada(void);

void leeTecla(void){
	uint8_t tecla;
	tecla=((GPIOD->DATA & 0x03) << 1)+(GPIOF->DATA & 0x11);
	
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
		UART_OutString("::");
	}
	else if (opcion == 2) {
		UART_OutString("\n\r...::BIENVENIDO A SNAKE::..\n\r1-Nueva Partida\n\r");
		UART_OutString("--->2-Cargar Partida\n\r3-Borrar partida\n\r");
		UART_OutString("::");
	}
	else if (opcion == 3) {
		UART_OutString("\n\r...::BIENVENIDO A SNAKE::..\n\r1-Nueva Partida\n\r");
		UART_OutString("2-Cargar Partida\n\r--->3-Borrar partida\n\r");
		UART_OutString("::");
	}
	else{
		UART_OutString("\n\r...::BIENVENIDO A SNAKE::..\n\r--1-Nueva Partida\n\r");
		UART_OutString("2-Cargar Partida\n\r3-Borrar partida");
		UART_OutString("::");
	}
}

int main(void) {
	
  DisableInterrupts();
	
  UART_Init();
  puertoD_init();
	puertoF_init ();
	Timer1_Init(100);//filtra tecla
	Timer2_Init(20);
	//inicSysTickInt (80000);
	TempSensorInit();
	cerar();
	crea_fruta();
	EnableInterrupts();
	
		
		while(1)
		{
			WaitForInterrupt();
		}
		
		return 0;
}


void SysTick_Handler () {
	//contadorTiempo++;
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
	if(empiezaJuego==0){
		switch (teclaApretada){
			case 0x16: //switch 2, enter
				if (opcion==1){
					empiezaJuego=1;
					teclaApretada=0x17;
					Timer2_Init(tiempoVelocidad);
				}
				else if (opcion==2){
					//cargar partida anterior 
					empiezaJuego=1;
					Timer2_Init(tiempoVelocidad);
					cargaSerpienteGuardada();
				}
				else if (opcion==3){
					//elimina datos serpiente guardada
					borrarGuardado();
				}
				break;
				
			case 0x07://switch 1
				break;
				
			case 0x15://boton izquierda, boton 3
				
				break;
				
			case 0x13: //boton derecha, boton 4
				if(!empiezaJuego){
					opcion++;
					if(opcion > 3)
						opcion=1;
					break;
				}
				
			default:
				break;
				
		}
		imprimeMenu();
		teclaApretada=0x17;
	}
	else{
		if (empiezaJuego!=2)
			imprimirTablero();
		//while(!finJuego()){
		if(!murio){
			//mueve
			mueve();

			//cargaSerpienteEnTablero
			if (empiezaJuego!=2)
				cargaSerpienteEnTablero();
			//imprime
			if (empiezaJuego!=2)
				imprimirTablero();
		}
		else{
			UART_OutString("HA MUERTO");
			
		}
	}
}

void Timer1_Init(unsigned char freq){ 
	
// Initalise Timer1 to trigger every specified seconds
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
// Initalise Timer2 to trigger every few seconds
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


void cargaSerpienteEnTablero (void){
	int i,j;
	for(i=0;i<FILAS;i++){
		for(j=0; j<COLUMNAS;j++){
			if(tablero[i][j]!='f'){
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
	//buscamos la serpiente y ponemos donde tiene que estar
	for(i=0;i<longitudSerpiente;i++){
		if (snake[i].x>=0){
			if (i==0){
				tablero[snake[i].x][snake[i].y]='*';
			}
			else
				tablero[snake[i].x][snake[i].y]='s';
		}
	}
}


//funcion para inicializar el tablero y la serpiente
void cerar(void){
	//primero inicializamos el tablero con las paredes
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
	//ahora inicializamos la serpiente
	for (i=0;i<(FILAS-2)*(COLUMNAS-2);i++){
		snake[i].x=-1;
		snake[i].y=-1;
		snake[i].direccion=0;
	}
	//aca debemos poner la posicion inicial de la serpiente con longitud inicial 3
	snake[0].x=7;
	snake[0].y=7;
	snake[0].direccion=IZQUIERDA;
	snake[1].x=7;
	snake[1].y=8;
	snake[1].direccion=IZQUIERDA;
	snake[2].x=7;
	snake[2].y=9;
	snake[2].direccion=IZQUIERDA;

	cargaSerpienteEnTablero();

}

void imprimirTablero(void){
	UART_OutString("\033\143");
	int i,j;
	for(i=0;i<FILAS;i++){
		for(j=0; j<COLUMNAS;j++){
			UART_OutChar( tablero[i][j]);
		}
		UART_OutChar('\r');
		UART_OutChar('\n');
	}
	UART_OutChar('\r');
	UART_OutChar('\n');
	UART_OutString("Puntuacion actual======");
	UART_OutUDec((longitudSerpiente-3)*10);
	UART_OutChar('\r');
	UART_OutChar('\n');
	/**for(i=0;i<longitudSerpiente;i++){
		printf("\nx: %d y: %d---- %d \n", snake[i].x,snake[i].y, snake[i].direccion);
	}**/
}

void mueve(void){
	char c;
	c=teclaApretada;
	if(c==0x15){ //valor de boton 3
		if(empiezaJuego==2){ //izquierda y vuelve a jugar
			empiezaJuego=1;
			Timer2_Init(tiempoVelocidad);
			return;
		}
		else{
			acomoda_cabeza(IZQUIERDA);
			teclaApretada=0x17;
		}
	}
	else if (c==0x13){ //valor de boton 4
		if(empiezaJuego==2){ // derecha vuelve al menu y guarda todo
			empiezaJuego=0;
			guardaSerpiente();
			return;
		}
		else{
			acomoda_cabeza(DERECHA);
			teclaApretada=0x17;
		}		
	}
	else if (c==0x07){
		if(empiezaJuego!=2){
			Timer2_Init(20);
			UART_OutString("\033\143");
			UART_OutString("Desea guardar partida?\n\rDerecha Si                  Izquierda No");
			empiezaJuego=2;
			return;
		}
	}
	else{
		if(empiezaJuego!=2){ // derecha vuelve al menu y guarda todo
			acomoda_cabeza(NADA);//UN CICLO Y NO PRESIONO NINGUN BOTON
			teclaApretada=0x17;
		}
	}
	
}

void acomoda_cabeza(char movimiento){
	//Como solo hay 2 botones si la direccion es izquierda o derecha solo puede ir hacia arriba y hacia abajo
	//Si la direccion es arriba o abajo solo puede ir a la izquierda o a la derecha
	if (snake[0].direccion == IZQUIERDA){

		if(movimiento==IZQUIERDA){ //va hacia abajo
			swap();
			//control de movimiento (para saber si avanza nomas, si comio algo o murio)
			if(tablero[snake[0].x+1][snake[0].y]=='f'){ //come algo
				longitudSerpiente++;
				crece();
			}
			else if (tablero[snake[0].x+1][snake[0].y]=='s' || tablero[snake[0].x+1][snake[0].y]=='#'){//choca contra su cuerpo
				murio=1; //muere 
			}
			snake[0].direccion=ABAJO;
			snake[0].x++;
		}
		else if (movimiento==DERECHA){ //va hacia arriba
			swap();
			//control de movimiento (para saber si avanza nomas, si comio algo o murio)
			if(tablero[snake[0].x-1][snake[0].y]=='f'){ //come algo
				longitudSerpiente++;
				crece();
			}
			else if (tablero[snake[0].x-1][snake[0].y]=='s' || tablero[snake[0].x-1][snake[0].y]=='#'){//choca contra su cuerpo
				murio=1; //muere 
			}
			snake[0].direccion=ARRIBA;
			snake[0].x--;
		}
		else{//solo avanza en ese ciclo
			if(tablero[snake[0].x][snake[0].y-1]=='f'){ //come algo
				longitudSerpiente++;
				crece();
			}
			else if (tablero[snake[0].x][snake[0].y-1]=='s' || tablero[snake[0].x][snake[0].y-1]=='#'){//choca contra su cuerpo
				murio=1; //muere 

			}
			swap();
			snake[0].y--;
		}

	}
	else if (snake[0].direccion == DERECHA){
		if(movimiento==DERECHA){ //va hacia abajo
			swap();
			//control de movimiento (para saber si avanza nomas, si comio algo o murio)
			if(tablero[snake[0].x+1][snake[0].y]=='f'){ //come algo
				longitudSerpiente++;
				crece();
			}
			else if (tablero[snake[0].x+1][snake[0].y]=='s' || tablero[snake[0].x+1][snake[0].y]=='#'){//choca contra su cuerpo
				murio=1; //muere 
			}
			snake[0].direccion=ABAJO;
			snake[0].x++;
		}
		else if (movimiento==IZQUIERDA){ //va hacia arriba
			swap();
			//control de movimiento (para saber si avanza nomas, si comio algo o murio)
			if(tablero[snake[0].x-1][snake[0].y]=='f'){ //come algo
				longitudSerpiente++;
				crece();
			}
			else if (tablero[snake[0].x-1][snake[0].y]=='s' || tablero[snake[0].x-1][snake[0].y]=='#'){//choca contra su cuerpo
				murio=1; //muere 
			}
			snake[0].direccion=ARRIBA;
			snake[0].x--;
		}
		else{//avanza hacia la derecha
			if(tablero[snake[0].x][snake[0].y+1]=='f'){ //come algo
				longitudSerpiente++;
				crece();
			}
			else if (tablero[snake[0].x][snake[0].y+1]=='s' || tablero[snake[0].x][snake[0].y+1]=='#'){//choca contra su cuerpo
				murio=1; //muere 
			}
			swap();
			snake[0].y++;
		}
	}
	else if (snake[0].direccion == ARRIBA || snake[0].direccion == ABAJO){
		if(movimiento==DERECHA){ //va hacia derecha
			swap();
			//control de movimiento (para saber si avanza nomas, si comio algo o murio)
			if(tablero[snake[0].x][snake[0].y+1]=='f'){ //come algo
				longitudSerpiente++;
				crece();
			}
			else if (tablero[snake[0].x][snake[0].y+1]=='s' || tablero[snake[0].x][snake[0].y+1]=='#'){//choca contra su cuerpo
				murio=1; //muere 
			}
			snake[0].direccion=DERECHA;
			snake[0].y++;
		}
		else if (movimiento==IZQUIERDA){ //va hacia izquierda
			swap();
			//control de movimiento (para saber si avanza nomas, si comio algo o murio)
			if(tablero[snake[0].x][snake[0].y-1]=='f'){ //come algo
				longitudSerpiente++;
				crece();
			}
			else if (tablero[snake[0].x][snake[0].y-1]=='s' || tablero[snake[0].x][snake[0].y-1]=='#'){//choca contra su cuerpo
				murio=1; //muere 
			}
			snake[0].direccion=IZQUIERDA;
			snake[0].y--;
		}
		else{//sigue
			if(snake[0].direccion == ARRIBA){
				if(tablero[snake[0].x-1][snake[0].y]=='f'){ //come algo
					longitudSerpiente++;
					crece();
				}
				else if (tablero[snake[0].x-1][snake[0].y]=='s' || tablero[snake[0].x-1][snake[0].y]=='#'){//choca contra su cuerpo
					murio=1; //muere 
				}
				swap();
				snake[0].x--;
			}
			else{
				if(tablero[snake[0].x+1][snake[0].y]=='f'){ //come algo
					longitudSerpiente++;
					crece();
				}
				else if (tablero[snake[0].x+1][snake[0].y]=='s' || tablero[snake[0].x+1][snake[0].y]=='#'){//choca contra su cuerpo
					murio=1; //muere 

				}
				swap();
				snake[0].x++;
			}
		}
	}
	
}

void crece(void){
	snake [longitudSerpiente-1].x=cola_x;
	snake [longitudSerpiente-1].y=cola_y;
	snake [longitudSerpiente-1].direccion=snake [longitudSerpiente-2].direccion;
	crea_fruta();
	if(longitudSerpiente%5==0){
		tiempoVelocidad++;
		Timer2_Init(tiempoVelocidad);
	}
}

//funcion que crea una frutita con el rand, en el tiva lograra esto con la funcion de temperatura
void crea_fruta(void){
	//crea una nueva frutita
	int bandera=1;
	int frutaX,frutaY;
	while (bandera){
		while((ADC0->RIS & 8) == 0) ;   				/* esperamos que termine la conversion */
		temperatura = ADC0->SSFIFO3 & 0xFFF;  	//tomamos solo los 12 bits leido por el ADC
		ADC0->ISC = 8;          								/* limpiamos la bandera*/
		srand(temperatura);
		frutaX=(rand()%(FILAS-2))+1;
		frutaY=(rand()%(COLUMNAS-2))+1;
		if(tablero[frutaX][frutaY]==' '){
			bandera=0;
			tablero[frutaX][frutaY]='f';
		}
	}
}

//Funcion que sirve para mover la serpiente en su lugar correctamente
void swap(void){
	int i;
	char aux_x,aux_y,aux_direccion;
	char aux_x2,aux_y2,aux_direccion2;

	for (i=1;i<longitudSerpiente;i++){

		if(i==longitudSerpiente-1){
			cola_x=snake[i].x;
			cola_y=snake[i].y;
		}
		if(i==1){
			aux_direccion=snake[i].direccion;
			aux_x=snake[i].x;
			aux_y=snake[i].y;
			snake[i].direccion=snake[i-1].direccion;
			snake[i].x=snake[i-1].x;
			snake[i].y=snake[i-1].y;
		}
		else if (i%2==0){
			aux_direccion2=snake[i].direccion;
			aux_x2=snake[i].x;
			aux_y2=snake[i].y;
			snake[i].direccion=aux_direccion;
			snake[i].x=aux_x;
			snake[i].y=aux_y;
		}
		else {
			aux_direccion=snake[i].direccion;
			aux_x=snake[i].x;
			aux_y=snake[i].y;
			snake[i].direccion=aux_direccion2;
			snake[i].x=aux_x2;
			snake[i].y=aux_y2;
		}
	}
}

int finJuego(void){
	if(murio)
		UART_OutString("\nHA Muerto!\n");	
	return murio;
}

void borrarGuardado(void){
	int i;
	//ahora inicializamos la serpiente
	for (i=0;i<(FILAS-2)*(COLUMNAS-2);i++){
		snakeGuardada[i].x=-1;
		snakeGuardada[i].y=-1;
		snakeGuardada[i].direccion=0;
	}
	//aca debemos poner la posicion inicial de la serpiente con longitud inicial 3
	snakeGuardada[0].x=7;
	snakeGuardada[0].y=7;
	snakeGuardada[0].direccion=IZQUIERDA;
	snakeGuardada[1].x=7;
	snakeGuardada[1].y=8;
	snakeGuardada[1].direccion=IZQUIERDA;
	snakeGuardada[2].x=7;
	snakeGuardada[2].y=9;
	snakeGuardada[2].direccion=IZQUIERDA;
	
	longitudSerpienteGuardada=3;
	
	cola_xGuardada=7;
	cola_yGuardada=9;
}

void guardaSerpiente(void){
	int i;
	//ahora inicializamos la serpiente
	for (i=0;i<(FILAS-2)*(COLUMNAS-2);i++){
		snakeGuardada[i].x=snake[i].x;
		snakeGuardada[i].y=snake[i].y;
		snakeGuardada[i].direccion=snake[i].direccion;
	}
	cola_xGuardada=cola_x;
	cola_yGuardada=cola_y;
	longitudSerpienteGuardada=longitudSerpiente;
	
}
void cargaSerpienteGuardada(void){
	int i;
	//ahora inicializamos la serpiente
	for (i=0;i<(FILAS-2)*(COLUMNAS-2);i++){
		snake[i].x=snakeGuardada[i].x;
		snake[i].y=snakeGuardada[i].y;
		snake[i].direccion=snakeGuardada[i].direccion;
	}
	cola_x=cola_xGuardada;
	cola_y=cola_yGuardada;
	longitudSerpiente=longitudSerpienteGuardada;
}
