#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    video.initGrabber(1280, 720);
    
    chain.setup(1280, 720, GL_RGB32F);
    chain.setImage(video.getTexture());
    gui.setup();
    
    chain.load("isf-test");
    addGuiParameters();
}

//--------------------------------------------------------------
void ofApp::update(){
    for (int j = 0; j < guiParams.size(); j++) {
        for (int i = 0; i < guiParams[j].parameters.size(); i++) {
            string name = guiParams[j].parameters[i].getName();
            string type = guiParams[j].types[i];
            if (type == "float") {
                ofParameter<float> value = guiParams[j].parameters.getFloat(name);
                chain.getShader(j)->setUniform<float>(name, value);
            }
            if (type == "ofVec2f") {
                ofParameter<ofVec2f> value = guiParams[j].parameters.getVec2f(name);
                chain.getShader(j)->setUniform<ofVec2f>(name, value);
            }
            if (type == "ofFloatColor") {
                ofVec4f col = guiParams[j].parameters.getVec4f(name);
                ofParameter<ofFloatColor> value = ofFloatColor(col.x, col.y, col.z, col.w);
                chain.getShader(j)->setUniform<ofFloatColor>(name, value);
            }
        }
    }
    video.update();
    chain.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (chain.size() > 0) {
        ofSetColor(255);
        chain.draw(0, 0);
        gui.draw();
    }
}

void ofApp::addGuiParameters(){
    int j = chain.size() - 1;
    ISFGuiParameters gr;
    gr.setup(chain.getShader(j)->getName());
    for (int i = 0; i < chain.getShader(j)->getInputs().size(); i++) {
        string name = chain.getShader(j)->getInputs().getUniform(i)->getName();
        string type;
        chain.getShader(j)->getInputs().getUniform(i)->useNoralizedValue = true;
        if (chain.getShader(j)->getInputs().getUniform(i)->isTypeOf<float>())
        {
            type = "float";
            ofParameter<float> param;
            param.set(name, 0.5, 0.0, 1.0);
            gr.parameters.add(param);
            gr.types.push_back("float");
        }
        else if (chain.getShader(j)->getInputs().getUniform(i)->isTypeOf<ofVec2f>())
        {
            type = "ofVec2f";
            ofParameter<ofVec2f> param;
            param.set(name, ofVec2f(ofGetWidth()*.5,ofGetHeight()*.5), ofVec2f(0,0),ofVec2f(ofGetWidth(), ofGetHeight()));
            gr.parameters.add(param);
            gr.types.push_back("ofVec2f");
        }
        else if (chain.getShader(j)->getInputs().getUniform(i)->isTypeOf<ofFloatColor>())
        {
            type = "ofFloatColor";
            ofParameter<ofFloatColor> param;
            param.set(name, ofFloatColor(0.5, 0.5, 0.5, 1.0), ofFloatColor(0., 0., 0., 0.), ofFloatColor(1., 1., 1., 1.));
            gr.parameters.add(param);
            gr.types.push_back("ofFloatColor");
        }
        else if (chain.getShader(j)->getInputs().getUniform(i)->isTypeOf<bool>())
        {
            type = "bool";
            ofParameter<bool> param;
            param.set(name, true);
            gr.parameters.add(param);
            gr.types.push_back("bool");
        }
        else if (chain.getShader(j)->getInputs().getUniform(i)->isTypeOf<ofTexture*>())
        {
            type = "ofTexture";
        }
    }
    gui.add(gr.parameters);
    guiParams.push_back(gr);
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
            chain.load(shaderName);
            addGuiParameters();
        }
    }
}
