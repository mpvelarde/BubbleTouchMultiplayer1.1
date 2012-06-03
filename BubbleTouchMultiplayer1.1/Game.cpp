#include "Bubble.h"
#include "Player.h"
#include "Game.h"
#include <cv.h>
#include <highgui.h>
#include "cvblob/cvblob.h"
#include <string.h>

const int DSIZE1 = 40;
const int DDIF = 20;

/*
* Funcion que inicializa el juego y sus componentes
*/
Game::Game()
{
	//Inicializo los jugadores
	player1=new Player();
	player2=new Player();

	//Inicializo mis imagenes, ventanas y componentes
	IplImage *img;
	CvCapture* capture = cvCaptureFromCAM(1);
	cvNamedWindow("BubbleTouchMultiplayer", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("BubbleTouchMultiplayer", 5, 5);

	//Inicializo las burbujas
	initBubbles();

	unsigned int blobNumber = 0;
	
	while(cvGrabFrame(capture))
	{
		int c; //numero de la tecla a obtener

		//Defino las variables de mis blobs
		cvb::CvBlobs blobsR;
		cvb::CvBlobs blobsB;

		//Defino los rangos de color a obtener
		//Rojo
		//CvScalar hsv_minR = cvScalar(150, 84, 130, 0);
		//CvScalar hsv_maxR = cvScalar(358, 256, 255, 0);
		CvScalar hsv_minR = cvScalar(170, 100, 80, 0);
		CvScalar hsv_maxR = cvScalar(180, 256, 255,0);

		//Azul
		//CvScalar hsv_minB = cvScalar( 104, 178, 70  );
		//CvScalar hsv_maxB = cvScalar( 130, 57, 131 );
		CvScalar hsv_minB = cvScalar( 110, 100, 90,0  );
		CvScalar hsv_maxB = cvScalar( 130, 178, 131,0 );

		//Capturo mi imagen
		img=cvQueryFrame(capture);
		
		//Obtengo el tamanio
		CvSize imgSize = cvGetSize(img);
		
		//Defino mis imagenes a utillizar en el filtrado
		IplImage *frame = cvCreateImage(imgSize, img->depth, img->nChannels); //Imagen original
		IplImage *segmentatedR = cvCreateImage(imgSize, 8, 1); //Imagen binaria con informacion del rojo
		IplImage *segmentatedB = cvCreateImage(imgSize, 8, 1); //Imagen binaria con informacion del azul
		IplImage *  hsv_frame    = cvCreateImage(imgSize, IPL_DEPTH_8U, 3); //Imagen HSV para filtrar los colores con mas facilidad

		cvCopy(img, frame); //Copiar mi imagen capturada en frame

		//Encontrar la posicion de los jugadores
		//Convertir mi frame de RGB a HSV
		cvCvtColor(frame, hsv_frame, CV_BGR2HSV);
		
		// Filtrar los colores fuera de rango para cada color
		cvInRangeS(hsv_frame, hsv_minR, hsv_maxR, segmentatedR);
		cvInRangeS(hsv_frame, hsv_minB, hsv_maxB, segmentatedB);
		
		//Suavizar los bordes de mis imagenes para hacerlos mas facil de detectar y agrupar
		cvSmooth( segmentatedR, segmentatedR, CV_GAUSSIAN, 9, 9 );
		cvSmooth( segmentatedB, segmentatedB, CV_GAUSSIAN, 9, 9 );

		//cvShowImage("R", segmentatedR );
		//cvShowImage("B", segmentatedB );
		
		//Inicializar las posiciones de las burbujas
		initBubblesPosicion(cvGetSize(frame));

		//Identificar los blobs en mi imagen
		findBlobs(frame, segmentatedR, blobsR);
		findBlobs(frame, segmentatedB, blobsB);

		//Dibujar las burbujas
		drawBubbles(frame);

		//Imprimir puntaje
		printScore(frame,player1,player2);
		
		//Imprimir mi frame
		cvShowImage("BubbleTouchMultiplayer", frame );

		//Buscar si los jugadores tocan las burbujas
		checkBubbles(blobsR,blobsB,player1,player2);

		//Si ya no hay burbujas que se acabe el juego
		if(isGameOver())break;

		//Finalizar con escape si el juego no ha acabado
		c=cvWaitKey(10);
		if(c == 27){
			cvReleaseImage(&frame);
			break;
		}
	}
	//std::cout << "GAME OVER" << std::endl;
	//cvWaitKey();

	cvDestroyWindow("BubbleTouchMultiplayer");
	cvReleaseImage(&img);
}
Game::~Game()
{
}
/*
* Funcion que inicializa mis burbujas
* param: CvSize imgSize - Tamanio de mi frame
*/
void Game::initBubbles()
{
	for(int i=0;i<NBUBBLES;i++)
	{
		bubbles[i]= Bubble();
	}
}
void Game::initBubblesPosicion(CvSize imgSize)
{
	for(int i=0;i<NBUBBLES;i++)
	{
		//Inicializar solo las burbujas que esten activas
		if(bubbles[i].getState()==1)
		{
			bubbles[i].setWidth(DSIZE1);
			bubbles[i].setHeight(DSIZE1);
			if(bubbles[i].getPosX()==0&&bubbles[i].getPosY()==0)
			{
				//Si la burbuja recien ha sido inicializada se le da una posicion aleatoria
				bubbles[i].setPosX(rand()%(imgSize.width-DSIZE1));
				bubbles[i].setPosY(rand()%(imgSize.height-DSIZE1));
			}
			else
			{
				//Si ha sido inicializado se le da una posicion cercana a su posicion actual
				int minx=0,maxx=0,miny=0,maxy=0;
				
				//Busco los posibles valores minimo y maximo para el random
				minx=bubbles[i].getPosX()-DSIZE1;
				maxx=bubbles[i].getPosX()+DSIZE1;
				miny=bubbles[i].getPosY()-DSIZE1;
				maxy=bubbles[i].getPosY()+DSIZE1;

				//Reviso que estos valores no hayan soprepasado las fronteras de la imagen
				if(minx<0) minx=0;
				if(miny<0) miny=0;
				if(maxx>(imgSize.width-DSIZE1-1)) maxx=imgSize.width-DSIZE1-1;
				if(maxy>imgSize.height-DSIZE1-1) maxy=imgSize.height-DSIZE1-1;

				//En base a estos valores establezco nuevas posiciones aleatorias
				bubbles[i].setPosX((rand()%(maxx-minx))+minx);
				bubbles[i].setPosY((rand()%(maxy-miny))+miny);
			}
		}
	}
}
/*
* Funcion que encuentra la posicion de los conjuntos de elementos positivos en la imagen binaria
* param: Iplimage *frame - Puntero a mi frame
* param: IplImage *segmentated - Puntero a mi imagen binaria
* param: CvBlobs &blobs - Referencia a mi variable blobs
*/
void Game::findBlobs(IplImage *frame, IplImage *segmentated, cvb::CvBlobs &blobs)
{
	IplImage *labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL,1);
	unsigned int result = cvLabel(segmentated, labelImg, blobs);
	cvFilterByArea(blobs, 1000, 6000);
	cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX);	
	cvReleaseImage(&labelImg);
}
/*
* Funcion que dibuja mi arreglo de burbujas en el frame
* param: IplImage *frame - Puntero a mi frame
*/
void Game::drawBubbles(IplImage *frame)
{
	IplImage *BubbleImg=0;

	for(int i=0;i<NBUBBLES;i++)
	{
		//Dibujar la burbuja solo si esta activa
		if(bubbles[i].getState()==1)
		{
			if((BubbleImg=cvLoadImage("b40a.png" ,1))==0)
			{
				std::cerr << "Bubble image loaded fail; ";
			}else
			{
				CvPoint p = cvPoint(bubbles[i].getPosX(), bubbles[i].getPosY());
				drawBubble(frame,BubbleImg,p);
			}
		}	
	}
}
/*
* Funcion que dibuja la burbuja individual
* param: IplImage * Frame - Puntero a mi frame
* param: IplImage *BubbleImg - Imagen de mi burbuja
* param: IplImage *MaskImg - Puntero a la mascara de mi burbuja
* para: CvPoint loc - Ubicacion donde voy a dibujar mi burbuja
*/
void Game::drawBubble(IplImage* Frame,IplImage* BubbleImg, CvPoint loc)
{
	//Escoger el ROI donde va a ir la burbuja
	CvRect burbuja = cvRect(loc.x,loc.y, BubbleImg->width, BubbleImg->height);
	cvSetImageROI(Frame, burbuja);
	cvAdd(Frame,BubbleImg,Frame);
	cvResetImageROI(Frame);

}
/*
* Funcion que revisa que burbujas coinciden con que blobs, las deshabilita y suma el puntaje del jugador
* param: CvBlobs blobsR - blobs del color del jugador 1
* param: CvBlobs blobsB - blobs del color del jugador 2
* param: Player *player1 - Puntero a mi jugador 1
* param: Player *player2 - Puntero a mi jugador 2
*/
void Game::checkBubbles(cvb::CvBlobs blobsR, cvb::CvBlobs blobsB, Player *player1, Player *player2)
{
	for(int i=0;i<NBUBBLES;i++)
	{
		if(bubbles[i].getState()==1){
			//JUGADOR 1
			for (cvb::CvBlobs::const_iterator it=blobsR.begin(); it!=blobsR.end(); ++it) 
			{
				//Objeto es mas grande que la burbuja----------------------------------------
				if((it->second->minx<bubbles[i].getPosX()&&it->second->maxx>(bubbles[i].getPosX()+DDIF)))  
				{
					//Burbuja dentro del objeto completamente
					if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<(bubbles[i].getPosY()+DDIF)&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))
					{
						//Burbuja sobresale del objeto hacia arriba
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>bubbles[i].getPosY()))
					{
						//Burbuja sobresale del objeto hacia abajo
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}
				}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))  
				{
					//Burbuja dentro del objeto completamente
					if((it->second->minx<bubbles[i].getPosX()&&it->second->maxx>(bubbles[i].getPosX()+DDIF)))
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->minx<(bubbles[i].getPosX()+DDIF)&&it->second->maxx>(bubbles[i].getPosX()+DDIF)))
					{
						//Burbuja sobresale del objeto hacia la izquierda
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->minx<bubbles[i].getPosX()&&it->second->maxx>bubbles[i].getPosX()))
					{
						//Burbuja sobresale del objeto hacia la derecha
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}
				}else if((it->second->minx<bubbles[i].getPosX()&&it->second->maxx>bubbles[i].getPosX()))  
				{
					//Objeto es mas pequenio que la burbuja--------------------------------------
					if((it->second->miny<(bubbles[i].getPosY()+DDIF)&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))  
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny>bubbles[i].getPosY()&&it->second->maxy<(bubbles[i].getPosY()+DDIF)))  
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>bubbles[i].getPosY()))  
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}
				}else if((it->second->minx>bubbles[i].getPosX()&&it->second->maxx>(bubbles[i].getPosX()+DDIF)))  
				{
					if((it->second->miny<(bubbles[i].getPosY()+DDIF)&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))  
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny>bubbles[i].getPosY()&&it->second->maxy<(bubbles[i].getPosY()+DDIF)))  
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>bubbles[i].getPosY()))  
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}
				}else if((it->second->minx>bubbles[i].getPosX()&&it->second->maxx<(bubbles[i].getPosX()+DDIF)))  
				{
					if((it->second->miny<(bubbles[i].getPosY()+DDIF)&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))  
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny>bubbles[i].getPosY()&&it->second->maxy<(bubbles[i].getPosY()+DDIF)))  
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>bubbles[i].getPosY()))  
					{
						player1->addOne2Score();
						bubbles[i].~Bubble();
					}
				}
			}
			//JUGADOR 2
			for (cvb::CvBlobs::const_iterator it=blobsB.begin(); it!=blobsB.end(); ++it) 
			{
				//Objeto es mas grande que la burbuja----------------------------------------
				if((it->second->minx<bubbles[i].getPosX()&&it->second->maxx>(bubbles[i].getPosX()+DDIF)))  
				{
					//Burbuja dentro del objeto completamente
					if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<(bubbles[i].getPosY()+DDIF)&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))
					{
						//Burbuja sobresale del objeto hacia arriba
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>bubbles[i].getPosY()))
					{
						//Burbuja sobresale del objeto hacia abajo
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}
				}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))  
				{
					//Burbuja dentro del objeto completamente
					if((it->second->minx<bubbles[i].getPosX()&&it->second->maxx>(bubbles[i].getPosX()+DDIF)))
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->minx<(bubbles[i].getPosX()+DDIF)&&it->second->maxx>(bubbles[i].getPosX()+DDIF)))
					{
						//Burbuja sobresale del objeto hacia la izquierda
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->minx<bubbles[i].getPosX()&&it->second->maxx>bubbles[i].getPosX()))
					{
						//Burbuja sobresale del objeto hacia la derecha
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}
				}else if((it->second->minx<bubbles[i].getPosX()&&it->second->maxx>bubbles[i].getPosX()))  
				{
					//Objeto es mas pequenio que la burbuja--------------------------------------
					if((it->second->miny<(bubbles[i].getPosY()+DDIF)&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))  
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny>bubbles[i].getPosY()&&it->second->maxy<(bubbles[i].getPosY()+DDIF)))  
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>bubbles[i].getPosY()))  
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}
				}else if((it->second->minx>bubbles[i].getPosX()&&it->second->maxx>(bubbles[i].getPosX()+DDIF)))  
				{
					if((it->second->miny<(bubbles[i].getPosY()+DDIF)&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))  
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny>bubbles[i].getPosY()&&it->second->maxy<(bubbles[i].getPosY()+DDIF)))  
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>bubbles[i].getPosY()))  
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}
				}else if((it->second->minx>bubbles[i].getPosX()&&it->second->maxx<(bubbles[i].getPosX()+DDIF)))  
				{
					if((it->second->miny<(bubbles[i].getPosY()+DDIF)&&it->second->maxy>(bubbles[i].getPosY()+DDIF)))  
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny>bubbles[i].getPosY()&&it->second->maxy<(bubbles[i].getPosY()+DDIF)))  
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}else if((it->second->miny<bubbles[i].getPosY()&&it->second->maxy>bubbles[i].getPosY()))  
					{
						player2->addOne2Score();
						bubbles[i].~Bubble();
					}
				}	
			}
		}	
	}
}
/*
* Funcion que revisa si el juego ha acabado
*/
bool Game::isGameOver()
{
	for(int i=0;i<NBUBBLES;i++)
	{
		if(bubbles[i].getState()==1)
		{
			return false;
		}
	}
	return true;
}
/*
* Funcion que imprime en el frame el puntaje de los jugadores
* param: IplImage *frame - Puntero al frame donde se va a imprimir
* param: Player *player1 - Jugador 1
* param: Player *player2 - Jugador 2
*/
void Game::printScore(IplImage *frame, Player *player1, Player *player2){
	//Inicializo y declaro las variables para mis letras
	CvFont font;
	double hScale=0.7;
	double vScale=0.7;
	int    lineWidth=1;

	//Defino mis variables para mostrar el puntaje
	char cNumber1[20];
	char cNumber2[20];

	//Incializar mi tipo de letra
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
	//Cambiar el tipo de dato de mi puntaje
	itoa ( player1->getScore(), cNumber1, 10);
	itoa ( player2->getScore(), cNumber2, 10);

	//Imprimir mi puntaje en el frame
	//Jugador 1
	cvPutText (frame,"Jugador 1: ",cvPoint(10,25), &font, cvScalar(2,69,252));
	cvPutText (frame,cNumber1,cvPoint(150,25), &font, cvScalar(2,69,252));

	//Jugador 2
	cvPutText (frame,"Jugador 2: ",cvPoint(frame->width/2,25), &font, cvScalar(2,69,252));
	cvPutText (frame,cNumber2,cvPoint((frame->width/2)+140,25), &font, cvScalar(2,69,252));
}
/*
* Encuentra el ganador del juego
*/
int Game::findWinner()
{
	if(player1->getScore()>player2->getScore())
	{
		return 1;
	}
	if(player2->getScore()>player1->getScore())
	{
		return 2;
	}
	return 0;
}