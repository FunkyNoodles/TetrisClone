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

void GameInfo::setScore(int score)
{
	this->score = score;
}

void GameInfo::addScore(int scoreToAdd)
{
	this->score += scoreToAdd;
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

void GameInfo::setPreviousTime(double time)
{
	previousTime = time;
}

double GameInfo::getPreviousTime()
{
	return previousTime;
}

void GameInfo::updateElapsedTime(double time)
{
	this->currentTime = time;
	currentElapsedTime = this->currentTime - (unpausedTime - pausedTime) - previousTime;
}

void GameInfo::setElapsedTime(double time)
{
	currentElapsedTime = time;
}

double GameInfo::getElapsedTime()
{
	return currentElapsedTime;
}
