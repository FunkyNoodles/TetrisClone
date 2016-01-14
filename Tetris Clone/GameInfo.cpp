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

double GameInfo::getDropSpeed()
{
	return this->dropSpeed;
}

void GameInfo::setDropSpeed(double dropSpeed)
{
	this->dropSpeed = dropSpeed;
}

bool GameInfo::isGamePaused()
{
	return isPaused;
}

void GameInfo::invertGamePaused()
{
	isPaused = !isPaused;
}
