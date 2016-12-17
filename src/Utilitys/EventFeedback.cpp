#include "EventFeedback.h"
#include <iostream>


void EventFeedback::setNumberSamples(int number)
{
	number_samples = number;
	std::cout << "[EventFeedback]::setNumberSamples -> MSAA samples is now: " << number << std::endl;
}

void EventFeedback::reset()
{
	restart = false;
	quitgame = true;
}

EventFeedback::EventFeedback()
{
}


EventFeedback::~EventFeedback()
{
}
