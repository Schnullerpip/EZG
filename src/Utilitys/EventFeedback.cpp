#include "EventFeedback.h"

void EventFeedback::setNumberSamples(int number)
{
	number_samples = number;
	//std::cout << "[EventFeedback]::setNumberSamples -> MSAA samples is now: " << number << std::endl;
}

void EventFeedback::reset()
{
	restart = false;
	quitgame = false;
}

EventFeedback::EventFeedback()
{
}


EventFeedback::~EventFeedback()
{
}
