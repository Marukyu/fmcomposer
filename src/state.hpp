#ifndef STATE_H
#define STATE_H

class State{
	public:
	State() {};
	virtual void draw() {};
	virtual void update() {};
	virtual void updateFromFM() {};
	virtual void handleEvents() {};
};

extern State *state;

#endif
