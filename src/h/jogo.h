#include "janela.h"
#include "textura.h"
#include "som.h"
#include <vector>
#include <sstream>

//constantes
const int FPS = 30;
const int TICKS_POR_FRAME = 1000 / FPS;
const int AMOSTRAS_FPS = 5;
const float ESCALA = 1;
const int TAMANHO_ESPACO = 64 * ESCALA;
const int JANELA_LARGURA = TAMANHO_ESPACO*20;
const int JANELA_ALTURA = TAMANHO_ESPACO*10;
//const int TAMANHO_FONTE_TITULO = TAMANHO_ESPACO/2;
const int TAMANHO_FONTE_TEMPO = TAMANHO_ESPACO/4;
const int TAMANHO_FONTE_DEBUG = TAMANHO_ESPACO/6;

const int TABULEIRO_POSX = TAMANHO_ESPACO*6;
const int TABULEIRO_POSY = TAMANHO_ESPACO*1;
const int TAB_QTDE_COLUNAS = 8;
const int TAB_QTDE_FILAS = 8;
const int TAB_RES_QTDE_COLUNAS = 2;
const int TAB_RES_QTDE_FILAS = TAB_QTDE_FILAS;
const int TAB_RES_BRANCO_POSX = TABULEIRO_POSX - TAB_RES_QTDE_COLUNAS*TAMANHO_ESPACO - TAMANHO_ESPACO;
const int TAB_RES_BRANCO_POSY = TABULEIRO_POSY;
const int TAB_RES_PRETO_POSX = TABULEIRO_POSX + TAB_QTDE_COLUNAS*TAMANHO_ESPACO + TAMANHO_ESPACO;
const int TAB_RES_PRETO_POSY = TABULEIRO_POSY;

const int TAMANHO_PADRAO_FONTE_PECAS_LETRA = TAMANHO_ESPACO;
const int TAMANHO_PADRAO_FONTE_PECAS_NOME = TAMANHO_ESPACO*0.23;

//const float SCALING_ADICIONAL_ARQUIVO_CARTAS = 0.81;
//const int LARGURA_PADRAO_CARTAS = TAMANHO_ESPACO*1.75;
//const int ALTURA_PADRAO_CARTAS = TAMANHO_ESPACO*2.4375;
const int ALTURA_PADRAO_CARTAS = TAMANHO_ESPACO*2;
const int LARGURA_PADRAO_CARTAS = ALTURA_PADRAO_CARTAS*0.75;
const int TAMANHO_PADRAO_FONTE_CARTAS_SIMBOLO = TAMANHO_ESPACO*0.4;
const int TAMANHO_PADRAO_FONTE_CARTAS_NOME = LARGURA_PADRAO_CARTAS*0.15;
const int QTDE_CARTAS_CADA_BARALHO = 26;
const int MAXIMO_CARTAS_NA_MAO = 3;
const int ESPACO_AO_REDOR_DAS_CARTAS = 5; //pixels
const int CARTAS_MAO_BRANCO_POSX = TAB_RES_BRANCO_POSX - LARGURA_PADRAO_CARTAS*1.25 - ESPACO_AO_REDOR_DAS_CARTAS;
const int CARTAS_MAO_BRANCO_POSY = TABULEIRO_POSY + ALTURA_PADRAO_CARTAS*1.25;//(TAB_QTDE_FILAS*TAMANHO_ESPACO - ALTURA_PADRAO_CARTAS)/2;
const int CARTAS_MAO_PRETO_POSX = TAB_RES_PRETO_POSX + TAB_RES_QTDE_COLUNAS*TAMANHO_ESPACO + LARGURA_PADRAO_CARTAS*0.25 + ESPACO_AO_REDOR_DAS_CARTAS;
const int CARTAS_MAO_PRETO_POSY = CARTAS_MAO_BRANCO_POSY;
const int BARALHO_BRANCO_POSX = CARTAS_MAO_BRANCO_POSX;
const int BARALHO_BRANCO_POSY = TABULEIRO_POSY + TAB_QTDE_FILAS*TAMANHO_ESPACO - ALTURA_PADRAO_CARTAS;
const int BARALHO_PRETO_POSX = CARTAS_MAO_PRETO_POSX;
const int BARALHO_PRETO_POSY = BARALHO_BRANCO_POSY;
const int DESCARTES_BRANCO_POSX = CARTAS_MAO_BRANCO_POSX;
const int DESCARTES_BRANCO_POSY = TABULEIRO_POSY;
const int DESCARTES_PRETO_POSX = CARTAS_MAO_PRETO_POSX;
const int DESCARTES_PRETO_POSY = DESCARTES_BRANCO_POSY;

