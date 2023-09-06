#include "h/janela.h"

Janela::Janela(){
	altura = largura = 0;
	jan = NULL;
}

Janela::~Janela(){
	SDL_DestroyWindow(jan);//cout<<"destruiu janela\n";
	altura = largura = 0;
	jan = NULL;
}

bool Janela::inicializar(string titulo, int larg, int alt){
	jan = SDL_CreateWindow(titulo.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, larg, alt, SDL_WINDOW_HIDDEN);
	if(jan == NULL){
		cout<<"Falha ao criar janela. Erro: "<<SDL_GetError()<<"\n";
		return false;
	}
	cout<<"Janela criada.\n";
	largura = larg;
	altura = alt;
	return true;
}

void Janela::exibir(){
	SDL_ShowWindow(jan);
}

//void Janela::redimensionar(int larg, int alt){ //in�til
//	SDL_SetWindowSize(jan,larg,alt);
//}
