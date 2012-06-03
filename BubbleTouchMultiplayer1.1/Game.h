#ifndef GAME_H
#define GAME_H

#include "Bubble.h"
#include "Player.h"
#include <cv.h>
#include "cvblob/cvblob.h"

const int NBUBBLES = 10;

/*
* Definicion de mi clase Game
*/
class Game
{
	public:
		Bubble bubbles[NBUBBLES];
		Player *player1;
		Player *player2;
		Game(); // default constructor
		~Game(); // do-nothing destructor
		void initBubbles(); //Inicializar las burbujas
		void initBubblesPosicion(CvSize imgSize); //Establecer las posiciones de las burbujas
		void findBlobs(IplImage *frame, IplImage *segementated, cvb::CvBlobs &blobs); //Encontrar los blobs en la imagen binaria
		void drawBubble(IplImage* Frame,IplImage* BubbleImg, CvPoint loc); //Dibujar una burbuja individual
		void drawBubbles(IplImage *frame); //Dibujar mi arreglo de burburjas
		void checkBubbles(cvb::CvBlobs blobsR, cvb::CvBlobs blobsB, Player *player1, Player *player2);//Revisar que blobs coinciden con la posicion de las burbujas
		bool isGameOver();//Revisar si se cumplen las condiciones de fin de juego
		void printScore(IplImage *frame, Player *player1, Player *player2); //Imprimir el puntaje de cada jugador en pantalla
		int findWinner(); //Encuentra un ganador
};

#endif