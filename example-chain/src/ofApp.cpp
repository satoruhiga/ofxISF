#include "ofMain.h"

#include "ofxISF.h"

class ofApp : public ofBaseApp
{
public:
	
	ofxISF::Chain chain;
	ofVideoGrabber video;
	
	void setup()
	{
		ofSetFrameRate(60);
		ofSetVerticalSync(true);
		ofBackground(0);
		
		video.initGrabber(1280, 720);
		
		chain.setup(1280, 720);
		chain.load("ZoomBlur");
        chain.load("CubicLensDistortion");
		
		chain.setImage(video.getTexture());
	}
	
	void update()
	{
		video.update();
		chain.update();
	}
	
	void draw()
	{
		chain.draw(0, 0);
	}
    
	void keyPressed(int key)
	{
		chain.setEnable("ZoomBlur", !chain.getEnable("ZoomBlur"));
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
