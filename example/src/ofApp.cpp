#include "ofMain.h"

#include "ofxISF.h"

class ofApp : public ofBaseApp
{
public:
	
	ofxISF::Shader isf;
	ofVideoGrabber video;
	
	void setup()
	{
		ofSetFrameRate(60);
		ofSetVerticalSync(true);
		ofBackground(0);
		
		video.initGrabber(1280, 720);
		
		isf.setup(1280, 720, GL_RGB32F);
		isf.load("isf-test.fs");
		
		isf.setImage("inputImage", video.getTextureReference());
	}
	
	void update()
	{
		video.update();
		
		float t = ofGetElapsedTimef() * 2;
		isf.setParam<float>("blurAmount", ofNoise(1, 0, 0, t) * 1.5);
		
		isf.update();
	}
	
	void draw()
	{
		isf.draw(0, 0);
	}

	void keyPressed(int key)
	{
		isf.load("isf-test.fs");
	}

	void keyReleased(int key)
	{
		
	}
	
	void mouseMoved(int x, int y)
	{
	}

	void mouseDragged(int x, int y, int button)
	{
	}

	void mousePressed(int x, int y, int button)
	{
	}

	void mouseReleased(int x, int y, int button)
	{
	}
	
	void windowResized(int w, int h)
	{
	}
};

int main(int argc, const char** argv)
{
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp);
	return 0;
}
