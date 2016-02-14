//
//  ISFGuiRenderer.hpp
//  example-chainGui
//
//  Created by Atsushi Tadokoro on 2/14/16.
//
//

#pragma once

#include "ofParameterGroup.h"
#include "ofParameter.h"
#include "ofMain.h"

/*
class ISFGuiRenderer;

template<typename ParameterType>
class ISFParam: public ofReadOnlyParameter<ParameterType,ISFGuiRenderer>{
    friend class ISFGuiRenderer;
};
*/


template <typename T>
class InputParameter {
public:
    string name;
    ofParameter<T> value;
};

class ISFGuiRenderer {
public:
    ISFGuiRenderer();
    
    void setup(string name);
    void draw();
    
    ofParameterGroup parameters;
    /*
    vector<InputParameter<float>> floatInputs;
    vector<InputParameter<ofVec2f>> vec2fInputs;
    vector<InputParameter<ofFloatColor>> colorInputs;
     */
};
