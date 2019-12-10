#include <stdio.h>
#define FILAS 15
#define COLUMNAS 25
#define IZQUIERDA 7
#define DERECHA 8
#define ARRIBA 9
#define ABAJO 10
#define NADA 11

//Datos definidos por el programador
struct serpiente {
	char x;
	char y;
	char direccion;
} typedef serpiente;

//Variables globales
//el maximo tamanho que podria tener la serpiente es de este segun las FILAS y COLUMNAS
//vector que contendra los datos de la posicion de la serpiente
serpiente snake [(FILAS-2)*(COLUMNAS-2)];
char tablero [FILAS][COLUMNAS];
int longitudSerpiente=3;
char cola_x,cola_y;

//FUNCIONES
void swap(void);
void mueve(void);
void acomoda_cabeza(char movimiento);
void crece(void);


void cargaSerpienteEnTablero (void){
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
	int i,j;
	for(i=0;i<FILAS;i++){
		for(j=0; j<COLUMNAS;j++){
			printf("%c",tablero[i][j]);
		}
		printf("\n");
	}
	for(i=0;i<longitudSerpiente;i++){
		printf("\nx: %d y: %d---- %d \n", snake[i].x,snake[i].y, snake[i].direccion);
	}
}

void mueve(void){
	char c;
	
	c=getchar();
	if(c=='a'){ //valor de boton 3
		acomoda_cabeza(IZQUIERDA);
	}
	else if (c=='d'){ //valor de boton 4
		acomoda_cabeza(DERECHA);
	}
	else{
		acomoda_cabeza(NADA);//UN CICLO Y NO PRESIONO NINGUN BOTON
	}
	
	while (getchar() != '\n');
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
				//muere

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
				//muere
			}
			snake[0].direccion=ARRIBA;
			snake[0].x--;
		}
		else{//solo avanza en ese ciclo
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
				//muere
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
				//muere
			}
			snake[0].direccion=ARRIBA;
			snake[0].x--;
		}
		else{//avanza hacia la derecha
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
				//muere
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
				//muere
			}
			snake[0].direccion=IZQUIERDA;
			snake[0].y--;
		}
		else{//sigue
			if(snake[0].direccion == ARRIBA){
				swap();
				snake[0].x--;
			}
			else{
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
	return 0;
}

int main (){	

	cerar();
	imprimirTablero();

	while(!finJuego()){
	//mueve
	mueve();
	//cargaSerpienteEnTablero
	cargaSerpienteEnTablero();
	//imprime
	imprimirTablero();

	}
	return 1;
}