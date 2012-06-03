// BubbleTouchMultiplayer1.1.cpp: define el punto de entrada de la aplicación de consola.
//
#include "Game.h"
#include <cv.h>
#include <highgui.h>
#include "cvblob/cvblob.h"
#include <string.h>

/*
* Funcion principal d emi programa
*/
void on_mouse(int evt, int x, int y, int flags, void* param);
void on_mouse2(int evt, int x, int y, int flags, void* param);
void on_mouse3(int evt, int x, int y, int flags, void* param);

int main()
{
	cvNamedWindow("Inicio", 1);
	IplImage *img;
	img=cvLoadImage("inicio.jpg");
	cvShowImage("Inicio", img );
	cvSetMouseCallback("Inicio", on_mouse, 0);
	cvWaitKey();
	cvDestroyWindow("Inicio");
	return 0;
}
void on_mouse(int evt, int x, int y, int flags, void* param)
{
	if(evt==CV_EVENT_LBUTTONDOWN)
	{
		if(x>232&&x<400&&y<265&&y>213)
		{
			//Iniciar el juego
			Game game = Game();
			int ganador = game.findWinner();
			//Mostrar ganador
			cvNamedWindow("Ganador", 1);
			IplImage *img;
			if(ganador == 0)
			{
				img=cvLoadImage("empate.jpg");
			}
			if(ganador == 1)
			{
				img=cvLoadImage("jugador1.jpg");
			}
			if(ganador == 2)
			{
				img=cvLoadImage("jugador2.jpg");
			}
			cvShowImage("Ganador", img );
			cvSetMouseCallback("Ganador", on_mouse3, 0);
		}
		if(x>229&&x<400&&y<327&&y>275)
		{
			//Mostrar instrucciones
			cvNamedWindow("Instrucciones", 1);
			IplImage *img;
			img=cvLoadImage("instrucciones.jpg");
			cvShowImage("Instrucciones", img );
			cvSetMouseCallback("Instrucciones", on_mouse2, 0);
		}
	}
}
void on_mouse2(int evt, int x, int y, int flags, void* param)
{
	if(evt==CV_EVENT_LBUTTONDOWN)
	{
		if(x>24&&x<200&&y<434&&y>379)
		{
			cvDestroyWindow("Instrucciones");
		}
	}
}
void on_mouse3(int evt, int x, int y, int flags, void* param)
{
	if(evt==CV_EVENT_LBUTTONDOWN)
	{
		if(x>24&&x<200&&y<434&&y>379)
		{
			cvDestroyWindow("Ganador");
		}
	}
}