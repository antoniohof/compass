#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxDatGui.h"
#include "ofxSerial.h"
#include "globals.h"

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
    
    //compass
    ofVec3f compassValues;
    bool bConnected;
    void searchForCompass();
    double searchInterval;
    void saveCompassConfig();
    
    //serial
    ofx::IO::SerialDevice compassSerial;
    
    //midi
    string selectedMidiDevice;
    ofxMidiOut midiOut;
    void refreshMidiList();

    //interface
    ofxDatGui * midiMenu;
    ofxDatGui * serialMenu;

    void setupInterface();

    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onDropdownEvent(ofxDatGuiDropdownEvent e);
    void onTextInputEvent(ofxDatGuiTextInputEvent e);
    
};
