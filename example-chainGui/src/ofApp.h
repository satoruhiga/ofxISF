#pragma once

#include "ofMain.h"
#include "ofxISF.h"
#include "ofxISF/Gui.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void dragEvent(ofDragInfo dragInfo);
    
    ofxISF::Chain chain;
    ofxISF::Gui gui;
    ofVideoGrabber video;
};
