#include <SDL.h>
#include <iostream>
using namespace std;

class Janela {
public:
	Janela();
	~Janela();
	bool inicializar(string titulo, int larg, int alt);
	void exibir();
//	void redimensionar(int larg, int alt); //inútil
	
	int altura,largura;
	SDL_Window* jan;
};
