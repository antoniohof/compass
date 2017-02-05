#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableAlphaBlending();
	ofEnableAntiAliasing();
	ofSetFrameRate(60);
	ofSetVerticalSync(false);


   ofLog() << "[SETUP] - initializing hofmeister compass software...";
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
       

    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLog() << "\t" << devicesInfo[i].getPort();
    }
    
    debug = false;
    
    //intro
    logo.load("logo.png");

	//setup midi
	variables.push_back("x");
	variables.push_back("y");
	variables.push_back("z");
	variables.push_back("heading");
      
    //setup compass
    bConnected = false;
    compassValues = ofVec3f(0,0,0);
	compassHeading = 0;

    selectedMidiDevice = "";
    
    setupInterface();
}

//--------------------------------------------------------------
void ofApp::update(){
    logo.update();
    midiMenu->update();
    serialMenu->update();

    //search if not connected;
    searchInterval += ofGetLastFrameTime();
    if(!bConnected && searchInterval > 3){
        searchInterval = 0;
        searchForCompass();
    }
    
    if(bConnected){
        stringstream data;
        try
        {
            // Read all bytes from the device;
            uint8_t buffer[1024];
            
            while (compassSerial.available() > 0)
            {
                std::size_t sz = compassSerial.readBytes(buffer, 1024);
                
                for (std::size_t i = 0; i < sz; ++i)
                {
                    if(buffer[i] == ofToChar(globals::separator)) {
                        ofLog() << "[SERIAL]" << "received: " << data.str();
						parseData(data.str());
                        data.clear();
                    }else{
                        data << buffer[i];
                    }
                }
            }
        }
        catch (const std::exception& exc)
        {
            ofLogError("[UPDATE] error reading data!") << exc.what();
        }
        
		calculateHeading();

		serialMenu->getValuePlotter("x")->setValue(compassValues.x);
		serialMenu->getValuePlotter("y")->setValue(compassValues.y);
		serialMenu->getValuePlotter("z")->setValue(compassValues.z);
    }
    
    serialMenu->getTextInput("status")->setText(bConnected ? "connected" : "disconnected");
    serialMenu->getTextInput("status")->setBackgroundColor(bConnected ? ofColor(0,150,0) : ofColor(150,0,0));
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofClear(0);
	ofSetColor(255);

    midiMenu->draw();
    serialMenu->draw();
    
    if(debug){
        ofSetColor(255);
        ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, 750);
        ofDrawBitmapString("selected midi device: " + selectedMidiDevice, 20, 770);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
			ofx::IO::ByteBuffer textBuffer1("1800/400/900");
			ofx::IO::ByteBuffer textBuffer2("1500/900/1900");

    switch (key) {
        case ' ':
            debug = !debug;
            break;
        
        case 's':
            // Send some new bytes to the device to have them echo'd back.
                  
            compassSerial.writeBytes(textBuffer1);
			break;
		case 'x':
			// Send some new bytes to the device to have them echo'd back.

			compassSerial.writeBytes(textBuffer2);

            break;
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::calculateHeading() {
	if (globals::compassOrientation == "horizontal") {

	}
	float heading = 0;

	// Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
	// Calculate heading when the magnetometer is level, then correct for signs of axis.
	if (globals::compassOrientation == "horizontal") {
		  heading = atan2(compassValues.y, compassValues.x);
	}else heading = atan2(compassValues.z, compassValues.y);

	float declinationAngle = 0.39;
	heading += declinationAngle;

	// Correct for when signs are reversed.
	if (heading < 0)
	heading += 2 * PI;

	// Check for wrap due to addition of declination.
	if (heading > 2 * PI)
	heading -= 2 * PI;

	// Convert radians to degrees
	compassHeading = heading * 180 / PI;

}

void ofApp::parseData(string data) {
	//read data
	vector<string> parsedData = ofSplitString(data, "/", true, true);

	if (parsedData.size() == 3) {
		float x = ofToFloat(parsedData[0]) / 100;
		float y = ofToFloat(parsedData[1]) / 100;
		float z = ofToFloat(parsedData[2]) / 100;
		compassValues = ofVec3f(x, y, z);

		ofLog() << "x:" << compassValues.x << "/ y:" << compassValues.y << "/ z:" << compassValues.z;
	}
}

void ofApp::searchForCompass(){
    ofLog() << "[UPDATE] - searching...";
    
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();

    for (std::size_t i = 0; i < devicesInfo.size(); ++i){
        string deviceName = devicesInfo[i].getPort();
        if(deviceName == globals::compassSerialName){
            
            bool success = compassSerial.setup(globals::compassSerialName, globals::baudrate);
            
            if(success)
            {
                //serialIn.registerAllEvents(this);
                ofLog() << "[SERIAL] successfull connected to " << globals::compassSerialName << " at " << globals::baudrate;
            }
            else
            {
                ofLog() << "[SERIAL] unable to connect to " << globals::compassSerialName << " at " << globals::baudrate;
            }
            bConnected = success;        
        }
    }
}

void ofApp::saveCompassConfig(){
    globals::xml.setValue("root:compassSerialName", globals::compassSerialName);
    globals::xml.setValue("root:baudrate", globals::baudrate);
    globals::xml.setValue("root:compassOrientation", globals::compassOrientation);
    globals::xml.saveFile();
}


void ofApp::setupInterface(){
    //midi
    midiMenu = new ofxDatGui(300,0);
    midiMenu->setAutoDraw(false);
    midiMenu->addHeader("OUT: midi", true);
    midiMenu->addButton("refresh");
    midiMenu->addLabel("select below your output device");
    midiMenu->addDropdown("devices", midiOut.getPortList());

	midiMenu->addBreak();
	midiMenu->addBreak();
	midiMenu->addBreak();


	midiMenu->addLabel("output 1")->setBackgroundColor(ofColor(150, 0, 0));
	midiMenu->addDropdown("variable to send 1", variables);
	ofxDatGuiFolder * output1 = new ofxDatGuiFolder("output 1 config");
	output1->addToggle("active");
	output1->addTextInput("channel", "1");
	output1->addTextInput("cc", "1");
	midiMenu->addFolder(output1);

	midiMenu->addBreak();
	midiMenu->addBreak();
	midiMenu->addBreak();

	midiMenu->addLabel("output 2")->setBackgroundColor(ofColor(150, 0, 0));
	midiMenu->addDropdown("variable to send 2", variables);
	ofxDatGuiFolder * output2 = new ofxDatGuiFolder("output 2 config");
	output2->addToggle("active");
	output2->addTextInput("channel", "1");
	output2->addTextInput("cc", "1");
	midiMenu->addFolder(output2);

	midiMenu->addBreak();
	midiMenu->addBreak();
	midiMenu->addBreak();

	midiMenu->addLabel("output 3")->setBackgroundColor(ofColor(150, 0, 0));
	midiMenu->addDropdown("variable to send 3", variables);
	ofxDatGuiFolder * output3 = new ofxDatGuiFolder("output 3 config");
	output3->addToggle("active");
	output3->addTextInput("channel", "1");
	output3->addTextInput("cc", "1");
	midiMenu->addFolder(output3);

	midiMenu->addBreak();
	midiMenu->addBreak();
	midiMenu->addBreak();

	midiMenu->addLabel("output 4")->setBackgroundColor(ofColor(150, 0, 0));
	midiMenu->addDropdown("variable to send 4", variables);
	ofxDatGuiFolder * output4 = new ofxDatGuiFolder("output 4 config");
	output4->addToggle("active");
	output4->addTextInput("channel", "1");
	output4->addTextInput("cc", "1");
	midiMenu->addFolder(output4);

    midiMenu->onButtonEvent(this, &ofApp::onButtonEvent);
    midiMenu->onDropdownEvent(this, &ofApp::onDropdownEvent);
    
    
    //serial
    serialMenu = new ofxDatGui(0,0);
    serialMenu->setAutoDraw(false);
    serialMenu->addHeader("IN: compass");
    serialMenu->addTextInput("status", "disconnected");

    serialMenu->addTextInput("bluetooth", globals::compassSerialName);
    serialMenu->addTextInput("baudrate", ofToString(globals::baudrate));
    vector<string> orientations;
    orientations.push_back("horizontal");
    orientations.push_back("vertical");
    serialMenu->addDropdown("orientation", orientations);
    serialMenu->addButton("save bluetooth config");
    serialMenu->addBreak();
    serialMenu->addValuePlotter("x", -180, 180);
    serialMenu->addValuePlotter("y", -180, 180);
    serialMenu->addValuePlotter("z", -180, 180);


    serialMenu->onButtonEvent(this, &ofApp::onButtonEvent);
    serialMenu->onTextInputEvent(this, &ofApp::onTextInputEvent);
    serialMenu->onDropdownEvent(this, &ofApp::onDropdownEvent);
}

void ofApp::refreshMidiList(){
    ofLog() << "[MIDI] refreshing midi out devices...";
    for(int i = 0; i < midiMenu->items.size(); i++){
        if(midiMenu->items.at(i) == midiMenu->getDropdown("devices")){
            midiMenu->items.erase(midiMenu->items.begin() + i);
			midiMenu->layoutGui();
			ofxDatGuiDropdown * dropdown = new ofxDatGuiDropdown("devices", midiOut.getPortList());
			dropdown->onDropdownEvent(this, &ofApp::onDropdownEvent);
			midiMenu->items.insert(midiMenu->items.begin() + i, dropdown);
			dropdown->onInternalEvent(midiMenu, &ofxDatGui::onInternalEventCallback);
			midiMenu->layoutGui();
			midiMenu->getDropdown("devices")->collapse();
			dropdown->onInternalEvent(midiMenu, &ofxDatGui::onInternalEventCallback);
			midiMenu->getDropdown("devices")->expand();
			midiMenu->layoutGui();

		}
    }


}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
    if(e.target->getName() == "refresh") refreshMidiList();
    else if(e.target->getName() == "save bluetooth config") saveCompassConfig();
}

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e){
    if(e.target->getName() == "devices"){
        if(e.target->getSelected()->getLabel() != selectedMidiDevice){
            midiOut.closePort();
            selectedMidiDevice = e.target->getSelected()->getLabel();
            midiOut.openPort(selectedMidiDevice);
            ofLog() << "[MIDI] selected midi device: " + selectedMidiDevice;
        }
    }else if(e.target->getName() == "orientation"){
        globals::compassOrientation = e.target->getSelected()->getLabel();
    }
}

void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e){
    globals::compassSerialName = serialMenu->getTextInput("bluetooth")->getText();
    globals::baudrate = ofToInt(serialMenu->getTextInput("baudrate")->getText());
    globals::compassOrientation = serialMenu->getDropdown("orientation")->getSelected()->getLabel();
}

