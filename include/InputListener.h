#pragma once
enum eventType;

class InputListener
{
public:
	virtual void actOnChange(eventType et) = 0;
	virtual ~InputListener()
	{
	}
};

