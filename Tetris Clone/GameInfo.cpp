#include "GameInfo.h"



GameInfo::GameInfo()
{
}


GameInfo::~GameInfo()
{
}

int GameInfo::getScore()
{
	return score;

}

void GameInfo::setScore(unsigned int score)
{
	this->score = score;
}
