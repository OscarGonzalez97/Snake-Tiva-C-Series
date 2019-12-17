#include <stdio.h>
#include <stdlib.h>
#include "UART.h"
#include "Systick.h"
#include "int.h"
#include "input.h"
#include "SensorTemperatura.h"

//CONSTANTES
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3
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
int contadorTiempo=0;
char tiempoAscii[11];
char opcion = 1;
char opcionMenu;
char teclaApretada;
char empiezaJuego=0;
int temperatura=0;
int tiempoVelocidad=9;
int nivel;
int nivelGuardado;
//para top 10
char letra1;
char letra2;
char letra3;
char letra4;
char letraAModificar=1;

struct ranking{
  char letra1;
  char letra2;
  char letra3;
  char letra4;
  int puntos;
} typedef ranking;

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
ranking top10[11];
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
char* mi_itoa(int num, char* str);
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
void rankingMenu(void);
void reiniciaRanking(void);
void ordenarRanking(void);
void printRanking(void);

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
	inicSysTickInt (15999999);
	TempSensorInit();
	cerar();
	reiniciaRanking();
	crea_fruta();
	EnableInterrupts();
	
		
		while(1)
		{
			WaitForInterrupt();
		}
		
		return 0;
}


void SysTick_Handler () {
	if(!murio && empiezaJuego!=2 && empiezaJuego != 0){
		contadorTiempo++;
		mi_itoa(contadorTiempo, tiempoAscii);
	}
}

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
}

void TIMER1A_Handler(void){
	TIMER1->ICR = 0x00000001;
	leeTecla();	
}

