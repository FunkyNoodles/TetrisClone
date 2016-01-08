#pragma once
class GameInfo
{
public:
	GameInfo();
	~GameInfo();
	int getScore();
	void setScore(unsigned int score);
private:
	unsigned int score = 0;
	double dropSpeed = 0;
};

