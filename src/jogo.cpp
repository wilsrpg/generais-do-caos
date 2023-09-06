#include "h/jogo.h"
#include <math.h>
#include <random>
#include <ctime> //pro random
//#include <iomanip> //pro texto do fps
#include <fstream> //pros arquivos de debug e relatório

//mostrar consentimentos na pilha

//oq acontece com peão na última fila qd tds os generais estão vivos? só volta pra reserva msm?

//bool bugCatcher=false;
//if(bugCatcher){system("pause");bugCatcher=false;}

//cores
struct CorPadrao { int r,g,b; };
CorPadrao corCasasEscurasPadrao = {131,86,49};
CorPadrao corCasasClarasPadrao = {190,187,131};
CorPadrao corCursorPadrao = {0,0,255};
CorPadrao corCartasVersoVermelhas = {160,32,32};
CorPadrao corCartasVersoPretas = {32,32,32};
CorPadrao corCartasVersoBorda = {255,255,255};

CorPadrao corAcaoSelecao = {0,0,255};
CorPadrao corAcaoMovimento = {0,255,0};
CorPadrao corAcaoAtaque = {255,0,0};
CorPadrao corAcaoInvocacao = {255,255,0};
CorPadrao corAcaoBencao = {255,0,255};
CorPadrao corAcaoCarta = {0,255,255};
CorPadrao corAcao[] = {corAcaoSelecao,corAcaoMovimento,corAcaoAtaque,corAcaoInvocacao,corAcaoBencao,corAcaoCarta};

bool exibindoGrade;
bool exibindoCasas;
bool exibindoAreas;
bool exibindoAreasSimplificadas;
bool usandoPilha;
bool consentindoComTeclaEspaco;
bool exibindoDebugger;

//renderizador global
SDL_Renderer* gameRenderer;

//pastas padrão
string pastaArquivos = "D:/Users/Wils/Documents/Code Projects/generais-do-caos/arquivos/";
string pastaDebug = "D:/Users/Wils/Documents/Code Projects/generais-do-caos/debug/";

//fluxos de dados
stringstream ssAux,ssAux2,ssRegistro;
ofstream ofsRegistro,ofsDebugger;
string registroArquivo = pastaDebug+"generaisdocaos-registrodepartidas.txt";
string debuggerArquivo = pastaDebug+"generaisdocaos-debugger.txt";

bool existe(void* ponteiro){
	return ponteiro != NULL;
}

Jogo::Jogo(){
	cout<<"Instanciando jogo...\n";
	gameRenderer = NULL;
	janela = new Janela();
	fundo = new Textura();
	fundoPosX = 0;
	fundoPosY = 0;
	fundoVelH = -0.5; //pixels/frame, negativo = pra esquerda
	fundoVelV = -0.15; //pixels/frame, negativo = pra cima
	
	tabPrincipal = new Tabuleiro(TABULEIRO_POSX, TABULEIRO_POSY, TAB_QTDE_COLUNAS, TAB_QTDE_FILAS);
	tabPrincipal->nome = "campo";
	
	jogador[0] = new Jogador(branco,tabPrincipal);
	jogador[1] = new Jogador(preto,tabPrincipal);
	
	jogador[0]->oponente = jogador[1];
	jogador[1]->oponente = jogador[0];
	jogadorDaVez = NULL;
	jogadorDosDescartesExibidos = NULL;
	
	cursor = new Cursor;
	
	for(unsigned int i=0; i<graficoDasPecas.size(); i++)
		graficoDasPecas[i] = new Textura();
	
	for(int j=0; j<2; j++)
		for(int i=0;i<8;i++){
			jogador[j]->general[i] = new PecaGeneral(jogador[j],(tipoPeca)(i/2 + (i>0?1:0))); //0=rei, 1=rainha, 2=bispo, 3=cavalo, 4=torre
			jogador[j]->unidade[i] = new PecaUnidade(jogador[j]);
			
			jogador[j]->todasAsPecas[i] = jogador[j]->general[i];
			jogador[j]->todasAsPecas[i+8] = jogador[j]->unidade[i];
		}
	pecaSobCursor = NULL;
	pecaSelecionada = NULL;
	copiaDaPecaSelecionadaNoCursor = new CopiaTransparenteDaImagemDaPecaSelecionada;
	
	casaAlvoSobCursor = NULL;
	
	for(unsigned int i=0; i<graficoDasCartas.size(); i++)
		graficoDasCartas[i] = new Textura();
	
	jogador[0]->baralho.resize(QTDE_CARTAS_CADA_BARALHO);
	jogador[1]->baralho.resize(QTDE_CARTAS_CADA_BARALHO);
	for(int j=0; j<2; j++)
		for(int i=0; i<QTDE_CARTAS_CADA_BARALHO; i++){
			//jogador 0 = dois últimos naipes (ouros e copas), jogador 1 = dois primeiros naipes (espadas e paus)
			jogador[j]->baralho[i] = new Carta((tipoCarta)(i%(QTDE_CARTAS_CADA_BARALHO/2)),(cartaNaipe)(i/13+2*(!j)),jogador[j]);
	}
	graficoDasCartasVerso[0] = new Textura();
	graficoDasCartasVerso[1] = new Textura();
	cartaSobCursor = NULL;
	cartaSelecionada = NULL;
	
//	musBgm = new Musica();
//	musVitoria = new Musica();
//	musMusica = new Musica();
	somBgm = new Som();
	if(existe(somBgm))
		somBgm->definirCanal(1);
	somVitoria = new Som();
	if(existe(somVitoria))
		somVitoria->definirCanal(2);
	somDerrota = new Som();
	if(existe(somDerrota))
		somDerrota->definirCanal(2);
	somSelecionar = new Som();
	somConfirmar = new Som();
	somCancelar = new Som();
	
	//tituloTexto = new Texto();
	debugTexto = new Texto();
	debug2Texto = new Texto();
	//fpsAtualTexto = new Texto();
	rodadaTexto = new Texto();
	tempoDePartidaTexto = new Texto();
	turnoTexto = new Texto();
	acoesTexto = new Texto();
	dadoTexto = new Texto();
	mensagemTexto = new Texto();
	infoPecaOuCartaTexto = new Texto();
	consentirTexto = new Texto();
	
	executando = true;
	exibindoGrade = true;
	exibindoCasas = true;
	exibindoAreas = true;
	exibindoAreasSimplificadas = false;
	usandoPilha = true;
	consentindoComTeclaEspaco = false;
	exibindoDebugger = false;
	turno = 0;
	acoesPorTurno = 0;
	tempoDePartida = 0;
	musicaHabilitada = true;
	olhandoDescartes = false;
	
	cout<<"Jogo instanciado com sucesso.\n";
}

bool Jogo::inicializar(){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		cout<<"Falha ao inicializar. Erro: "<<SDL_GetError()<<"\n";
		return false;
	}
	cout<<"SDL2 inicializado.\n";
	
	//Set texture filtering to linear
	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		cout<<"Warning: Linear texture filtering not enabled!\n";
	
	if(!janela->inicializar("Generais do Caos",JANELA_LARGURA,JANELA_ALTURA))
		return false;
	
	gameRenderer = SDL_CreateRenderer(janela->jan,-1,0);
	if(gameRenderer == NULL){
		cout<<"Falha ao criar renderizador. Erro: "<<SDL_GetError()<<"\n";
		return false;
	}
	SDL_SetRenderDrawColor(gameRenderer,255,255,255,255);
	SDL_SetRenderDrawBlendMode(gameRenderer,SDL_BLENDMODE_BLEND);
	cout<<"Renderizador criado.\n";
	
	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags)){
		cout<<"Falha ao inicializar subsistema de imagens (SDL_image). Erro: "<<IMG_GetError()<<"\n";
		return false;
	}
	cout<<"Subsistema de imagens (SDL_image) inicializado.\n";
	
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
		cout<<"Falha ao inicializar subsistema de áudio (SDL_mixer). Erro: "<<Mix_GetError()<<"\n";
		return false;
	}
	cout<<"Subsistema de áudio (SDL_mixer) inicializado.\n";
	
	if(TTF_Init() == -1){
		cout<<"Falha ao inicializar subsistema de fontes (SDL_ttf). Erro: "<<TTF_GetError()<<"\n";
		return false;
	}
	cout<<"Subsistema de fontes (SDL_ttf) inicializado.\n";
	
	srand(time(nullptr)); //inicializando a seed do rand()
	cout<<"\n";
	if(!carregarArquivos())
		return false;
	
	fimDeJogo();
	//colocarPecasNasReservas();
	for(int i=0;i<8;i++)
		for(int j=0; j<2; j++){
			moverPecaPara(jogador[j]->unidade[i],jogador[j]->reserva,(j+1)%2,i);
			moverPecaPara(jogador[j]->general[i],jogador[j]->reserva,j%2,i);
		}
	
	ticksAtual = SDL_GetTicks();
	for(int i=0; i<AMOSTRAS_FPS; i++)
		fpsAcumulado[i] = FPS;
	
	//ofsDebugger.open(debuggerArquivo.c_str(),ios::trunc);
	
	janela->exibir();
	if(musicaHabilitada)
		somBgm->tocar(-1);
	return true;
}

bool carregarVarios(SDL_Renderer* gameRenderer, string endereco, vector<Textura*>& v, int ncol, int nfil){
	SDL_Surface* superficieTemp = IMG_Load(endereco.c_str());
	SDL_Texture* texTotalTemp = NULL;
	
	if(superficieTemp == NULL){
		cout<<"Falha ao carregar arquivo '"<<endereco.c_str()<<"'. Erro: "<<IMG_GetError()<<"\n";
		return false;
	} else {
		texTotalTemp = SDL_CreateTextureFromSurface(gameRenderer,superficieTemp);
		if(texTotalTemp == NULL){
			cout<<"Falha ao criar textura do arquivo '"<<endereco.c_str()<<"'. Erro: "<<SDL_GetError()<<"\n";
			return false;
		} else {
			int largTotal= superficieTemp->w;
			int altTotal = superficieTemp->h;
			int largCada = largTotal/ncol;
			int altCada = altTotal/nfil;
			SDL_FreeSurface(superficieTemp);
			
			SDL_Rect corte = {0,0,largCada,altCada};
			for(int f=0,i=0; f<nfil; f++)
				for(int c=0; c<ncol; c++){
					SDL_Texture* texCorteTemp = SDL_CreateTexture(gameRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
																												largCada, altCada);
					SDL_SetTextureBlendMode(texCorteTemp,SDL_BLENDMODE_BLEND);
					SDL_SetRenderTarget(gameRenderer,texCorteTemp);
					corte.x = c*largCada;
					corte.y = f*altCada;
					SDL_RenderCopy(gameRenderer,texTotalTemp,&corte,NULL);
					v[i]->tex = texCorteTemp;
					v[i]->altura = altCada*ESCALA;
					v[i]->largura = largCada*ESCALA;
					i++;
					texCorteTemp = NULL;
					SDL_DestroyTexture(texCorteTemp);
				}
		}
	}
	SDL_SetRenderTarget(gameRenderer,NULL);
	return true;
}

bool Jogo::carregarArquivos(){
	string endereco;
	
	endereco = pastaArquivos+"fundo.png";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	fundo->carregar(gameRenderer,endereco);
	
	endereco = pastaArquivos+"tabuleiro.png";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	if(!tabPrincipal->tex->carregar(gameRenderer,endereco)){
		cout<<"O programa desenhará o modelo padrão do tabuleiro.\n";
		exibindoGrade = exibindoCasas = false;
		tabPrincipal->largura = tabPrincipal->nColunas*TAMANHO_ESPACO;
		tabPrincipal->altura = tabPrincipal->nFilas*TAMANHO_ESPACO;
		tabPrincipal->posW = tabPrincipal->posX+tabPrincipal->largura;
		tabPrincipal->posH = tabPrincipal->posY+tabPrincipal->altura;
		for(int j=0; j<2; j++){
			jogador[j]->reserva->largura = jogador[j]->reserva->nColunas*TAMANHO_ESPACO;
			jogador[j]->reserva->altura = jogador[j]->reserva->nFilas*TAMANHO_ESPACO;
			jogador[j]->reserva->posW = jogador[j]->reserva->posX+jogador[j]->reserva->largura;
			jogador[j]->reserva->posH = jogador[j]->reserva->posY+jogador[j]->reserva->altura;
		}
	} else {
		int largTP = tabPrincipal->tex->largura;
		int largTR = largTP/TAB_QTDE_COLUNAS*TAB_RES_QTDE_COLUNAS;
		int altTP = tabPrincipal->tex->altura;
		int altTR = altTP/TAB_QTDE_FILAS*TAB_RES_QTDE_FILAS;
		SDL_Rect cortesTabRes[2] = { {0, 0, largTR, altTR} , {largTP-largTR, altTP-altTR, largTR, altTR} };
		
		tabPrincipal->tex->largura = TAMANHO_ESPACO * tabPrincipal->nColunas;
		tabPrincipal->tex->altura = TAMANHO_ESPACO * tabPrincipal->nFilas;
		tabPrincipal->largura = tabPrincipal->tex->largura;
		tabPrincipal->altura = tabPrincipal->tex->altura;
		tabPrincipal->posW = tabPrincipal->posX+tabPrincipal->largura;
		tabPrincipal->posH = tabPrincipal->posY+tabPrincipal->altura;
		
		for(int j=0; j<2; j++){
			SDL_Rect areaCorte = cortesTabRes[j];
			jogador[j]->reserva->tex->carregar(gameRenderer,endereco,&areaCorte);
			jogador[j]->reserva->tex->largura = TAMANHO_ESPACO * TAB_RES_QTDE_COLUNAS;
			jogador[j]->reserva->tex->altura = TAMANHO_ESPACO * TAB_RES_QTDE_FILAS;
			jogador[j]->reserva->largura = jogador[j]->reserva->tex->largura;
			jogador[j]->reserva->altura = jogador[j]->reserva->tex->altura;
			jogador[j]->reserva->posW = jogador[j]->reserva->posX+jogador[j]->reserva->largura;
			jogador[j]->reserva->posH = jogador[j]->reserva->posY+jogador[j]->reserva->altura;
		}
	}
	
//	endereco = pastaArquivos+"cursor.png";
//	if(!cursor->tex->carregar(gameRenderer,endereco)){
//		cout<<"O programa desenhará o modelo padrão do cursor.\n";
		cursor->largura = TAMANHO_ESPACO;
		cursor->altura = TAMANHO_ESPACO;
//	} else {
//		cout<<"Arquivo '"<<endereco<<"' carregado.\n";
//		cursor->tex->largura *= ESCALA;
//		cursor->tex->altura *= ESCALA;
//		cursor->largura = cursor->tex->largura;
//		cursor->altura = cursor->tex->altura;
//	}
	moverCursorPara(jogador[0]->reserva->posX,jogador[0]->reserva->posY);
	
	endereco = pastaArquivos+"pecas.png";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	if(!carregarVarios(gameRenderer,endereco,graficoDasPecas,6,2)){
		cout<<"O programa desenhará o modelo padrão das peças.\n";
		for(int j=0; j<2; j++)
			for(int i=0; i<16; i++){
				jogador[j]->todasAsPecas[i]->texNome->carregar(pastaArquivos+"fonte.ttf",TAMANHO_PADRAO_FONTE_PECAS_NOME);
				if(j == 0){
					jogador[j]->todasAsPecas[i]->texNome->r = 255;
					jogador[j]->todasAsPecas[i]->texNome->g = 255;
					jogador[j]->todasAsPecas[i]->texNome->b = 255;
				}
				jogador[j]->todasAsPecas[i]->texNome->escrever(gameRenderer,jogador[j]->todasAsPecas[i]->nome,true,TAMANHO_ESPACO*7/8);
			}
	} else {
		cout<<"Arquivo '"<<endereco<<"' carregado.\n";
		for(int i=0;i<8;i++){
			jogador[0]->unidade[i]->tex = graficoDasPecas[5];
			jogador[1]->unidade[i]->tex = graficoDasPecas[11];
			jogador[0]->general[i]->tex = graficoDasPecas[i/2 + (i>0?1:0)];
			jogador[1]->general[i]->tex = graficoDasPecas[6+i/2 + (i>0?1:0)];
		}
		float redimensionamento = jogador[0]->todasAsPecas[0]->tex->largura / (float)TAMANHO_ESPACO;
		int alturaRedimensionada = jogador[0]->todasAsPecas[0]->tex->altura / redimensionamento;
		for(int j=0; j<2; j++)
			for(int i=0; i<16; i++){
				jogador[j]->todasAsPecas[i]->tex->largura = TAMANHO_ESPACO;
				jogador[j]->todasAsPecas[i]->tex->altura = alturaRedimensionada;
			}
	}
	
	endereco = pastaArquivos+"cartas-frente.png";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	if(!carregarVarios(gameRenderer,endereco,graficoDasCartas,13,4)){
		cout<<"O programa desenhará o modelo padrão das cartas.\n";
		for(int j=0; j<2; j++)
			for(int i=0; i<QTDE_CARTAS_CADA_BARALHO; i++){
				jogador[j]->baralho[i]->largura = LARGURA_PADRAO_CARTAS;
				jogador[j]->baralho[i]->altura = ALTURA_PADRAO_CARTAS;
				jogador[j]->baralho[i]->texTextoSimbolo->carregar(pastaArquivos+"fonte.ttf",TAMANHO_PADRAO_FONTE_CARTAS_SIMBOLO);
				jogador[j]->baralho[i]->texTextoNaipe->carregar(pastaArquivos+"fonte.ttf",TAMANHO_PADRAO_FONTE_CARTAS_SIMBOLO);
				jogador[j]->baralho[i]->texTextoNome->carregar(pastaArquivos+"fonte.ttf",TAMANHO_PADRAO_FONTE_CARTAS_NOME);
				if(j == 0){
					jogador[j]->baralho[i]->texTextoSimbolo->r = 255;
					jogador[j]->baralho[i]->texTextoNaipe->r = 255;
					jogador[j]->baralho[i]->texTextoNome->r = 255;
				}
			}
		for(int j=0; j<2; j++)
			for(int i=0; i<QTDE_CARTAS_CADA_BARALHO; i++){
				jogador[j]->baralho[i]->texTextoSimbolo->escrever(gameRenderer,jogador[j]->baralho[i]->simbolo);
				jogador[j]->baralho[i]->texTextoNaipe->escrever(gameRenderer,jogador[j]->baralho[i]->naipe);
				jogador[j]->baralho[i]->texTextoNome->escrever(gameRenderer,jogador[j]->baralho[i]->nome,true,LARGURA_PADRAO_CARTAS);
			}
	} else {
		cout<<"Arquivo '"<<endereco<<"' carregado.\n";
		for(unsigned int i=0; i<graficoDasCartas.size(); i++){
			graficoDasCartas[i]->largura = LARGURA_PADRAO_CARTAS;
			graficoDasCartas[i]->altura = ALTURA_PADRAO_CARTAS;
		}
		for(int i=0; i<QTDE_CARTAS_CADA_BARALHO; i++){
			//jogador preto tem os 2 primeiros naipes (espadas e paus)
			jogador[1]->baralho[i]->tex = graficoDasCartas[i];
			jogador[1]->baralho[i]->largura = jogador[1]->baralho[i]->tex->largura;
			jogador[1]->baralho[i]->altura = jogador[1]->baralho[i]->tex->altura;
			//jogador branco tem os 2 últimos naipes (ouros e copas), por isso o índice abaixo
			jogador[0]->baralho[i]->tex = graficoDasCartas[i+QTDE_CARTAS_CADA_BARALHO];
			jogador[0]->baralho[i]->largura = jogador[0]->baralho[i]->tex->largura;
			jogador[0]->baralho[i]->altura = jogador[0]->baralho[i]->tex->altura;
		}
	}
	
	endereco = pastaArquivos+"cartas-verso.png";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	/*if(!graficoDasCartasVerso->carregar(gameRenderer,endereco)){
		cout<<"O programa desenhará o modelo padrão do verso das cartas.\n";
		graficoDasCartasVerso->largura = LARGURA_PADRAO_CARTAS;
		graficoDasCartasVerso->altura = ALTURA_PADRAO_CARTAS;
	} else {
		cout<<"Arquivo '"<<endereco<<"' carregado.\n";
		graficoDasCartasVerso->largura = LARGURA_PADRAO_CARTAS;
		graficoDasCartasVerso->altura = ALTURA_PADRAO_CARTAS;
		for(int j=0; j<2; j++)
			for(int i=0; i<QTDE_CARTAS_CADA_BARALHO; i++)
				jogador[j]->baralho[i]->texVerso = graficoDasCartasVerso;
	}*/
	if(!carregarVarios(gameRenderer,endereco,graficoDasCartasVerso,2,1)){
		cout<<"O programa desenhará o modelo padrão do verso das cartas.\n";
		for(int j=0; j<2; j++){
			graficoDasCartasVerso[j]->largura = LARGURA_PADRAO_CARTAS;
			graficoDasCartasVerso[j]->altura = ALTURA_PADRAO_CARTAS;
		}
	} else {
		cout<<"Arquivo '"<<endereco<<"' carregado.\n";
		for(int j=0; j<2; j++){
			graficoDasCartasVerso[j]->largura = LARGURA_PADRAO_CARTAS;
			graficoDasCartasVerso[j]->altura = ALTURA_PADRAO_CARTAS;
			for(int i=0; i<QTDE_CARTAS_CADA_BARALHO; i++)
				jogador[j]->baralho[i]->texVerso = graficoDasCartasVerso[j];
		}
	}
	for(int j=0; j<2; j++)
		jogador[j]->reembaralhar();
	
	endereco = pastaArquivos+"mus-bgm.mp3";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	somBgm->carregar(endereco);
	endereco = pastaArquivos+"mus-vitoria.mp3";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	somVitoria->carregar(endereco);
	endereco = pastaArquivos+"mus-derrota.mp3";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	somDerrota->carregar(endereco);
	
	endereco = pastaArquivos+"som-selecionar.wav";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	somSelecionar->carregar(endereco);
	endereco = pastaArquivos+"som-confirmar.wav";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	somConfirmar->carregar(endereco);
	endereco = pastaArquivos+"som-cancelar.wav";
	cout<<"Carregando arquivo '"<<endereco<<"'...\n";
	somCancelar->carregar(endereco);
	
	Mix_Volume(-1,64);
	volumeGeral = Mix_Volume(-1,-1);
	
	endereco = pastaArquivos+"fonte.ttf";
	//tituloTexto->carregar(endereco,TAMANHO_FONTE_TITULO);
	debugTexto->carregar(endereco,TAMANHO_FONTE_DEBUG);
	debug2Texto->carregar(endereco,TAMANHO_FONTE_DEBUG);
	//fpsAtualTexto->carregar(endereco,TAMANHO_FONTE_TEMPO);
	rodadaTexto->carregar(endereco,TAMANHO_FONTE_TEMPO);
	tempoDePartidaTexto->carregar(endereco,TAMANHO_FONTE_TEMPO);
	turnoTexto->carregar(endereco,TAMANHO_FONTE_TEMPO);
	acoesTexto->carregar(endereco,TAMANHO_FONTE_TEMPO);
	dadoTexto->carregar(endereco,TAMANHO_FONTE_TEMPO);
	mensagemTexto->carregar(endereco,TAMANHO_FONTE_TEMPO);
	infoPecaOuCartaTexto->carregar(endereco,TAMANHO_FONTE_TEMPO);
	consentirTexto->carregar(endereco,TAMANHO_FONTE_TEMPO);
	
	if(fundo->largura > 0){
		//tituloTexto->definirCor(255,255,255);
		debugTexto->definirCor(255,255,255);
		debug2Texto->definirCor(255,255,255);
		//fpsAtualTexto->definirCor(255,255,255);
		rodadaTexto->definirCor(255,255,255);
		tempoDePartidaTexto->definirCor(255,255,255);
		turnoTexto->definirCor(255,255,255);
		acoesTexto->definirCor(255,255,255);
		dadoTexto->definirCor(255,255,255);
		mensagemTexto->definirCor(255,255,255);
		infoPecaOuCartaTexto->definirCor(255,255,255);
		consentirTexto->definirCor(255,255,255);
	}
	//tituloTexto->escrever(gameRenderer, "Generais do Caos");
	
	return true;
}

