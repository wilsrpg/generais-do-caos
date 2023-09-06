#include "h/jogo.h"
#include <locale.h> //pra acentuação no terminal

int main(int argc, char* args[]){
	setlocale(LC_ALL, "Portuguese"); //pra acentuação no terminal
	
	Jogo* jogo = new Jogo();
	
	if(!jogo->inicializar())
		cout<<"Erro ao inicializar.\n";
	else while(jogo->executando){
		jogo->processarEventos();
		jogo->atualizar();
		jogo->renderizar();
	}
	
	//jogo->terminar();
	delete jogo;
	return 0;
}
