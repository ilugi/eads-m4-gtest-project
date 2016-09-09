#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <string>

class Display
{
public:
	Display() {}
	virtual ~Display() {}

	virtual void
	show (std::string message) = 0;
};

#endif /* DISPLAY_H_ */