void Jogo::processarEventos(){
	SDL_Event e;
	if(SDL_PollEvent(&e)!=0){
		//botão fechar
		if(e.type == SDL_QUIT)
			executando = false;
		
		//teclado
		if(e.type == SDL_KEYDOWN){
			switch (e.key.keysym.sym){
			case SDLK_ESCAPE:
				somCancelar->tocar();
				cancelarSelecao();
				executando = false;
				break;
				
			case SDLK_UP:
			case SDLK_KP_8:
				moverCursorPara(cima);
				break;
			case SDLK_DOWN:
			case SDLK_KP_2:
			//case SDLK_KP_5:
				moverCursorPara(baixo);
				break;
			case SDLK_LEFT:
			case SDLK_KP_4:
				moverCursorPara(esquerda);
				break;
			case SDLK_RIGHT:
			case SDLK_KP_6:
				moverCursorPara(direita);
				break;
				
			case SDLK_KP_7:
				moverCursorPara(cima);
				moverCursorPara(esquerda);
				break;
			case SDLK_KP_9:
				moverCursorPara(cima);
				moverCursorPara(direita);
				break;
			case SDLK_KP_1:
				moverCursorPara(baixo);
				moverCursorPara(esquerda);
				break;
			case SDLK_KP_3:
				moverCursorPara(baixo);
				moverCursorPara(direita);
				break;
				
			case SDLK_PAGEUP:
				moverCursorPara(cima,true);
				break;
			case SDLK_PAGEDOWN:
				moverCursorPara(baixo,true);
				break;
			case SDLK_HOME:
				moverCursorPara(esquerda,true);
				break;
			case SDLK_END:
				moverCursorPara(direita,true);
				break;
				
			case SDLK_z:
			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				somSelecionar->tocar();
				selecionar();
				break;
				
			case SDLK_x:
			case SDLK_BACKSPACE:
			case SDLK_KP_0:
				somCancelar->tocar();
				cancelarSelecao();
				break;
				
			case SDLK_LSHIFT:
				if(jogador[1]->esperandoConsentimento){
					jogador[0]->consentiu = true;
					cancelarSelecao();
				}
				break;
				
			case SDLK_RSHIFT:
				if(jogador[0]->esperandoConsentimento){
					jogador[1]->consentiu = true;
					cancelarSelecao();
				}
				break;
				
			case SDLK_BACKSLASH:
				if(jogando)
					jogador[0]->consentindoAutomaticamente = !jogador[0]->consentindoAutomaticamente;
				break;
				
			case SDLK_SLASH:
				if(jogando)
					jogador[1]->consentindoAutomaticamente = !jogador[1]->consentindoAutomaticamente;
				break;
				
			case SDLK_LCTRL:
				jogador[0]->exibindoCartasDasMaos = !jogador[0]->exibindoCartasDasMaos;
				for(unsigned int i=0; i<jogador[0]->mao.size(); i++)
					jogador[0]->mao[i]->revelada = !jogador[0]->mao[i]->revelada || jogador[0]->mao[i]->ativada ||
																				 jogador[0]->mao[i]->anulada || jogador[0]->exibindoCartasDasMaos;
				break;
			
			case SDLK_RCTRL:
				jogador[1]->exibindoCartasDasMaos = !jogador[1]->exibindoCartasDasMaos;
				for(unsigned int i=0; i<jogador[1]->mao.size(); i++)
					jogador[1]->mao[i]->revelada = !jogador[1]->mao[i]->revelada || jogador[1]->mao[i]->ativada ||
																				 jogador[1]->mao[i]->anulada || jogador[1]->exibindoCartasDasMaos;
				break;
				
			case SDLK_SPACE:
				if(consentindoComTeclaEspaco){
					if(jogador[1]->esperandoConsentimento){
						jogador[0]->consentiu = true;
						cancelarSelecao();
					} else if(jogador[0]->esperandoConsentimento){
						jogador[1]->consentiu = true;
						cancelarSelecao();
					}
				}
				break;
				
				
			case SDLK_INSERT:
				if(jogando && existe(cartaSobCursor))
					if(cartaSobCursor->localC == locBaralho){
						cartaSobCursor->jog->puxarCarta();
						if(cartaSobCursor->jog->mao.size() == MAXIMO_CARTAS_NA_MAO || cartaSobCursor->jog->baralho.size() == 0)
//							cartaSobCursor = cartaSobCursor->jog->mao.front();
							moverCursorPara(cartaSobCursor->jog->mao.front()->posX,cartaSobCursor->jog->mao.front()->posY);
							//moverCursorPara(cima);
						//if(cartaSobCursor->jog->baralho.size()>0)
						else
//							cartaSobCursor = cartaSobCursor->jog->baralho.back();
							moverCursorPara(cartaSobCursor->jog->baralho.back()->posX,cartaSobCursor->jog->baralho.back()->posY);
//						else
//							cartaSobCursor = cartaSobCursor->jog->mao.front();
					}
				break;
				
			case SDLK_DELETE:
				if(jogando && existe(cartaSobCursor))
					if(cartaSobCursor->localC == locMao){
						cartaSobCursor->jog->descartar(cartaSobCursor);
						//cartaSobCursor = cartaSobCursor->jog->descartes.back();
						if(cartaSobCursor->jog->mao.size()>0)
							//cartaSobCursor = cartaSobCursor->jog->mao.back();
							moverCursorPara(cartaSobCursor->jog->mao.back()->posX,cartaSobCursor->jog->mao.back()->posY);
						else
							moverCursorPara(baixo);
						//cartaSobCursor = obterCartaEm(mouseX,mouseY);
					}
				break;
				
				
			case SDLK_F1:
				somConfirmar->tocar();
				if(musicaHabilitada){
					somVitoria->parar();
					somDerrota->parar();
					if(!somBgm->tocando())
						somBgm->tocar(-1);
				}
				if(SDL_GetModState() & KMOD_ALT)
					novoJogo2();  //pra agilizar testes
				else
					novoJogo();
				break;
				
			case SDLK_F2:
				somConfirmar->tocar();
				exibindoGrade = !exibindoGrade;
				break;
				
			case SDLK_F3:
				somConfirmar->tocar();
				exibindoCasas = !exibindoCasas;
				break;
				
			case SDLK_F4:
				if(jogando){
					somConfirmar->tocar();
					if(!exibindoAreas && !exibindoAreasSimplificadas)
						exibindoAreas = true;
					else if(exibindoAreas && !exibindoAreasSimplificadas)
						exibindoAreasSimplificadas = true;
					else if(exibindoAreas && exibindoAreasSimplificadas)
						exibindoAreas = exibindoAreasSimplificadas = false;
				}
				break;
				
			case SDLK_F5:
				musicaHabilitada = !musicaHabilitada;
				if(musicaHabilitada)
					somBgm->tocar();
				else
					somBgm->parar();
				break;
				
			case SDLK_F6:
				if(Mix_Volume(-1,-1) != 0)
					Mix_Volume(-1,0);
				else {
					somConfirmar->tocar();
					Mix_Volume(-1,volumeGeral);
				}
				break;
				
			case SDLK_F7:
				volumeGeral = Mix_Volume(-1,-1);
				if(volumeGeral > 0){
					somSelecionar->tocar();
					volumeGeral -= 8;
					if(volumeGeral < 0) volumeGeral = 0;
					Mix_Volume(-1,volumeGeral);
				}
				break;
								
			case SDLK_F8:
				volumeGeral = Mix_Volume(-1,-1);
				if(volumeGeral < 128){
					somSelecionar->tocar();
					volumeGeral += 8;
					if(volumeGeral > 128) volumeGeral = 128;
					Mix_Volume(-1,volumeGeral);
				}
				break;
				
			case SDLK_F9:
				somConfirmar->tocar();
				consentindoComTeclaEspaco = !consentindoComTeclaEspaco;
				break;
				
			case SDLK_F10:
				somConfirmar->tocar();
				usandoPilha = !usandoPilha;
				break;
				
			case SDLK_F11:
				somConfirmar->tocar();
				exibindoDebugger = !exibindoDebugger;
				break;
				
			case SDLK_F12:
				somConfirmar->tocar();
				for(int j=0; j<2; j++){
					jogador[j]->exibindoCartasDasMaos = !jogador[j]->exibindoCartasDasMaos;
					for(unsigned int i=0; i<jogador[j]->mao.size(); i++)
						if(jogador[j]->exibindoCartasDasMaos)
							jogador[j]->mao[i]->revelada = true;
						else
							jogador[j]->mao[i]->revelada = false;
				}
				break;
				
			/*case SDLK_F12:
				somConfirmar->tocar();
				cout<<"\nImprimindo relatório...\n";
				cout<<ssRegistro.str();
				cout<<"\nFim do relatório.\n";
				break;
				*/
			}
		}
		
		//mouse
		if(e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN){
			SDL_GetMouseState(&mouseX,&mouseY);
			moverCursorPara(mouseX,mouseY);
		}
		if(e.type == SDL_MOUSEBUTTONDOWN){
			if(e.button.button == SDL_BUTTON_LEFT){
				somSelecionar->tocar();
				selecionar();
			}
			if(e.button.button == SDL_BUTTON_RIGHT){
				somCancelar->tocar();
				cancelarSelecao();
			}
		}
		
		/*/janela
		if(e.type == SDL_WINDOWEVENT){
			if(e.window.type == SDL_WINDOWEVENT_RESIZED){
				janela->largura = e.window.data1;
				janela->altura = e.window.data2;
				SCALING = janela->largura/JANELA_LARGURA;
			}
		}*/
	}
}

void Jogo::atualizar(){
	atualizarFps();
	atualizarTempoDePartida();
	//if(exibindoDebugger) //debugger deve estar sempre ativo, só sua exibição q deve ser alternável
	//if((totalDeQuadros)%FPS == 0)
		debugger();
	pecaSobCursor = obterPecaEm(cursor);
	//cartaSobCursor = obterCartaEm(cursor); dentro d moverCursorPara()
	
	if(rolandoDadoInicial)
		rolarDadoInicial();
	
	if(jogando){
		definirPecasSelecionaveis();
		definirCartasSelecionaveis();
		definirCasasSelecionaveis();
		casaAlvoSobCursor = obterCasaAlvoEm(cursor);
		
		//posicionar cópia da peça selecionada, deve ficar depois de definirCasasSelecionaveis() e antes de redefinirRaiosDeBencao()
		copiaDaPecaSelecionadaNoCursor->visivel = false;
		if(existe(pecaSelecionada) && existe(casaAlvoSobCursor))
			for(unsigned int i=0; i<casasAlvo.size(); i++)
				if(casaAlvoSobCursor == &casasAlvo[i]){
					moverPecaPara(copiaDaPecaSelecionadaNoCursor,cursor);
					copiaDaPecaSelecionadaNoCursor->visivel = true;
					break;
				}
		
		redefinirRaiosDeBencao();
		concederBonusPorBencaos();
		concederBonusPorSuporte();
		aplicarBonus();
		checarPilha();
		checarPassagemDeTurno();
	}
}

void Jogo::renderizar(){
	SDL_SetRenderDrawColor(gameRenderer,255,255,255,255);
	SDL_RenderClear(gameRenderer);
	
	//fundo passando
	if(fundo->largura > 0){
		int repeticoesH = 2 + janela->largura / fundo->largura;
		int repeticoesV = 2 + janela->altura / fundo->altura;
		fundoPosX+=fundoVelH;
		fundoPosY+=fundoVelV;
		if(fundoPosX > fundo->largura) fundoPosX -= fundo->largura;
		if(fundoPosX < -fundo->largura) fundoPosX += fundo->largura;
		if(fundoPosY > fundo->altura) fundoPosY -= fundo->altura;
		if(fundoPosY < -fundo->altura) fundoPosY += fundo->altura;
		for(int i=0; i<repeticoesH; i++)
			for(int j=0; j<repeticoesV; j++)
				fundo->renderizar(gameRenderer, i*fundo->largura + fundoPosX, j*fundo->altura + fundoPosY);
	}
	
	//tabuleiros
	tabPrincipal->renderizar();
	jogador[0]->reserva->renderizar();
	jogador[1]->reserva->renderizar();
	
	//cursor
	if(cursor->visivel)
		cursor->renderizar();
	
	//tds os tipos d casas
	if(exibindoAreas){
		for(unsigned int i=0; i<casasAlvo.size(); i++)
			casasAlvo[i].renderizar();
		
		for(unsigned int i=0; i<casasAlcance.size(); i++)
			casasAlcance[i].renderizar(OPAC_CASAS_ALCANCE);
		
		if(existe(pecaSobCursor)){
			if(existe(pecaSelecionada)){
				if(copiaDaPecaSelecionadaNoCursor->visivel){
					if(pecaSobCursor->jog == pecaSelecionada->jog){
						for(unsigned int i=0; i<copiaDaPecaSelecionadaNoCursor->casasBencao.size(); i++)
							copiaDaPecaSelecionadaNoCursor->casasBencao[i].renderizar();
					} else
						for(unsigned int i=0; i<pecaSobCursor->casasBencao.size(); i++)
							pecaSobCursor->casasBencao[i].renderizar();
				} else
					for(unsigned int i=0; i<pecaSobCursor->casasBencao.size(); i++)
						pecaSobCursor->casasBencao[i].renderizar();
			} else
				for(unsigned int i=0; i<pecaSobCursor->casasBencao.size(); i++)
					pecaSobCursor->casasBencao[i].renderizar();
		} else if(copiaDaPecaSelecionadaNoCursor->visivel){
			copiaDaPecaSelecionadaNoCursor->renderizar();
			for(unsigned int i=0; i<copiaDaPecaSelecionadaNoCursor->casasBencao.size(); i++)
				copiaDaPecaSelecionadaNoCursor->casasBencao[i].renderizar();
		}
	}
	
	//quadrado de seleção na peça selecionada, indpndntmnt se as áreas estão sendo mostradas e sem estar dentro do vetor casasAlvo
	if(existe(pecaSelecionada)){
		CasaAlvo qps = {pecaSelecionada->tab, pecaSelecionada->coluna, pecaSelecionada->fila, acaoSel};
		qps.renderizar();
	}
	
	//peças
	for(int j=0; j<tabPrincipal->nFilas; j++)
		for(int i=0; i<tabPrincipal->nColunas; i++)
			if(existe(obterPecaEm(tabPrincipal,i,j)))
				obterPecaEm(tabPrincipal,i,j)->renderizar();
	for(int jog=0; jog<2; jog++)
		for(int j=0; j<jogador[jog]->reserva->nFilas; j++)
			for(int i=0; i<jogador[jog]->reserva->nColunas; i++)
				if(existe(obterPecaEm(jogador[jog]->reserva,i,j)))
					obterPecaEm(jogador[jog]->reserva,i,j)->renderizar();
	
	if(exibindoAreas && copiaDaPecaSelecionadaNoCursor->visivel)
		copiaDaPecaSelecionadaNoCursor->renderizar();
	
	//cartas
	for(int j=0; j<2; j++)
		for(unsigned int i=0; i<jogador[j]->baralho.size(); i++)
			jogador[j]->baralho[i]->renderizar();
	for(int j=0; j<2; j++)
		if(jogador[j] != jogadorDosDescartesExibidos) //se estiver mostrando, só renderiza depois
			for(unsigned int i=0; i<jogador[j]->descartes.size(); i++)
				jogador[j]->descartes[i]->renderizar();
	//renderiza primeiro as cartas inativas, depois as ativas, pra estas ficarem por cima
	for(int j=0; j<2; j++)
		for(unsigned int i=0; i<jogador[j]->mao.size(); i++)
			if(!jogador[j]->mao[i]->ativada)
				jogador[j]->mao[i]->renderizar();
	for(int j=0; j<2; j++)
		for(unsigned int i=0; i<jogador[j]->mao.size(); i++)
			if(jogador[j]->mao[i]->ativada)
				jogador[j]->mao[i]->renderizar();
	
	//a carta selecionada fica em primeiro plano
	if(existe(cartaSelecionada))
		cartaSelecionada->renderizar();
	if(existe(cartaSelecionada)){
		SDL_Rect quadr = {cartaSelecionada->posX, cartaSelecionada->posY, cartaSelecionada->largura, cartaSelecionada->altura};
		SDL_SetRenderDrawColor(gameRenderer,corAcaoSelecao.r,corAcaoSelecao.g,corAcaoSelecao.b,OPAC_SELECAO_CARTA);
		SDL_RenderFillRect(gameRenderer,&quadr);
	}
	
	//passei pra depois de olhandoDescartes
	/*if(cartaSobCursor != NULL && cartaSobCursor != cartaSelecionada)
		cartaSobCursor->renderizar();
	if(cartaSobCursor != NULL){
		SDL_Rect quadr;
		SDL_SetRenderDrawColor(gameRenderer,corCasasSelecao.r,corCasasSelecao.g,corCasasSelecao.b,OPAC_CURSOR);
		for(int i=0; i<CURSOR_PADRAO_ESPESSURA; i++){
			quadr = {cartaSobCursor->posX+i,cartaSobCursor->posY+i,cartaSobCursor->largura-i*2,cartaSobCursor->altura-i*2};
			SDL_RenderDrawRect(gameRenderer,&quadr);
		}
	}*/
	
	//textos
	mensagemTexto->renderizar(gameRenderer, tabPrincipal->posX, 0);
	rodadaTexto->renderizar(gameRenderer, 0, 0);
	tempoDePartidaTexto->renderizar(gameRenderer, JANELA_LARGURA-tempoDePartidaTexto->largura, 0);
	if(existe(jogadorDaVez)){
		turnoTexto->renderizar(gameRenderer, ((int)jogadorDaVez->cor)*(JANELA_LARGURA-turnoTexto->largura),
														 tempoDePartidaTexto->altura);
		acoesTexto->renderizar(gameRenderer, ((int)jogadorDaVez->cor)*(JANELA_LARGURA-acoesTexto->largura),
													 tempoDePartidaTexto->altura + turnoTexto->altura);
	}
	//textos: dados
	ssAux.str("");
	dadoTexto->escrever(gameRenderer," ");
	if(jogador[0]->dado > 0){
		ssAux<<"Dado: "<<jogador[0]->dado;
		dadoTexto->escrever(gameRenderer,ssAux.str());
		dadoTexto->renderizar(gameRenderer, 0, tempoDePartidaTexto->altura + turnoTexto->altura + acoesTexto->altura);
	}
	ssAux.str("");
	if(jogador[1]->dado > 0){
		ssAux<<"Dado: "<<jogador[1]->dado;
		dadoTexto->escrever(gameRenderer,ssAux.str());
		dadoTexto->renderizar(gameRenderer, JANELA_LARGURA - dadoTexto->largura,
													tempoDePartidaTexto->altura + turnoTexto->altura + acoesTexto->altura);
	}
	//textos: prompt pra consentir
	if(jogador[0]->esperandoConsentimento){
		SDL_Rect quadr = {JANELA_LARGURA - consentirTexto->largura - TAMANHO_ESPACO/2,
											JANELA_ALTURA - (consentirTexto->altura + TAMANHO_ESPACO)/2,
											consentirTexto->largura, consentirTexto->altura};
		SDL_SetRenderDrawColor(gameRenderer,128,128,128,128);
		SDL_RenderFillRect(gameRenderer,&quadr);
		consentirTexto->renderizar(gameRenderer, JANELA_LARGURA - consentirTexto->largura - TAMANHO_ESPACO/2,
																						 JANELA_ALTURA - (consentirTexto->altura + TAMANHO_ESPACO)/2);
		desenharCirculo(gameRenderer, JANELA_LARGURA - TAMANHO_ESPACO/2, JANELA_ALTURA - TAMANHO_ESPACO, TAMANHO_ESPACO/3, true);
	}
	if(jogador[1]->esperandoConsentimento){
		SDL_Rect quadr = {TAMANHO_ESPACO/2, JANELA_ALTURA - (consentirTexto->altura + TAMANHO_ESPACO)/2,
											consentirTexto->largura, consentirTexto->altura};
		SDL_SetRenderDrawColor(gameRenderer,128,128,128,128);
		SDL_RenderFillRect(gameRenderer,&quadr);
		consentirTexto->renderizar(gameRenderer,  TAMANHO_ESPACO/2, JANELA_ALTURA - (consentirTexto->altura + TAMANHO_ESPACO)/2);
		desenharCirculo(gameRenderer, TAMANHO_ESPACO/2, JANELA_ALTURA - TAMANHO_ESPACO, TAMANHO_ESPACO/3, true);
	}
	
	//pilha
	ssAux.str(" ");
	int posX;
	int posY = tabPrincipal->posH;
	CorPadrao corCirculo;
	Textura* pilhaTex;
	int largAux,altAux;
	double escala;
	for(unsigned int i=0; i<pilha.size(); i++){
		posX = (janela->largura-TAMANHO_ESPACO)/2 - TAMANHO_ESPACO*(pilha.size()-1)/2 + i*(TAMANHO_ESPACO);
		corCirculo = corAcao[pilha[i].casa.tipo];
		SDL_SetRenderDrawColor(gameRenderer,corCirculo.r,corCirculo.g,corCirculo.b,128);
		desenharCirculo(gameRenderer,posX+TAMANHO_ESPACO/2,posY+TAMANHO_ESPACO/2,TAMANHO_ESPACO/3,true);
		
		if(pilha[i].jog->esperandoConsentimento){
			int raioC = TAMANHO_ESPACO/10;
			int corJ = (1-(int)pilha[i].jog->cor)*255;
			corCirculo = CorPadrao{corJ,corJ,corJ};
			SDL_SetRenderDrawColor(gameRenderer,corCirculo.r,corCirculo.g,corCirculo.b,255);
			desenharCirculo(gameRenderer,posX+raioC,posY+raioC+(int)pilha[i].jog->cor*(TAMANHO_ESPACO-2*raioC),raioC,true);
			//contorno
			corJ = (1-(int)pilha[i].jog->oponente->cor)*255;
			corCirculo = CorPadrao{corJ,corJ,corJ};
			SDL_SetRenderDrawColor(gameRenderer,corCirculo.r,corCirculo.g,corCirculo.b,255);
			desenharCirculo(gameRenderer,posX+raioC,posY+raioC+(int)pilha[i].jog->oponente->cor*(TAMANHO_ESPACO-2*raioC),raioC);
		}
		if(pilha[i].jog->oponente->esperandoConsentimento){
			int raioC = TAMANHO_ESPACO/10;
			int corJ = (1-(int)pilha[i].jog->oponente->cor)*255;
			corCirculo = CorPadrao{corJ,corJ,corJ};
			SDL_SetRenderDrawColor(gameRenderer,corCirculo.r,corCirculo.g,corCirculo.b,255);
			desenharCirculo(gameRenderer,posX+raioC,posY+raioC+(int)pilha[i].jog->oponente->cor*(TAMANHO_ESPACO-2*raioC),raioC,true);
			//contorno
			corJ = (1-(int)pilha[i].jog->cor)*255;
			corCirculo = CorPadrao{corJ,corJ,corJ};
			SDL_SetRenderDrawColor(gameRenderer,corCirculo.r,corCirculo.g,corCirculo.b,255);
			desenharCirculo(gameRenderer,posX+raioC,posY+raioC+(int)pilha[i].jog->oponente->cor*(TAMANHO_ESPACO-2*raioC),raioC);
		}
		
		/*if(pilha[i].cartaAlvo != NULL){ //anuCar
			pilhaTex = pilha[i].cartaAlvo->tex;
		} else if(pilha[i].peca != NULL){ //mov, atq, inv, movUni, movGen, atqExt
			pilhaTex = pilha[i].peca->tex;
		} else if(pilha[i].pecaAlvo != NULL){ //atq, def, troca, atqExt, movGen+troca, atqDir, conAtq, conPec, os 4 bon
			pilhaTex = pilha[i].pecaAlvo->tex;
		} else { //recUni, revUni
			pilhaTex = pilha[i].jog->unidade[0]->tex;
		}
		
		if(pilha[i].cartaAlvo != NULL) escala = 0.3;
		else escala = 0.5;
		largAux = (TAMANHO_ESPACO - pilhaTex->largura*escala)/2;
		altAux = (TAMANHO_ESPACO - pilhaTex->altura*escala)/2;
		if(pilha[i].cartaAlvo != NULL)
			pilhaTex->renderizar(gameRenderer, posX+largAux, posY+altAux, escala, true);
		else
			pilhaTex->renderizar(gameRenderer, posX+largAux, posY+altAux, escala);
		
		if(mouseX >= posX && mouseX < posX+TAMANHO_ESPACO && mouseY >= posY && mouseY < posY+TAMANHO_ESPACO){
			if(pilha[i].cartaAlvo != NULL)
				cartaSobCursor = pilha[i].carta;
			else if(pilha[i].peca != NULL) //mov, atq, inv, movUni, movGen, atqExt
				pecaSobCursor = pilha[i].peca;
			else if(pilha[i].pecaAlvo != NULL) //atq, def, troca, atqExt, movGen+troca, atqDir, conAtq, conPec, os 4 bon
				pecaSobCursor = pilha[i].pecaAlvo;
			else //recUni, revUni
				cartaSobCursor = pilha[i].carta;
		}*/
		if(existe(pilha[i].carta)){
			pilhaTex = pilha[i].carta->tex;
			escala = 0.3;
		} else {
			pilhaTex = pilha[i].peca->tex;
			escala = 0.5;
		}
		largAux = (TAMANHO_ESPACO - pilhaTex->largura*escala)/2;
		altAux = (TAMANHO_ESPACO - pilhaTex->altura*escala)/2;
		
		if(existe(pilha[i].carta)) pilhaTex->renderizar(gameRenderer, posX+largAux, posY+altAux, escala, true);
		else pilhaTex->renderizar(gameRenderer, posX+largAux, posY+altAux, escala);
		
		if(mouseX >= posX && mouseX < posX+TAMANHO_ESPACO && mouseY >= posY && mouseY < posY+TAMANHO_ESPACO){
			if(existe(pilha[i].carta)) cartaSobCursor = pilha[i].carta;
			else pecaSobCursor = pilha[i].peca;
			ssAux<<pilha[i].descricao;
			//cout<<ssAux.str();
		}
	}
	/*
	SDL_SetRenderDrawColor(gameRenderer,255,255,255,128);
	SDL_RenderDrawLine(gameRenderer,1,51,51,51);
	SDL_RenderDrawLine(gameRenderer,51,1,51,51);
	SDL_SetRenderDrawColor(gameRenderer,255,255,255,128);
	desenharCirculo(gameRenderer,26,26,25,true);*/
	
	//mouse sobre peça ou carta na pilha mostra peças/cartas/casas afetadas
	if(existe(pecaSobCursor)){
		for(unsigned int i=0; i<pilha.size(); i++){
			if(existe(pilha[i].peca))
				if(pilha[i].peca == pecaSobCursor){
					pilha[i].casa.renderizar();
					if(existe(pilha[i].carta))
						if(pilha[i].carta->localC == locMao){
							SDL_Rect quadr = {pilha[i].carta->posX, pilha[i].carta->posY, pilha[i].carta->largura, pilha[i].carta->altura};
							SDL_SetRenderDrawColor(gameRenderer,corAcaoSelecao.r,corAcaoSelecao.g,corAcaoSelecao.b,OPAC_SELECAO_CARTA);
							SDL_RenderFillRect(gameRenderer,&quadr);
						}
					if(existe(pilha[i].pecaAlvo) && pilha[i].pecaAlvo != pecaSobCursor){
						CasaAlvo qps = {pilha[i].peca->tab, pilha[i].peca->coluna, pilha[i].peca->fila, acaoSel};
						qps.renderizar();
					}
				}
			if(existe(pilha[i].pecaAlvo))
				if(pilha[i].pecaAlvo == pecaSobCursor){
					pilha[i].casa.renderizar();
					if(existe(pilha[i].peca)){
						CasaAlvo qps = {pilha[i].peca->tab, pilha[i].peca->coluna, pilha[i].peca->fila, acaoSel};
						qps.renderizar();
					}
					if(existe(pilha[i].carta))
						if(pilha[i].carta->localC == locMao){
							SDL_Rect quadr = {pilha[i].carta->posX, pilha[i].carta->posY, pilha[i].carta->largura, pilha[i].carta->altura};
							SDL_SetRenderDrawColor(gameRenderer,corAcaoSelecao.r,corAcaoSelecao.g,corAcaoSelecao.b,OPAC_SELECAO_CARTA);
							SDL_RenderFillRect(gameRenderer,&quadr);
						}
				}
		}
	}
	if(existe(cartaSobCursor)){
		for(unsigned int i=0; i<pilha.size(); i++){
			if(existe(pilha[i].carta))
				if(pilha[i].carta == cartaSobCursor){
					if(existe(pilha[i].peca)){
						CasaAlvo qpp = {pilha[i].peca->tab, pilha[i].peca->coluna, pilha[i].peca->fila, acaoSel};
						qpp.renderizar();
					}
					if(pilha[i].cartaAlvo == NULL)
						pilha[i].casa.renderizar();
					else {
						SDL_Rect quadr = {pilha[i].cartaAlvo->posX, pilha[i].cartaAlvo->posY, pilha[i].cartaAlvo->largura, pilha[i].cartaAlvo->altura};
						SDL_SetRenderDrawColor(gameRenderer,corAcaoSelecao.r,corAcaoSelecao.g,corAcaoSelecao.b,OPAC_SELECAO_CARTA);
						SDL_RenderFillRect(gameRenderer,&quadr);
					}
				}
			if(existe(pilha[i].cartaAlvo))
				if(pilha[i].cartaAlvo == cartaSobCursor && cartaSobCursor->localC == locMao){
					SDL_Rect quadr = {pilha[i].carta->posX, pilha[i].carta->posY, pilha[i].carta->largura, pilha[i].carta->altura};
					SDL_SetRenderDrawColor(gameRenderer,corAcaoSelecao.r,corAcaoSelecao.g,corAcaoSelecao.b,OPAC_SELECAO_CARTA);
					SDL_RenderFillRect(gameRenderer,&quadr);
				}
		}
	}
	
	//olhando descartes, em primeiro plano
	for(int j=0; j<2; j++)
		if(jogador[j] == jogadorDosDescartesExibidos)
			for(unsigned int i=0; i<jogador[j]->descartes.size(); i++)
				jogador[j]->descartes[i]->renderizar();
	
	//carta sob cursor em primeiro plano e com cursor
	if(existe(cartaSobCursor) && cartaSobCursor != cartaSelecionada)
		cartaSobCursor->renderizar();
	if(existe(cartaSobCursor)){
		SDL_Rect quadr;
		SDL_SetRenderDrawColor(gameRenderer,corAcaoSelecao.r,corAcaoSelecao.g,corAcaoSelecao.b,OPAC_CURSOR);
		for(int i=0; i<CURSOR_PADRAO_ESPESSURA; i++){
			quadr = {cartaSobCursor->posX+i,cartaSobCursor->posY+i,cartaSobCursor->largura-i*2,cartaSobCursor->altura-i*2};
			SDL_RenderDrawRect(gameRenderer,&quadr);
		}
	}
	
	//quadro de informações sobre peças e cartas, à frente de tds as cartas
	//ssAux.str(" "); //passei pra antes da renderização da pilha
	if(ssAux.str().length() <= 1){ //caso n tenha nada escrito da pilha
		if(existe(cartaSobCursor)){
			if(cartaSobCursor->revelada){
				ssAux<<cartaSobCursor->nome;
				if(cartaSobCursor->ativada) ssAux<<" (ativada)";
				if(cartaSobCursor->anulada) ssAux<<" (anulada)";
				ssAux<<"\n"<<cartaSobCursor->descricao;
			}
		} else if(existe(pecaSobCursor)){
			ssAux<<pecaSobCursor->nome;
			if(pecaSobCursor->congelada) ssAux<<" (congelada)";
			if(pecaSobCursor->ressurreicao) ssAux<<" (ressurreição)";
			ssAux<<"\nAtq: "<<pecaSobCursor->ataque<<" Def: "<<pecaSobCursor->defesa
					 <<" Mov: "<<pecaSobCursor->movimento<<" Alc: "<<pecaSobCursor->alcance;
			if(pecaSobCursor->tipo != peao) ssAux<<"\nBênção: ";
			if(pecaSobCursor->tipo == rei) ssAux<<"ressurreição";
			if(pecaSobCursor->tipo == rainha) ssAux<<"ataque + 1";
			if(pecaSobCursor->tipo == bispo) ssAux<<"alcance + 1";
			if(pecaSobCursor->tipo == cavalo) ssAux<<"movimento + 1";
			if(pecaSobCursor->tipo == torre) ssAux<<"defesa + 1";
		}
		/*else if(pecaSelecionada != NULL){
			ssAux<<"Peça: "<<pecaSelecionada->nome;
			if(pecaSelecionada->congelada) ssAux<<" (congelada)";
			if(pecaSelecionada->ressurreicao) ssAux<<" (ressurreição)";
			ssAux<<"\nAtq: "<<pecaSelecionada->ataque<<"    Def: "<<pecaSelecionada->defesa
					 <<"    Mov: "<<pecaSelecionada->movimento<<"    Alc: "<<pecaSelecionada->alcance;
		}
		else if(cartaSelecionada != NULL){
			ssAux<<"Carta: "<<cartaSelecionada->nome;
			if(cartaSelecionada->ativada) ssAux<<" (ativada)";
			if(cartaSelecionada->anulada) ssAux<<" (anulada)";
			ssAux<<"\n"<<cartaSelecionada->descricao;
		}*/
		if(existe(pecaSelecionada) && existe(cartaSobCursor)){ //sacrifícios
			if(pecaSelecionada->jog == jogadorDaVez && cartaSobCursor->jog == jogadorDaVez && pecaSelecionada->tab == tabPrincipal
			&& pecaSelecionada->tipo != peao && !jogadorDaVez->usandoAtqExt && !jogadorDaVez->usandoMovGen){
				if(pecaSelecionada->classe == classeGeneral && cartaSobCursor->localC == locDescartes){
					ssAux.str("");
					ssAux<<"Sacrifique seu general para devolver as cartas de sua pilha de descartes ao seu baralho e reembaralhar.";
				} else if(pecaSelecionada->tipo == rei && cartaSobCursor->localC == locBaralho)
					ssAux<<"Sacrifique seu rei para desistir da partida.";
			}
		}
	}
	if(ssAux.str().length() > 1){//cout<<"entrou, length="<<ssAux.str().length()<<"\n";
//		cout<<"frase: "<<ssAux.str()<<"\n";
//		cout<<"enter em: "<<ssAux.str().find("\n")+1<<"\n";
//		cout<<"substr: "<<ssAux.str().substr(ssAux.str().find("\n")+1, ssAux.str().find("\n",ssAux.str().find("\n")+1)-ssAux.str().find("\n")-1)<<"\n";
//		system("pause");
		
		//vê se a primeira linha do texto informativo é maior que a largura padrão do quadro
		infoPecaOuCartaTexto->escrever(gameRenderer, ssAux.str().substr(0,ssAux.str().find("\n")-1));
		if(infoPecaOuCartaTexto->largura > TAMANHO_ESPACO*4)
			infoPecaOuCartaTexto->escrever(gameRenderer,ssAux.str(),true,TAMANHO_ESPACO*4);
		else { //se não for, vê se a segunda é
			infoPecaOuCartaTexto->escrever(gameRenderer,
			ssAux.str().substr(ssAux.str().find("\n")+1, ssAux.str().find("\n",ssAux.str().find("\n")+1)-ssAux.str().find("\n")-1));
			if(infoPecaOuCartaTexto->largura > TAMANHO_ESPACO*4)
				infoPecaOuCartaTexto->escrever(gameRenderer,ssAux.str(),true,TAMANHO_ESPACO*4);
			else //se não for, reduz o tamanho do quadro
				infoPecaOuCartaTexto->escrever(gameRenderer,ssAux.str(),true,infoPecaOuCartaTexto->largura);
		}
		/*/escreve a segunda linha do texto informativo pra definir o tamanho do quadro
		infoPecaOuCartaTexto->escrever(gameRenderer,
			ssAux.str().substr(ssAux.str().find("\n")+1, ssAux.str().find("\n",ssAux.str().find("\n")+1)-ssAux.str().find("\n")-1));
		cout<<infoPecaOuCartaTexto->largura<<" > "<<TAMANHO_ESPACO*4<<"?\n";
		if(infoPecaOuCartaTexto->largura > TAMANHO_ESPACO*4){cout<<"s\n";
			infoPecaOuCartaTexto->escrever(gameRenderer,ssAux.str(),true,TAMANHO_ESPACO*4);
		} else {cout<<"n\n";
			infoPecaOuCartaTexto->escrever(gameRenderer,ssAux.str(),true,infoPecaOuCartaTexto->largura);
		}cout<<"\n"<<infoPecaOuCartaTexto->largura<<"\n";*/
		int quadroPosX = max(mouseX - infoPecaOuCartaTexto->largura, 0);
		int quadroPosY = max(mouseY - infoPecaOuCartaTexto->altura, 0);
//		int quadroPosX = max(cursor->posX - infoPecaOuCartaTexto->largura, 0);
//		int quadroPosY = max(cursor->posY - infoPecaOuCartaTexto->altura, 0);
		SDL_Rect quadr = {quadroPosX, quadroPosY, infoPecaOuCartaTexto->largura, infoPecaOuCartaTexto->altura};
		SDL_SetRenderDrawColor(gameRenderer,128,128,128,192);
		SDL_RenderFillRect(gameRenderer,&quadr);
		infoPecaOuCartaTexto->renderizar(gameRenderer, quadroPosX, quadroPosY);
	}
	
	//debugger à frente de tudo
	if(exibindoDebugger){
		SDL_SetRenderDrawColor(gameRenderer,128,128,128,128);
		SDL_Rect quadr = {0, 0, debugTexto->largura, debugTexto->altura};
		SDL_RenderFillRect(gameRenderer,&quadr);
		debugTexto->renderizar(gameRenderer, 0, 0);
		quadr = {JANELA_LARGURA-TAMANHO_ESPACO*3, 0, debug2Texto->largura, debug2Texto->altura};
		SDL_RenderFillRect(gameRenderer,&quadr);
		debug2Texto->renderizar(gameRenderer, JANELA_LARGURA-TAMANHO_ESPACO*3, 0);
	}
	
	SDL_RenderPresent(gameRenderer);
}

