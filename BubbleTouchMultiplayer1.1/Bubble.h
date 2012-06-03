#ifndef BUBBLE_H
#define BUBBLE_H

#include <string>
using std::string;

/*
* Definicion de mi clase Bubble
*/
class Bubble
{
	private:
		int width;
		int height;
		int x;
		int y;
		int state; //Me indica si la burbuja esta activa
	public:
		Bubble(); // default constructor
		~Bubble(); // do-nothing destructor
		void setWidth(int bwidth); //Establecer el ancho de mi burbuja
		int getWidth(); //Obtener el ancho de mi burbuja
		void setHeight(int bheight); //Establecer el alto de mi burbuja
		int getHeight(); //Obtener el alto de mi burbuja
		void setPosX(int bx); //Establecer la posicion en x de mi burbuja
		int getPosX(); //Obtener la posicion en x de mi burbuja
		void setPosY(int by); //Establecer la posicion en y de mi burbuja
		int getPosY(); //Obtener la posicion en y de mi burbuja
		int getState(); //Obtener el estado de mi burbuja
};

#endif