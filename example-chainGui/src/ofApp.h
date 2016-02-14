#pragma once

#include "ofMain.h"
#include "ofxISF.h"
#include "ofxGui.h"
#include "ISFGuiRenderer.hpp"

/*
class ParamGui {
public:
    vector<InputParameter<float>> floatInputs;
    vector<InputParameter<ofVec2f>> vec2fInputs;
    vector<InputParameter<ofFloatColor>> colorInputs;
};
 */

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void setupGui();
    
    ofxISF::Chain chain;
    ofVideoGrabber video;
    ofxPanel gui;
    //ofParameterGroup parameters;
    vector<ISFGuiRenderer> guiRenderer;

    /*
    vector<ofxPanel> guis;
    vector<inputParameter<float>> floatInputs;
    vector<inputParameter<ofVec2f>> vec2fInputs;
    vector<inputParameter<ofFloatColor>> colorInputs;
     */
};
