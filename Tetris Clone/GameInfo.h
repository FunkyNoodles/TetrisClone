#pragma once
class GameInfo
{
public:
	GameInfo();
	~GameInfo();
	int getScore();
	void setScore(int score);
	void addScore(int scoreToAdd);
	double getDropSpeed();
	void setDropSpeed(double dropSpeed);
	bool isGamePaused();
	void invertGamePaused();
	void setPreviousTime(double time);
	double getPreviousTime();
	void updateElapsedTime(double Time);
	void setElapsedTime(double time);
	double getElapsedTime();
private:
	// Score
	int score = 0;
	// Block drop speed
	double dropSpeed = 0.7;
	bool isPaused = false;

	// Time keeping
	double currentTime = 0.0;
	double pausedTime = 0.0;
	double unpausedTime = 0.0;
	double previousTime = 0.0;
	double currentElapsedTime = 0.0;
};

