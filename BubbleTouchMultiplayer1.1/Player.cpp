#include "Player.h"
#include <iostream>

Player::Player()
{
	score=0;
}
Player::~Player()
{
}
void Player::setScore(int pscore)
{
	if(pscore<0){
		std::cerr << "Score can’t be negative; "
		<< pscore << " score set to 0.\n";
		pscore = 0;
	}else
		score=pscore;
}
double Player::getScore()
{
	return score;
}
void Player::addOne2Score()
{
	score++;
}