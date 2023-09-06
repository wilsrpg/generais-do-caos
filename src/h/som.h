#include <SDL_mixer.h>
#include <iostream>
using namespace std;

class Musica { //inútil, Som já faz tudo isso e melhor
public:
	Musica();
	~Musica();
	bool carregar(string endereco);
	void tocar(int loops=-1);
	bool tocando();
	bool emPausa();
	void pausar();
	void despausar();
	void parar();
	
	Mix_Music* musica;
};

class Som {
public:
	Som();
	~Som();
	bool carregar(string endereco);
	void tocar(int loops=0);
	void definirCanal(int c);
	
	bool tocando();
	bool emPausa();
	void pausar();
	void despausar();
	void parar();
	
	int canal;
	Mix_Chunk* som;
};