const int CURSOR_PADRAO_ESPESSURA = 5; //pixels
const int OPAC_CURSOR = 255;
const int OPAC_CASAS_ALVO = 96;
const int OPAC_CASAS_ALCANCE = 48;
const int OPAC_SELECAO_CARTA = 144;
const int UNI_ATQ = 4;
const int UNI_DEF = 0;
const int UNI_MOV = 1;
const int UNI_ALC = 1;
const int GEN_ATQ = 4;
const int GEN_DEF = 1;
const int GEN_ALC = 1;
const int AS_ATQ = 4;

enum direcao { nenhuma,cima,direita,baixo,esquerda };
enum corJogador { branco,preto };
enum tipoPeca { rei,rainha,bispo,cavalo,torre,peao };
enum tipoClasse { classeUnidade,classeGeneral,classeNucleo };
enum tipoAcao { acaoSel,acaoMov,acaoAtq,acaoInv,acaoBen,acaoCar };
enum tipoCarta { atqDir,conAtq,bonAtq,bonDef,bonMov,bonAlc,recUni,movUni,movGen,revUni,anuCar,conPec,atqExt };
enum cartaNaipe { espadas,paus,ouros,copas };
enum tipoBonus { nenhum,bAtq,bDef,bMov,bAlc };
enum localCarta { locBaralho,locMao,locDescartes };

class Tabuleiro {
public:
	Tabuleiro(int x,int y, int nCols, int nFils);
	void renderizar();
	
	int posX,posY,posW,posH;
	int altura,largura;
	int coluna,fila,nColunas,nFilas;
	string nome;
	Textura* tex;
};

class CasaAlvo {
public:
	void renderizar();
	void renderizar(int opacidade);
	
	//int posX,posY;
	Tabuleiro* tab;
	int coluna,fila;
	tipoAcao tipo;
};

class Cursor {
public:
	Cursor();
	void renderizar();
	
	int posX,posY;
	int altura,largura;
	int coluna,fila;
	Tabuleiro* tab;
	bool visivel;
	Textura* tex;
};

class Jogador;

class Peca {
public:
	void renderizar();
	void redefinirAtributos();
	
	tipoClasse classe;
	tipoPeca tipo;
	corJogador cor;
	bool selecionavel;
	//bool emJogo;
	bool congelada;
	bool ressurreicao;
	bool bonusSuporteDef;
	int posX,posY;
	int altura,largura;
	int coluna,fila;
	Tabuleiro* tab;
	Jogador* jog;
	int ataque,defesa,movimento,alcance;
	vector<CasaAlvo> casasBencao;
	vector<tipoBonus> bonus;
	vector<tipoBonus> bonusPorCartas;
	string nome,sufixoGenero;
	Textura* tex;
	Texto* texNome;
};

class PecaUnidade : public Peca {
public:
	PecaUnidade(Jogador* j);
};

class PecaGeneral : public Peca {
public:
	PecaGeneral(Jogador* j,tipoPeca t);
	void abencoarPeao(Peca* p);
};

class CopiaTransparenteDaImagemDaPecaSelecionada : public Peca {
public:
	void renderizar();
	
	bool visivel;
};

class Carta {
public:
	Carta(tipoCarta t, cartaNaipe n, Jogador* j);
	void renderizar();
	
	int posX,posY;
	int altura,largura;
	tipoCarta tipo;
	corJogador cor;
	localCarta localC;
	Jogador* jog;
	string simbolo;
	string naipe;
	string nome;
	string descricao;
	bool selecionavel;
	bool revelada;
	bool ativada;
	bool anulada;
	Textura* tex;
	Textura* texVerso;
	Texto* texTextoSimbolo;
	Texto* texTextoNaipe;
	Texto* texTextoNome;
};

