//#include "ofxBlackmagic.h" // See https://github.com/elliotwoods/ofxBlackmagic2
#include "HelpState.h"

void HelpState::setup()
{
}

void HelpState::update()
{
}

void HelpState::draw()
{
    ofSetColor(0);
    ofBackgroundHex(0xcccccc);

    ofDrawBitmapString("'0' or Backspace : change mode to Device selecting", 10, 20);
    ofDrawBitmapString("'1' : change mode to Projection adjusting mode (cam1)", 10, 40);
    ofDrawBitmapString("'2' : change mode to Projection adjusting mode (cam2)", 10, 60);
    ofDrawBitmapString("'l' : load user settings", 10, 80);
    ofDrawBitmapString("'s' : save user settings", 10, 100);
    ofDrawBitmapString("'d' : toggle showing debug message", 10, 120);
    ofDrawBitmapString("'h' : show help", 10, 140);
    ofDrawBitmapString("'q' or Esc: quit", 10, 160);
}

string HelpState::getName()
{
	return "help";
}

