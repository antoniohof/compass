#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxDatGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
    bool debug;
    
    //logo intro
    ofImage logo;
    
    //window settings
    bool isRetina;
    
    //device orientation
    //swap z with y, etc
    
    //serial in
    ofSerial serialIn;
    
    
    //midi out
    ofxMidiOut midiOut;
    double checkMidiListInterval;
    void updateMidiList();

    //gui
    ofxDatGui * midiGui;
    
    ofxDatGuiFolder * serialFolder;

    void setupGui();

    
};
