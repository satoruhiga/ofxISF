#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    video.initGrabber(1280, 720);
    
    isf.setup(1280, 720, GL_RGB32F);
    isf.load("Circular Screen");
    isf.setImage("inputImage", video.getTexture());

    setupGui();
}

//--------------------------------------------------------------
void ofApp::update(){
    for (int i = 0; i < floatInputs.size(); i++) {
        string name = floatInputs[i].name;
        float value = floatInputs[i].value;
        isf.setUniform<float>(name, value);
    }
    for (int i = 0; i < vec2fInputs.size(); i++) {
        string name = vec2fInputs[i].name;
        ofVec2f value = vec2fInputs[i].value;
        isf.setUniform<ofVec2f>(name, value);
    }
    for (int i = 0; i < colorInputs.size(); i++) {
        string name = colorInputs[i].name;
        ofFloatColor value = colorInputs[i].value;
        isf.setUniform<ofFloatColor>(name, value);
    }
    
    video.update();
    isf.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    isf.draw(0, 0);
    gui.draw();
}

void ofApp::setupGui(){
    gui.setup(isf.getName());
    for (int i = 0; i < isf.getInputs().size(); i++) {
        string name = isf.getInputs().getUniform(i)->getName();
        string type;
        isf.getInputs().getUniform(i)->useNoralizedValue = true;
        if (isf.getInputs().getUniform(i)->isTypeOf<float>()) {
            type = "float";
            inputParameter<float> param;
            param.name = name;
            gui.add(param.value.set(param.name, 0.5, 0, 1));
            floatInputs.push_back(param);
        } else if (isf.getInputs().getUniform(i)->isTypeOf<ofVec2f>()){
            type = "ofVec2f";
            inputParameter<ofVec2f> param;
            param.name = name;
            gui.add(param.value.set(param.name,ofVec2f(ofGetWidth()*.5,ofGetHeight()*.5),ofVec2f(0,0),ofVec2f(ofGetWidth(),ofGetHeight())));
            vec2fInputs.push_back(param);
        } else if (isf.getInputs().getUniform(i)->isTypeOf<ofFloatColor>()){
            type = "ofFloatColor";
            inputParameter<ofFloatColor> param;
            param.name = name;
            gui.add(param.value.set(param.name, ofFloatColor(0.5, 0.5, 0.5, 1.0), ofFloatColor(0., 0., 0., 0.), ofFloatColor(1., 1., 1., 1.)));
            colorInputs.push_back(param);
        } else if (isf.getInputs().getUniform(i)->isTypeOf<ofTexture*>()){
            type = "ofTexture";
        } else if (isf.getInputs().getUniform(i)->isTypeOf<bool>()){
            type = "bool";
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    for(unsigned int i = 0; i < dragInfo.files.size(); i++){
        string extention = dragInfo.files[i].substr(dragInfo.files[i].length()-3, dragInfo.files[i].length());
        string shaderName = dragInfo.files[i].substr(0,dragInfo.files[i].length()-3);
        if (extention == ".fs" || extention == ".vs") {
            isf.load(shaderName);
            gui.clear();
            setupGui();
        }
    }
}