class Jogador {
public:
	Jogador(corJogador c, Tabuleiro* t);
	//~Jogador();
	void reembaralhar(Peca* genSacrificio=NULL);
	void puxarCarta();
	void descartar(Carta* c);
	void alinharCartasDaMao();
	
	bool esperandoConsentimento,consentiu,consentindoAutomaticamente;
	bool promovendoPeao;
	bool trocandoGeneraisDeLugarComAcao;
	bool exibindoCartasDasMaos;
	//bool exibindoDescartes;
	bool usandoMovUni;
	bool usandoMovGen;
	bool usandoAtqExt;
	
	Jogador* oponente; //pra facilitar algumas instruções
	corJogador cor;
	Tabuleiro* campo;
	int colunaDosGenerais,colunaSecundaria;
	Tabuleiro* reserva;
	int dado;
	PecaUnidade* unidade[8];
	PecaGeneral* general[8];
	Peca* todasAsPecas[16]; //endereços dos generais (0-7) e das unidades (8-15), pra facilitar algumas funções
	Peca* ultimaUnidadeQueFalhouNumAtaque;
	//vector<Peca*> unidadesPerdidasNoTurno;
	//vector<CasaAlvo> locaisDeOndeUnidadesRetornaramPraReservaNoTurno;
	vector<CasaAlvo> lapidesNoTurno;
	vector<Carta*> baralho;
	vector<Carta*> descartes;
	vector<Carta*> mao;
	string nome;
};

class Efeito {
public:
	Efeito(Jogador* j, Peca* p, CasaAlvo* ca); //movimento, invocação
	Efeito(Jogador* j, Peca* p, CasaAlvo* ca, Peca* pa, bool defendendo=false); //ataque, trocandoGeneraisDeLugarComAcao, defesa contra peças
	Efeito(Jogador* j, Carta* c, CasaAlvo* ca); //recUni, revUni
	Efeito(Jogador* j, Carta* c, Peca* p, CasaAlvo* ca); //movUni, movGen sem troca
	Efeito(Jogador* j, Carta* c, Peca* p, CasaAlvo* ca, Peca* pa); //atqExt, movGen com troca
	Efeito(Jogador* j, Carta* c, Carta* c2, CasaAlvo* ca); //anuCar
	Efeito(Jogador* j, Carta* c, CasaAlvo* ca, Peca* pa, bool defendendo=false); //demais cartas e defesa contra ás
	~Efeito();
	
	Jogador* jog;
	Peca* peca;
	CasaAlvo casa;
	Carta* carta;
	Peca* pecaAlvo;
	Carta* cartaAlvo;
	string descricao;
};

class Jogo {
public:
	Jogo();
	~Jogo();
	
	bool inicializar();
	bool carregarArquivos();
	void processarEventos();
	void atualizar();
	void renderizar();
//	void terminar();
	
	void debugger();
	//void colocarPecasNasReservas();
	void fimDeJogo();
	void novoJogo();
	void novoJogo2(); //remover
	void rolarDadoInicial();
	void atualizarFps();
	void atualizarTempoDePartida();
	void definirPecasSelecionaveis();
	void definirCartasSelecionaveis();
	void definirCasasSelecionaveis();
	void definirCasasAlcanceEAlvos(Peca* p);
	void redefinirRaiosDeBencao();
	void atribuirRaiosDeBencao(Peca* gen);
	void concederBonusPorBencaos();
	void concederBonusPorSuporte();
	void aplicarBonus();
	void moverCursorPara(int x, int y, direcao d=nenhuma);
	void moverCursorPara(direcao dir, bool ateOLimite=false);
	Peca* obterPecaEm(Tabuleiro* t, int col, int fil);
	Peca* obterPecaEm(Cursor* c);
	Peca* obterPecaEm(CasaAlvo* ca);
	Carta* obterCartaEm(int x, int y);
	Carta* obterCartaEm(Cursor* c);
	CasaAlvo* obterCasaAlvoEm(Tabuleiro* t, int col, int fil);
	CasaAlvo* obterCasaAlvoEm(Cursor* c);
	CasaAlvo* obterCasaAlvoEm(CasaAlvo* ca);
	bool estaoNoMesmoLugar(Cursor* c, CasaAlvo* ca);
	bool estaoNoMesmoLugar(Peca* p, CasaAlvo* ca);
	bool estaoNoMesmoLugar(CasaAlvo* ca, CasaAlvo* ca2);
	bool estaDentroDoTabuleiro(Tabuleiro* tab, CasaAlvo* c);
	Tabuleiro* estaDentroDeQualTabuleiro(int x, int y);
	void selecionar();
	void cancelarSelecao();
	void selecionarPeca(Peca* peca);
	void selecionarPecaIntermediariaSobCursor();
	void selecionarCarta(Carta* carta);
	void olharDescartes(Jogador* j);
	void guardarDescartes();
	void adicionarEfeitoNaPilhaNoCursor(Carta* cartaAnulada=NULL);
	void checarPilha();
	void executarEfeito(Efeito e);
	void moverPecaPara(Peca* p, Tabuleiro* t, int col, int fil);
	void moverPecaPara(Peca* p, Cursor* c);
	void moverPecaPara(Peca* p, Peca* pd);
	void moverPecaPara(Peca* p, CasaAlvo* ca);
	void atacarPeca(Peca* atacante, Peca* alvo);
	void atacarPeca(Carta* as, Peca* alvo);
	void executarAtaque(Jogador* j,Peca* alvo);
	void matarPeca(Peca* p);
	void voltarParaReserva(Peca* peca);
	void removerBonusPorCartas();
	void checarPassagemDeTurno();
	void vitoria(Jogador* j);
	
