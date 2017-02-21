#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableAlphaBlending();
	ofEnableAntiAliasing();
	ofSetFrameRate(60);
	ofSetVerticalSync(false);

    ofLog() << "[SETUP] - initializing hofmeister compass software...";

#ifdef  _WIN32 || defined _WIN64
	ofLog() << "[SETUP] - windows cannot create virtual midi ports > use loop midi software";
#elif
	midiOut.openVirtualPort("virtual compass out");
	ofLog() << "[SETUP] - creating virtual midi device for output reasons";
#endif

    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
       
    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLog() << "[SETUP] - available serial ports > " << devicesInfo[i].getPort();
    }
    
    debug = false;
    
    //intro
    logo.load("logo.png");

	//setup midii
	variables.push_back("x");
	variables.push_back("y");
	variables.push_back("z");
	variables.push_back("heading");
	
	for (int i = 0; i < 4; i++)
	{
		pair<int,int> output = pair<int, int>(1, 1);
		midiOutputs.push_back(output);
	}

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
                       // ofLog() << "[SERIAL]" << "received: " << data.str();
						parseData(data.str());
                        data.clear();
						break;
                    }else{
                        data << buffer[i];
                    }
                }
				break;
            }
        }
        catch (const std::exception& exc)
        {
            ofLogError("[UPDATE] error reading data!") << exc.what();
        }           

        
		calculateHeading();

		//update interface
		serialMenu->getValuePlotter("x")->setValue(compassValues.x);
		serialMenu->getValuePlotter("y")->setValue(compassValues.y);
		serialMenu->getValuePlotter("z")->setValue(compassValues.z);
    }
    
    serialMenu->getTextInput("status")->setText(bConnected ? "connected" : "disconnected");
    serialMenu->getTextInput("status")->setBackgroundColor(bConnected ? ofColor(0,150,0) : ofColor(150,0,0));


	//midi
	//if active send to channels
	midiSendInterval += ofGetLastFrameTime();
	if (midiSendInterval > 0.25) {
		midiSendInterval = 0;
		sendMidiData();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofClear(0);
	ofSetColor(255);

    midiMenu->draw();
    serialMenu->draw();
    
    if(debug){
        ofSetColor(255);
        ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, 900);
        ofDrawBitmapString("selected midi device: " + selectedMidiDevice, 20, 920);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
			ofx::IO::ByteBuffer textBuffer1("50/80/30");
			ofx::IO::ByteBuffer textBuffer2("10/5/70");

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

void ofApp::sendMidiData() {
	if (!midiOut.isOpen()) return;
	for (int i = 0; i < midiOutputs.size(); i++) {
		bool isActive = midiMenu->getToggle("active " + ofToString(i))->getChecked();
		int channel = midiOutputs[i].first;
		int cc = midiOutputs[i].second;
		string variable = midiMenu->getDropdown("variable to send " + ofToString(i))->getSelected()->getLabel();
		float value = 0;
		if (variable == "x") value = ofMap(compassValues.x, -20.0f, 20.0f, 0, 127, true);
		else if (variable == "y") value = ofMap(compassValues.y, -20.0f, 20.0f, 0, 127, true);
		else if (variable == "z") value = ofMap(compassValues.z, -20.0f, 20.0f, 0, 127, true);
		else if (variable == "heading") value = ofMap(compassHeading, 0.f, 360.0f, 0, 127, true);;

		if (isActive && value != 0) {
			midiOut.sendControlChange(channel, cc, value);
		}
	}
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

	serialMenu->getTextInput("HEADING")->setText(ofToString(compassHeading));
	//ofLog() << "heading: " << compassHeading;
}

void ofApp::parseData(string data) {
	//read data
	vector<string> parsedData = ofSplitString(data, "/", true, true);

	if (parsedData.size() == 3) {
		float x = ofToFloat(parsedData[0]) / 100;
		float y = ofToFloat(parsedData[1]) / 100;
		float z = ofToFloat(parsedData[2]) / 100;
		compassValues = ofVec3f(x, y, z);
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
    midiMenu = new ofxDatGui(500,0);
    midiMenu->setAutoDraw(false);
    midiMenu->addHeader("OUT: midi", true);
    midiMenu->addButton("refresh");
    midiMenu->addLabel("select below your output device");
    midiMenu->addDropdown("devices", midiOut.getPortList());

	midiMenu->addBreak();
	midiMenu->addBreak();
	midiMenu->addBreak();

	//make it a for loop
	for (int i = 0; i < midiOutputs.size(); i++) {
		midiMenu->addLabel("output " + ofToString(i))->setBackgroundColor(ofColor(150, 0, 0));
		midiMenu->addToggle("active " + ofToString(i));
		midiMenu->addDropdown("variable to send " + ofToString(i), variables);
		midiMenu->addTextInput("channel " + ofToString(i), ofToString(midiOutputs[i].first));
		midiMenu->addTextInput("cc " + ofToString(i), ofToString(midiOutputs[i].second));

		midiMenu->addBreak();
		midiMenu->addBreak();
		midiMenu->addBreak();
	}
	
    midiMenu->onButtonEvent(this, &ofApp::onButtonEvent);
    midiMenu->onDropdownEvent(this, &ofApp::onDropdownEvent);
	midiMenu->onTextInputEvent(this, &ofApp::onTextInputEvent);

    
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
    serialMenu->addValuePlotter("x", -20.0f, 20.0f);
    serialMenu->addValuePlotter("y", -20.0f, 20.0f);
    serialMenu->addValuePlotter("z", -20.0f, 20.0f);
	serialMenu->addTextInput("HEADING", "");


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

	//callback to set chanel and stuff
	for (int i = 0; i < midiOutputs.size(); i++) {
		midiOutputs[i].first = ofToInt(midiMenu->getTextInput("channel "  + ofToString(i))->getText());
		midiOutputs[i].second = ofToInt(midiMenu->getTextInput("cc " + ofToString(i))->getText());
	}
}

