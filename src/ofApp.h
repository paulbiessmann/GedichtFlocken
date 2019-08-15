#pragma once

#include "ofMain.h"
#include "customParticle.h"
#include "ofxPostProcessing.h"
#include "ofxVideoRecorder.h"

enum sceneMode{
    MODE_EXPLODE,
    MODE_SNOW,
    MODE_TEX_GLITCH
};

class ofApp : public ofBaseApp{
  public:
	void setup();
	void update();
	void draw();
    void exit();
    
    void resetSchnipsel();
    void resetParticles();
    void initParticles(vector <customParticle>  &pThis, ofImage &imgThis);
    void initFullTexParticles(vector <customParticle> &pThis, vector <ofImage> &imgThis);
    void initSnowFlakes(vector <customParticle> &pThis, ofImage &imgThis);
    void initTexVecs();
    
	
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
    
    bool    bInitSchnipsel;
    int     numFullTexParticles;
    vector <ofImage> schnipselImgs;
    vector <ofImage> versesImg;
    
    vector <customParticle> pSnowFlakes;
    ofImage snowFlake;
    
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
    
    sceneMode mode;
    
	// shader glitch
    ofShader    shaderGlitch;
    ofFbo       screenData;
    
    int fullWidth = 3840;
    int fullHeight = 2160;
//    int fullWidth = 1920;
//    int fullHeight = 1080;
    
    
    bool    bUpdateExplode;
    bool    bUpdateSnow;
    
    bool    bEnd;
    
    
    
/* Tex Vec */
    ofTexture tex;
    
    vector <ofTexture> texVecs;
    vector <ofColor> colTexVecs;
    
    vector <int> texVecsPosX;
    vector <int> texVecsPosY;
    vector <int> texVecsPosZ;
    
    int texVecSize;
    int texVecNum;
    
    bool bTexVec = false;
    
    int growing = 0;
    
    ofVec2f center;
    ofFbo   texVecGetter;
    float dirX = 0;
    float dirY = 0;
    
    vector <ofVec3f> texVecPosDraw;
    
    int recordedFrame;
    
    
};
