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
    ofParameterGroup parameters;
};
