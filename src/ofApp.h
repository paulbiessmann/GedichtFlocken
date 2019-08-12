#pragma once

#include "ofMain.h"
#include "customParticle.h"
#include "ofxPostProcessing.h"
#include "ofxVideoRecorder.h"


class ofApp : public ofBaseApp{
  public:
	void setup();
	void update();
	void draw();
    void exit();
    
    void resetParticles();
    void initParticles(vector <customParticle>  &pThis, ofImage &imgThis);
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	vector<ofVec3f> boxPositions;
	vector<float> boxSizes;
	vector<ofColor> boxColors;
	
	//ofxDOF depthOfField;
	ofEasyCam cam;
    
    ofImage text;
    
    bool    isNew = false;
    int     drawSize;
    
    int     num;
    vector <int> vNum;
    
    bool    bInit = false;
    vector <ofVec3f> vPosVerse;
    
    ofxPostProcessing post;
    ofLight light;
    
    vector <customParticle> p;
    vector <customParticle> p1;
    vector <customParticle> p2;
    vector <customParticle> p3;
    vector <customParticle> p4;
    vector <customParticle> p5;
    vector <customParticle> p6;
    
    int particleResolution;
    
    vector <ofImage> versesImg;
    
    ofImage kuppelGrid;
    
    /* Recording */
    ofxVideoRecorder    vidRecorder;
    
    bool                bRecording;
    ofFbo               recordFbo;
    ofFbo               recordFboFlip;
    ofPixels            recordPixels;
    
    
    string fileName;
    string fileExt;
    
    void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
    
    
	// shader glitch
    ofShader    shaderGlitch;
    ofFbo       screenData;
    
    int fullWidth = 3840;
    int fullHeight = 2160;
    
};
