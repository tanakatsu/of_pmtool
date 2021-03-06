#pragma once

//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "ofxState.h"
#include "SharedData.h"

class ProjectionState : public itg::ofxState<SharedData>
{
public:
    void setup();
	void update();
	void draw();
	void keyPressed(int key);
	string getName();
};