Jogo::~Jogo(){
/*	terminar();
}

void Jogo::terminar(){*/
	if(jogando){
		char partidaTimestamp[20];
		time_t rawtime = time(NULL);
		struct tm* timeinfo = localtime(&rawtime);
		strftime(partidaTimestamp,20,"%H:%M:%S %d/%m/%Y",timeinfo);
		
		ssAux.str("");
		ssAux<<"Partida cancelada ("<<partidaTimestamp<<").\n";
		ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
		cout<<ssAux.str();
	}
	ofsRegistro.close();
	ofsDebugger.close();
	
	tabPrincipal->tex->liberar();
	delete tabPrincipal->tex;
	jogador[0]->reserva->tex->liberar();
	delete jogador[0]->reserva->tex;
	jogador[1]->reserva->tex->liberar();
	delete jogador[1]->reserva->tex;
	delete tabPrincipal;
	delete jogador[0]->reserva;
	delete jogador[1]->reserva;
	
	cursor->tex->liberar();
	delete cursor->tex;
	delete cursor;
	
	//tituloTexto->~Texto();
	debugTexto->~Texto();
	debug2Texto->~Texto();
	//fpsAtualTexto->~Texto();
	rodadaTexto->~Texto();
	tempoDePartidaTexto->~Texto();
	turnoTexto->~Texto();
	acoesTexto->~Texto();
	dadoTexto->~Texto();
	mensagemTexto->~Texto();
	infoPecaOuCartaTexto->~Texto();
	consentirTexto->~Texto();
	
	for(unsigned int i=0; i<graficoDasPecas.size(); i++){
		graficoDasPecas[i]->liberar();
		//delete graficoDasPecas[i]->tex;
		delete graficoDasPecas[i];
	}
	
	for(int j=0; j<2; j++)
		for(int i=0; i<16; i++){
			//jogador[j]->todasAsPecas[i]->tex->~Textura();
			//jogador[j]->todasAsPecas[i]->texNome->~Texto();
			//delete jogador[j]->todasAsPecas[i]->tex;
			delete jogador[j]->todasAsPecas[i]->texNome;
			delete jogador[j]->todasAsPecas[i];
		}
	delete copiaDaPecaSelecionadaNoCursor;
	
	for(unsigned int i=0; i<graficoDasCartas.size(); i++){
		graficoDasCartas[i]->liberar();
		//delete graficoDasCartas[i]->tex;
		delete graficoDasCartas[i];
	}
	graficoDasCartasVerso[0]->liberar();
	graficoDasCartasVerso[1]->liberar();
	//delete graficoDasCartasVerso->tex;
	delete graficoDasCartasVerso[1];
	delete graficoDasCartasVerso[0];
	
	for(int j=0; j<2; j++){
		for(unsigned int i=0; i<jogador[j]->baralho.size(); i++){
			//delete jogador[j]->baralho[i]->tex;
			//delete jogador[j]->baralho[i]->texVerso;
			delete jogador[j]->baralho[i]->texTextoSimbolo;
			delete jogador[j]->baralho[i]->texTextoNaipe;
			delete jogador[j]->baralho[i]->texTextoNome;
			delete jogador[j]->baralho[i];
		}
		for(unsigned int i=0; i<jogador[j]->mao.size(); i++){
			//delete jogador[j]->mao[i]->tex;
			//delete jogador[j]->mao[i]->texVerso;
			delete jogador[j]->mao[i]->texTextoSimbolo;
			delete jogador[j]->mao[i]->texTextoNaipe;
			delete jogador[j]->mao[i]->texTextoNome;
			delete jogador[j]->mao[i];
		}
		for(unsigned int i=0; i<jogador[j]->descartes.size(); i++){
			//delete jogador[j]->descartes[i]->tex;
			//delete jogador[j]->descartes[i]->texVerso;
			delete jogador[j]->descartes[i]->texTextoSimbolo;
			delete jogador[j]->descartes[i]->texTextoNaipe;
			delete jogador[j]->descartes[i]->texTextoNome;
			delete jogador[j]->descartes[i];
		}
	}
	
	delete jogador[1];
	delete jogador[0];
	
//	for(int j=0; j<2; j++)
//		for(int i=0; i<16; i++)
//			jogador[j]->todasAsPecas[i]->texNome->liberar();
	
//	somBgm->~Musica();
//	somBgm = NULL;
//	delete somBgm;
//	somVitoria->~Musica();
//	somVitoria = NULL;
//	delete somVitoria;
	somBgm->~Som();
	somBgm = NULL;
	delete somBgm;
	somVitoria->~Som();
	somVitoria = NULL;
	delete somVitoria;
	somDerrota->~Som();
	somDerrota = NULL;
	delete somDerrota;
//	somMusica->~Som();
//	somMusica = NULL;
//	delete somMusica;
	somSelecionar->~Som();
	somSelecionar = NULL;
	delete somSelecionar;
	somConfirmar->~Som();
	somConfirmar = NULL;
	delete somConfirmar;
	somCancelar->~Som();
	somCancelar = NULL;
	delete somCancelar;
	
	janela->~Janela();
	janela = NULL;
	delete janela;
	
	SDL_DestroyRenderer(gameRenderer);
	gameRenderer = NULL;
	//delete gameRenderer;
	
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Jogo::debugger(){
	ssAux.str("");
	//ofsDebugger.open(debuggerArquivo.c_str(),ios::trunc);
	ofsDebugger.open(debuggerArquivo.c_str(),ios_base::trunc);
	//ofsDebugger.open(debuggerArquivo.c_str(),ofstream::trunc);
	
	ssAux<<"exibindoGrade="<<exibindoGrade
		<<"\nexibindoCasas="<<exibindoCasas
		<<"\nexibindoAreas="<<exibindoAreas
		<<"\nexibindoAreasSimplificadas="<<exibindoAreasSimplificadas
		<<"\nusandoPilha="<<usandoPilha
		<<"\nexibindoDebug="<<exibindoDebugger
		<<"\nfpsAtual="<<fpsAtual
		<<"\ntocandoMusica="<<(Mix_Playing(1) > 0 || Mix_Playing(2) > 0)
		<<"\nvolumeGeral="<<volumeGeral;
	
	ssAux<<"\n\nmouse="<<mouseX<<","<<mouseY
		<<"\nno tabuleiro=";
	if(existe(estaDentroDeQualTabuleiro(mouseX,mouseY)))
		ssAux<<estaDentroDeQualTabuleiro(mouseX,mouseY)->nome;
	ssAux<<"\ncursor="<<cursor->coluna<<","<<cursor->fila<<" ("<<cursor->posX<<","<<cursor->posY<<")"
		<<"\nno tabuleiro=";
	if(existe(cursor->tab))
		ssAux<<cursor->tab->nome;
	
	ssAux<<"\njogando="<<jogando
		<<"\ntempoDePartida="<<tempoDePartida
		<<"\nfaseInicial="<<naFaseInicial
		<<"\nturno="<<turno
		<<"\nacoesPorTurno="<<acoesPorTurno
		<<"\npilhaEmAcao="<<pilhaEmAcao
		<<"\natacandoComAs="<<atacandoComAs
		<<"\nfaseDeDano="<<emFaseDeDano
		<<"\nolhandoDescartes="<<olhandoDescartes;
	if(olhandoDescartes)
		ssAux<<"\njogadorDosDescartesExibidos="<<jogadorDosDescartesExibidos->nome;
	
	for(int j=0; j<2; j++){
		ssAux<<"\n\njogador "<<jogador[j]->cor<<"="<<jogador[j]->nome
			<<"\npromovendoPeao="<<jogador[j]->promovendoPeao
			//<<"\ntrocandoGeneraisDe\nLugarComAcao="<<jogador[j]->trocandoGeneraisDeLugarComAcao
			<<"\nexibindoCartasDasMaos="<<jogador[j]->exibindoCartasDasMaos
			<<"\nusandoMovUni="<<jogador[j]->usandoMovUni
			<<"\nusandoMovGen="<<jogador[j]->usandoMovGen
			<<"\nusandoAtqExt="<<jogador[j]->usandoAtqExt
			<<"\nconsentindoAutomaticamente="<<jogador[j]->consentindoAutomaticamente
			<<"\nesperandoConsentimento="<<jogador[j]->esperandoConsentimento
			<<"\ndado="<<jogador[j]->dado
			<<"\nultimaUnidadeQueFalhou\nNumAtaque=";
		if(existe(jogador[j]->ultimaUnidadeQueFalhouNumAtaque))
			ssAux<<jogador[j]->ultimaUnidadeQueFalhouNumAtaque->nome;
		ssAux<<"\nlapidesNoTurno=";
		for(unsigned int i=0; i<jogador[j]->lapidesNoTurno.size(); i++){
			ssAux<<jogador[j]->lapidesNoTurno[i].coluna<<","<<jogador[j]->lapidesNoTurno[i].fila;
			if(i < jogador[j]->lapidesNoTurno.size()-1) ssAux<<"; ";
		}
	}
	
	debugTexto->escrever(gameRenderer,ssAux.str(),true,TAMANHO_ESPACO*3);
	//if((totalDeQuadros*2)%FPS == 0) //pra só escrever a cada meio segundo, pra ???
		ofsDebugger<<ssAux.str()<<"\n\n";
	
	//debugger2
	ssAux.str("");
	ssAux<<"pecaSobCursor=";
	if(pecaSobCursor == NULL);
		//ssAux<<"\n\n\n\n\n\n\n";
	else
		ssAux<<pecaSobCursor->nome
		<<"\natq "<<pecaSobCursor->ataque<<", def "<<pecaSobCursor->defesa
		<<"\nmov "<<pecaSobCursor->movimento<<", alc "<<pecaSobCursor->alcance
		<<"\ncasasBencao.size()="<<pecaSobCursor->casasBencao.size()
		<<"\nbonus.size()="<<pecaSobCursor->bonus.size()
		<<"\nbonusPorCartas.size()="<<pecaSobCursor->bonusPorCartas.size()
		<<"\nselecionável="<<pecaSobCursor->selecionavel//<<"  emJogo="<<pecaSobCursor->emJogo
		<<"\nautorreviver="<<pecaSobCursor->ressurreicao
		<<"\ncongelada="<<pecaSobCursor->congelada;
	
	ssAux<<"\n\npecaSelecionada=";
	if(pecaSelecionada == NULL);
		//ssAux<<"\n\n\n\n\n\n\n";
	else
		ssAux<<pecaSelecionada->nome<<" em "<<pecaSelecionada->coluna<<","<<pecaSelecionada->fila
		<<"\natq "<<pecaSelecionada->ataque<<", def "<<pecaSelecionada->defesa
		<<"\nmov "<<pecaSelecionada->movimento<<", alc "<<pecaSelecionada->alcance
		<<"\ncasasBencao.size="<<pecaSelecionada->casasBencao.size()
		<<"\nbonus.size()="<<pecaSelecionada->bonus.size()
		<<"\nbonusPorCartas.size()="<<pecaSelecionada->bonusPorCartas.size()
		<<"\nselecionável="<<pecaSelecionada->selecionavel//<<"  emJogo="<<pecaSelecionada->emJogo
		<<"\nautorreviver="<<pecaSelecionada->ressurreicao
		<<"\ncongelada="<<pecaSelecionada->congelada;
	
	ssAux<<"\n\n-copiaDaPecaSelecionada\nNoCursor= ";
	if(copiaDaPecaSelecionadaNoCursor == NULL);
		//ssAux<<"\n\n";
	else
		ssAux<<copiaDaPecaSelecionadaNoCursor->nome
		<<"\nvisivel="<<copiaDaPecaSelecionadaNoCursor->visivel
		<<"\ncasasBencao.size="<<copiaDaPecaSelecionadaNoCursor->casasBencao.size();
	
	ssAux<<"\n\ncasaAlvoSobCursor=";
	if(casaAlvoSobCursor == NULL);
		//ssAux<<"\n";
	else {
		if(casaAlvoSobCursor->tipo==acaoMov) ssAux<<"mov";
		if(casaAlvoSobCursor->tipo==acaoAtq) ssAux<<"atq";
		if(casaAlvoSobCursor->tipo==acaoInv) ssAux<<"inv";
		if(casaAlvoSobCursor->tipo==acaoSel) ssAux<<"sel";
		if(casaAlvoSobCursor->tipo==acaoBen) ssAux<<"ben";
		if(casaAlvoSobCursor->tipo==acaoCar) ssAux<<"car";
		ssAux<<" em "<<casaAlvoSobCursor->coluna<<","<<casaAlvoSobCursor->fila
		<<"\nno tabuleiro="<<casaAlvoSobCursor->tab->nome;
	}
	ssAux<<"\ncasasAlvo.size="<<casasAlvo.size();
	
	ssAux<<"\n\ncartaSobCursor=";
	if(cartaSobCursor == NULL);
		//ssAux<<"\n\n\n\n\n";
	else {
		ssAux<<cartaSobCursor->simbolo<<cartaSobCursor->naipe<<" ("<<cartaSobCursor->nome<<") em "
		<<cartaSobCursor->posX<<","<<cartaSobCursor->posY<<" (";
		if(cartaSobCursor->localC == locBaralho) ssAux<<"baralho";
		if(cartaSobCursor->localC == locBaralho) ssAux<<"mão";
		if(cartaSobCursor->localC == locBaralho) ssAux<<"descartes";
		ssAux<<")"
		<<"\njogador: "<<cartaSobCursor->cor
		<<"\nselecionável="<<cartaSobCursor->selecionavel
		<<"\nrevelada="<<cartaSobCursor->revelada
		<<"\nativada="<<cartaSobCursor->ativada
		<<"\nanulada="<<cartaSobCursor->anulada;
	}
	
	ssAux<<"\n\ncartaSelecionada=";
	if(cartaSelecionada == NULL);
		//ssAux<<"\n\n\n\n\n";
	else
		ssAux<<cartaSelecionada->simbolo<<cartaSelecionada->naipe<<" ("<<cartaSelecionada->nome<<") em "
		<<cartaSelecionada->posX<<","<<cartaSelecionada->posY<<" (mão)"
		<<"\njogador: "<<cartaSelecionada->cor
		<<"\nselecionável="<<cartaSelecionada->selecionavel
		<<"\nrevelada="<<cartaSelecionada->revelada
		<<"\nativada="<<cartaSelecionada->ativada
		<<"\nanulada="<<cartaSelecionada->anulada;
	
	ssAux<<"\nqtde de efeitos na pilha="<<pilha.size();
	for(unsigned int i=0; i<pilha.size(); i++){
		ssAux<<"\n\nefeito "<<i<<" da pilha:";
		if(existe(pilha[i].jog)) ssAux<<"\njog="<<pilha[i].jog->cor;
		if(existe(pilha[i].carta)) ssAux<<"\ncarta="<<pilha[i].carta->nome;
		if(existe(pilha[i].peca)) ssAux<<"\npeca="<<pilha[i].peca->nome;
		if(existe(pilha[i].pecaAlvo)) ssAux<<"\npecaAlvo="<<pilha[i].pecaAlvo->nome;
		if(existe(pilha[i].cartaAlvo)) ssAux<<"\ncartaAlvo="<<pilha[i].cartaAlvo->nome;
		ssAux<<"\ncasa="<<pilha[i].casa.coluna<<","<<pilha[i].casa.fila<<" (";
		if(pilha[i].casa.tipo == acaoMov) ssAux<<"mov";
		if(pilha[i].casa.tipo == acaoAtq) ssAux<<"atq";
		if(pilha[i].casa.tipo == acaoInv) ssAux<<"inv";
		if(pilha[i].casa.tipo == acaoCar) ssAux<<"car";
		if(pilha[i].casa.tipo == acaoBen) ssAux<<"BEN";
		if(pilha[i].casa.tipo == acaoSel) ssAux<<"SEL";
		ssAux<<")";
	}
	ssAux<<"\n";
	
	debug2Texto->escrever(gameRenderer,ssAux.str(),true,TAMANHO_ESPACO*3);
	//if((totalDeQuadros*2)%FPS == 0) //pra só escrever a cada meio segundo, pra ???
		ofsDebugger<<ssAux.str();
	
	//if((totalDeQuadros/5)%FPS == 0)
	//	cout<<ofsDebugger.;
	
	ofsDebugger.close();
}

void Jogo::fimDeJogo(){
	jogando = false;
	rolandoDadoInicial = false;
	naFaseInicial = false;
	pilhaEmAcao = false;
	atacandoComAs = false;
	emFaseDeDano = false;
	sacrificouRei = false;
	for(int j=0; j<2; j++){
		jogador[j]->esperandoConsentimento = false;
		jogador[j]->consentiu = false;
		jogador[j]->consentindoAutomaticamente = false;
		jogador[j]->promovendoPeao = false;
		jogador[j]->trocandoGeneraisDeLugarComAcao = false;
		jogador[j]->exibindoCartasDasMaos = false;
		//jogador[j]->exibindoDescartes = false;
		jogador[j]->usandoMovUni = false;
		jogador[j]->usandoMovGen = false;
		jogador[j]->usandoAtqExt = false;
	}
	cancelarSelecao();
	definirPecasSelecionaveis();
	definirCartasSelecionaveis();
	definirCasasSelecionaveis();
	for(int j=0; j<2; j++)
		for(unsigned int i=0; i<jogador[j]->mao.size(); i++)
			jogador[j]->mao[i]->revelada = true;
}

void Jogo::novoJogo(){
	if(jogando){
		char partidaTimestamp[20];
		time_t rawtime = time(NULL);
		struct tm* timeinfo = localtime(&rawtime);
		strftime(partidaTimestamp,20,"%H:%M:%S %d/%m/%Y",timeinfo);
		
		ssAux.str("");
		ssAux<<"Partida cancelada ("<<partidaTimestamp<<").\n";
		ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
		cout<<ssAux.str();
	}
	fimDeJogo();
	pilha.clear();
	for(int j=0; j<2; j++){
		jogador[j]->ultimaUnidadeQueFalhouNumAtaque = NULL;
		jogador[j]->lapidesNoTurno.clear();
		for(int i=0; i<16; i++){
			jogador[j]->todasAsPecas[i]->congelada = jogador[j]->todasAsPecas[i]->ressurreicao = false;
			jogador[j]->todasAsPecas[i]->bonusSuporteDef = false;
		}
	}
	//colocarPecasNasReservas();
	for(int i=0;i<8;i++)
		for(int j=0; j<2; j++){
			moverPecaPara(jogador[j]->unidade[i],jogador[j]->reserva,(j+1)%2,i);
			moverPecaPara(jogador[j]->general[i],jogador[j]->reserva,j%2,i);
		}
	for(int j=0; j<2; j++){
		jogador[j]->reembaralhar();
		moverPecaPara(jogador[j]->general[0],tabPrincipal,jogador[j]->colunaDosGenerais,3);
		moverPecaPara(jogador[j]->unidade[0],tabPrincipal,jogador[j]->colunaSecundaria,jogador[j]->general[0]->fila);
	}
	redefinirRaiosDeBencao();
	concederBonusPorBencaos();
	moverCursorPara(mouseX,mouseY);
	if(cursor->tab == NULL && obterCartaEm(mouseX,mouseY) == NULL)
		moverCursorPara(jogador[0]->reserva->posX,jogador[0]->reserva->posY);
	
	rolandoDadoInicial = true;
	jogador[0]->esperandoConsentimento = true;
	jogador[1]->esperandoConsentimento = true;
	jogador[0]->dado = 0;
	jogador[1]->dado = 0;
	rodadaTexto->escrever(gameRenderer,"Fase de iniciativa");
	turnoTexto->escrever(gameRenderer," ");
	acoesTexto->escrever(gameRenderer," ");
	mensagemTexto->escrever(gameRenderer,"Rolem os dados! Quem obtiver o maior resultado começa o jogo.",true,tabPrincipal->largura);
	consentirTexto->escrever(gameRenderer, "Aperte Shift para rolar seu dado");
	ssAux2.str("");
	
	/*string registroNome = "generaisdocaos-registro";
	char regTimestamp[21];
	time_t rawtime = time(NULL);
	struct tm* timeinfo = localtime(&rawtime);
  strftime(regTimestamp,21,"-%Y-%m-%d-%H-%M-%S",timeinfo);
  string registroTimestamp = regTimestamp;
  string registroExtensao = ".txt";
	
  string registroArquivo = registroNome+registroTimestamp+registroExtensao;
	//cout<<registroArquivo;
	ofsRegistro.open(registroArquivo.c_str(),fstream::in);
	int i=1;
	while(ofsRegistro.is_open()){
		i++;
		registroArquivo = registroNome+registroTimestamp+"("+to_string(i)+")"+registroExtensao;
		ofsRegistro.open(registroArquivo.c_str(),fstream::in);
	}
	ofsRegistro.close();
	
	ofsRegistro.open(registroArquivo.c_str(),fstream::out);
	//ofsRegistro<<"aew";*/
	
	ofsRegistro.open(registroArquivo.c_str(),ios_base::app);
	
	char partidaTimestamp[20];
	time_t rawtime = time(NULL);
	struct tm* timeinfo = localtime(&rawtime);
  strftime(partidaTimestamp,20,"%H:%M:%S %d/%m/%Y",timeinfo);
  
	ssRegistro.str("");
	ssAux.str("");
	ssAux<<"\n*******\n";
	ssAux<<"Partida iniciada ("<<partidaTimestamp<<").\n";
	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
	cout<<ssAux.str();
	
//	ssAux.str("");
//	ssAux<<turno/2+1<<"ª rodada";
//	rodadaTexto->escrever(gameRenderer,ssAux.str());
//	jogadorTexto->escrever(gameRenderer,jogadorDaVez->nome);
//	ssAux.str("");
//	ssAux<<"Ações: "<<acoesPorTurno;
//	acoesTexto->escrever(gameRenderer,ssAux.str());
	
	ticksInicioDaPartidaAtual = SDL_GetTicks();
}

void Jogo::novoJogo2(){ //remover (só pra acelerar o início do jogo)
	novoJogo();
	for(int j=0; j<2; j++){
		jogador[j]->esperandoConsentimento = false;
		jogador[j]->consentiu = false;
		jogador[j]->dado = 0;
	}
	jogadorDaVez = jogador[0];
	consentirTexto->escrever(gameRenderer, "Aperte Shift para prosseguir");
	jogando = true;
	rolandoDadoInicial = false;
	naFaseInicial = false;
	turno = 0;
	acoesPorTurno = 2;
	//dadoTexto->escrever(gameRenderer," ");
	//rodadaTexto->escrever(gameRenderer,"Fase de iniciativa");
	mensagemTexto->escrever(gameRenderer," ");
	
	int fila;
	Peca* p;
	for(int j=0; j<2; j++)
		for(int i=0; i<3; i++){
			do{
				fila = rand()%tabPrincipal->nFilas;
				p = obterPecaEm (tabPrincipal, jogador[j]->colunaDosGenerais, fila);
			}while(existe(p));
			moverPecaPara(jogador[j]->general[i*2+2], tabPrincipal, jogador[j]->colunaDosGenerais, fila);
			
			ssAux.str("");
			ssAux<<jogador[j]->nome<<" invoca "<<jogador[j]->general[i*2+2]->nome<<" em ";
			ssAux<<jogador[j]->colunaDosGenerais<<","<<fila<<".\n";
			ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
			cout<<ssAux.str();
		}
	
	for(int j=0; j<2; j++)
		for(int i=0; i<MAXIMO_CARTAS_NA_MAO; i++)
			jogador[j]->puxarCarta();
	cancelarSelecao();
}

void Jogo::rolarDadoInicial(){
	if(jogador[0]->consentiu){
		jogador[1]->esperandoConsentimento = false;
		if(desempatando){
			desempatando = false;
			jogador[0]->dado = 0;
			jogador[1]->dado = 0;
		}
		if(jogador[0]->dado == 0){
			somConfirmar->tocar();
			jogador[0]->dado = rand()%6+1;
			ssAux.str("");
//			if(jogador[1]->dado > 0) //pra mostrar acima o resultado do outro
//				ssAux<<jogador[1]->nome<<" rola um "<<jogador[1]->dado<<" no dado.\n";
			ssAux<<jogador[0]->nome<<" rola um "<<jogador[0]->dado<<" no dado.\n";
			ssAux2<<ssAux.str();
//			mensagemTexto->escrever(gameRenderer,ssAux2.str(),true,tabPrincipal->largura); //na msg, aparece os 2 resultados...
//			ssAux.str("");
//			ssAux<<jogador[0]->nome<<" rola um "<<jogador[0]->dado<<" no dado.\n"; //... mas no terminal só aparece 1x cada
			ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
			cout<<ssAux.str();
			mensagemTexto->escrever(gameRenderer,ssAux2.str(),true,tabPrincipal->largura);
//			ssAuxDado.str("");
//			ssAuxDado<<"Dado: "<<jogador[0]->dado;
//			dadoTexto->escrever(gameRenderer,ssAuxDado.str());
		}
	}
	if(jogador[1]->consentiu){
		jogador[0]->esperandoConsentimento = false;
		if(desempatando){
			desempatando = false;
			jogador[0]->dado = 0;
			jogador[1]->dado = 0;
		}
		if(jogador[1]->dado == 0){
			somConfirmar->tocar();
			jogador[1]->dado = rand()%6+1;
			ssAux.str("");
//			if(jogador[0]->dado > 0) //pra mostrar acima o resultado do outro
//				ssAux<<jogador[0]->nome<<" rola um "<<jogador[0]->dado<<" no dado.\n";
			ssAux<<jogador[1]->nome<<" rola um "<<jogador[1]->dado<<" no dado.\n";
			ssAux2<<ssAux.str();
//			mensagemTexto->escrever(gameRenderer,ssAux.str(),true,tabPrincipal->largura); //na msg, aparece os 2 resultados...
//			ssAux.str("");
//			ssAux<<jogador[1]->nome<<" rola um "<<jogador[1]->dado<<" no dado.\n"; //... mas no terminal só aparece 1x cada
			ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
			cout<<ssAux.str();
			mensagemTexto->escrever(gameRenderer,ssAux2.str(),true,tabPrincipal->largura);
//			ssAuxDado.str("");
//			ssAuxDado<<"Dado: "<<jogador[1]->dado;
//			dadoTexto->escrever(gameRenderer,ssAuxDado.str());
		}
	}
	if(jogador[0]->consentiu && jogador[1]->consentiu && jogadorDaVez == NULL){
		ssAux.str("");
		if(jogador[0]->dado == jogador[1]->dado){
			ssAux<<"Empate! Rolem os dados novamente.\n";
			desempatando = true;
//			jogador[0]->dado = 0;
//			jogador[1]->dado = 0;
		} else {
			consentirTexto->escrever(gameRenderer, "Aperte Shift para prosseguir");
			if(jogador[0]->dado > jogador[1]->dado){
				//ssAux<<jogador[1]->nome<<" começa!\n";
//				Jogador* j = jogador[0];
//				jogador[0] = jogador[1];
//				jogador[1] = j;
				jogadorDaVez = jogador[0];
			} else// if(jogador[0]->dado > jogador[1]->dado)
				//ssAux<<jogador[0]->nome<<" começa!\n";
				jogadorDaVez = jogador[1];
			ssAux<<jogadorDaVez->nome<<" começa!\n";
		}
		ssAux2<<ssAux.str();
		ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
		cout<<ssAux.str();
		mensagemTexto->escrever(gameRenderer,ssAux2.str(),true,tabPrincipal->largura);
		ssAux2.str("");
		
		jogador[0]->esperandoConsentimento = true;
		jogador[1]->esperandoConsentimento = true;
		jogador[0]->consentiu = false;
		jogador[1]->consentiu = false;
	}
	if(jogador[0]->consentiu && jogador[1]->consentiu && existe(jogadorDaVez)){
		jogador[0]->esperandoConsentimento = false;
		jogador[1]->esperandoConsentimento = false;
		jogador[0]->consentiu = false;
		jogador[1]->consentiu = false;
		jogador[0]->dado = 0;
		jogador[1]->dado = 0;
		rolandoDadoInicial = false;
		jogando = true;
		naFaseInicial = true;
		turno = 0;
		acoesPorTurno = 1;
//		dadoTexto->escrever(gameRenderer," ");
		mensagemTexto->escrever(gameRenderer," ");
	}
}

void Jogo::atualizarFps(){
	ticksInicial = ticksAtual;
	ticksAtual = SDL_GetTicks();
	
	//limitador de FPS
	if(ticksAtual-ticksInicial<TICKS_POR_FRAME)
		SDL_Delay(TICKS_POR_FRAME-(ticksAtual-ticksInicial));
	ticksAtual = SDL_GetTicks();
	
	for(int i=0; i<AMOSTRAS_FPS-1; i++)
		fpsAcumulado[i] = fpsAcumulado[i+1];
	fpsAcumulado[AMOSTRAS_FPS-1] = 1000/(float)(ticksAtual-ticksInicial);
	
	fpsAtual = 0;
	for(int i=0; i<AMOSTRAS_FPS; i++)
		fpsAtual += fpsAcumulado[i];
	
	fpsAtual /= AMOSTRAS_FPS;
	
//	ssTempo.str("");
//	ssTempo<<fixed<<setprecision(1)<<fpsAtual<<" FPS";
//	if(totalDeQuadros%FPS == 0) //pra só escrever a cada segundo, pra ser menos incômodo na tela
//		fpsAtualTexto->escrever(gameRenderer,ssTempo.str());
	
	totalDeQuadros++;
}

void Jogo::atualizarTempoDePartida(){
	if(rolandoDadoInicial || jogando)
		tempoDePartida = SDL_GetTicks() - ticksInicioDaPartidaAtual;
	segundos = (tempoDePartida/1000)%60;
	minutos = (tempoDePartida/1000/60)%60;
	horas = tempoDePartida/1000/60/60;
	
	ssTempo.str("");
	ssTempo<<"Tempo de jogo: ";
	if(horas > 0)
		ssTempo<<horas<<":";
	ssTempo<<(minutos < 10 ? "0" : "")<<minutos<<":"<<(segundos < 10 ? "0" : "")<<segundos<<"."<<(tempoDePartida/100)%10;
	tempoDePartidaTexto->escrever(gameRenderer,ssTempo.str());
}

void Jogo::definirPecasSelecionaveis(){
	if(!jogando || olhandoDescartes){
		for(int j=0; j<2; j++)
			for(int i=0;i<16;i++){
				jogador[j]->todasAsPecas[i]->selecionavel = false;
//				jogador[j]->todasAsPecas[i]->congelada = jogador[j]->todasAsPecas[i]->autorreviver = false;
//				jogador[j]->todasAsPecas[i]->bonusSuporteDef = false;
			}
	} //jogando
	else if(naFaseInicial){
		for(int j=0; j<2; j++)
			for(int i=0; i<8; i++)
				if(jogador[j] == jogadorDaVez && jogador[j]->general[i]->tab == jogador[j]->reserva
				&& jogador[j]->general[i-1+((i-1)%2)*2]->tab == jogador[j]->reserva) //o outro general do msm tipo
					jogador[j]->general[i]->selecionavel = true;
				else
					jogador[j]->general[i]->selecionavel = false;
	} //após a fase inicial, promovendo unidade
	else if(jogador[0]->promovendoPeao || jogador[1]->promovendoPeao){
		for(int j=0; j<2; j++)
			for(int i=0; i<8; i++){
				jogador[j]->unidade[i]->selecionavel = false;
				if(jogador[j]->promovendoPeao && jogador[j]->general[i]->tab == jogador[j]->reserva)
					jogador[j]->general[i]->selecionavel = true;
				else
					jogador[j]->general[i]->selecionavel = false;
			}
		for(int j=0; j<2; j++){
			bool algumGeneralNaReserva=false;
			for(int i=0; i<8 && !algumGeneralNaReserva; i++)
				if(jogador[j]->general[i]->selecionavel)
					algumGeneralNaReserva = true;
			if(!algumGeneralNaReserva) //tds os generais em campo, e aí?
				jogador[j]->promovendoPeao = false; //pobre peão, apenas volta pra reserva :')
		}
	} //turno normal
	else if(/*pilha.size()>0 || posAcao || */pilhaEmAcao)
		for(int j=0; j<2; j++)
			for(int i=0; i<16; i++)
				jogador[j]->todasAsPecas[i]->selecionavel = false;
	else {
		for(int j=0; j<2; j++)
			for(int i=0; i<8; i++){
				jogador[j]->unidade[i]->selecionavel = !jogador[j]->unidade[i]->congelada && (jogador[j] == jogadorDaVez);
				jogador[j]->general[i]->selecionavel = !jogador[j]->general[i]->congelada && (jogador[j] == jogadorDaVez)
																							 && jogador[j]->general[i]->tab == tabPrincipal;
			}
	}
}

void Jogo::definirCartasSelecionaveis(){
	for(int j=0; j<2; j++)
		for(unsigned int i=0; i<jogador[j]->mao.size(); i++){
			jogador[j]->mao[i]->selecionavel = false;
			if(!jogando || naFaseInicial || jogador[j]->esperandoConsentimento || jogador[j]->oponente->promovendoPeao || jogador[j]->promovendoPeao
			|| !jogador[j]->mao[i]->revelada || jogador[j]->mao[i]->ativada || jogador[j]->mao[i]->anulada || olhandoDescartes)
				continue;
			else if(jogador[j]->mao[i]->tipo == anuCar){
				for(int jj=0; jj<2; jj++)
					for(unsigned int ii=0; ii<jogador[jj]->mao.size(); ii++)
						if(jogador[jj]->mao[ii]->ativada)
							jogador[j]->mao[i]->selecionavel = true;
			} else if(emFaseDeDano){
				if(jogador[j]->mao[i]->tipo == bonAtq)
					jogador[j]->mao[i]->selecionavel = !atacandoComAs;
				else if(jogador[j]->mao[i]->tipo == bonDef)
					jogador[j]->mao[i]->selecionavel = true;
			} else if(jogador[j]->mao[i]->tipo == bonAtq || jogador[j]->mao[i]->tipo == bonDef){
					jogador[j]->mao[i]->selecionavel = false;
			} else if(jogador[j]->mao[i]->tipo == conAtq){
				jogador[j]->mao[i]->selecionavel = existe(jogador[j]->oponente->ultimaUnidadeQueFalhouNumAtaque);
			} else if(jogador[j]->mao[i]->tipo == recUni){
				for(int ii=0; ii<8; ii++)
					if(jogador[j]->unidade[ii]->tab == jogador[j]->reserva){
						jogador[j]->mao[i]->selecionavel = true;
						break;
					}
			} else if(jogador[j]->mao[i]->tipo == atqDir || jogador[j]->mao[i]->tipo == bonMov || jogador[j]->mao[i]->tipo == movUni){
				for(int ii=0; ii<8; ii++)
					if(jogador[j]->unidade[ii]->tab == tabPrincipal || jogador[j]->oponente->unidade[ii]->tab == tabPrincipal){
						jogador[j]->mao[i]->selecionavel = true;
						break;
					}
			} else if(jogador[j]->mao[i]->tipo == movGen){
				for(int ii=1; ii<8; ii++) //exceto o rei
					if(jogador[j]->general[ii]->tab == tabPrincipal){
						jogador[j]->mao[i]->selecionavel = true;
						break;
					}
			} else if(jogador[j]->mao[i]->tipo == revUni){
				if(jogador[j]->lapidesNoTurno.size()>0)
					for(int ii=0; ii<8; ii++)
						if(jogador[j]->unidade[ii]->tab == jogador[j]->reserva){
							jogador[j]->mao[i]->selecionavel = true;
							break;
						}
			} else if(jogador[j]->mao[i]->tipo == conPec){
				for(int ii=1; ii<16; ii++) //qualker peça, exceto os reis
					if(jogador[j]->todasAsPecas[ii]->tab == tabPrincipal || jogador[(j+1)%2]->todasAsPecas[ii]->tab == tabPrincipal){
						jogador[j]->mao[i]->selecionavel = true;
						break;
					}
			} else //bonAlc e atqExt
				jogador[j]->mao[i]->selecionavel = true;
		}
}

void Jogo::definirCasasSelecionaveis(){
	casasAlvo.clear();
	casasAlcance.clear();
	casaAlvoSobCursor = NULL;
	if(existe(pecaSelecionada)){
		Peca* p = pecaSelecionada;
		CasaAlvo destino;
		int proxCasa;
		p->cor == branco ? proxCasa = +1 : proxCasa = -1;
		if(p->tipo == peao){
			if(p->tab == tabPrincipal){
				if(!jogador[0]->usandoMovUni && !jogador[1]->usandoMovUni){
				//movimento
					if(!p->jog->usandoAtqExt)
						for(int i=1; i <= p->movimento; i++){
							destino = {tabPrincipal,p->coluna+proxCasa*i,p->fila,acaoMov};
							if(obterPecaEm(&destino) == NULL && estaDentroDoTabuleiro(tabPrincipal,&destino))
								casasAlvo.push_back(destino);
							else break;
						}
					//alcance
					definirCasasAlcanceEAlvos(p);
				} //usandoMovUni
				else {
					int ladoDaPeca[4]={+1,0,-1,0};
					for(int i=0; i<4; i++){
						destino = {tabPrincipal,p->coluna+ladoDaPeca[i%4],p->fila+ladoDaPeca[(i+1)%4],acaoMov};
						if(obterPecaEm(&destino) == NULL && estaDentroDoTabuleiro(tabPrincipal,&destino))
							casasAlvo.push_back(destino);
					}
				}
			} //invocação de peão
			else {
				int primCol = p->jog->colunaDosGenerais;
				for(int i=0; i<8; i++){
					destino = {tabPrincipal,primCol,i,acaoInv};
					if(obterPecaEm(&destino) == NULL)
						casasAlvo.push_back(destino);
					else {
						CasaAlvo destinoFrente;
						destinoFrente = {tabPrincipal,primCol+proxCasa,i,acaoInv};
						if(obterPecaEm(&destino)->tipo != peao && obterPecaEm(&destinoFrente) == NULL)
							casasAlvo.push_back(destinoFrente);
					}
				}
			}
		} //generais
		else
			if(p->tab == tabPrincipal){
				if(p->classe == classeGeneral && !p->jog->usandoAtqExt){ //movimento
					for(int i=0; i<8; i++){
						destino = {tabPrincipal,p->coluna,i,acaoMov};
						if(obterPecaEm(&destino) == NULL)
							casasAlvo.push_back(destino);
						//troca de lugar dos generais:
						else if(obterPecaEm(&destino)->classe == classeGeneral && p->fila != i)
							if(p->jog == jogadorDaVez){
								if(acoesPorTurno >= (2 - p->jog->usandoMovGen))
									casasAlvo.push_back(destino);
							} //else //...2 movGen
					}
				} //ataque
				if(!p->jog->usandoMovGen)
					definirCasasAlcanceEAlvos(p);
			} else { //invocando generais
				for(int i=0; i<8; i++){
					destino = {tabPrincipal,p->jog->colunaDosGenerais,i,acaoInv};
					if(obterPecaEm(&destino) == NULL)
						casasAlvo.push_back(destino);
					else if(obterPecaEm(&destino)->tipo == peao) //em cima de peões
						casasAlvo.push_back(destino);
				}
			}
	} //não existe peça selecionada
	/*else if(sacrificandoGeneral){
		for(int i=1;i<8;i++)
			if(jogadorDaVez->general[i]->tab == tabPrincipal && !jogadorDaVez->general[i]->congelada)
				casasAlvo.push_back(CasaAlvo{tabPrincipal, jogadorDaVez->general[i]->coluna,
																		 jogadorDaVez->general[i]->fila, acaoAtq});
	} else if(sacrificandoRei)
		casasAlvo.push_back(CasaAlvo{tabPrincipal, jogadorDaVez->general[0]->coluna,
																 jogadorDaVez->general[0]->fila, acaoAtq});*/
	else if(cartaSelecionada == NULL){
		for(int j=0; j<2; j++)
			for(int i=0;i<16;i++)
				if(jogador[j]->todasAsPecas[i]->selecionavel){
					Peca* p = jogador[j]->todasAsPecas[i];
					if(p->tab == tabPrincipal)
						casasAlvo.push_back(CasaAlvo{tabPrincipal,p->coluna,p->fila,acaoSel});
					else
						casasAlvo.push_back(CasaAlvo{jogador[j]->reserva,p->coluna,p->fila,acaoInv});
				}
	} //existe carta selecionada
	else {
		switch(cartaSelecionada->tipo){
		case atqDir: case bonMov:
			//qualker unidade em jogo
			for(int j=0; j<2; j++)
				for(int i=0; i<8; i++)
					if(jogador[j]->unidade[i]->tab == tabPrincipal)
						casasAlvo.push_back(CasaAlvo{tabPrincipal,jogador[j]->unidade[i]->coluna,jogador[j]->unidade[i]->fila,acaoCar});
			break;
		case conAtq: //só numa unidade que realizou um ataque mal sucedido
			for(int j=0; j<2; j++)
				if(existe(jogador[j]->ultimaUnidadeQueFalhouNumAtaque))
						casasAlvo.push_back(CasaAlvo{tabPrincipal, jogador[j]->ultimaUnidadeQueFalhouNumAtaque->coluna,
																				 jogador[j]->ultimaUnidadeQueFalhouNumAtaque->fila, acaoCar});
			break;
		case bonAtq:
			//peça atacante em fase de dano
//			if(emFaseDeDano)
//				if(pilha.back().peca != NULL)
//					casasAlvo.push_back(CasaAlvo{tabPrincipal,pilha.back().peca->coluna,pilha.back().peca->fila,acaoCar});
			if(existe(pecaAtacando))
				casasAlvo.push_back(CasaAlvo{tabPrincipal,pecaAtacando->coluna,pecaAtacando->fila,acaoCar});
			break;
		case bonDef:
			//peça alvo em fase de dano
//			if(emFaseDeDano)
//				casasAlvo.push_back(CasaAlvo{tabPrincipal,pilha.back().pecaAlvo->coluna,pilha.back().pecaAlvo->fila,acaoCar});
			if(existe(pecaDefendendo))
				casasAlvo.push_back(CasaAlvo{tabPrincipal,pecaDefendendo->coluna,pecaDefendendo->fila,acaoCar});
			break;
		case bonAlc:
			//qualker peça em jogo não congelada
			for(int j=0; j<2; j++)
				for(int i=0; i<16; i++)
					if(jogador[j]->todasAsPecas[i]->tab == tabPrincipal && !jogador[j]->todasAsPecas[i]->congelada)
						casasAlvo.push_back(CasaAlvo{tabPrincipal,jogador[j]->todasAsPecas[i]->coluna,jogador[j]->todasAsPecas[i]->fila,acaoCar});
			break;
		case recUni:
			//procura um peão na reserva
			bool algumaUnidadeNaReserva;
			algumaUnidadeNaReserva = false; //(se inicializar na declaração, o compilador reclama)
			for(int i=0; i<8 && !algumaUnidadeNaReserva; i++)
				if(cartaSelecionada->jog->unidade[i]->tab == cartaSelecionada->jog->reserva){
					algumaUnidadeNaReserva = true;
				}
			//só define casas se encontrar
			if(algumaUnidadeNaReserva){
				int primCol;
				primCol = cartaSelecionada->jog->colunaDosGenerais;
				int proxCasa;
				cartaSelecionada->jog->cor == branco ? proxCasa = +1 : proxCasa = -1;
				CasaAlvo destino;
				for(int i=0; i<tabPrincipal->nFilas; i++){
					destino = {tabPrincipal,primCol,i,acaoCar};
					if(obterPecaEm(&destino) == NULL)
						casasAlvo.push_back(destino);
					else {
						CasaAlvo destinoFrente = {tabPrincipal,primCol+proxCasa,i,acaoCar};
						if(obterPecaEm(&destino)->tipo != peao && obterPecaEm(&destinoFrente) == NULL)
							casasAlvo.push_back(destinoFrente);
					}
				}
			}
			break;
		case movUni: 
			//qualker unidade em jogo não congelada
			for(int j=0; j<2; j++)
				for(int i=0; i<8; i++)
					if(jogador[j]->unidade[i]->tab == tabPrincipal && !jogador[j]->unidade[i]->congelada)
						casasAlvo.push_back(CasaAlvo{tabPrincipal,jogador[j]->unidade[i]->coluna,jogador[j]->unidade[i]->fila,acaoCar});
			break;
		case movGen:
			//só os próprios generais, exceto o rei
			for(int i=1; i<8; i++)
				if(cartaSelecionada->jog->general[i]->tab == tabPrincipal)
					casasAlvo.push_back(CasaAlvo{cartaSelecionada->jog->campo,
																			 cartaSelecionada->jog->general[i]->coluna,
																			 cartaSelecionada->jog->general[i]->fila,
																			 acaoCar});
			break;
		case revUni: //só numa unidade sua que voltou pra reserva neste turno
			for(unsigned int i=0; i<cartaSelecionada->jog->lapidesNoTurno.size(); i++)
				casasAlvo.push_back(cartaSelecionada->jog->lapidesNoTurno[i]);
			break;
		case anuCar: //só em cartas ativadas, antes de serem descartadas
			//anuCar não gera casasAlvo
			break;
		case conPec:
			//qualker peça em jogo não congelada, exceto o rei
			for(int j=0; j<2; j++)
				for(int i=0; i<16; i++)
					if(jogador[j]->todasAsPecas[i]->tab == tabPrincipal && !jogador[j]->todasAsPecas[i]->congelada && jogador[j]->todasAsPecas[i]->tipo != rei)
						casasAlvo.push_back(CasaAlvo{tabPrincipal,jogador[j]->todasAsPecas[i]->coluna,jogador[j]->todasAsPecas[i]->fila,acaoCar});
			break;
		case atqExt:
			//só as próprias peças
			for(int i=0; i<16; i++)
				if(cartaSelecionada->jog->todasAsPecas[i]->tab == tabPrincipal)
					casasAlvo.push_back(CasaAlvo{cartaSelecionada->jog->campo,
																			 cartaSelecionada->jog->todasAsPecas[i]->coluna,
																			 cartaSelecionada->jog->todasAsPecas[i]->fila,
																			 acaoCar});
			break;
		}
	}
}

void Jogo::definirCasasAlcanceEAlvos(Peca* p){
	if(p->alcance > 0){
		CasaAlvo alvo;
		int proxCasa;
		p->cor == branco ? proxCasa = +1 : proxCasa = -1;
		
		//alcance 1
		alvo = {tabPrincipal,p->coluna+proxCasa,p->fila,acaoAtq};
		if(existe(obterPecaEm(&alvo))){
			if(obterPecaEm(&alvo)->cor != p->cor)
				casasAlvo.push_back(alvo);
			else
				casasAlcance.push_back(alvo);
		} else if(obterCasaAlvoEm(&alvo) == NULL){
			if(estaDentroDoTabuleiro(tabPrincipal,&alvo))
				casasAlcance.push_back(alvo);
		} else if(obterCasaAlvoEm(&alvo)->tipo != acaoMov) //pra n pintar a casa mov à frente do peão
				casasAlcance.push_back(alvo);
	
		//alcance>1, casas ao redor
		int sinalPosNeg[4]={+1,-1,-1,+1};
		for(int a=1; a < p->alcance; a++)
			for(int x=0,y; x < a; x++){
				y = a-x;
				int xOuY[2]={x,y};
				for(int i=0; i<4; i++){
					alvo = {tabPrincipal,p->coluna+proxCasa+sinalPosNeg[i%4]*xOuY[!(i%2)],p->fila+sinalPosNeg[(i+1)%4]*xOuY[i%2],acaoAtq};
					if(obterPecaEm(&alvo) != pecaSelecionada && estaDentroDoTabuleiro(tabPrincipal,&alvo))
						casasAlcance.push_back(alvo);
					if(existe(obterPecaEm(&alvo)) && estaDentroDoTabuleiro(tabPrincipal,&alvo))
						if(obterPecaEm(&alvo)->cor != p->cor)
							casasAlvo.push_back(alvo);
				}
			}
	}
}

void Jogo::redefinirRaiosDeBencao(){
	copiaDaPecaSelecionadaNoCursor->casasBencao.clear();
	for(int j=0; j<2; j++)
		for(int i=0;i<8;i++){
			jogador[j]->unidade[i]->casasBencao.clear();
			jogador[j]->general[i]->casasBencao.clear();
			
			if(jogador[j]->general[i]->tab == tabPrincipal && !jogador[j]->general[i]->congelada)
				atribuirRaiosDeBencao(jogador[j]->general[i]);
		}
	
	if(existe(pecaSelecionada) && existe(casaAlvoSobCursor))
		if(pecaSelecionada->classe == classeGeneral)
			atribuirRaiosDeBencao(copiaDaPecaSelecionadaNoCursor);
}

void Jogo::atribuirRaiosDeBencao(Peca* gen){
	int dirHori = 1;
	int dirVert = 1;
	if(gen->cor == preto)
		dirHori = -1;
	
	if(gen->tipo != rei){
		//bênção abaixo
		int proxFila = 0;
		if(gen->fila < 7)
			for(int i=1;i<7;i++){
				gen->fila+i<=7 ? proxFila++ : proxFila--;
				gen->casasBencao.push_back(CasaAlvo{tabPrincipal,gen->coluna+i*dirHori,gen->fila+proxFila,acaoBen});
			}
		//bênção acima
		proxFila=0;
		dirVert=-1;
		if(gen->fila > 0)
			for(int i=1;i<7;i++){
				gen->fila+i*dirVert>=0 ? proxFila-- : proxFila++;
				gen->casasBencao.push_back(CasaAlvo{tabPrincipal,gen->coluna+i*dirHori,gen->fila+proxFila,acaoBen});
			}
	}
	//bênção à frente
	bool repetida;
	int qtdeCasasBencao = gen->casasBencao.size();
	for(int i=1;i<7;i++){
		repetida = false;
		for(int j=0; j<qtdeCasasBencao; j++)
			if(gen->casasBencao[j].coluna == gen->coluna+i*dirHori && gen->casasBencao[j].fila == gen->fila){
				repetida = true;
				break;
			}
		if(!repetida)
			gen->casasBencao.push_back(CasaAlvo{tabPrincipal,gen->coluna+i*dirHori,gen->fila,acaoBen});
	}
}

void Jogo::concederBonusPorBencaos(){
	for(int j=0; j<2; j++)
		for(int i=0; i<16; i++)
			jogador[j]->todasAsPecas[i]->redefinirAtributos();
	if(existe(pecaSelecionada))
		pecaSelecionada->redefinirAtributos();
	for(int j=0; j<2; j++)
		for(int i=0;i<8;i++){
			jogador[j]->unidade[i]->casasBencao.clear();
			jogador[j]->unidade[i]->ressurreicao = false;
	}
	
	Peca* p;
	for(int j=0; j<2; j++)
		for(int i=0; i<8; i++)
			for(unsigned int iCasa=0; iCasa<jogador[j]->general[i]->casasBencao.size(); iCasa++){
				p = obterPecaEm(&jogador[j]->general[i]->casasBencao[iCasa]);
				if(existe(p))
					if(p->cor == jogador[j]->general[i]->cor && !jogador[j]->general[i]->congelada && (!p->congelada || i == 0)) //i0 eh rei, q tbm dá auto-ress a peão congelado
						jogador[j]->general[i]->abencoarPeao(p);
				p = copiaDaPecaSelecionadaNoCursor;
				if(p->cor == jogador[j]->general[i]->cor && pecaSobCursor == NULL)
					if(estaoNoMesmoLugar(p, &jogador[j]->general[i]->casasBencao[iCasa]) && !jogador[j]->general[i]->congelada)
						jogador[j]->general[i]->abencoarPeao(p);
			}
}

void Jogo::concederBonusPorSuporte(){
	Peca* p;
	for(int j=0; j<2; j++)
		for(int i=0; i<8; i++){
			//bônus de defesa por peão ao lado
			jogador[j]->unidade[i]->bonusSuporteDef = false;
			if(jogador[j]->unidade[i]->tab == tabPrincipal){
				p = obterPecaEm(tabPrincipal, jogador[j]->unidade[i]->coluna, jogador[j]->unidade[i]->fila-1);
				if(existe(p))
					if(p->tipo == peao && jogador[j]->unidade[i]->jog == p->jog && !jogador[j]->unidade[i]->congelada && !p->congelada){
						if(!jogador[j]->unidade[i]->bonusSuporteDef){
							jogador[j]->unidade[i]->bonus.push_back(bDef);
							jogador[j]->unidade[i]->casasBencao.push_back(CasaAlvo{p->tab, p->coluna, p->fila, acaoBen});
						}
						if(!p->bonusSuporteDef){
							p->bonus.push_back(bDef);
							p->casasBencao.push_back(CasaAlvo{jogador[j]->unidade[i]->tab, jogador[j]->unidade[i]->coluna,
																																		 jogador[j]->unidade[i]->fila, acaoBen});
						}
						jogador[j]->unidade[i]->bonusSuporteDef = p->bonusSuporteDef = true;
					}
				//bônus de ataque por peão atrás
				p = obterPecaEm(tabPrincipal, jogador[j]->unidade[i]->coluna+(j == 0 ? -1 : +1), jogador[j]->unidade[i]->fila);
				if(existe(p))
					if(p->tipo == peao && jogador[j]->unidade[i]->jog == p->jog && !p->congelada){
						jogador[j]->unidade[i]->bonus.push_back(bAtq);
						jogador[j]->unidade[i]->casasBencao.push_back(CasaAlvo{p->tab, p->coluna, p->fila, acaoBen});
					}
			}
		}
	auto cp = copiaDaPecaSelecionadaNoCursor;
	if(cp->visivel && cp->tipo == peao){
		p = obterPecaEm(tabPrincipal, cp->coluna, cp->fila-1);
		if(existe(p))
			if(p->tipo == peao && p->jog == cp->jog && p != pecaSelecionada)
				cp->casasBencao.push_back(CasaAlvo{p->tab, p->coluna, p->fila, acaoBen});
		
		p = obterPecaEm(tabPrincipal, cp->coluna, cp->fila+1);
		if(existe(p))
			if(p->tipo == peao && p->jog == cp->jog && p != pecaSelecionada)
				cp->casasBencao.push_back(CasaAlvo{p->tab, p->coluna, p->fila, acaoBen});
		
		p = obterPecaEm(tabPrincipal, cp->coluna+((int)cp->jog->cor == 0 ? -1 : +1), cp->fila); //checa atrás
		if(existe(p))
			if(p->tipo == peao && p->jog == cp->jog && p != pecaSelecionada)
				cp->casasBencao.push_back(CasaAlvo{p->tab, p->coluna, p->fila, acaoBen});
	}
}

void Jogo::aplicarBonus(){
	for(int j=0; j<2; j++)
		for(int p=0; p<16; p++){
			for(unsigned int i=0; i < jogador[j]->todasAsPecas[p]->bonus.size(); i++){
				if(jogador[j]->todasAsPecas[p]->bonus[i] == bAtq) jogador[j]->todasAsPecas[p]->ataque++;
				if(jogador[j]->todasAsPecas[p]->bonus[i] == bDef) jogador[j]->todasAsPecas[p]->defesa++;
				if(jogador[j]->todasAsPecas[p]->bonus[i] == bMov) jogador[j]->todasAsPecas[p]->movimento++;
				if(jogador[j]->todasAsPecas[p]->bonus[i] == bAlc) jogador[j]->todasAsPecas[p]->alcance++;
			}
			for(unsigned int i=0; i < jogador[j]->todasAsPecas[p]->bonusPorCartas.size(); i++){
				if(jogador[j]->todasAsPecas[p]->bonusPorCartas[i] == bAtq) jogador[j]->todasAsPecas[p]->ataque++;
				if(jogador[j]->todasAsPecas[p]->bonusPorCartas[i] == bDef) jogador[j]->todasAsPecas[p]->defesa++;
				if(jogador[j]->todasAsPecas[p]->bonusPorCartas[i] == bMov) jogador[j]->todasAsPecas[p]->movimento++;
				if(jogador[j]->todasAsPecas[p]->bonusPorCartas[i] == bAlc) jogador[j]->todasAsPecas[p]->alcance++;
			}
		}
}

void Jogo::moverCursorPara(int x, int y, direcao setaDoTeclado){
	Tabuleiro* t = estaDentroDeQualTabuleiro(x,y);
	//if(t != NULL && !olhandoDescartes){
	if(existe(t) && !olhandoDescartes){
		cursor->tab = t;
		cursor->coluna = (x - t->posX)/TAMANHO_ESPACO;
		cursor->fila = (y - t->posY)/TAMANHO_ESPACO;
		cursor->visivel = true;
		cursor->posX = t->posX + cursor->coluna*TAMANHO_ESPACO;
		cursor->posY = t->posY + cursor->fila*TAMANHO_ESPACO;
		cartaSobCursor = NULL;
	} else {
		cursor->tab = NULL;
		cursor->coluna = -1;
		cursor->fila = -1;
		cursor->visivel = false;
		//posX e posY do cursor permanecem, pra caso a psoa decida mover o cursor pelo teclado
		
		Carta* cd = obterCartaEm(x,y);
		//if(cd == NULL)
		if(!existe(cd))
			cartaSobCursor = NULL;
		else {
//			cursor->posX = cd->posX;
//			cursor->posY = cd->posY;
		//cout<<"achou "<<cd->nome<<"\n";
		if(!olhandoDescartes){
			if(setaDoTeclado != nenhuma){
	//			if(olhandoDescartes && (cd->localC != locDescartes || cd->jog != cartaSobCursor->jog))
	//				return;
	//			if(!olhandoDescartes || cd->localC == locDescartes && cd->jog == jogadorDosDescartesExibidos){
				if(cd->localC == locMao){
					if(cartaSobCursor == NULL){
						if(setaDoTeclado == cima || setaDoTeclado == direita)
							cartaSobCursor = cd->jog->mao.front();
						else if(setaDoTeclado == baixo || setaDoTeclado == esquerda)
							cartaSobCursor = cd->jog->mao.back();
					} else if(cartaSobCursor->localC != locMao || (cartaSobCursor->localC == locMao && cartaSobCursor->jog != cd->jog)){
						if(setaDoTeclado == cima || setaDoTeclado == direita)
							cartaSobCursor = cd->jog->mao.front();
						else if(setaDoTeclado == baixo || setaDoTeclado == esquerda)
							cartaSobCursor = cd->jog->mao.back();
					} else {
						for(unsigned int i=0; i<cartaSobCursor->jog->mao.size(); i++)
							if(cartaSobCursor->jog->mao[i] == cartaSobCursor){
								if((setaDoTeclado == cima || setaDoTeclado == direita) && i<cartaSobCursor->jog->mao.size())
									cartaSobCursor = cartaSobCursor->jog->mao[i+1];
								else if((setaDoTeclado == baixo || setaDoTeclado == esquerda) && i>0)
									cartaSobCursor = cartaSobCursor->jog->mao[i-1];
								break;
							}
					}
				} else if(existe(cartaSobCursor)){
					if(cartaSobCursor->localC == locDescartes){
						if(cartaSobCursor->jog == jogador[0] && setaDoTeclado == direita)
							moverCursorPara(TAB_RES_BRANCO_POSX,TAB_RES_BRANCO_POSY);
						else if(cartaSobCursor->jog == jogador[1] && setaDoTeclado == esquerda){cout<<TAB_RES_PRETO_POSX<<","<<TAB_RES_PRETO_POSY;
							moverCursorPara(TAB_RES_PRETO_POSX+TAMANHO_ESPACO,TAB_RES_PRETO_POSY);}
						else if(setaDoTeclado == baixo){
							if(cartaSobCursor->jog == jogador[0] && jogador[0]->mao.size()>0)
								cartaSobCursor = jogador[0]->mao.back();
							else if(cartaSobCursor->jog == jogador[1] && jogador[1]->mao.size()>0)
								cartaSobCursor = jogador[1]->mao.back();
							else
								moverCursorPara(baixo,true);
						}
					} else
						cartaSobCursor = cd;
				} else// if(!olhandoDescartes || obterCartaEm(x,y)->localC == locDescartes && obterCartaEm(x,y)->jog == jogadorDosDescartesExibidos)
					cartaSobCursor = cd;//obterCartaEm(x,y);
			} else// if(!olhandoDescartes || obterCartaEm(x,y)->localC == locDescartes && obterCartaEm(x,y)->jog == jogadorDosDescartesExibidos)
				cartaSobCursor = cd;//obterCartaEm(x,y);
		} else if(cd->localC == locDescartes && cd->jog == jogadorDosDescartesExibidos)
			cartaSobCursor = cd;
		else
			cartaSobCursor = NULL;
		
		if(existe(cartaSobCursor)){
		/*cursor->tab = NULL;
		cursor->coluna = -1;
		cursor->fila = -1;
		cursor->visivel = false;*/
			cursor->posX = cartaSobCursor->posX;
			cursor->posY = cartaSobCursor->posY;
		}
		}/* else {
	Tabuleiro* t = estaDentroDeQualTabuleiro(x,y);
	if(t != NULL){
		cursor->tab = t;
		cursor->coluna = (x - t->posX)/TAMANHO_ESPACO;
		cursor->fila = (y - t->posY)/TAMANHO_ESPACO;
		cursor->visivel = true;
		cursor->posX = t->posX + cursor->coluna*TAMANHO_ESPACO;
		cursor->posY = t->posY + cursor->fila*TAMANHO_ESPACO;
		cartaSobCursor = NULL;
		}
		else{
		cursor->tab = NULL;
		cursor->coluna = -1;
		cursor->fila = -1;
		cursor->visivel = false;
			//cartaSobCursor = NULL;
		}*/
	}
}
void Jogo::moverCursorPara(direcao dir, bool ateOLimite){
	if(!cursor->visivel && cartaSobCursor == NULL){
		moverCursorPara(cursor->posX,cursor->posY);
		return;
	}
//	int limEsquerda = tabPrincipal->posX;
	int limEsquerda = CARTAS_MAO_BRANCO_POSX;
	int limCima = tabPrincipal->posY;
//	int limDireita = tabPrincipal->posX+tabPrincipal->largura;
	int limDireita = CARTAS_MAO_PRETO_POSX+LARGURA_PADRAO_CARTAS;
	int limBaixo = tabPrincipal->posY+tabPrincipal->altura;
//	for(int j=0; j<2; j++){
//		if(jogador[j]->reserva->posY < limCima) limCima = jogador[j]->reserva->posY;
//		if(jogador[j]->reserva->posH > limBaixo) limBaixo = jogador[j]->reserva->posH;
//		if(jogador[j]->reserva->posW > limDireita) limDireita = jogador[j]->reserva->posW;
//		if(jogador[j]->reserva->posX < limEsquerda) limEsquerda = jogador[j]->reserva->posX;
//	}
	
	Tabuleiro* t = NULL;
	Carta* c = NULL;
	int i=0;
	int offsetH,offsetV;
	
	if(dir == cima){
		if(cartaSobCursor == NULL){
			offsetV = 0;
			offsetH = TAMANHO_ESPACO/2;
		} else {
			offsetV = 0;
			offsetH = cartaSobCursor->largura/2;
		}
		if(ateOLimite){
			if(cartaSobCursor == NULL)
				moverCursorPara(cursor->posX+offsetH,limCima);
			else {
				while(obterCartaEm(cursor->posX+offsetH,limCima+i) == NULL)
					i++;
				moverCursorPara(cursor->posX+offsetH,limCima+i);
			}
		} else
			while(t == NULL && c == NULL && cursor->posY-i-offsetV >= 0){
				i++;
				if(!olhandoDescartes)
				t = estaDentroDeQualTabuleiro(cursor->posX+offsetH, cursor->posY-i-offsetV);
				if(existe(t))
					moverCursorPara(cursor->posX+offsetH, cursor->posY-i-offsetV);
				else {
						//cout<<cursor->posX+offsetH<<","<<cursor->posY-i-offsetV<<"\n";
					c = obterCartaEm(cursor->posX+offsetH, cursor->posY-i-offsetV);
					if(existe(c)){
						if(!olhandoDescartes || (c->localC == locDescartes && c->jog == jogadorDosDescartesExibidos))
						//cout<<"achou "<<c->nome<<"\n";
						moverCursorPara(cursor->posX+offsetH, cursor->posY-i-offsetV, dir);
						else
							c = NULL;
					}
				}
			}
	}
	if(dir == baixo){
		if(cartaSobCursor == NULL){
			offsetV = TAMANHO_ESPACO;
			offsetH = TAMANHO_ESPACO/2;
		} else {
			offsetV = cartaSobCursor->altura;
			offsetH = cartaSobCursor->largura/2;
		}
		if(ateOLimite){
			if(cartaSobCursor == NULL)
				moverCursorPara(cursor->posX+offsetH,limBaixo-1);
			else {
				while(obterCartaEm(cursor->posX+offsetH,limBaixo-1-i) == NULL)
					i++;
				moverCursorPara(cursor->posX+offsetH,limBaixo-1-i);
			}
		} else
			while(t == NULL && c == NULL && cursor->posY+i+offsetV < JANELA_ALTURA){
				if(!olhandoDescartes)
				t = estaDentroDeQualTabuleiro(cursor->posX+offsetH, cursor->posY+i+offsetV);
				if(existe(t))
					moverCursorPara(cursor->posX+offsetH, cursor->posY+i+offsetV);
				else {
						//cout<<cursor->posX+offsetH<<","<<cursor->posY-i-offsetV<<"\n";
					c = obterCartaEm(cursor->posX+offsetH, cursor->posY+i+offsetV);
					if(existe(c)){
						if(!olhandoDescartes || (c->localC == locDescartes && c->jog == jogadorDosDescartesExibidos))
						//cout<<"achou "<<c->nome<<"\n";
						moverCursorPara(cursor->posX+offsetH, cursor->posY+i+offsetV, dir);
						else
							c = NULL;
					}
				}
				i++;
			}
	}
	if(dir == esquerda){
		if(cartaSobCursor == NULL){
			offsetV = TAMANHO_ESPACO/2;
			offsetH = 0;
		} else {
			offsetV = cartaSobCursor->altura/3;
			offsetH = 0;
		}
		if(ateOLimite){
			while(t == NULL && c == NULL){
				c = obterCartaEm(limEsquerda+i, max(cursor->posY+offsetV,limCima));
				if(existe(c)){
					if(c->localC == locMao){
						moverCursorPara(c->jog->mao.front()->posX,c->jog->mao.front()->posY);
						return;
					}
				} else
					if(!olhandoDescartes)
						t = estaDentroDeQualTabuleiro(limEsquerda+i, max(cursor->posY+offsetV,limCima));
				
				if(t == NULL && c == NULL)
					i++;
			}
			moverCursorPara(limEsquerda+i,max(cursor->posY+offsetV,limCima));
//				if(t != NULL)
//					moverCursorPara(cursor->posX-i-offsetH, cursor->posY+offsetV);
//				else {
						//cout<<cursor->posX-i-offsetH<<","<<cursor->posY+offsetV<<"\n";
			/*if(cartaSobCursor == NULL)
				moverCursorPara(limEsquerda,max(cursor->posY+offsetV,limCima));
			else {
				while(obterCartaEm(limEsquerda+i,max(cursor->posY+offsetV,limCima)) == NULL)
					i++;
				moverCursorPara(limEsquerda+i,max(cursor->posY+offsetV,limCima));
			}*/
		} else
			while(t == NULL && c == NULL && cursor->posX-i-offsetH >= 0){
				i++;
				if(!olhandoDescartes)
				t = estaDentroDeQualTabuleiro(cursor->posX-i-offsetH, cursor->posY+offsetV);
				if(existe(t))
					moverCursorPara(cursor->posX-i-offsetH, cursor->posY+offsetV);
				else {
						//cout<<cursor->posX-i-offsetH<<","<<cursor->posY+offsetV<<"\n";
					c = obterCartaEm(cursor->posX-i-offsetH, cursor->posY+offsetV);
					if(existe(c)){
						if(!olhandoDescartes || (c->localC == locDescartes && c->jog == jogadorDosDescartesExibidos))
						//cout<<"achou "<<c->nome<<"\n";
						moverCursorPara(cursor->posX-i-offsetH, cursor->posY+offsetV, dir);
						else
							c = NULL;
					}
				}
			}
	}
	if(dir == direita){
		if(cartaSobCursor == NULL){
			offsetV = TAMANHO_ESPACO/2;
			offsetH = TAMANHO_ESPACO;
		} else {
			offsetV = cartaSobCursor->altura/3;
			offsetH = cartaSobCursor->largura;
		}
		if(ateOLimite){
			while(t == NULL && c == NULL){
				c = obterCartaEm(limDireita-1-i, max(cursor->posY+offsetV,limCima));
				if(existe(c)){
					if(c->localC == locMao){
						moverCursorPara(c->jog->mao.back()->posX,c->jog->mao.back()->posY);
						return;
					}
				} else
					if(!olhandoDescartes)
						t = estaDentroDeQualTabuleiro(limDireita-1-i, max(cursor->posY+offsetV,limCima));
				
				if(t == NULL && c == NULL)
					i++;
			}
			moverCursorPara(limDireita-1-i,max(cursor->posY+offsetV,limCima));
			/*if(cartaSobCursor == NULL)
				moverCursorPara(limDireita-1,max(cursor->posY+offsetV,limCima));
			else {
				while(obterCartaEm(limDireita-1-i,max(cursor->posY+offsetV,limCima)) == NULL)
					i++;
				moverCursorPara(limDireita-1-i,max(cursor->posY+offsetV,limCima));
			}*/
		} else
			while(t == NULL && c == NULL && cursor->posX+i+offsetH < JANELA_LARGURA){
				if(!olhandoDescartes)
				t = estaDentroDeQualTabuleiro(cursor->posX+i+offsetH, cursor->posY+offsetV);
				if(existe(t))
					moverCursorPara(cursor->posX+i+offsetH, cursor->posY+offsetV);
				else {
						//cout<<cursor->posX+i+offsetH<<","<<cursor->posY+offsetV<<"\n";
					c = obterCartaEm(cursor->posX+i+offsetH, cursor->posY+offsetV);
					if(existe(c)){
						if(!olhandoDescartes || (c->localC == locDescartes && c->jog == jogadorDosDescartesExibidos))
						//cout<<"achou "<<c->nome<<"\n";
						moverCursorPara(cursor->posX+i+offsetH, cursor->posY+offsetV, dir);
						else
							c = NULL;
					}
				}
				i++;
			}
	}
}

Peca* Jogo::obterPecaEm(Tabuleiro* t, int col, int fil){
	Peca* p;
	for(int j=0; j<2; j++)
		for(int i=0; i<16; i++){
			p = jogador[j]->todasAsPecas[i];
			if(p->tab == t && p->coluna == col && p->fila == fil)
				return p;
		}
	return NULL;
}
Peca* Jogo::obterPecaEm(Cursor* c){
	return obterPecaEm(c->tab, c->coluna, c->fila);
}
Peca* Jogo::obterPecaEm(CasaAlvo* ca){
	return obterPecaEm(ca->tab, ca->coluna, ca->fila);
}

Carta* Jogo::obterCartaEm(int x, int y){
	if(olhandoDescartes){
		Jogador* j = jogadorDosDescartesExibidos;
		for(unsigned int i=j->descartes.size(); i>0; i--)
			if(x >= j->descartes[i-1]->posX && x < j->descartes[i-1]->posX + j->descartes[i-1]->largura
			&& y >= j->descartes[i-1]->posY && y < j->descartes[i-1]->posY + j->descartes[i-1]->altura)
				return j->descartes[i-1];
	} else {
		for(int j=0; j<2; j++){
			for(unsigned int i=jogador[j]->descartes.size(); i>0; i--)
				if(x >= jogador[j]->descartes[i-1]->posX && x < jogador[j]->descartes[i-1]->posX + jogador[j]->descartes[i-1]->largura
				&& y >= jogador[j]->descartes[i-1]->posY && y < jogador[j]->descartes[i-1]->posY + jogador[j]->descartes[i-1]->altura)
						return jogador[j]->descartes.back();
			for(unsigned int i=jogador[j]->mao.size(); i>0; i--)
				if(x >= jogador[j]->mao[i-1]->posX && x < jogador[j]->mao[i-1]->posX + jogador[j]->mao[i-1]->largura
				&& y >= jogador[j]->mao[i-1]->posY && y < jogador[j]->mao[i-1]->posY + jogador[j]->mao[i-1]->altura)
					return jogador[j]->mao[i-1];
	//		if(jogador[j]->baralho.size()>0)
	//			if(x >= jogador[j]->baralho.back()->posX && x < jogador[j]->baralho.back()->posX + jogador[j]->baralho.back()->largura
	//			&& y >= jogador[j]->baralho.back()->posY && y < jogador[j]->baralho.back()->posY + jogador[j]->baralho.back()->altura)
	//					return jogador[j]->baralho.back();
			for(unsigned int i=jogador[j]->baralho.size(); i>0; i--)
				if(x >= jogador[j]->baralho[i-1]->posX && x < jogador[j]->baralho[i-1]->posX + jogador[j]->baralho[i-1]->largura
				&& y >= jogador[j]->baralho[i-1]->posY && y < jogador[j]->baralho[i-1]->posY + jogador[j]->baralho[i-1]->altura)
					return jogador[j]->baralho.back();
		}
	}
	return NULL;
}

Carta* Jogo::obterCartaEm(Cursor* c){
	return obterCartaEm(c->posX, c->posY);
}

CasaAlvo* Jogo::obterCasaAlvoEm(Tabuleiro* t, int col, int fil){
	for(unsigned int i=0; i<casasAlvo.size(); i++)
		if(casasAlvo[i].tab == t && casasAlvo[i].coluna == col && casasAlvo[i].fila == fil)
			return &casasAlvo[i];
	return NULL;
}
CasaAlvo* Jogo::obterCasaAlvoEm(Cursor* c){
	return obterCasaAlvoEm(c->tab, c->coluna, c->fila);
}
CasaAlvo* Jogo::obterCasaAlvoEm(CasaAlvo* ca){
	return obterCasaAlvoEm(ca->tab, ca->coluna, ca->fila);
}

bool Jogo::estaoNoMesmoLugar(Cursor* c, CasaAlvo* ca){
	return (c->tab == ca->tab && c->coluna == ca->coluna && c->fila == ca->fila);
}
bool Jogo::estaoNoMesmoLugar(Peca* p, CasaAlvo* ca){
	return (p->tab == ca->tab && p->coluna == ca->coluna && p->fila == ca->fila);
}
bool Jogo::estaoNoMesmoLugar(CasaAlvo* ca, CasaAlvo* ca2){
	return (ca->tab == ca2->tab && ca->coluna == ca2->coluna && ca->fila == ca2->fila);
}

bool Jogo::estaDentroDoTabuleiro(Tabuleiro* t, CasaAlvo* ca){
	return ca->tab == t && ca->coluna >= 0 && ca->coluna < t->nColunas && ca->fila >= 0 && ca->fila < t->nFilas;
}
Tabuleiro* Jogo::estaDentroDeQualTabuleiro(int x, int y){
	Tabuleiro* t = NULL;
	
	if(x >= tabPrincipal->posX && x < tabPrincipal->posW && y >= tabPrincipal->posY && y < tabPrincipal->posH)
		t = tabPrincipal;
	if(x >= jogador[0]->reserva->posX && x < jogador[0]->reserva->posW && y >= jogador[0]->reserva->posY && y < jogador[0]->reserva->posH)
		t = jogador[0]->reserva;
	if(x >= jogador[1]->reserva->posX && x < jogador[1]->reserva->posW && y >= jogador[1]->reserva->posY && y < jogador[1]->reserva->posH)
		t = jogador[1]->reserva;
	
	return t;
}

void Jogo::selecionar(){
	if(!jogando) return;
	if(olhandoDescartes)
		guardarDescartes();
	else if(pecaSelecionada == NULL){
		if(cartaSelecionada == NULL){
			if(cartaSobCursor == NULL){
				if(pecaSobCursor == NULL)
					cancelarSelecao(); //clique no nada
				/*else if(sacrificandoGeneral && casaAlvoSobCursor != NULL){
					jogadorDaVez->reembaralhar(pecaSobCursor);
					mensagemTexto->escrever(gameRenderer,ssAux.str(),true,tabPrincipal->largura);
					voltarParaReserva(pecaSobCursor);
					cancelarSelecao();
				} else if(sacrificandoRei && casaAlvoSobCursor != NULL){
					vitoria(jogadorDaVez->oponente);
				} */else if(pecaSobCursor->selecionavel)
					selecionarPeca(pecaSobCursor);
				else
					cancelarSelecao(); //clique em peça do oponente
			} //existe cartaSobCursor
			else if(cartaSobCursor->selecionavel)
				selecionarCarta(cartaSobCursor);
			/*else if(cartaSobCursor->jog == jogadorDaVez && !pilhaEmAcao){
				if(sacrificandoGeneral || sacrificandoRei)
					sacrificandoGeneral = sacrificandoRei = false;
				else if(cartaSobCursor->localC == locDescartes)
					sacrificandoGeneral = true;
				else if(cartaSobCursor->localC == locBaralho)
					sacrificandoRei = true;
				else
					cancelarSelecao();
			} */
			else if(cartaSobCursor->localC == locDescartes)
				olharDescartes(cartaSobCursor->jog);
			else
				cancelarSelecao();
		} //existe cartaSelecionada, não existe pecaSelecionada
		else if(casaAlvoSobCursor == NULL){
			if(cartaSobCursor == NULL)
				cancelarSelecao(); //clique no nada
			else if(cartaSelecionada->tipo == movGen && cartaSobCursor->tipo == movGen && cartaSobCursor->jog == cartaSelecionada->jog
					 && cartaSobCursor != cartaSelecionada && cartaSobCursor->localC == locMao){
				cout<<"\a";//...2 movGen //usar 2 movGen pra trocar lugar d seus generais
				cancelarSelecao();
			} else if(cartaSelecionada->tipo == anuCar && cartaSobCursor->ativada)
				adicionarEfeitoNaPilhaNoCursor(cartaSobCursor);
			else if(cartaSelecionada->jog->descartes.size()>0){
				if(cartaSobCursor == cartaSelecionada->jog->descartes.back()){
					cartaSelecionada->jog->descartar(cartaSelecionada); //se puder descartar a qualker momento
					cartaSobCursor = obterCartaEm(cursor);
					cancelarSelecao();
				} else
					cancelarSelecao();
			} else
				cancelarSelecao();
		} //existe cartaSelecionada e casaAlvoSobCursor
		else if((cartaSelecionada->tipo == movUni || cartaSelecionada->tipo == movGen || cartaSelecionada->tipo == atqExt)
				 && casaAlvoSobCursor->tipo == acaoCar)
			selecionarPecaIntermediariaSobCursor();
		else
			adicionarEfeitoNaPilhaNoCursor();
	} //existe pecaSelecionada
	else if(pecaSobCursor == NULL){
		if(cartaSobCursor == NULL){
			if(casaAlvoSobCursor == NULL)
				cancelarSelecao();
			else
				//selecionarCasa(casaAlvoSobCursor);
				adicionarEfeitoNaPilhaNoCursor();
		} //existe pecaSelecionada e cartaSobCursor
		else if(pecaSelecionada->jog == jogadorDaVez && cartaSobCursor->jog == jogadorDaVez && pecaSelecionada->tab == tabPrincipal
		&& pecaSelecionada->tipo != peao && !jogadorDaVez->usandoMovGen && !jogadorDaVez->usandoAtqExt){
			if(pecaSelecionada->classe == classeGeneral && cartaSobCursor->localC == locDescartes){
				cartaSobCursor = NULL; //pra qd reembaralhar n aparecer o contorno nela no meio do baralho kk
				jogadorDaVez->reembaralhar(pecaSelecionada);
				mensagemTexto->escrever(gameRenderer,ssAux.str(),true,tabPrincipal->largura);
				voltarParaReserva(pecaSelecionada);
				cancelarSelecao(); //pra peça n ficar selecionada na reserva e.e
			} else if(pecaSelecionada->tipo == rei && cartaSobCursor->localC == locBaralho){
				sacrificouRei = true;
				vitoria(jogadorDaVez->oponente);
			} else
				cancelarSelecao();
		} else
			cancelarSelecao();
	} //existe pecaSelecionada e pecaSobCursor
	else if(casaAlvoSobCursor == NULL)
		cancelarSelecao();
	else {
		if(pecaSelecionada->classe == classeGeneral && pecaSobCursor->classe == classeGeneral && casaAlvoSobCursor->tipo == acaoMov)
			pecaSelecionada->jog->trocandoGeneraisDeLugarComAcao = true;
		adicionarEfeitoNaPilhaNoCursor();
	}
}

void Jogo::cancelarSelecao(){
	pecaSelecionada = NULL;
	copiaDaPecaSelecionadaNoCursor->nome = "";
	copiaDaPecaSelecionadaNoCursor->visivel = false;
//	sacrificandoGeneral = false;
//	sacrificandoRei = false;
	
	if(olhandoDescartes)
		guardarDescartes();
	
	if(existe(cartaSelecionada)){
		if(!(cartaSelecionada->jog->usandoMovUni || cartaSelecionada->jog->usandoMovGen || cartaSelecionada->jog->usandoAtqExt))
			cartaSelecionada = NULL;
		else
			cartaSelecionada->jog->usandoMovUni = cartaSelecionada->jog->usandoMovGen = cartaSelecionada->jog->usandoAtqExt = false;
	}
}

void Jogo::selecionarPeca(Peca* p){
	pecaSelecionada = p;
	copiaDaPecaSelecionadaNoCursor->tex = pecaSelecionada->tex;
	copiaDaPecaSelecionadaNoCursor->texNome = pecaSelecionada->texNome;
	copiaDaPecaSelecionadaNoCursor->jog = pecaSelecionada->jog;
	copiaDaPecaSelecionadaNoCursor->classe = pecaSelecionada->classe;
	copiaDaPecaSelecionadaNoCursor->tipo = pecaSelecionada->tipo;
	copiaDaPecaSelecionadaNoCursor->cor = pecaSelecionada->cor;
	copiaDaPecaSelecionadaNoCursor->nome = pecaSelecionada->nome;
	moverPecaPara(copiaDaPecaSelecionadaNoCursor,jogador[0]->reserva,0,0);
}

void Jogo::selecionarPecaIntermediariaSobCursor(){
	if(cartaSelecionada->tipo == movUni)
		cartaSelecionada->jog->usandoMovUni = true;
	else if(cartaSelecionada->tipo == movGen)
		cartaSelecionada->jog->usandoMovGen = true;
	else if(cartaSelecionada->tipo == atqExt)
		cartaSelecionada->jog->usandoAtqExt = true;
	
	selecionarPeca(pecaSobCursor);
}

void Jogo::selecionarCarta(Carta* c){
	cartaSelecionada = c;
}

void Jogo::olharDescartes(Jogador* j){
	olhandoDescartes = true;
	jogadorDosDescartesExibidos = j;
	//j->exibindoDescartes = true;
	unsigned int cartasPorLinha = 10;
	if(j->descartes.size() < cartasPorLinha)
		cartasPorLinha = j->descartes.size();
	int numLinhas = (j->descartes.size()-1)/10 + 1; //arredondar pra cima
	int c = j->descartes.size();
	//cout<<"entrou\n";
	for(int l=0; l<numLinhas; l++)
		for(unsigned int i=0; i<cartasPorLinha && c>0; i++){
			//if(l*cartasPorLinha + i == j->descartes.size()) break;
			c = j->descartes.size()-1 - (l*cartasPorLinha + i); //começa pela carta do topo da pilha de descartes
			//if(c < 0) break;
			//cout<<c<<"\n";
			j->descartes[c]->posX = (janela->largura - cartasPorLinha*LARGURA_PADRAO_CARTAS
															- (cartasPorLinha-1)*ESPACO_AO_REDOR_DAS_CARTAS)/2
															+ i*(LARGURA_PADRAO_CARTAS+ESPACO_AO_REDOR_DAS_CARTAS);
			j->descartes[c]->posY = (janela->altura - ALTURA_PADRAO_CARTAS*numLinhas
															- (numLinhas-1)*ESPACO_AO_REDOR_DAS_CARTAS)/2
															+ l*(ALTURA_PADRAO_CARTAS+ESPACO_AO_REDOR_DAS_CARTAS);
		}
	moverCursorPara(j->descartes.back()->posX,j->descartes.back()->posY);
	//cartaSobCursor = j->descartes[0];
}

void Jogo::guardarDescartes(){
	olhandoDescartes = false;
	//cartaSobCursor = jogadorDosDescartesExibidos->descartes.back();
	//for(int j=0; j<2; j++){
//		if(jogador[j]->exibindoDescartes)
//			jogador[j]->exibindoDescartes = false;
		for(unsigned int i=0; i<jogadorDosDescartesExibidos->descartes.size(); i++)
			if(jogadorDosDescartesExibidos->cor == branco){
				jogadorDosDescartesExibidos->descartes[i]->posX = DESCARTES_BRANCO_POSX;
				jogadorDosDescartesExibidos->descartes[i]->posY = DESCARTES_BRANCO_POSY - i*2; //pra fazer o monte
			} else {
				jogadorDosDescartesExibidos->descartes[i]->posX = DESCARTES_PRETO_POSX;
				jogadorDosDescartesExibidos->descartes[i]->posY = DESCARTES_PRETO_POSY - i*2; //pra fazer o monte
			}
	//}
	moverCursorPara(jogadorDosDescartesExibidos->descartes.back()->posX,jogadorDosDescartesExibidos->descartes.back()->posY);
	jogadorDosDescartesExibidos = NULL;
}

void Jogo::adicionarEfeitoNaPilhaNoCursor(Carta* cartaAnulada){
	//cópias
//	Peca* ps = &(*pecaSelecionada);
//	Peca* psc = &(*pecaSobCursor);
//	Carta* cs = &(*cartaSelecionada);
//	CasaAlvo* casc = &(*casaAlvoSobCursor);
	
	if(existe(cartaSelecionada))
		if(cartaSelecionada->tipo != conAtq){
			//conAtq só funciona imediatamnt após o ataque que falhou, por isso,
			//se qualker outro efeito for usado, conAtq n poderá mais ser ativado
			jogador[0]->ultimaUnidadeQueFalhouNumAtaque = NULL;
			jogador[1]->ultimaUnidadeQueFalhouNumAtaque = NULL;
		}
	
	ssAux.str("");
	if(cartaSelecionada == NULL){
		if(!pecaSelecionada->jog->promovendoPeao)
			acoesPorTurno--;
		else
			pecaSelecionada->jog->promovendoPeao = false;
		if(casaAlvoSobCursor->tipo == acaoInv){
			pilha.push_back(Efeito(pecaSelecionada->jog,pecaSelecionada,casaAlvoSobCursor));
//			if(pecaSelecionada->jog->promovendoPeao)
//				pecaSelecionada->jog->promovendoPeao = false;
		} else
		if(casaAlvoSobCursor->tipo == acaoMov){
			if(pecaSelecionada->jog->trocandoGeneraisDeLugarComAcao){
				pecaSelecionada->jog->trocandoGeneraisDeLugarComAcao = false;
				acoesPorTurno--;
				pilha.push_back(Efeito(pecaSelecionada->jog,pecaSelecionada,casaAlvoSobCursor,pecaSobCursor));
			} else
				pilha.push_back(Efeito(pecaSelecionada->jog,pecaSelecionada,casaAlvoSobCursor));
		} else
		if(casaAlvoSobCursor->tipo == acaoAtq)
			pilha.push_back(Efeito(pecaSelecionada->jog,pecaSelecionada,casaAlvoSobCursor,pecaSobCursor));
		
		cout<<"Ações restantes: "<<acoesPorTurno<<".\n";
	} //com carta
	else {
		cartaSelecionada->revelada = true;
		cartaSelecionada->ativada = true;
		cartaSelecionada->jog->usandoMovUni = cartaSelecionada->jog->usandoMovGen = cartaSelecionada->jog->usandoAtqExt = false;
		switch(cartaSelecionada->tipo){
		case atqDir: case conAtq: case bonAtq: case bonDef: case bonMov: case bonAlc: case conPec:
			pilha.push_back(Efeito(cartaSelecionada->jog,cartaSelecionada,casaAlvoSobCursor,pecaSobCursor));
			break;
		case recUni: case revUni:
			pilha.push_back(Efeito(cartaSelecionada->jog,cartaSelecionada,casaAlvoSobCursor));
			break;
		case movUni: case movGen:
			if(cartaSelecionada->jog->trocandoGeneraisDeLugarComAcao){
				pecaSelecionada->jog->trocandoGeneraisDeLugarComAcao = false;
				acoesPorTurno--;
				pilha.push_back(Efeito(cartaSelecionada->jog,cartaSelecionada,pecaSelecionada,casaAlvoSobCursor,pecaSobCursor));
			} else
				pilha.push_back(Efeito(cartaSelecionada->jog,cartaSelecionada,pecaSelecionada,casaAlvoSobCursor));
			break;
		case anuCar:
			pilha.push_back(Efeito(cartaSelecionada->jog,cartaSelecionada,cartaAnulada,casaAlvoSobCursor));
			break;
		case atqExt:
			pilha.push_back(Efeito(cartaSelecionada->jog,cartaSelecionada,pecaSelecionada,casaAlvoSobCursor,pecaSobCursor));
			break;
		}
	}
	cancelarSelecao();
	jogador[0]->esperandoConsentimento = jogador[1]->esperandoConsentimento = false;
	jogador[0]->consentiu = jogador[1]->consentiu = false;
	
	pilhaEmAcao = true;
	jogadorDoUltimoEfeito = pilha.back().jog;
	if(usandoPilha)
		jogadorDoUltimoEfeito->esperandoConsentimento = jogadorDoUltimoEfeito->oponente->mao.size() > 0;
																										//&& !jogadorDoUltimoEfeito->oponente->consentindoAutomaticamente;
	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
	cout<<ssAux.str();
	mensagemTexto->escrever(gameRenderer,ssAux.str(),true,tabPrincipal->largura);
	
	//cout<<"\n>adicionou efeito na pilha, casa.tipo="<<pilha.back().casa.tipo<<", pilha.size="<<pilha.size()<<"\n";
}

void Jogo::checarPilha(){
	if(pilhaEmAcao){
//		if(pilha.size() > 0)
//			jogadorDoUltimoEfeito = pilha.back().jog;
		if(!jogadorDoUltimoEfeito->esperandoConsentimento || (jogadorDoUltimoEfeito->esperandoConsentimento
		&& (jogadorDoUltimoEfeito->oponente->consentiu || jogadorDoUltimoEfeito->oponente->consentindoAutomaticamente))){
			jogadorDoUltimoEfeito->esperandoConsentimento = false;
			if(usandoPilha)
				jogadorDoUltimoEfeito->oponente->esperandoConsentimento = jogadorDoUltimoEfeito->mao.size() > 0;
																																	//&& !jogadorDoUltimoEfeito->oponente->consentindoAutomaticamente;
			//toda ação/carta requer que ambos os jogadores consintam antes e depois de executar seu efeito
			if(!jogadorDoUltimoEfeito->oponente->esperandoConsentimento || (jogadorDoUltimoEfeito->oponente->esperandoConsentimento
			&& (jogadorDoUltimoEfeito->consentiu || jogadorDoUltimoEfeito->consentindoAutomaticamente))){
				jogadorDoUltimoEfeito->oponente->esperandoConsentimento = false;
				
				jogadorDoUltimoEfeito->consentiu = jogadorDoUltimoEfeito->oponente->consentiu = false;
				if(usandoPilha)
					jogadorDoUltimoEfeito->esperandoConsentimento = jogadorDoUltimoEfeito->oponente->mao.size() > 0;
																													//&& !jogadorDoUltimoEfeito->oponente->consentindoAutomaticamente;
				if(pilha.size() > 0){
					//cout<<"\n>iniciando execução do efeito "<<pilha.size()-1<<" da pilha";
					executarEfeito(pilha.back());
					pilha.erase(pilha.end()-1);
				} else {
					pilhaEmAcao = false;
					jogadorDoUltimoEfeito->esperandoConsentimento = false;
					jogador[0]->ultimaUnidadeQueFalhouNumAtaque = NULL; //conAtq só funciona imediatamnt após o ataque q falhou, por isso,
					jogador[1]->ultimaUnidadeQueFalhouNumAtaque = NULL; //ao terminar a pilha, conAtq n pode mais ser ativado
					if(!naFaseInicial)
						mensagemTexto->escrever(gameRenderer," ");
					//cout<<"\n>fim da pilha";
				}
			}
		}
	}
}

void Jogo::executarEfeito(Efeito e){
	//cout<<"\n>começou a executar, casa.tipo="<<e.casa.tipo;
	ssAux.str("");
	
	if(e.casa.tipo == acaoInv){
		if(e.peca->classe == classeGeneral && existe(e.pecaAlvo))
			if(e.pecaAlvo->tipo == peao)
				voltarParaReserva(e.pecaAlvo); //se puder invocar general sobre peão
		if(obterPecaEm(&e.casa) == NULL){
			moverPecaPara(e.peca,&e.casa);
			ssAux<<e.peca->nome<<" invocad"<<e.peca->sufixoGenero<<" em "<<e.casa.coluna<<","<<e.casa.fila<<".\n";
		} else
			ssAux<<"Casa em "<<e.casa.coluna<<","<<e.casa.fila<<" está ocupada. A ação foi mal sucedida.\n";
		
//		if(e.jog->promovendoPeao){ //passei pra adicionarEfeito
//			e.jog->promovendoPeao = false;
//			pilhaEmAcao = true;
//		}
		//else {
//		ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//		cout<<ssAux.str();
		if(e.carta == NULL)
			removerBonusPorCartas(); //promovendo tbm afeta isso?
		//}
	}
	
	if(e.casa.tipo == acaoMov){
		bool daPraMover=false;
//		if(e.peca->tab != tabPrincipal || e.peca->congelada) //checa se a peça ainda tá ativa
//			daPraMover = false;
//		else {
		if(e.peca->tab != tabPrincipal) //checa se a peça ainda tá ativa
			ssAux<<e.peca->nome<<" não está mais no tabuleiro. ";
		else if(e.peca->congelada)
			ssAux<<e.peca->nome<<" não pode ser movid"<<e.peca->sufixoGenero<<" porque está congelad"<<e.peca->sufixoGenero<<". ";
		else if(existe(e.pecaAlvo)){ //general não se move pra cima de peão, então pecaAlvo é general
			if(e.pecaAlvo->congelada)
				ssAux<<e.pecaAlvo->nome<<" não pode ser movid"<<e.pecaAlvo->sufixoGenero
					<<" porque está congelad"<<e.pecaAlvo->sufixoGenero<<". ";
//			else if(!estaoNoMesmoLugar(e.pecaAlvo,&e.casa))
//				ssAux<<e.pecaAlvo->nome<<" já foi movid"<<e.pecaAlvo->sufixoGenero<<". ";
			else {
				if(e.carta == NULL)
					daPraMover = true;
				else if(e.carta->ativada)
					daPraMover = true;
				else
					ssAux<<e.carta->nome<<" foi anulada. ";
				//else if(acoesPorTurno > 0){
					//acoesPorTurno--;
					//finalizarAcao();
				if(daPraMover){
					moverPecaPara(e.pecaAlvo,e.peca);
					ssAux<<e.pecaAlvo->nome<<" movid"<<e.pecaAlvo->sufixoGenero<<" para "<<e.peca->coluna<<","<<e.peca->fila<<".\n";
					//acoesPorTurno--;
					//finalizarAcao();
				}
			}
			/*if(acoesPorTurno > 0){ //fazendo uns testes, teve uma hr q ficou -1 o.o por isso colokei essa checagem aki e abaixo
//				ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//				cout<<ssAux.str();
				contarAcao(); //por enqt, sem trocar d lugar com 2 movGen
			} else {
				cout<<"com pecaAlvo e ACOES POR TURNO <= 0? o.o\n";
				system("pause");
			}*/
		} else
			daPraMover = true;
		
		if(daPraMover){
			daPraMover = false; //pra fazer os novos testes
			Peca* p = obterPecaEm(&e.casa);
			if(existe(p)){
//				if(e.peca->classe == classeGeneral && p->tipo == peao){ //se puder mover general pra cima de peões
//					voltarParaReserva(p);
//					daPraMover = true;
//				} else
				ssAux<<"Casa em "<<e.casa.coluna<<","<<e.casa.fila<<" está ocupada. ";
			} else if(e.carta == NULL){
				if(e.peca->classe == classeGeneral)
					daPraMover = true;
				else { //checa se a casa destino ainda tá no limite do movimento do peão e se a trajetória tá livre
					if(e.peca->fila != e.casa.fila)
						ssAux<<e.peca->nome<<" mudou de fila. ";
					else {
						int mult;
						e.jog->cor == branco ? mult = +1 : mult = -1;
						int dist = (e.casa.coluna - e.peca->coluna) * mult;
						daPraMover = true;
						if(dist > 0 && dist <= e.peca->movimento)
							for(int i=1; i<=dist && daPraMover; i++)
								if(existe(obterPecaEm(e.peca->tab, e.peca->coluna+i*mult, e.peca->fila))){
									daPraMover = false;
									ssAux<<"Houve interferência no caminho de "<<e.peca->nome<<". ";
								}
					}/* else
						daPraMover = false;*/
				}/* else 
					daPraMover = true;*/
			} else if(!e.carta->ativada)
				ssAux<<e.carta->nome<<" foi anulada. ";
			else if(e.peca->classe == classeGeneral)
				daPraMover = true;
			else { //checa se o peão ainda tá vizinho à casa destino
				int distH = e.casa.coluna - e.peca->coluna;
				if(distH < 0) distH *= -1;
				int distV = e.casa.fila - e.peca->fila;
				if(distV < 0) distV *= -1;
				if(distH+distV > 1)
					ssAux<<"O destino está fora do alcance. ";
				else
					daPraMover = true;
			}
		}
		
		if(daPraMover){
			moverPecaPara(e.peca,&e.casa);
			ssAux<<e.peca->nome<<" movid"<<e.peca->sufixoGenero<<" para "<<e.casa.coluna<<","<<e.casa.fila<<".\n";
		} else
			ssAux<<"A ação foi mal sucedida.\n";
		
		//checa se tem peão na última coluna
		if(e.peca->tipo == peao && e.peca->coluna == e.peca->jog->oponente->colunaDosGenerais){
			ssAux<<"Peão na última coluna, iniciando promoção.\n";
			voltarParaReserva(e.peca);
			e.peca->jog->promovendoPeao = true;
			pilhaEmAcao = false;
			jogador[0]->esperandoConsentimento = jogador[1]->esperandoConsentimento = false;
//			e.peca->jog->esperandoConsentimento = e.peca->jog->oponente->mao.size() > 0
//																						&& !e.peca->jog->oponente->consentindoAutomaticamente;
		}
		//fazendo uns testes com generais, teve uma hr q acoesPorTurno ficou -1 o.o por isso colokei essa checagem tbm
		if(e.carta == NULL){
			//if(acoesPorTurno > 0){
//				ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//				cout<<ssAux.str();
				removerBonusPorCartas();
			/*} else {
				cout<<"sem carta e ACOES POR TURNO <= 0? o.o\n";
				system("pause");
			}*/
		}
		//finalizarAcao();
	}
	
	if(e.casa.tipo == acaoAtq){ //ataques de peças (ataque de ás eh em casaCarta) e danos sofridos, tanto por peças qt por ás
		if(!emFaseDeDano){
			bool daPraAtacar=false;
			if(e.peca->tab != tabPrincipal)
				ssAux<<e.peca->nome<<" não está mais no tabuleiro. ";
			else if(e.peca->congelada)
				ssAux<<e.peca->nome<<" não pode atacar porque está congelad"<<e.peca->sufixoGenero<<". ";
			else if(e.pecaAlvo->tab != tabPrincipal)
				ssAux<<e.pecaAlvo->nome<<" não está mais no tabuleiro. ";
			else if(existe(e.carta)){
				if(!e.carta->ativada)
					ssAux<<e.carta->nome<<" foi anulada. ";
				else
					daPraAtacar = true;
			} else
				daPraAtacar = true;
			
			if(daPraAtacar){
				int frente;
				e.jog->cor == branco ? frente = +1 : frente = -1;
				CasaAlvo frenteDaPeca = {e.peca->tab, e.peca->coluna+frente, e.peca->fila, acaoAtq};
				int distH = e.pecaAlvo->coluna - frenteDaPeca.coluna; //o ponto base do ataque é à frente da peça
				if(distH < 0) distH *= -1;
				int distV = e.pecaAlvo->fila - frenteDaPeca.fila;
				if(distV < 0) distV *= -1;
				if(distH+distV < e.peca->alcance){ //< em vez de <=, pq alc1 == frente da peça (ponto base)
					ssAux<<e.peca->nome<<" ataca "<<e.pecaAlvo->nome<<".\n";
					atacarPeca(e.peca,e.pecaAlvo);
					/*if(e.carta == NULL){
//						ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//						cout<<ssAux.str();
						contarAcao();
					}*/
				} else {
					ssAux<<e.pecaAlvo->nome<<" está fora do alcance de "<<e.peca->nome<<". A ação foi mal sucedida.\n";
					removerBonusPorCartas();
				}
			} else {
				ssAux<<"A ação foi mal sucedida.\n";
				removerBonusPorCartas();
			}
		}
		else { //emFaseDeDano, tanto por peça qt por ás
			emFaseDeDano = atacandoComAs = false;
			pecaAtacando = pecaDefendendo = NULL;
			int atq;
			if(existe(e.peca)) atq = e.peca->ataque;
			if(existe(e.carta)) atq = AS_ATQ;
			
			if(e.jog->dado <= atq - e.pecaAlvo->defesa){
				ssAux<<"O ataque foi bem sucedido!\n";
//				ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//				cout<<ssAux.str();
				if(e.pecaAlvo->tipo == rei)
					vitoria(e.jog);
				else
					matarPeca(e.pecaAlvo);
			} else {
				ssAux<<"O ataque foi mal sucedido!\n";
				if(existe(e.peca))
					e.jog->ultimaUnidadeQueFalhouNumAtaque = e.peca;
			}
			e.jog->dado = 0;
			if(e.carta == NULL)
				removerBonusPorCartas();
			else { //após um ataque por carta, só os bônus de ataque e defesa são removidos; os outros serão removidos ao fim da ação?
				if(existe(e.peca))
					for(int i=e.peca->bonusPorCartas.size()-1; i >= 0; i++)
						if(e.peca->bonusPorCartas[i] == bAtq || e.peca->bonusPorCartas[i] == bDef)
							e.peca->bonusPorCartas.erase(e.peca->bonusPorCartas.begin()+i);
				for(int i=e.pecaAlvo->bonusPorCartas.size()-1; i >= 0; i++)
					if(e.pecaAlvo->bonusPorCartas[i] == bAtq || e.pecaAlvo->bonusPorCartas[i] == bDef)
						e.pecaAlvo->bonusPorCartas.erase(e.pecaAlvo->bonusPorCartas.begin()+i);
			}
//			dadoTexto->escrever(gameRenderer," ");
		}
	}
	
	/*/deve ficar antes dos efeitos de cartas
	if(jogador[0]->usandoMovUni || jogador[0]->usandoMovGen || jogador[0]->usandoAtqExt
	|| jogador[1]->usandoMovUni || jogador[1]->usandoMovGen || jogador[1]->usandoAtqExt){
		jogador[0]->usandoMovUni = jogador[0]->usandoMovGen = jogador[0]->usandoAtqExt = false;
		jogador[1]->usandoMovUni = jogador[1]->usandoMovGen = jogador[1]->usandoAtqExt = false;
		cout<<"descartando "<<e.carta->nome<<" (carta com ação intermediária)\n";
		e.carta->jog->descartar(e.carta);
		e.carta = NULL;
		system("pause");
	}*/
	
	if(e.casa.tipo == acaoCar){
		if(!e.carta->ativada)
			ssAux<<e.carta->nome<<" foi anulada. A ação foi mal sucedida.\n";
		else if(existe(e.pecaAlvo)){
			if(e.pecaAlvo->tab != tabPrincipal)
				ssAux<<e.pecaAlvo->nome<<" não está mais no tabuleiro. A ação foi mal sucedida.\n";
			else if(e.carta->tipo == atqDir){
				ssAux<<e.jog->nome<<" ataca "<<e.pecaAlvo->nome<<" com "<<e.carta->nome<<".\n";
				atacarPeca(e.carta,e.pecaAlvo);
			} else if(e.carta->tipo == conAtq){
				ssAux<<e.jog->nome<<" revida com "<<e.carta->nome<<".\n";
				matarPeca(e.pecaAlvo);
			} else if(e.carta->tipo == bonAtq){
				e.pecaAlvo->bonusPorCartas.push_back(bAtq);
				ssAux<<"Ataque de "<<e.pecaAlvo->nome<<" aumentado em 1.\n";
			} else if(e.carta->tipo == bonDef){
				e.pecaAlvo->bonusPorCartas.push_back(bDef);
				ssAux<<"Defesa de "<<e.pecaAlvo->nome<<" aumentada em 1.\n";
			} else if(e.carta->tipo == bonMov){
			e.pecaAlvo->bonusPorCartas.push_back(bMov);
				ssAux<<"Movimento de "<<e.pecaAlvo->nome<<" aumentado em 1.\n";
			} else if(e.carta->tipo == bonAlc){
				e.pecaAlvo->bonusPorCartas.push_back(bAlc);
				ssAux<<"Alcance de "<<e.pecaAlvo->nome<<" aumentado em 1.\n";
			} else if(e.carta->tipo == conPec){
				e.pecaAlvo->congelada = true;
				ssAux<<e.pecaAlvo->nome<<" está congelad"<<e.pecaAlvo->sufixoGenero<<".\n";
			}
		/*else if(e.carta->tipo == atqDir){
			if(e.pecaAlvo->tab == tabPrincipal)
				atacarPeca(e.carta,e.pecaAlvo);
			else
				ssAux<<e.pecaAlvo->nome<<" não está mais no tabuleiro. A ação foi mal sucedida.\n";
		} else if(e.carta->tipo == conAtq){
			if(e.pecaAlvo->tab == tabPrincipal)
				matarPeca(e.pecaAlvo);
			else
				ssAux<<e.pecaAlvo->nome<<" não está mais no tabuleiro. A ação foi mal sucedida.\n";
		} else if(e.carta->tipo == bonAtq){
			if(e.pecaAlvo->tab == tabPrincipal)
				e.pecaAlvo->bonusPorCartas.push_back(bAtq);
			else
				ssAux<<e.pecaAlvo->nome<<" não está mais no tabuleiro. A ação foi mal sucedida.\n";
		} else if(e.carta->tipo == bonDef){
			if(e.pecaAlvo->tab == tabPrincipal)
				e.pecaAlvo->bonusPorCartas.push_back(bDef);
			else
				ssAux<<e.pecaAlvo->nome<<" não está mais no tabuleiro. A ação foi mal sucedida.\n";
		} else if(e.carta->tipo == bonMov){
			if(e.pecaAlvo->tab == tabPrincipal)
				e.pecaAlvo->bonusPorCartas.push_back(bMov);
			else
				ssAux<<e.pecaAlvo->nome<<" não está mais no tabuleiro. A ação foi mal sucedida.\n";
		} else if(e.carta->tipo == bonAlc){
			if(e.pecaAlvo->tab == tabPrincipal)
				e.pecaAlvo->bonusPorCartas.push_back(bAlc);
			else
				ssAux<<e.pecaAlvo->nome<<" não está mais no tabuleiro. A ação foi mal sucedida.\n";*/
		} else if(e.carta->tipo == recUni || e.carta->tipo == revUni){
			if(existe(obterPecaEm(&e.casa)))
				ssAux<<"Casa em "<<e.casa.coluna<<","<<e.casa.fila<<" está ocupada. A ação foi mal sucedida.\n";
			else for(int i=0; i<8; i++)
				if(e.jog->unidade[i]->tab == e.jog->reserva){
					moverPecaPara(e.jog->unidade[i],&e.casa);
					
					ssAux<<e.jog->unidade[i]->nome;
					if(e.carta->tipo == recUni)
						ssAux<<" invocad";
					if(e.carta->tipo == revUni)
						ssAux<<" revivid";
					ssAux<<e.jog->unidade[i]->sufixoGenero<<" em "<<e.casa.coluna<<","
					<<e.casa.fila<<".\n";
					
					break;
				}
		//movUni, movGen e atqExt transferidos pra selecionarPecaIntermediaria()
		//revUni unido a recUni, pq a execução é a msm
		} else if(e.carta->tipo == anuCar){
			e.cartaAlvo->ativada = false;
			e.cartaAlvo->anulada = true;
			ssAux<<e.cartaAlvo->nome<<" anulada.\n";
		/*} else if(e.carta->tipo == conPec){
			if(e.pecaAlvo->tab == tabPrincipal)
				e.pecaAlvo->congelada = true;
			else
				ssAux<<e.pecaAlvo->nome<<" não está mais no tabuleiro. A ação foi mal sucedida.\n";*/
		}
	}
	//cout<<ssAux.str().length();
	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
	cout<<ssAux.str();
	mensagemTexto->escrever(gameRenderer,ssAux.str(),true,tabPrincipal->largura);
	
	if(existe(e.carta) && !(e.jog->usandoMovUni || e.jog->usandoMovGen || e.jog->usandoAtqExt)){
		//cout<<"descartando "<<e.carta->nome<<" (normal)\n";
		e.jog->descartar(e.carta);
		//e.carta = obterCartaEm(cursor); //pq no cursor??
		e.carta = NULL;
		//system("pause");
	}
	//se nenhum jogador tiver usado carta q requer seleção de uma peça
	if(!(jogador[0]->usandoMovUni || jogador[0]->usandoMovGen || jogador[0]->usandoAtqExt)
	&& !(jogador[1]->usandoMovUni || jogador[1]->usandoMovGen || jogador[1]->usandoAtqExt))
		cancelarSelecao();
	//cout<<"\n>execução finalizada\n";
}

void Jogo::moverPecaPara(Peca* p, Tabuleiro* t, int col, int fil){
	p->tab = t;
	p->coluna = col;
	p->fila = fil;
}
void Jogo::moverPecaPara(Peca* p, Cursor* c){
	moverPecaPara(p, c->tab, c->coluna, c->fila);
}
void Jogo::moverPecaPara(Peca* p, Peca* pd){
	moverPecaPara(p, pd->tab, pd->coluna, pd->fila);
}
void Jogo::moverPecaPara(Peca* p, CasaAlvo* ca){
	moverPecaPara(p, ca->tab, ca->coluna, ca->fila);
}

/*void Jogo::atacarPeca(Peca* atacante, Peca* alvo){
	emFaseDeDano = true;
	pecaDefendendo = alvo;
	pecaAtacando = atacante;
	atacante->jog->dado = rand()%6+1;
//	cout<<"\n>dado="<<atacante->jog->dado<<"; se deixar, ";
//	if(atacante->jog->dado <= atacante->ataque - alvo->defesa)
//		cout<<"mata\n";
//	else
//		cout<<"não mata\n";
	
	ssAux.str("");
//	ssAux<<"Jogador ";
//	if(atacante->jog->cor == branco) ssAux<<"branco";
//	if(atacante->jog->cor == preto) ssAux<<"preto";
	ssAux<<atacante->jog->nome<<" rola um "<<atacante->jog->dado<<" no dado.\n";
	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
	cout<<ssAux.str();
	
	CasaAlvo casa = {alvo->tab,alvo->coluna,alvo->fila,acaoAtq};
	pilha.insert(pilha.end()-1, Efeito(atacante->jog, atacante, &casa, alvo, true));
}
void Jogo::atacarPeca(Carta* as, Peca* alvo){
	emFaseDeDano = true;
	pecaDefendendo = alvo;
	atacandoComAs = true;
	as->jog->dado = rand()%6+1;
//	cout<<"\n>dado="<<as->jog->dado<<"; se deixar, ";
//	if(as->jog->dado <= AS_ATQ - alvo->defesa)
//		cout<<"mata\n";
//	else
//		cout<<"não mata\n";
	
	ssAux.str("");
//	ssAux<<"Jogador ";
//	if(as->jog->cor == branco) ssAux<<"branco";
//	if(as->jog->cor == preto) ssAux<<"preto";
	ssAux<<as->jog->nome<<" rola um "<<as->jog->dado<<" no dado.\n";
	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
	cout<<ssAux.str();
	
	CasaAlvo casa = {alvo->tab,alvo->coluna,alvo->fila,acaoAtq};
	pilha.insert(pilha.end()-1, Efeito(as->jog, as, &casa, alvo, true));
}*/

void Jogo::atacarPeca(Peca* atacante, Peca* alvo){
	pecaAtacando = atacante;
	executarAtaque(atacante->jog,alvo);
	CasaAlvo vitima = {alvo->tab,alvo->coluna,alvo->fila,acaoAtq};
	pilha.insert(pilha.end()-1, Efeito(atacante->jog, atacante, &vitima, alvo, true));
}
void Jogo::atacarPeca(Carta* as, Peca* alvo){
	atacandoComAs = true;
	executarAtaque(as->jog,alvo);
	CasaAlvo vitima = {alvo->tab,alvo->coluna,alvo->fila,acaoAtq};
	pilha.insert(pilha.end()-1, Efeito(as->jog, as, &vitima, alvo, true));
}
void Jogo::executarAtaque(Jogador* j,Peca* alvo){
	emFaseDeDano = true;
	pecaDefendendo = alvo;
	j->dado = rand()%6+1;
//	cout<<"\n>dado="<<atacante->jog->dado<<"; se deixar, ";
//	if(atacante->jog->dado <= atacante->ataque - alvo->defesa)
//		cout<<"mata\n";
//	else
//		cout<<"não mata\n";
	
	//ssAux.str("");
//	ssAux<<"Jogador ";
//	if(atacante->jog->cor == branco) ssAux<<"branco";
//	if(atacante->jog->cor == preto) ssAux<<"preto";
	ssAux<<j->nome<<" rola um "<<j->dado<<" no dado.\n";
	//ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
	//cout<<ssAux.str();
	
//	ssAuxDado.str("");
//	ssAuxDado<<"Dado: "<<j->dado;
//	dadoTexto->escrever(gameRenderer,ssAuxDado.str());
}

void Jogo::matarPeca(Peca* p){
	if(p->ressurreicao){
		ssAux<<p->nome<<" tem a bênção da ressurreição, ";
		//verifica se a casa à frente do rei da peça atacada está vazia
		CasaAlvo destino = {tabPrincipal, p->jog->colunaSecundaria, p->jog->general[0]->fila, acaoBen};
		if(obterPecaEm(&destino) == NULL || estaoNoMesmoLugar(p, &destino)){
			ssAux<<"por isso, volta ao ponto de retorno.\n";
			moverPecaPara(p, &destino);
		} else {
			ssAux<<"mas o ponto de retorno está ocupado.\n";
			voltarParaReserva(p);
		}
	} else
		voltarParaReserva(p);
}

void Jogo::voltarParaReserva(Peca* p){
	//ssAux.str("");
	ssAux<<p->nome<<" em "<<p->coluna<<","<<p->fila<<" volta para reserva.\n";
//	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//	cout<<ssAux.str();
//	ssAux.str(""); //qd uma peça atacava, aparecia 2x a msg d defesa da peça alvo, por isso colokei isso aki
	
	int colunaDestino,filaDestino;
	if(p->tipo == peao){
		if(p->coluna != p->jog->oponente->colunaDosGenerais)
			p->jog->lapidesNoTurno.push_back(CasaAlvo{p->tab,p->coluna,p->fila,acaoCar});
		p->cor == branco ? colunaDestino = 1 : colunaDestino = 0;
		for(filaDestino=0; filaDestino < p->jog->reserva->nFilas; filaDestino++)
			if(obterPecaEm(p->jog->reserva, colunaDestino, filaDestino) == NULL){
				moverPecaPara(p, p->jog->reserva, colunaDestino, filaDestino);
				break;
			}
	} else {
		p->cor == branco ? colunaDestino = 0 : colunaDestino = 1;
		if(p->tipo == rainha)
			moverPecaPara(p,p->jog->reserva,colunaDestino,1);
		for(int i=2; i<=4; i++)
			if(i == (int)p->tipo){
				if(obterPecaEm(p->jog->reserva, colunaDestino, i*2-2) == NULL)
					moverPecaPara(p,p->jog->reserva,colunaDestino,i*2-2);
				else
					moverPecaPara(p,p->jog->reserva,colunaDestino,i*2-1);
			}
	}
	p->redefinirAtributos();
}

void Jogo::removerBonusPorCartas(){
	//acoesPorTurno--;
	//if(!naFaseInicial){
	//	ssAux.str("");
		//ssAux<<"Ações restantes: "<<acoesPorTurno<<".\n";
	//	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
	//	cout<<ssAux.str();
	//}
//	cout<<"Ações restantes: "<<acoesPorTurno<<".\n";
	
//	jogador[0]->ultimaUnidadeQueFalhouNumAtaque = NULL;
//	jogador[1]->ultimaUnidadeQueFalhouNumAtaque = NULL;
	//remover bônus de cartas
	for(int j=0; j<2; j++)
		for(int i=0; i<16; i++)
			jogador[j]->todasAsPecas[i]->bonusPorCartas.clear();
}

void Jogo::checarPassagemDeTurno(){
	if(acoesPorTurno == 0 && !jogador[0]->promovendoPeao && !jogador[1]->promovendoPeao /*&& !posAcao */&& !pilhaEmAcao){
		turno++;
		jogadorDaVez = jogadorDaVez->oponente;
		if(!naFaseInicial){
			ssAux.str(""); //turno++ ants d escrever, pq turno 0 == 1º turno
			ssAux<<"Fim do turno "<<turno<<".\n\n";
			ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
			cout<<ssAux.str();
		}
		
		casasAlvo.clear(); //pra remover uma piscada das peças selecionáveis do jogador q tá passando o turno
		jogador[0]->ultimaUnidadeQueFalhouNumAtaque = NULL;
		jogador[1]->ultimaUnidadeQueFalhouNumAtaque = NULL;
		jogador[0]->lapidesNoTurno.clear();
		jogador[1]->lapidesNoTurno.clear();
		//descongelar
		for(int j=0; j<2; j++)
			for(int i=0; i<16; i++)
				jogador[j]->todasAsPecas[i]->congelada = false;
		
		if(naFaseInicial && turno >= 6){
			turno = 0;
			naFaseInicial = false;
			ssAux.str("");
			ssAux<<"Fim da fase de invocação de generais.\n";
			ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
			cout<<ssAux.str();
			
			for(int j=0; j<2; j++)
				for(int i=0; i<MAXIMO_CARTAS_NA_MAO; i++)
					jogador[j]->puxarCarta();
		}
		if(naFaseInicial)
			acoesPorTurno = 1;
		else {
			acoesPorTurno = 2;
			for(int i=jogadorDaVez->mao.size(); i<MAXIMO_CARTAS_NA_MAO; i++)
				jogadorDaVez->puxarCarta();
		}
	}
	
	if(naFaseInicial){ //aki
		rodadaTexto->escrever(gameRenderer,"Fase de invocação de generais");
		ssAux.str("");
		ssAux<<"Turno: "<<jogadorDaVez->nome;
		turnoTexto->escrever(gameRenderer,ssAux.str());
		acoesTexto->escrever(gameRenderer," ");
	} else {
		ssAux.str("");
		ssAux<<turno/2+1<<"ª rodada";
		rodadaTexto->escrever(gameRenderer,ssAux.str());
		ssAux.str("");
		ssAux<<"Turno: "<<jogadorDaVez->nome;
		turnoTexto->escrever(gameRenderer,ssAux.str());
		ssAux.str("");
		ssAux<<"Ações: "<<acoesPorTurno;
		acoesTexto->escrever(gameRenderer,ssAux.str());
	}
}

void Jogo::vitoria(Jogador* j){
	somConfirmar->tocar();
	somBgm->parar();
	ssAux.str("");
	if(sacrificouRei){
		if(musicaHabilitada)
			somDerrota->tocar();
		ssAux<<j->oponente->nome<<" desistiu...";
	} else {
		if(musicaHabilitada)
			somVitoria->tocar();
		ssAux<<j->oponente->general[0]->nome<<" foi derrotado!";
	}
	ssAux<<" Vitória do "<<j->nome<<"!\n";
	if(sacrificouRei){ //se foi por batalha, executarEfeito já vai fazer isso
		ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
		cout<<ssAux.str();
		mensagemTexto->escrever(gameRenderer,ssAux.str(),true,tabPrincipal->largura);
	}
	fimDeJogo();
}

Tabuleiro::Tabuleiro(int x, int y, int nCols, int nFils){
	posX = x;
	posY = y;
	nColunas = nCols;
	nFilas = nFils;
	tex = new Textura();
}

void Tabuleiro::renderizar(){
	if(tex->tex == NULL){
		SDL_Rect casa = {posX,posY,TAMANHO_ESPACO,TAMANHO_ESPACO};
		
		for (int i=0; i<nColunas; i++){
			for (int j=0; j<nFilas; j++){
				if((i+j)%2==0) //alternância das cores escura e clara
					SDL_SetRenderDrawColor(gameRenderer,corCasasEscurasPadrao.r,corCasasEscurasPadrao.g,corCasasEscurasPadrao.b,255);
				else
					SDL_SetRenderDrawColor(gameRenderer,corCasasClarasPadrao.r,corCasasClarasPadrao.g,corCasasClarasPadrao.b,255);
				SDL_RenderFillRect(gameRenderer,&casa);
				casa.y+=TAMANHO_ESPACO;
			}
			casa.x+=TAMANHO_ESPACO;
			casa.y=posY;
		}
	} else
		tex->renderizar(gameRenderer,posX,posY);
	
	if(exibindoGrade){
		SDL_SetRenderDrawColor(gameRenderer,128,128,128,128);
		for(int i=0; i<nColunas; i++)
			SDL_RenderDrawLine(gameRenderer,posX+i*TAMANHO_ESPACO,posY,posX+i*TAMANHO_ESPACO,posH); //linhas verticais
		for(int i=0; i<nFilas; i++)
			SDL_RenderDrawLine(gameRenderer,posX,posY+i*TAMANHO_ESPACO,posW,posY+i*TAMANHO_ESPACO); //linhas horizontais
		//linhas finais pra fechar
		SDL_RenderDrawLine(gameRenderer,posW,posY,posW,posH);
		SDL_RenderDrawLine(gameRenderer,posX,posH,posW,posH);
	}
	
	if(exibindoCasas){
		SDL_Rect casa = {posX,posY,TAMANHO_ESPACO,TAMANHO_ESPACO};
		SDL_SetRenderDrawColor(gameRenderer,192,192,192,64);
		for (int i=0; i<nColunas; i++){
			for (int j=0; j<nFilas; j++){
				if((i+j)%2==0) //só pinta as casas escuras
					SDL_RenderFillRect(gameRenderer,&casa);
				casa.y+=TAMANHO_ESPACO;
			}
			casa.x+=TAMANHO_ESPACO;
			casa.y=posY;
		}
	}
}

Cursor::Cursor(){
	posX = posY = 0;
	coluna = fila = 0;
	tex = new Textura();
}

void Cursor::renderizar(){
	if(tex->tex == NULL){
		SDL_SetRenderDrawColor(gameRenderer,corCursorPadrao.r,corCursorPadrao.g,corCursorPadrao.b,OPAC_CURSOR);
		SDL_Rect quadr;
		for(int i=0; i<CURSOR_PADRAO_ESPESSURA; i++){
			quadr = {posX+i,posY+i,TAMANHO_ESPACO-i*2,TAMANHO_ESPACO-i*2};
			SDL_RenderDrawRect(gameRenderer,&quadr);
		}
	} else
		tex->renderizar(gameRenderer,tab->posX+coluna*TAMANHO_ESPACO,tab->posY+fila*TAMANHO_ESPACO);
}

PecaUnidade::PecaUnidade(Jogador* j){
	jog = j;
	cor = j->cor;
	classe = classeUnidade;
	tipo = peao;
	tab = j->reserva;
	coluna = fila = 0;
	redefinirAtributos();
//	ataque = UNI_ATQ;
//	defesa = UNI_DEF;
//	movimento = UNI_MOV;
//	alcance = UNI_ALC;
	congelada = ressurreicao = false;
	
	nome = "Peão";
	sufixoGenero = "o";
	if(cor == branco) nome += " branc"+sufixoGenero;
	if(cor == preto) nome += " pret"+sufixoGenero;
	
	tex = new Textura();
	texNome = new Texto();
}

PecaGeneral::PecaGeneral(Jogador* j,tipoPeca t){
	jog = j;
	cor = j->cor;
	if(t == rei)
		classe = classeNucleo;
	else
		classe = classeGeneral;
	tipo = t;
	tab = j->reserva;
	coluna = fila = 0;
	redefinirAtributos();
//	ataque = GEN_ATQ;
//	defesa = GEN_DEF;
//	movimento = 0;
//	alcance = GEN_ALC;
	congelada = ressurreicao = false;
	
	if(tipo == rei)  nome = "Rei";
	if(tipo == rainha)  nome = "Rainha";
	if(tipo == bispo)  nome = "Bispo";
	if(tipo == cavalo)  nome = "Cavalo";
	if(tipo == torre)  nome = "Torre";
	
	if(cor == branco) nome += " branc";
	if(cor == preto) nome += " pret";
	
	if(tipo == rainha || tipo == torre) sufixoGenero = "a";
	else sufixoGenero = "o";
	
	nome += sufixoGenero;
	
	tex = new Textura();
	texNome = new Texto();
}

void Peca::renderizar(){
	if(tex->largura > 0)
		tex->renderizar(gameRenderer, tab->posX + coluna*TAMANHO_ESPACO + (TAMANHO_ESPACO-tex->largura)/2,
										tab->posY + fila*TAMANHO_ESPACO + TAMANHO_ESPACO-tex->altura);
	else
		texNome->renderizar(gameRenderer,tab->posX+coluna*TAMANHO_ESPACO+TAMANHO_ESPACO/8,
												 tab->posY+fila*TAMANHO_ESPACO+(TAMANHO_ESPACO-texNome->altura)/2);
}

void Peca::redefinirAtributos(){
	if(tipo == peao){
		ataque = UNI_ATQ;
		defesa = UNI_DEF;
		movimento = UNI_MOV;
		alcance = UNI_ALC;
	} else {
		ataque = GEN_ATQ;
		defesa = GEN_DEF;
		movimento = 0;
		alcance = GEN_ALC;
	}
	bonus.clear();
}

void PecaGeneral::abencoarPeao(Peca* p){
	if(tipo == rei) p->ressurreicao = true;
	if(p->congelada) return;
	if(tipo == rainha) p->bonus.push_back(bAtq);
	if(tipo == bispo) p->bonus.push_back(bAlc);
	if(tipo == cavalo) p->bonus.push_back(bMov);
	if(tipo == torre) p->bonus.push_back(bDef);
	p->casasBencao.push_back(CasaAlvo{tab,coluna,fila,acaoBen});
}

void CopiaTransparenteDaImagemDaPecaSelecionada::renderizar(){
	if(tex->largura > 0){
		SDL_SetTextureAlphaMod(tex->tex, 127);
		tex->renderizar(gameRenderer, tab->posX + coluna*TAMANHO_ESPACO + (TAMANHO_ESPACO-tex->largura)/2,
										tab->posY + fila*TAMANHO_ESPACO + TAMANHO_ESPACO-tex->altura);
		SDL_SetTextureAlphaMod(tex->tex, 255);
	} else
		texNome->renderizar(gameRenderer,tab->posX+coluna*TAMANHO_ESPACO+TAMANHO_ESPACO/8,
												 tab->posY+fila*TAMANHO_ESPACO+(TAMANHO_ESPACO-texNome->altura)*2);
}

Carta::Carta(tipoCarta t, cartaNaipe n, Jogador* j){
	jog = j;
	cor = j->cor;
	tipo = t;
	localC = locBaralho;
	string sb[13] = { "A","2","3","4","5","6","7","8","9","10","J","Q","K" };
	simbolo = sb[(int)t];
	//string np[4] = { "\x6","\x5","\x4","\x3" }; //SDL só desenhou quadrados :')
	string np[4] = { "E","P","O","C" };
	naipe = np[(int)n];
	string nm[13] = {"Ataque direto","Contra-Ataque Fatal","Aumentar Ataque","Aumentar Defesa","Aumentar Movimento","Aumentar Alcance",
						"Recrutar Unidade","Mover Unidade","Mover General","Reviver Unidade","Anular Carta","Congelar Peça","Ataque Extra"};
	nome = nm[(int)t];
	string dc[13] = { "Ataca uma unidade em campo (Atq: 4).",
		"Mata uma unidade que foi mal sucedida num ataque (só pode ser usada imediatamente após o ataque).",
		"Aumenta em 1 o ataque da peça atacante (só pode ser usada durante um ataque).",
		"Aumenta em 1 a defesa da peça atacada (só pode ser usada durante um ataque).",
		"Aumenta em 1 o movimento de uma unidade em campo.",
		"Aumenta em 1 o alcance de uma peça em campo.",
		"Invoca uma unidade sua no campo.",
		"Move uma unidade em campo para uma casa vizinha (em qualquer direção).",
		"Move um general seu.",
		"Revive uma unidade sua que foi morta neste turno.",
		"Anula a ativação de uma carta.",
		"Congela uma peça em campo (exceto rei), impedindo-a de agir e de conceder ou receber bênçãos (exceto ressurreição).",
		"Ataca com uma peça." };
	descricao = dc[(int)t];
	selecionavel = false;
	revelada = false;
	ativada = false;
	anulada = false;
	posX = posY = 0;
	tex = new Textura();
	texVerso = new Textura();
	texTextoNaipe = new Texto();
	texTextoSimbolo = new Texto();
	texTextoNome = new Texto();
}

void Carta::renderizar(){
	SDL_Rect quadr = {posX,posY,largura,altura};
	if(!revelada && !ativada && !anulada){
		if(texVerso->tex == NULL){
			SDL_SetRenderDrawColor(gameRenderer,0,0,0,255);
			SDL_RenderDrawRect(gameRenderer,&quadr);
			
			SDL_Rect borda;
			SDL_SetRenderDrawColor(gameRenderer,corCartasVersoBorda.r,corCartasVersoBorda.g,corCartasVersoBorda.b,255);
			for(int i=1; i<largura/16; i++){ //tamanho da borda referente à largura
				borda = {posX+i,posY+i,largura-i*2,altura-i*2};
				SDL_RenderDrawRect(gameRenderer,&borda);
			}
			
			if(jog->cor == branco)
				SDL_SetRenderDrawColor(gameRenderer,corCartasVersoVermelhas.r,corCartasVersoVermelhas.g,corCartasVersoVermelhas.b,255);
			if(jog->cor == preto)
				SDL_SetRenderDrawColor(gameRenderer,corCartasVersoPretas.r,corCartasVersoPretas.g,corCartasVersoPretas.b,255);
			SDL_Rect carta = {posX+largura/16,posY+largura/16,largura-largura/8,altura-largura/8}; //tamanho da borda referente à largura
			SDL_RenderFillRect(gameRenderer,&carta);
		} else
			texVerso->renderizar(gameRenderer,posX,posY);
	} else if(tex->tex == NULL){
		SDL_SetRenderDrawColor(gameRenderer,0,0,0,255);
		SDL_RenderDrawRect(gameRenderer,&quadr);
		
		SDL_SetRenderDrawColor(gameRenderer,255,255,255,255);
		SDL_Rect carta = {posX+1,posY+1,largura-2,altura-2};
		SDL_RenderFillRect(gameRenderer,&carta);
		
		texTextoSimbolo->renderizar(gameRenderer,posX+largura/16,posY+largura/16);
		texTextoNaipe->renderizar(gameRenderer,posX+largura/16,posY+largura/16+texTextoSimbolo->altura);
		texTextoNome->renderizar(gameRenderer,posX+largura/8,posY+(altura-texTextoNome->altura)*3/4);
	} else
		tex->renderizar(gameRenderer,posX,posY);
	
	if(ativada){
		SDL_SetRenderDrawColor(gameRenderer,0,255,255,128);
		SDL_RenderFillRect(gameRenderer,&quadr);
	} else if(localC == locMao && revelada && !selecionavel){
		SDL_SetRenderDrawColor(gameRenderer,128,128,128,128);
		SDL_RenderFillRect(gameRenderer,&quadr);
	}
}

void CasaAlvo::renderizar(){
	renderizar(OPAC_CASAS_ALVO);
}
void CasaAlvo::renderizar(int opacidade){
	if(exibindoAreasSimplificadas) opacidade*=2;
	
//	if(tipo==acaoSel) SDL_SetRenderDrawColor(gameRenderer,corAcaoSelecao.r,corAcaoSelecao.g,corAcaoSelecao.b,opacidade);
//	if(tipo==acaoInv) SDL_SetRenderDrawColor(gameRenderer,corAcaoInvocacao.r,corAcaoInvocacao.g,corAcaoInvocacao.b,opacidade);
//	if(tipo==acaoMov) SDL_SetRenderDrawColor(gameRenderer,corAcaoMovimento.r,corAcaoMovimento.g,corAcaoMovimento.b,opacidade);
//	if(tipo==acaoAtq) SDL_SetRenderDrawColor(gameRenderer,corAcaoAtaque.r,corAcaoAtaque.g,corAcaoAtaque.b,opacidade);
//	if(tipo==acaoBen) SDL_SetRenderDrawColor(gameRenderer,corAcaoBencao.r,corAcaoBencao.g,corAcaoBencao.b,opacidade);
//	if(tipo==acaoCar) SDL_SetRenderDrawColor(gameRenderer,corAcaoCarta.r,corAcaoCarta.g,corAcaoCarta.b,opacidade);
	SDL_SetRenderDrawColor(gameRenderer,corAcao[(int)tipo].r,corAcao[(int)tipo].g,corAcao[(int)tipo].b,opacidade);
	
	SDL_Rect quadr;
	if(exibindoAreasSimplificadas){
		for(int i=0; i<CURSOR_PADRAO_ESPESSURA; i++){
			quadr = {tab->posX+coluna*TAMANHO_ESPACO+i,tab->posY+fila*TAMANHO_ESPACO+i,TAMANHO_ESPACO-i*2,TAMANHO_ESPACO-i*2};
			SDL_RenderDrawRect(gameRenderer,&quadr);
		}
	} else {
		quadr = {tab->posX+coluna*TAMANHO_ESPACO,tab->posY+fila*TAMANHO_ESPACO,TAMANHO_ESPACO,TAMANHO_ESPACO};
		SDL_RenderFillRect(gameRenderer,&quadr);
	}
}

Jogador::Jogador(corJogador c, Tabuleiro* t){
	cor = c;
	campo = t;
	dado = 0;
	ultimaUnidadeQueFalhouNumAtaque = NULL;
	if(cor==branco){
		nome = "Jogador branco";
		colunaDosGenerais = 0;
		colunaSecundaria = 1;
		reserva = new Tabuleiro(TAB_RES_BRANCO_POSX, TAB_RES_BRANCO_POSY, TAB_RES_QTDE_COLUNAS, TAB_RES_QTDE_FILAS);
		reserva->nome = "reserva branco";
	}
	if(cor==preto){
		nome = "Jogador preto";
		colunaDosGenerais = t->nColunas-1;
		colunaSecundaria = t->nColunas-2;
		reserva = new Tabuleiro(TAB_RES_PRETO_POSX, TAB_RES_PRETO_POSY, TAB_RES_QTDE_COLUNAS, TAB_RES_QTDE_FILAS);
		reserva->nome = "reserva preto";
	}
}

void Jogador::reembaralhar(Peca* genSacrificio){
	if(genSacrificio == NULL){
		//retorna as cartas da mão pro baralho
		for(unsigned int i=0; i<mao.size(); i++)
			baralho.push_back(mao[i]);
		mao.clear();
	} else {
		ssAux.str("");
//		ssAux<<"Jogador ";
//		if(genSacrificio->jog->cor == branco) ssAux<<"branco";
//		if(genSacrificio->jog->cor == preto) ssAux<<"preto";
		ssAux<<genSacrificio->jog->nome<<" sacrifica um general para retornar os descartes para o baralho e reembaralhar.\n";
		ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
		cout<<ssAux.str();
	}
	
	//retorna as cartas dos descartes pro baralho
	for(unsigned int i=0; i<descartes.size(); i++)
		baralho.push_back(descartes[i]);
	descartes.clear();
	
	//reconfigura as cartas no baralho
	for(unsigned int i=0; i<baralho.size(); i++){
		baralho[i]->localC = locBaralho;
		baralho[i]->selecionavel = false;
		baralho[i]->ativada = false;
		baralho[i]->anulada = false;
		baralho[i]->revelada = false;
	}
	
	//embaralha
	int totalCartas = baralho.size();
	vector<Carta*> embaralhar = baralho;
	baralho.clear();
	int cartaAleatoria;
	
	for(int i=0; i<totalCartas; i++){
		cartaAleatoria = rand()%embaralhar.size();
		baralho.push_back(embaralhar[cartaAleatoria]);
		embaralhar.erase(embaralhar.begin()+cartaAleatoria);
	}
	
	//reposiciona
	for(unsigned int i=0; i<baralho.size(); i++){
		if(cor == branco){
			baralho[i]->posX = BARALHO_BRANCO_POSX;
			baralho[i]->posY = BARALHO_BRANCO_POSY-i*2; //-i*2 pra fazer o monte
		} else {
			baralho[i]->posX = BARALHO_PRETO_POSX;
			baralho[i]->posY = BARALHO_PRETO_POSY-i*2; //-i*2 pra fazer o monte
		}
	}
}

void Jogador::puxarCarta(){
	if(baralho.size()>0 && mao.size() < 3){
		mao.push_back(baralho.back());
		baralho.pop_back();
		mao.back()->localC = locMao;
		if(exibindoCartasDasMaos)
			mao.back()->revelada = true;
		alinharCartasDaMao();
		
		ssAux.str("");
//		ssAux<<"Jogador ";
//		if(cor == branco) ssAux<<"branco";
//		if(cor == preto) ssAux<<"preto";
		ssAux<<nome<<" puxa ";
		cout<<ssAux.str();
		
		if(exibindoCartasDasMaos)
			cout<<mao.back()->nome<<".\n";
		else
			cout<<"uma carta.\n";
		
		ssAux<<mao.back()->nome<<".\n";
		ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
	}
}

void Jogador::descartar(Carta* c){
	if(c->localC == locMao){
		c->ativada = false;
		c->anulada = false;
		c->revelada = true;
		c->selecionavel= false;
		c->localC = locDescartes;
		descartes.push_back(c);
		for(unsigned int i=0; i<mao.size(); i++)
			if(mao[i] == c) mao.erase(mao.begin()+i);
		
		if(cor == branco){
			descartes.back()->posX = DESCARTES_BRANCO_POSX;
			descartes.back()->posY = DESCARTES_BRANCO_POSY-(descartes.size()-1)*2; //pra fazer o monte
		} else {
			descartes.back()->posX = DESCARTES_PRETO_POSX;
			descartes.back()->posY = DESCARTES_PRETO_POSY-(descartes.size()-1)*2; //pra fazer o monte
		}
		alinharCartasDaMao();
		
		ssAux.str("");
//		ssAux<<"Jogador ";
//		if(cor == branco) ssAux<<"branco";
//		if(cor == preto) ssAux<<"preto";
		ssAux<<nome<<" descarta "<<c->nome<<".\n";
		ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
		cout<<ssAux.str();
		ssAux.str("");
	}
}

void Jogador::alinharCartasDaMao(){
	for(unsigned int i=0; i<mao.size(); i++){
		if(cor == branco){ //distância entre as cartas da mão, tanto vertical qt horizontal, com base na largura
			mao[i]->posX = CARTAS_MAO_BRANCO_POSX - (mao.size()-1)*mao[i]->largura/8 + i*mao[i]->largura/4;
			mao[i]->posY = CARTAS_MAO_BRANCO_POSY + (mao.size()-1)*mao[i]->largura/8 - i*mao[i]->largura/4;
		} else {
			mao[i]->posX = CARTAS_MAO_PRETO_POSX - (mao.size()-1)*mao[i]->largura/8 + i*mao[i]->largura/4;
			mao[i]->posY = CARTAS_MAO_PRETO_POSY + (mao.size()-1)*mao[i]->largura/8 - i*mao[i]->largura/4;
		}
	}
}

Efeito::Efeito(Jogador* j, Peca* p, CasaAlvo* ca){ //movimento, invocação
	jog=j;
	peca=p;
	casa=*ca;
	carta=NULL;
	pecaAlvo=NULL;
	cartaAlvo=NULL;
	
//	ssAux.str("");
//	ssAux<<"Jogador ";
//	if(jog->cor == branco) ssAux<<"branco";
//	if(jog->cor == preto) ssAux<<"preto";
	ssAux<<jog->nome;
	if(casa.tipo == acaoMov) ssAux<<" move "<<peca->nome<<" em "<<peca->coluna<<","<<peca->fila<<" para ";
	if(casa.tipo == acaoInv) ssAux<<" invoca "<<peca->nome<<" em ";
	ssAux<<casa.coluna<<","<<casa.fila<<".\n";
//	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//	cout<<ssAux.str();
	descricao = ssAux.str();
}
Efeito::Efeito(Jogador* j, Peca* p, CasaAlvo* ca, Peca* pa, bool defendendo){ //ataque, trocandoGeneraisDeLugarComAcao, defesa contra peças
	jog=j;
	peca=p;
	casa=*ca;
	carta=NULL;
	pecaAlvo=pa;
	cartaAlvo=NULL;
	
//	ssAux.str("");
	if(!defendendo){
//		ssAux<<"Jogador ";
//		if(jog->cor == branco) ssAux<<"branco";
//		if(jog->cor == preto) ssAux<<"preto";
		ssAux<<jog->nome;
		if(casa.tipo == acaoMov)
			ssAux<<" troca o lugar de "<<peca->nome<<" em "<<peca->coluna<<","<<peca->fila<<" com "
					<<pecaAlvo->nome<<" em "<<pecaAlvo->coluna<<","<<pecaAlvo->fila<<".\n";
		if(casa.tipo == acaoAtq)
			ssAux<<" ataca "<<pecaAlvo->nome<<" em "<<pecaAlvo->coluna<<","<<pecaAlvo->fila<<" com "
					<<peca->nome<<" em "<<peca->coluna<<","<<peca->fila<<".\n";
	} else {
//		ssAux<<pecaAlvo->nome<<" em "<<pecaAlvo->coluna<<","<<pecaAlvo->fila<<" está sendo atacado por "
//				<<peca->nome<<" em "<<peca->coluna<<","<<peca->fila<<".\n";
	}
//	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//	cout<<ssAux.str();
//	ssAux.str(""); //qd uma peça atacava, aparecia 2x a msg d defesa da peça alvo, por isso colokei isso aki
	descricao = ssAux.str();
}
Efeito::Efeito(Jogador* j, Carta* c, CasaAlvo* ca){ //recUni, revUni
	jog=j;
	peca=NULL;
	casa=*ca;
	carta=c;
	pecaAlvo=NULL;
	cartaAlvo=NULL;
	
//	ssAux.str("");
//	ssAux<<"Jogador ";
//	if(jog->cor == branco) ssAux<<"branco";
//	if(jog->cor == preto) ssAux<<"preto";
	ssAux<<jog->nome<<" usa "<<carta->nome<<" em "<<casa.coluna<<","<<casa.fila<<".\n";
//	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//	cout<<ssAux.str();
	descricao = ssAux.str();
}
Efeito::Efeito(Jogador* j, Carta* c, Peca* p, CasaAlvo* ca){ //movUni, movGen sem troca
	jog=j;
	peca=p;
	casa=*ca;
	carta=c;
	pecaAlvo=NULL;
	cartaAlvo=NULL;
	
//	ssAux.str("");
//	ssAux<<"Jogador ";
//	if(jog->cor == branco) ssAux<<"branco";
//	if(jog->cor == preto) ssAux<<"preto";
	ssAux<<jog->nome<<" usa "<<carta->nome<<" para mover "<<peca->nome<<" em "<<peca->coluna<<","<<peca->fila<<" para "
			<<casa.coluna<<","<<casa.fila<<".\n";
//	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//	cout<<ssAux.str();
	descricao = ssAux.str();
}
Efeito::Efeito(Jogador* j, Carta* c, Peca* p, CasaAlvo* ca, Peca* pa){ //atqExt, movGen com troca
	jog=j;
	peca=p;
	casa=*ca;
	carta=c;
	pecaAlvo=pa;
	cartaAlvo=NULL;
	
//	ssAux.str("");
//	ssAux<<"Jogador ";
//	if(jog->cor == branco) ssAux<<"branco";
//	if(jog->cor == preto) ssAux<<"preto";
	ssAux<<jog->nome<<" usa "<<carta->nome;
	if(casa.tipo == acaoMov)
		ssAux<<" para trocar o lugar de "<<peca->nome<<" em "<<peca->coluna<<","<<peca->fila<<" com "
				<<pecaAlvo->nome<<" em "<<pecaAlvo->coluna<<","<<pecaAlvo->fila<<".\n";
	if(casa.tipo == acaoAtq)
		ssAux<<" para atacar "<<pecaAlvo->nome<<" em "<<pecaAlvo->coluna<<","<<pecaAlvo->fila<<" com "
				<<peca->nome<<" em "<<peca->coluna<<","<<peca->fila<<".\n";
//	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//	cout<<ssAux.str();
	descricao = ssAux.str();
}
Efeito::Efeito(Jogador* j, Carta* c, Carta* c2, CasaAlvo* ca){ //anuCar
	jog=j;
	peca=NULL;
	casa=CasaAlvo{j->reserva,0,0,acaoCar};;
	carta=c;
	pecaAlvo=NULL;
	cartaAlvo=c2;
	
//	ssAux.str("");
//	ssAux<<"Jogador ";
//	if(jog->cor == branco) ssAux<<"branco";
//	if(jog->cor == preto) ssAux<<"preto";
	ssAux<<jog->nome<<" usa "<<carta->nome<<" em "<<cartaAlvo->nome<<" (jogador ";
	if(cartaAlvo->jog->cor == branco) ssAux<<"branco";
	if(cartaAlvo->jog->cor == preto) ssAux<<"preto";
	ssAux<<").\n";
//	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//	cout<<ssAux.str();
	descricao = ssAux.str();
}
Efeito::Efeito(Jogador* j, Carta* c, CasaAlvo* ca, Peca* pa, bool defendendo){ //demais cartas e defesa contra ás
	jog=j;
	peca=NULL;
	casa=*ca;
	carta=c;
	pecaAlvo=pa;
	cartaAlvo=NULL;
	
//	ssAux.str("");
	if(!defendendo){
//		ssAux<<"Jogador ";
//		if(jog->cor == branco) ssAux<<"branco";
//		if(jog->cor == preto) ssAux<<"preto";
		ssAux<<jog->nome<<" usa "<<carta->nome<<" em "<<pecaAlvo->nome<<" em "<<pecaAlvo->coluna<<","<<pecaAlvo->fila<<".\n";
	} else {
		//ssAux<<pecaAlvo->nome<<" em "<<pecaAlvo->coluna<<","<<pecaAlvo->fila<<" está sendo atacado por "<<carta->nome
		//<<" de "<<jog->nome<<".\n";
//		<<" do jogador ";
//		if(jog->cor == branco) ssAux<<"branco";
//		if(jog->cor == preto) ssAux<<"preto";
//		ssAux<<".\n";
	}
//	ssRegistro<<ssAux.str();ofsRegistro<<ssAux.str();
//	cout<<ssAux.str();
//	ssAux.str(""); //qd o jogador atacava com ás, aparecia 2x a msg d defesa da peça alvo, por isso colokei isso aki
	descricao = ssAux.str();
}
Efeito::~Efeito(){
	jog=NULL;
	peca=NULL;
	//casa=; //é objeto, não ponteiro
	carta=NULL;
	pecaAlvo=NULL;
	cartaAlvo=NULL;
}
