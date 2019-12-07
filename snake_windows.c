#include <stdio.h>

//funcion
void imprimeMenu(short int opcion){
	if (opcion == 1) {
		printf("\n...::BIENVENIDO A SNAKE::..\n--->1-Nueva Partida\n");
		printf("2-Cargar Partida\n3-Borrar partida\n");
		printf("4-Cambiar dificultad\n::");
	}
	else if (opcion == 2) {
		printf("\n...::BIENVENIDO A SNAKE::..\n1-Nueva Partida\n");
		printf("--->2-Cargar Partida\n3-Borrar partida\n");
		printf("4-Cambiar dificultad\n::");
	}
	else if (opcion == 3) {
		printf("\n...::BIENVENIDO A SNAKE::..\n1-Nueva Partida\n");
		printf("2-Cargar Partida\n--->3-Borrar partida\n");
		printf("4-Cambiar dificultad\n::");
	}
	else if (opcion == 4) {
		printf("\n...::BIENVENIDO A SNAKE::..\n1-Nueva Partida\n");
		printf("2-Cargar Partida\n3-Borrar partida\n");
		printf("--->4-Cambiar dificultad\n::");
	}
	else{
		printf("\n...::BIENVENIDO A SNAKE::..\n1-Nueva Partida\n");
		printf("2-Cargar Partida\n3-Borrar partida\n");
		printf("4-Cambiar dificultad\n::");
	}
}

int main (){
	short int opcion;
	opcion=1;
	imprimeMenu (opcion);
	char boton;
	do{
		boton=getchar();
		putchar(boton);
		while (getchar() != '\n');

		switch(boton){
			case 'w'://arriba
				opcion--;
				if (opcion == 0){
					opcion=5;
					imprimeMenu(opcion);
				}
				else
					imprimeMenu(opcion);
				break;
			case 's'://abajo
				opcion++;
				if (opcion == 6){
					opcion=1;
					imprimeMenu(opcion);
				}
				else
					imprimeMenu(opcion);
				break;
			case 'f': //enter
				printf("\nSelecciono la opcion %i del menu", opcion);
				//Sale del while
				break;
			default:
				printf("\nERROR:Opcion no válida\n");
		}

	}while(opcion != 5);
	return 1;
}