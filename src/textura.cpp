#include "h/textura.h"
#include <math.h> //pro sqrt() do desenho do círculo

/*
void desenharCirculo(SDL_Renderer* gameRenderer, int centroX, int centroY, int raio, bool preenchido){
	//double x,y;
	int x,y;
	int aux=-1,aux2=-1;
	for(int i=0; i<raio; i++){ //calcula o 1o quadrante e espelha o resto
		x = sqrt((double)raio*(double)raio - (double)i*(double)i); //teorema de Pitágoras
		y = i;
		for(int j=i; j<raio; j++){
			x = sqrt((double)raio*(double)raio - (double)i*(double)i); //teorema de Pitágoras
			if(aux != x){
				aux = x;
				SDL_RenderDrawPoint(gameRenderer,centroX+x, centroY+y);
				SDL_RenderDrawPoint(gameRenderer,centroX+y, centroY+x);
			}
		}
	}
}*/

/*void desenharCirculo(SDL_Renderer* gameRenderer, int centroX, int centroY, int raio, bool preenchido){
	double x,y;
	//int aux=-1;
//	cout<<"centro: "<<centroX<<","<<centroY<<", raio: "<<raio<<"\n";
	for(int i=0; i<((double)raio*sqrt(2)/2); i++){ //calcula a metade do 1o quadrante e espelha o resto
//		cout<<raio*raio<<" - "<<(centroY-i)*(centroY-i)<<" = "<<raio*raio - (centroY-i)*(centroY-i)<<"\n";
//		cout<<"sqrt(16) = "<<sqrt(16)<<"\n";
		y = i;
		x = sqrt((double)raio*(double)raio - (double)y*(double)y); //teorema de Pitágoras
//		cout<<x<<","<<y<<"\n";
		if(preenchido){
			SDL_RenderDrawLine(gameRenderer, centroX+x, centroY-y, centroX-x, centroY-y);
//			if(int(y) > 0) //pra n desenhar a linha "do equador" 2x e ela ficar mais nítida q o resto
//				SDL_RenderDrawLine(gameRenderer, centroX+x, centroY+y, centroX-x, centroY+y);
				SDL_RenderDrawLine(gameRenderer, centroX+x, centroY+y+1, centroX-x, centroY+y+1);
			//if((int)aux != (int)x){ //pra n desenhar as linhas "dos polos" mais d 1x e elas ficarem mais nítidas q o resto
				//aux = x;
				SDL_RenderDrawLine(gameRenderer, centroX+(raio-x), centroY-(raio-y)+1, centroX-(raio-x), centroY-(raio-y)+1);
				SDL_RenderDrawLine(gameRenderer, centroX+(raio-x), centroY+(raio-y), centroX-(raio-x), centroY+(raio-y));
			//}
//			SDL_RenderDrawLine(gameRenderer, centroX+x, centroY-raio+y, centroX-x, centroY-raio+y);
//			SDL_RenderDrawLine(gameRenderer, centroX+x, centroY+raio-y, centroX-x, centroY+raio-y);
		} else {
			SDL_RenderDrawPoint(gameRenderer,centroX+x, centroY-y);
			SDL_RenderDrawPoint(gameRenderer,centroX+y, centroY-x);
			
			SDL_RenderDrawPoint(gameRenderer,centroX-x, centroY-y);
			SDL_RenderDrawPoint(gameRenderer,centroX-y, centroY-x);
			
			SDL_RenderDrawPoint(gameRenderer,centroX-x, centroY+y);
			SDL_RenderDrawPoint(gameRenderer,centroX-y, centroY+x);
			
			SDL_RenderDrawPoint(gameRenderer,centroX+x, centroY+y);
			SDL_RenderDrawPoint(gameRenderer,centroX+y, centroY+x);
		}
	}
}*/

