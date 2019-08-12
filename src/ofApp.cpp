#include "ofApp.h"

float fps = 25;

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetBackgroundColor(255);
    ofEnableAlphaBlending();
    
    
    
/************ Video Record *********/
    ofSetFrameRate((int) fps);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    fileName = "Flocken";
    fileExt = ".mov"; // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats
    
    // override the default codecs if you like
    // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
    vidRecorder.setVideoCodec("prores");
    vidRecorder.setVideoBitrate("2000k");
    
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    
    recordFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    recordFboFlip.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    bRecording = false;
    
   // keyReleased('r');

/** end recording **/
    

    kuppelGrid.load("Tiefsee_UV_GRID_1.tif");
    
    vPosVerse.resize(6);
    vNum.resize(6);
    versesImg.resize(6);
    versesImg[0].load("Strophen/1.png");
    versesImg[1].load("Strophen/2.png");
    versesImg[2].load("Strophen/3.png");
    versesImg[3].load("Strophen/4.png");
    versesImg[4].load("Strophen/5.png");
    versesImg[5].load("Strophen/6.png");
        
    float imgScaleFac = 5;
    particleResolution = 5;

    
    for (int i = 0; i<versesImg.size(); i++){
        int oldWidth   = versesImg[i].getWidth();
        int oldHeight  = versesImg[i].getHeight();
        
        versesImg[i].resize(oldWidth / imgScaleFac, oldHeight / imgScaleFac);
        vNum[i] = (versesImg[i].getWidth() / particleResolution) * (versesImg[i].getHeight()/particleResolution);
        
        vPosVerse[i] = ofVec3f((ofGetWidth()/8)*(i+1), ofRandom((ofGetHeight()/2) - 200, (ofGetHeight()/2) + 200), ofRandom(-100,100));
        
    }
    
   
    p1.assign(vNum[0], customParticle());
    p2.assign(vNum[1], customParticle());
    p3.assign(vNum[2], customParticle());
    p4.assign(vNum[3], customParticle());
    p5.assign(vNum[4], customParticle());
    p6.assign(vNum[5], customParticle());

    resetParticles();

    
