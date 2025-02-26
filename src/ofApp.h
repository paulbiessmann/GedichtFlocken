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

struct groupParticles {
    vector  <customParticle> p;
    ofVec3f vPosVerse;
    ofColor vColorVerse;
    int     vNum;
    ofImage versesImg;
    float   uniqueVal;
    ofVec3f vel;
    bool    bInit;
    float   relVerseAge;
};

class ofApp : public ofBaseApp{
  public:
	void setup();
	void update();
	void draw();
    void exit();
    void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
    
    void resetSchnipsel();
    void resetParticles();
    void initParticles(vector <customParticle>  &pThis, ofImage &imgThis, ofColor &colorThis);
    void initFullTexParticles(vector <customParticle> &pThis, vector <ofImage> &imgThis);
    void initSnowFlakes(vector <customParticle> &pThis, ofImage &imgThis);
    void initTexVecs();
    
    void updateFullVerses();
    void drawFullVerses(int startNum);

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
    ofLight light2;
    
    vector <customParticle> p;
    
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
    float dirZ = 0;
    
    vector <ofVec3f> texVecPosDraw;
    
    int   iColorUpdate = 0;
    int   iAlphaUpdate = 220;

    
    unsigned long   recordedFrame;
    float   relativeFr;
    int     gPUpdateSize;

    float   waitCounter     = 0;
    bool    bPause = false;
    
    bool    bFirstCallScene3 = true;
    
};