void desenharCirculo(SDL_Renderer* gameRenderer, int centroX, int centroY, int raio, bool preenchido){
	double x,y;
	//SDL_RenderDrawLine(gameRenderer, centroX-raio, centroY-raio, centroX+raio, centroY-raio);
	if(preenchido)
		for(int i=0; i<raio; i++){ //calcula a metade d cima e espelha embaixo
			y = i;
			x = int(sqrt((double)raio*(double)raio - (double)y*(double)y)); //teorema de Pitágoras
			SDL_RenderDrawLine(gameRenderer, centroX+x, centroY-y, centroX-x, centroY-y);
//			if(int(y) > 0) //pra n desenhar a linha "do equador" 2x e ela ficar mais nítida q o resto
//				SDL_RenderDrawLine(gameRenderer, centroX+x, centroY+y, centroX-x, centroY+y);
				SDL_RenderDrawLine(gameRenderer, centroX+x, centroY+y+1, centroX-x, centroY+y+1);
			//if((int)aux != (int)x){ //pra n desenhar as linhas "dos polos" mais d 1x e elas ficarem mais nítidas q o resto
				//aux = x;
//				SDL_RenderDrawLine(gameRenderer, centroX+(raio-x), centroY-(raio-y)+1, centroX-(raio-x), centroY-(raio-y)+1);
//				SDL_RenderDrawLine(gameRenderer, centroX+(raio-x), centroY+(raio-y), centroX-(raio-x), centroY+(raio-y));
			//}
//			SDL_RenderDrawLine(gameRenderer, centroX+x, centroY-raio+y, centroX-x, centroY-raio+y);
//			SDL_RenderDrawLine(gameRenderer, centroX+x, centroY+raio-y, centroX-x, centroY+raio-y);
	} else
	for(int i=0; i<((double)raio*sqrt(2)/2); i++){ //calcula a metade do 1o quadrante e espelha o resto
		y = i;
		x = int(sqrt((double)raio*(double)raio - (double)y*(double)y)); //teorema de Pitágoras
		
		SDL_RenderDrawPoint(gameRenderer,centroX-x, centroY-y);
		SDL_RenderDrawPoint(gameRenderer,centroX-y, centroY-x);
		
		SDL_RenderDrawPoint(gameRenderer,centroX+x, centroY-y);
		SDL_RenderDrawPoint(gameRenderer,centroX+y, centroY-x);
		
		SDL_RenderDrawPoint(gameRenderer,centroX+x, centroY+y);
		SDL_RenderDrawPoint(gameRenderer,centroX+y, centroY+x);
		
		SDL_RenderDrawPoint(gameRenderer,centroX-x, centroY+y);
		SDL_RenderDrawPoint(gameRenderer,centroX-y, centroY+x);
	}
}

Textura::Textura(){
	altura = largura = 0;
	tex = NULL;
}

Textura::~Textura(){
	liberar();
}

void Textura::liberar(){
	if(tex != NULL){//cout<<"destruiu textura\n";
		SDL_DestroyTexture(tex);
		tex = NULL;
		altura = largura = 0;
	}
}

bool Textura::carregar(SDL_Renderer* gameRenderer,string endereco, SDL_Rect* corte){
	liberar();
	SDL_Surface* superficieTemp = IMG_Load(endereco.c_str());
	SDL_Texture* texturaTemp = NULL;
	
	if(superficieTemp == NULL)
		cout<<"Falha ao carregar arquivo '"<<endereco.c_str()<<"'. Erro: "<<IMG_GetError()<<"\n";
	else {
		texturaTemp = SDL_CreateTextureFromSurface(gameRenderer,superficieTemp);
		if(texturaTemp == NULL)
			cout<<"Falha ao criar textura do arquivo '"<<endereco.c_str()<<"'. Erro: "<<SDL_GetError()<<"\n";
		else {
			if(corte == NULL) {
				cout<<"Arquivo '"<<endereco<<"' carregado.\n";
				largura = superficieTemp->w;
				altura = superficieTemp->h;
			} else {
				SDL_Texture* texCorteTemp = SDL_CreateTexture(gameRenderer,
																											SDL_PIXELFORMAT_RGBA8888,
																											SDL_TEXTUREACCESS_TARGET,
																											corte->w,
																											corte->h);
				if(texCorteTemp == NULL)
					cout<<"Falha ao criar textura para corte do arquivo '"<<endereco.c_str()<<"'. Erro: "<<SDL_GetError()<<"\n";
				else {
					SDL_SetTextureBlendMode(texCorteTemp,SDL_BLENDMODE_BLEND);
					SDL_SetRenderTarget(gameRenderer,texCorteTemp);
					SDL_RenderCopy(gameRenderer,texturaTemp,corte,NULL);
					SDL_SetRenderTarget(gameRenderer,NULL);
					
					texturaTemp = texCorteTemp;
					largura = corte->w;
					altura = corte->h;
				}
				texCorteTemp = NULL;
				SDL_DestroyTexture(texCorteTemp);
			}
		}
		SDL_FreeSurface(superficieTemp);
	}
	
	tex = texturaTemp;
	return tex != NULL;
}

