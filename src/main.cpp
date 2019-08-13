#include "ofApp.h"

//--------------------------------------------------------------
int main(){
    ofGLWindowSettings settings;
    //settings.setGLVersion(3,2);
    settings.setGLVersion(2, 1);
//    settings.width = 1920;//1920;
//    settings.height = 1080;//1080;
    settings.width = 3840;
    settings.height = 2160;
    ofCreateWindow(settings);
	ofRunApp(new ofApp()); // start the app
}
