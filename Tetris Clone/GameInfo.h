#pragma once
class GameInfo
{
public:
	GameInfo();
	~GameInfo();
	int getScore();
	void setScore(unsigned int score);
	double getDropSpeed();
	void setDropSpeed(double dropSpeed);
	bool isGamePaused();
	void invertGamePaused();
private:
	// score
	unsigned int score = 0;
	// Block drop speed
	double dropSpeed = 0.7;
	bool isPaused = false;
};

