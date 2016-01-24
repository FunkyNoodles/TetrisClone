#pragma once
class GameInfo
{
public:
	GameInfo();
	~GameInfo();
	//Scores
	int getScore();
	void setScore(int score);
	void addScore(int scoreToAdd);
	void setLastPlaceBlockColorValue(unsigned int colorValue);
	int getLastPlaceBlockColorValue();
	void incrementStreak();
	void resetStreak();
	int getStreak();

	// Other game variables
	double getDropSpeed();
	void setDropSpeed(double dropSpeed);
	bool isGamePaused();
	void invertGamePaused();
	bool isGameOver();
	void setGameOver(bool data);

	// Time keeping variables
	void setPreviousTime(double time);
	double getPreviousTime();
	void updateElapsedTime(double Time);
	void setElapsedTime(double time);
	double getElapsedTime();

	
private:
	// Score
	int score = 0;

	// Scoring variables

	// Last block placed using color values
	int lastPlacedBlockColorValue = -1;
	// Streak
	int streak = 0;
	// Block drop speed
	double dropSpeed = 0.7;
	bool isPaused = false;
	bool isOver = false;

	// Time keeping
	double currentTime = 0.0;
	double pausedTime = 0.0;
	double unpausedTime = 0.0;
	double previousTime = 0.0;
	double currentElapsedTime = 0.0;
};