/*void Textura::renderizar(SDL_Renderer* gameRenderer,int x, int y, SDL_Rect* corte){
	SDL_Rect rendQuad = { x, y, largura, altura };
	
	if(corte != NULL){
		rendQuad.w = corte->w;
		rendQuad.h = corte->h;
	}
	
	SDL_RenderCopy(gameRenderer,tex,corte,&rendQuad);
}*/
void Textura::renderizar(SDL_Renderer* gameRenderer,int x, int y, double escala, bool umQuarto){
	SDL_Rect rendQuad = { x, y, largura, altura };
	SDL_Rect rendSrc = { 0, 0, largura*4, altura*4 }; //n entendi pq *4 o.o mas dá certo
	if(escala != 1){
		rendQuad = { x, y, int(largura*escala), int(altura*escala) };
		if(umQuarto)
			rendSrc = { 0, 0, largura/4, altura/4 };
	}
	SDL_RenderCopy(gameRenderer,tex,&rendSrc,&rendQuad);
}

/*struct TexturaFonte : Textura
{
	bool carregar(string endereco);
}fonteTex;

bool TexturaFonte::carregar(string endereco,ren)
{
	liberar();
	
	SDL_Color cor = { 0,0,0 };
	SDL_Surface* superficieTemp = TTF_RenderText_Blended(gFonte,endereco.c_str(),cor);
	
	if(superficieTemp == NULL)
		cout<<"Falha ao carregar arquivo '"<<endereco.c_str()<<"'. Erro: "<<TTF_GetError()<<"\n";
	else
	{
		tex = SDL_CreateTextureFromSurface(ren,superficieTemp);
		if(tex == NULL)
			cout<<"Falha ao criar textura da fonte. Erro: "<<SDL_GetError()<<"\n";
		else
		{
			largura = superficieTemp->w;
			altura = superficieTemp->h;
		}
		SDL_FreeSurface(superficieTemp);
	}
	
	return tex != NULL;
}*/

Texto::Texto(){
	fonte = NULL;
}

Texto::~Texto(){
	if(fonte != NULL){//cout<<"destruiu texto\n";
		TTF_CloseFont(fonte);
		fonte = NULL;
	}
}

bool Texto::carregar(string endereco, int tamanho){
	fonte = TTF_OpenFont(endereco.c_str(), tamanho);
	/*if(fonte == NULL)
		cout<<"Falha ao carregar arquivo '"<<endereco<<"'. Erro: "<<TTF_GetError()<<"\n";
	else
		cout<<"Arquivo '"<<endereco<<"' carregado.\n";*/
	return fonte != NULL;
}

/*void Fonte::liberar(){
	Textura::liberar();
	TTF_CloseFont(fonte);
	fonte = NULL;
}*/

/*void Fonte::escrever(SDL_Renderer* gameRenderer, string texto, int r, int g, int b, int a){
	liberar();
	SDL_Surface* superficieTemp = TTF_RenderText_Blended(fonte,texto.c_str(),SDL_Color{r,g,b,a});
*/

