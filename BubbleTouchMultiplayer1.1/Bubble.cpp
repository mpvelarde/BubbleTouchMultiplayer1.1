#include "Bubble.h"
#include <iostream>
#include <string.h>

using std::string;

Bubble::Bubble()
{
	width=0.0;
	height=0.0;
	x=0.0;
	y=0.0;
	state=1;
}
Bubble::~Bubble()
{
	state=0;
	//std::cout << "Bubble destroyed" << std::endl;
}
void Bubble::setWidth(int bwidth)
{
	if(bwidth<0){
		std::cerr << "Width can’t be negative; "
		<< bwidth << " width set to 0.\n";
		bwidth = 0;
	}else
		width=bwidth;
}
int Bubble::getWidth()
{
	return width;
}
void Bubble::setHeight(int bheight)
{
	if(bheight<0){
		std::cerr << "Height can’t be negative; "
		<< bheight << " height set to 0.\n";
		bheight = 0;
	}else
		height=bheight;
}
int Bubble::getHeight()
{
	return height;
}
void Bubble::setPosX(int bx)
{
	if(bx<0){
		std::cerr << "Position in X can’t be negative; "
		<< bx << " x set to 0.\n";
		bx = 0;
	}else
		x=bx;
}
int Bubble::getPosX()
{
	return x;
}
void Bubble::setPosY(int by)
{
	if(by<0){
		std::cerr << "Position in Y can’t be negative; "
		<< by << " y set to 0.\n";
		by = 0;
	}else
		y=by;
}
int Bubble::getPosY()
{
	return y;
}
int Bubble::getState()
{
	return state;
}