	Janela* janela;
	Textura* fundo;
	float fundoPosX;
	float fundoPosY;
	float fundoVelH;
	float fundoVelV;
	
	//Texto* tituloTexto;
	Texto* debugTexto;
	Texto* debug2Texto;
	//Texto* fpsAtualTexto;
	Texto* rodadaTexto;
	Texto* tempoDePartidaTexto;
	Texto* turnoTexto;
	Texto* acoesTexto;
	Texto* dadoTexto;
	Texto* mensagemTexto;
	//Texto* Texto;
	Texto* infoPecaOuCartaTexto;
	Texto* consentirTexto;
	
//	Musica* musBgm;
//	Musica* musVitoria;
//	Musica* musMusica;
	Som* somBgm;
	Som* somVitoria;
	Som* somDerrota;
	Som* somSelecionar;
	Som* somConfirmar;
	Som* somCancelar;
	int volumeGeral;
	bool musicaHabilitada;
	
	stringstream ssTempo;
	int totalDeQuadros;
	Uint32 ticksInicial,ticksAtual;
	float fpsAtual,fpsAcumulado[AMOSTRAS_FPS];
	int tempoDePartida,ticksInicioDaPartidaAtual;
	int segundos,minutos,horas;
	
	bool executando;
	int mouseX,mouseY;
	bool rolandoDadoInicial,desempatando;
	bool jogando;
	bool naFaseInicial;
	int turno;
	int acoesPorTurno;
	//bool esperandoConsentimentoJ,consentiuJ,posAcao;
	bool pilhaEmAcao;
	bool atacandoComAs;
	bool emFaseDeDano;
	//bool sacrificandoGeneral,sacrificandoRei;
	bool sacrificouRei;
	bool olhandoDescartes;
	
	Jogador* jogador[2];
	Jogador* jogadorDoUltimoEfeito;
	//Jogador* jogadorIniciativa;
	Jogador* jogadorDaVez;
	Jogador* jogadorDosDescartesExibidos;
	Jogador* vencedor;
	
	Tabuleiro* tabPrincipal;
	Cursor* cursor;
	vector<Textura*> graficoDasPecas{12};
	Peca* pecaSobCursor;
	Peca* pecaSelecionada;
	CopiaTransparenteDaImagemDaPecaSelecionada* copiaDaPecaSelecionadaNoCursor;
	
	vector<Textura*> graficoDasCartas{QTDE_CARTAS_CADA_BARALHO*2};
	vector<Textura*> graficoDasCartasVerso{2};
	Carta* cartaSelecionada;
	Carta* cartaSobCursor;
	
	vector<CasaAlvo> casasAlvo;
	vector<CasaAlvo> casasAlcance;
	CasaAlvo* casaAlvoSobCursor;
	
	vector<Efeito> pilha;
	Peca* pecaAtacando;
	Peca* pecaDefendendo;
};