void Texto::definirCor(Uint8 rr, Uint8 gg, Uint8 bb, Uint8 aa){
	r=rr;
	g=gg;
	b=bb;
	a=aa;
}

/*void converterEmUnicode(string texto, Uint16* textoUnicode){
	string acentosChar = "ÁÉÍÓÚÂÊÔÃÕÀÇáéíóúâêôãõàçª";
	Uint16 acentosUnicode[] = {0xc1,0xc9,0xcd,0xd3,0xda,0xc2,0xca,0xd4,0xc3,0xd5,0xc0,0xc7,
														 0xe1,0xe9,0xed,0xf3,0xfa,0xe2,0xea,0xf4,0xe3,0xf5,0xe0,0xe7,0xaa};
	
	for(int i=0; i<texto.length(); i++){
		if((int)(texto[i]) < 0){ //os caracteres 128-255 qd exibidos como inteiros ficam negativos
			for(int j=0; j<acentosChar.length(); j++){
				if(texto[i] == acentosChar[j]){
					textoUnicode[i] = acentosUnicode[j];
					break;
				}
				if(j == acentosChar.length()){
					cout<<"\n\nCARACTERE NÃO CONVERTIDO: "<<texto[i]<<"\n\n";
					system("pause");
				}
			}
		} else
			textoUnicode[i] = texto[i];
	}
}*/

void Texto::escrever(SDL_Renderer* gameRenderer, string texto, bool quebraDeLinha, int largQuebraDeLinha){
	liberar();
	SDL_Surface* superficieTemp;
//	if(quebraDeLinha)
//		superficieTemp = TTF_RenderUTF8_Blended_Wrapped(fonte,texto.c_str(),SDL_Color{r,g,b,a},largQuebraDeLinha);
//	else
//		superficieTemp = TTF_RenderUTF8_Blended(fonte,textoUnicode,SDL_Color{r,g,b,a});
//	int comp = texto.length();
//	Uint16 textoUnicode[comp];
//	//converterEmUnicode(texto,textoUnicode);
//	
//	for(int i=0; i<texto.length(); i++)
//		textoUnicode[i] = texto[i];
	
	if(quebraDeLinha)
		superficieTemp = TTF_RenderUTF8_Blended_Wrapped(fonte,texto.c_str(),SDL_Color{r,g,b,a},largQuebraDeLinha);
	else
		superficieTemp = TTF_RenderUTF8_Blended(fonte,texto.c_str(),SDL_Color{r,g,b,a});
	
//	if(quebraDeLinha)
//		superficieTemp = TTF_RenderUNICODE_Blended_Wrapped(fonte,textoUnicode,SDL_Color{r,g,b,a},largQuebraDeLinha);
//	else
//		superficieTemp = TTF_RenderUNICODE_Blended(fonte,textoUnicode,SDL_Color{r,g,b,a});
	
	if(superficieTemp == NULL)
		cout<<"Falha ao renderizar texto. Erro: "<<TTF_GetError()<<"\n";
	else {
		tex = SDL_CreateTextureFromSurface(gameRenderer,superficieTemp);
		if(tex == NULL)
			cout<<"Falha ao criar textura do texto renderizado. Erro: "<<SDL_GetError()<<"\n";
		else {
			largura = superficieTemp->w;
			altura = superficieTemp->h;
		}
		SDL_FreeSurface(superficieTemp);
	}
}

//void Texto::alinharAEsquerda(){
//	TTF_SetFontWrappedAlign(fonte,TTF_WRAPPED_ALIGN_LEFT);
//}
//void Texto::alinharADireita(){
//	TTF_SetFontWrappedAlign(fonte,TTF_WRAPPED_ALIGN_RIGHT);
//}
//void Texto::alinharCentralizado(){
//	TTF_SetFontWrappedAlign(fonte,TTF_WRAPPED_ALIGN_CENTER);
//}
