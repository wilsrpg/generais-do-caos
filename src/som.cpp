#include "h/som.h"

Musica::Musica(){
	musica = NULL;
}

Musica::~Musica(){
	Mix_FreeMusic(musica);//cout<<"destruiu m�sica\n";
	musica = NULL;
}

bool Musica::carregar(string endereco){
	musica = Mix_LoadMUS(endereco.c_str());
	if(musica == NULL){
		cout<<"Falha ao carregar arquivo '"<<endereco<<"'. Erro: "<<Mix_GetError()<<"\n";
		return false;
	} else {
		cout<<"Arquivo '"<<endereco<<"' carregado.\n";
		Mix_VolumeMusic(63);
		return true;
	}
}

void Musica::tocar(int loops){
	Mix_PlayMusic(musica, loops);
}

bool Musica::tocando(){
	return Mix_PlayingMusic();
}

bool Musica::emPausa(){
	return Mix_PausedMusic();
}

void Musica::pausar(){
	if(Mix_PlayingMusic())
		if(!Mix_PausedMusic())
			Mix_PauseMusic();
}

void Musica::despausar(){
	if(Mix_PlayingMusic())
		if(Mix_PausedMusic())
			Mix_PauseMusic();
}

void Musica::parar(){
	if(Mix_PlayingMusic())
		Mix_HaltMusic();
}


Som::Som(){
	som = NULL;
	canal = -1;
}

Som::~Som(){
	Mix_FreeChunk(som);//cout<<"destruiu som\n";
	som = NULL;
}

bool Som::carregar(string endereco){
	som = Mix_LoadWAV(endereco.c_str());
	if(som == NULL){
		cout<<"Falha ao carregar arquivo '"<<endereco<<"'. Erro: "<<Mix_GetError()<<"\n";
		return false;
	} else {
		cout<<"Arquivo '"<<endereco<<"' carregado.\n";
		Mix_VolumeChunk(som,63);
		return true;
	}
}

void Som::tocar(int loops){
	Mix_PlayChannel(canal, som, loops);
}

void Som::definirCanal(int c){
	if(c >= -1 && c <= 8)
	canal = c;
}

bool Som::tocando(){
	return Mix_Playing(canal);
}

bool Som::emPausa(){
	return Mix_Paused(canal);
}

void Som::pausar(){
	if(Mix_Playing(canal))
		if(!Mix_Paused(canal))
			Mix_Pause(canal);
}

void Som::despausar(){
	if(Mix_Playing(canal))
		if(Mix_Paused(canal))
			Mix_Pause(canal);
}

void Som::parar(){
	if(Mix_Playing(canal))
		Mix_HaltChannel(canal);
}
