#pragma once
#include <vector>

class EventFeedback
{
public:

	/*---------------properties------------*/
	//gameloop specific
	bool restart = false;
	bool quitgame = true;
	//antialiasing specific
	int number_samples = 1;

	std::vector<std::pair<int, int>> hints;
	/*-------------------------------------*/

	//methods
	void setNumberSamples(int number);
	void reset();
	EventFeedback();
	~EventFeedback();
};

