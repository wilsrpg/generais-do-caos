#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
using namespace std;

void desenharCirculo(SDL_Renderer* gameRenderer, int centroX, int centroY, int raio, bool preenchido=false);

class Textura {
public:
	Textura();
	~Textura();
	
	bool carregar(SDL_Renderer* gameRenderer, string endereco, SDL_Rect* corte = NULL);
	//void renderizar(SDL_Renderer* gameRenderer, int x, int y, SDL_Rect* corte = NULL);
	void renderizar(SDL_Renderer* gameRenderer, int x, int y, double escala=1, bool umQuarto=false);
	void liberar();
	
	int altura,largura;
	SDL_Texture* tex;
};

class Texto : public Textura {
public:
	Texto();
	~Texto();
	
	bool carregar(string endereco, int tamanho);
	void definirCor(Uint8 rr, Uint8 gg, Uint8 bb, Uint8 aa=255);
	void escrever(SDL_Renderer* gameRenderer, string texto, bool quebraDeLinha=false, int largQuebraDeLinha=1);
//	void alinharAEsquerda();
//	void alinharADireita();
//	void alinharCentralizado();
	
	Uint8 r=0,g=0,b=0,a=255;
	TTF_Font* fonte;
};
