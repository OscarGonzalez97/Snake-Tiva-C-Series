#include <stdio.h>
#define FILAS 15
#define COLUMNAS 15
#define IZQUIERDA 90
//Datos definidos por el programador
struct serpiente {
	char x;
	char y;
	char esCabeza;
	char direccion;
} typedef serpiente;

//Variables globales
//el maximo tamanho que podria tener la serpiente es de este segun las FILAS y COLUMNAS
//matriz que contendra los datos de la posicion de la serpiente
serpiente snake [(FILAS-2)*(COLUMNAS-2)];
char tablero [FILAS][COLUMNAS];
int longitudSerpiente=3;

//FUNCIONES
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
		snake[i].esCabeza=0;
		snake[i].direccion=0;
	}
	//aca debemos poner la posicion inicial de la serpiente con longitud inicial 3
	snake[0].x=6;
	snake[0].y=6;
	snake[0].esCabeza=1;
	snake[0].direccion=IZQUIERDA;
	snake[1].x=6;
	snake[1].y=7;
	snake[1].esCabeza=0;
	snake[1].direccion=IZQUIERDA;
	snake[2].x=6;
	snake[2].y=8;
	snake[2].esCabeza=0;
	snake[2].direccion=IZQUIERDA;
}
void imprimirTablero(void){
	int i,j;
	for(i=0;i<FILAS;i++){
		for(j=0; j<COLUMNAS;j++){
			printf("%c",tablero[i][j]);
		}
		printf("\n");
	}
}

int finJuego(void){
	return 1;
}

int main (){
	cerar();
	imprimirTablero();
	while(!finJuego()){
	
	}
	return 1;
}