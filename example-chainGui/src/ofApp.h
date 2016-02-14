#pragma once

#include "ofMain.h"
#include "ofxISF.h"

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
