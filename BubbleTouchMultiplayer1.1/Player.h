#ifndef PLAYER_H
#define PLAYER_H

#include <cv.h>

/*
* Definicion de mi clase jugador
*/
class Player
{
	private:
		int score;
	public:
		Player(); // default constructor
		~Player(); // do-nothing destructor
		void setScore(int score); //Setear el puntaje de mi jugador
		double getScore(); //obtener el puntaje de mi jugador
		void addOne2Score(); //Aumentar en 1 el puntaje del jugador
};

#endif