/*  shader glitch  */
   // shaderGlitch.load("glitch1Shader");
    
}
//--------------------------------------------------------------
void ofApp::initParticles(vector <customParticle> &pThis, ofImage &imgThis){
    int textWidth   = imgThis.getWidth();
    int textHeight  = imgThis.getHeight();
    ofPixels pixels = imgThis.getPixels();
    int pCount = 0;
    
    
    
    //points
    if(pThis[0].drawMode == PARTICLE_MODE_POINTS){
        for(unsigned int i = 0; i < textWidth - particleResolution; i += particleResolution){
            for(unsigned int j = 0; j < textHeight - particleResolution; j += particleResolution){
    
                pThis[pCount].setStartingTime(ofGetElapsedTimef()+2.0);
                
                ofColor pxColor = pixels.getColor(i, j);
    
                pThis[pCount].setColor(pxColor);
                pThis[pCount].setPos(ofVec3f(i, j, ofRandom(-100,100)));
                pThis[pCount].setParticleSize(particleResolution);
                pCount++;
    
            }
        }
    }
    else if(pThis[0].drawMode == PARTICLE_MODE_TEXTURES){ //textures
        for(unsigned int i = 0; i <= textWidth-particleResolution ; i += particleResolution){
            for(unsigned int j = 0; j <= textHeight-particleResolution; j += particleResolution){
                
                pThis[pCount].setStartingTime(ofGetElapsedTimef() + 2.0);

                ofColor pxColor = pixels.getColor(i, j);
                ofImage pxImage = imgThis;
                pxImage.crop(i,j,particleResolution,particleResolution);
                
                pThis[pCount].setParticleImg(pxImage);
                pThis[pCount].setPos(ofVec3f(i, j, ofRandom(-100,100)));
                pThis[pCount].setParticleSize(particleResolution);
                pCount++;
            }
        }
        
    }

}
//--------------------------------------------------------------
void ofApp::resetParticles(){
  
    for(unsigned int i = 0; i < p1.size(); i++){
        p1[i].reset();
        p1[i].setGlobalPos(vPosVerse[0]);
    }
    for(unsigned int i = 0; i < p2.size(); i++){
        p2[i].reset();
        p2[i].setGlobalPos(vPosVerse[1]);
    }
    for(unsigned int i = 0; i < p3.size(); i++){
        p3[i].reset();
        p3[i].setGlobalPos(vPosVerse[2]);
    }
    for(unsigned int i = 0; i < p4.size(); i++){
        p4[i].reset();
        p4[i].setGlobalPos(vPosVerse[3]);
    }
    for(unsigned int i = 0; i < p5.size(); i++){
        p5[i].reset();
        p5[i].setGlobalPos(vPosVerse[4]);
    }
    for(unsigned int i = 0; i < p6.size(); i++){
        p6[i].reset();
        p6[i].setGlobalPos(vPosVerse[5]);
    }
    
    
//    for(unsigned int i = 0; i < p.size(); i++){
//        p[i].reset();
//    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
	
	//swim the depth of field
	//depthOfField.setFocalDistance(ofMap(sin(ofGetElapsedTimef()/2),-1,1, 20, 150));
    
//    if(drawSize >= p.size()){
//        drawSize = p.size();
//    }else if(ofGetFrameNum() % 20 == 0){
//        drawSize++;
//    }
//
    for(unsigned int i = 0; i < p1.size(); i++){
        p1[i].update();
    }
    for(unsigned int i = 0; i < p2.size(); i++){
        p2[i].update();
    }
    for(unsigned int i = 0; i < p3.size(); i++){
        p3[i].update();
    }
    for(unsigned int i = 0; i < p4.size(); i++){
        p4[i].update();
    }
    for(unsigned int i = 0; i < p5.size(); i++){
        p5[i].update();
    }
    for(unsigned int i = 0; i < p6.size(); i++){
        p6[i].update();
    }
    
//    for(unsigned int i = 0; i < p.size(); i++){
//        p[i].update();
//    }
    
    recordFbo.getTexture().readToPixels(recordPixels);
    if(bRecording){
        bool success = vidRecorder.addFrame(recordPixels);
        if (!success) {
            ofLogWarning("This frame was not added!");
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    
    recordFbo.begin();
    ofClear(0,0,0,255);
    ofSetBackgroundColor(ofColor(255,255,255,255));

    ofSetColor(255);
    ofDrawRectangle(0,0,fullWidth, fullHeight);
    kuppelGrid.draw(0,0, fullWidth, fullHeight);
    
    
    float t = ofGetElapsedTimef();
   
    if(0){
    cout << "frame " << ofGetFrameNum() << "\n";
    }
    
    if(ofGetFrameNum() < 20  ){
        versesImg[0].draw(vPosVerse[0]);
        versesImg[1].draw(vPosVerse[1]);
        versesImg[2].draw(vPosVerse[2]);
        versesImg[3].draw(vPosVerse[3]);
        versesImg[4].draw(vPosVerse[4]);
        versesImg[5].draw(vPosVerse[5]);
    }else{
        if(!bInit){
            initParticles(p1, versesImg[0]);
            initParticles(p2, versesImg[1]);
            initParticles(p3, versesImg[2]);
            initParticles(p4, versesImg[3]);
            initParticles(p5, versesImg[4]);
            initParticles(p6, versesImg[5]);
            bInit = true;
        }
        //versesImg[0].draw(0,0);

        for(unsigned int i = 0; i < p1.size()-particleResolution; i++){
            p1[i].draw();
        }
        for(unsigned int i = 0; i < p2.size()-particleResolution; i++){
            p2[i].draw();
        }
        for(unsigned int i = 0; i < p3.size()-particleResolution; i++){
            p3[i].draw();
        }
        for(unsigned int i = 0; i < p4.size()-particleResolution; i++){
            p4[i].draw();
        }
        for(unsigned int i = 0; i < p5.size()-particleResolution; i++){
            p5[i].draw();
        }
        for(unsigned int i = 0; i < p6.size()-particleResolution; i++){
            p6[i].draw();
        }
    }
    
    

    
    recordFbo.end();
    
    recordFbo.draw(0,0);
    
   
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
//    unsigned idx = key - '0';
//    if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());
//
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

//    if(key == 'n'){
//        boxPositions.push_back(ofVec3f(ofRandom(-300, 300),
//                                       ofRandom(0, 300),
//                                       ofRandom(-800, -500)));
//        boxSizes.push_back(ofRandom(30, 100));
//        boxColors.push_back(ofColor(255));
//
//        num++;
//        p.assign(num, customParticle());
//
//    }
    
    if(key=='r'){
        bRecording = !bRecording;
        if(bRecording && !vidRecorder.isInitialized()) {
            //insert your path in the following row and your settings 1920x1080 60fps - the last 2 parms are zero because in my case I don't want to record audio..
            ofFilePath f;
            vidRecorder.setup( fileName+"_"+ofGetTimestampString()+".mov", recordFbo.getWidth(),recordFbo.getHeight(), (int) fps, 0, 0);
            
            //Start recording
            vidRecorder.start();
        }
        else if(!bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(true);
        }
        else if(bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(false);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::exit(){
    ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    vidRecorder.close();
}

//--------------------------------------------------------------
void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
    cout << "The recoded video file is now complete." << endl;
}