void TIMER2A_Handler(void){
	TIMER2->ICR = 0x00000001;
  //imprimirTablero();
	if(empiezaJuego==0){
		switch (teclaApretada){
			case 0x16: //switch 2, enter
				if (opcion==1){
					empiezaJuego=1;
					nivel=empiezaJuego;
					teclaApretada=0x17;
					Timer2_Init(tiempoVelocidad);
				}
				else if (opcion==2){
					//cargar partida anterior 
					empiezaJuego=nivelGuardado;
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
	else if(empiezaJuego==10 || empiezaJuego==11){ //ranking top 10
		rankingMenu();
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
			//ranking de puntuaciones
			empiezaJuego=10; //abre parte de ranking 10
			Timer2_Init(10);
			letra1='a';
			letra2='a';
			letra3='a';
			letra4='a';
			UART_OutString("\n\rNavegue con las teclas del protoboard y presione el SW derecho del controlador para guardar\n\r");
			UART_OutChar(letra1);
			UART_OutChar(letra2);
			UART_OutChar(letra3);
			UART_OutChar(letra4);
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
	if(empiezaJuego==1){
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
	}
	else if (empiezaJuego==3){ // dos obstaculos
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
		if (tablero[10][10]==' ')//ponemos fijo esto como nivel 2 si no hay serpiente alli
			tablero[10][10]='#';
		if (tablero[10][11]==' ')//ponemos fijo esto como nivel 2 si no hay serpiente alli
			tablero[10][11]='#';
		if (tablero[15][15]==' ')//ponemos fijo esto como nivel 2 si no hay serpiente alli
			tablero[15][15]='#';
		if (tablero[15][16]==' ')//ponemos fijo esto como nivel 2 si no hay serpiente alli
			tablero[15][16]='#';
	}
	else if (empiezaJuego==4){
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
		if (tablero[10][10]==' ')//ponemos fijo esto como nivel 3 si no hay serpiente alli
			tablero[10][10]='#';
		if (tablero[10][11]==' ')//ponemos fijo esto como nivel 3 si no hay serpiente alli
			tablero[10][11]='#';
		if (tablero[15][15]==' ')//ponemos fijo esto como nivel 3 si no hay serpiente alli
			tablero[15][15]='#';
		if (tablero[15][16]==' ')//ponemos fijo esto como nivel 3 si no hay serpiente alli
			tablero[15][16]='#';
		if (tablero[13][13]==' ')//ponemos fijo esto como nivel 3 si no hay serpiente alli
			tablero[13][13]='#';
		if (tablero[13][14]==' ')//ponemos fijo esto como nivel 3 si no hay serpiente alli
			tablero[13][14]='#';
		if (tablero[5][5]==' ')//ponemos fijo esto como nivel 3 si no hay serpiente alli
			tablero[5][5]='#';
		if (tablero[5][6]==' ')//ponemos fijo esto como nivel 3 si no hay serpiente alli
			tablero[5][6]='#';
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
	UART_OutString("Tiempo en segundos======");
	UART_OutString(tiempoAscii);
	UART_OutChar('\r');
	UART_OutChar('\n');
	//tiempo en segundos tiempoAscii
	/**for(i=0;i<longitudSerpiente;i++){
		printf("\nx: %d y: %d---- %d \n", snake[i].x,snake[i].y, snake[i].direccion);
	}**/
}

void mueve(void){
	char c;
	c=teclaApretada;
	if(c==0x15){ //valor de boton 3
		if(empiezaJuego==2){ //izquierda y vuelve a jugar
			empiezaJuego=nivel;
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
	if(longitudSerpiente%7==0){ //avanza a nivel 2
		empiezaJuego=3;
		nivel=3;
	}
	if(longitudSerpiente%13==0){ //avanza a nivel 3
		empiezaJuego=4;
		nivel=4;
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
	nivelGuardado=1;
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
	nivelGuardado=nivel;
	
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
	nivel=nivelGuardado;

}

void rankingMenu(void){
	char c;
	c=teclaApretada;
	if(empiezaJuego==10){
		if(c==0x15){ //valor de boton 3 cambia de letra
			UART_OutString("\033\143");
			UART_OutString("\n\rNavegue con las teclas del protoboard y presione el SW derecho del controlador para guardar\n\r");
			if(letraAModificar==1){
				letra1++;
				if (letra1 == ('z'+1))
					letra1='a';
			}
			else if (letraAModificar==2){
				letra2++;
				if (letra2 == ('z'+1))
					letra2='a';
			}
			else if (letraAModificar==3){
				letra3++;
				if (letra3 == ('z'+1))
					letra3='a';
			}
			else if (letraAModificar==4){
				letra4++;
				if (letra4 == ('z'+1))
					letra4='a';
			}
			UART_OutChar(letra1);
			UART_OutChar(letra2);
			UART_OutChar(letra3);
			UART_OutChar(letra4);
		}
		else if (c==0x13){ //valor de boton 4 avanza de letra
			UART_OutString("\n\rNavegue con las teclas del protoboard y presione el SW derecho del controlador para guardar\n\r");
			letraAModificar++;
			if(letraAModificar==5){
				letraAModificar=1;
			}
		}
		else if(c==0x16){//SW derecha guarda y ordena
			UART_OutString("\n\rNavegue con las teclas del protoboard y presione el SW derecho del controlador para guardar\n\r");
			top10[0].letra1=letra1;
			top10[0].letra2=letra2;
			top10[0].letra3=letra3;
			top10[0].letra4=letra4;
			top10[0].puntos=(longitudSerpiente-3)*10;
			ordenarRanking();
			empiezaJuego=11;
		}
	}
	if(empiezaJuego==11){
		UART_OutString("\n\rPresione  para volver al menu principal\n\r");
		printRanking();
		if (c==0x07){//SW izquierda
			empiezaJuego=0;
			cerar();
			murio=0;
			crea_fruta();
			longitudSerpiente=3;
			contadorTiempo=0;
			letraAModificar=1;
		}
	}
}

void ordenarRanking(void){
  register int a,b;
  char aux_letra1, aux_letra2, aux_letra3, aux_letra4;
  int aux_puntos;

	for(a=1;a<11;a++){
		for(b=10;b>=a;b--){
			if(top10[b-1].puntos>top10[b].puntos){	//En esta parte se controla con su vecino y luego hace el cambio

				aux_letra1=top10[b].letra1;
				aux_letra2=top10[b].letra2;
				aux_letra3=top10[b].letra3;
				aux_letra4=top10[b].letra4;
				aux_puntos=top10[b].puntos;

				top10[b].letra1=top10[b-1].letra1;
				top10[b].letra2=top10[b-1].letra2;
				top10[b].letra3=top10[b-1].letra3;
				top10[b].letra4=top10[b-1].letra4;
				top10[b].puntos=top10[b-1].puntos;

				top10[b-1].letra1=aux_letra1;
				top10[b-1].letra2=aux_letra2;
				top10[b-1].letra3=aux_letra3;
				top10[b-1].letra4=aux_letra4;
				top10[b-1].puntos=aux_puntos;
			}
		}
	}
}
void printRanking(void){
	int i;
	UART_OutString("\033\143");
	UART_OutString("..::TOP 10 PLAYERS SNAKE TIVA :...");
	UART_OutString("\n\r");
	for (i=10;i>0;i--){
		UART_OutChar(top10[i].letra1);
		UART_OutChar(top10[i].letra2);
		UART_OutChar(top10[i].letra3);
		UART_OutChar(top10[i].letra4);
		UART_OutString("----");
		UART_OutUDec(top10[i].puntos);
		UART_OutString("\r\n");
	}
}
void reiniciaRanking(void){
	int i;
  for(i=0;i<11;i++){
    top10[i].letra1='a';
    top10[i].letra2='a';
    top10[i].letra3='a';
    top10[i].letra4='a';
    top10[i].puntos=0;
  }
}

