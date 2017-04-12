#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    frame = 0;
}

//--------------------------------------------------------------
void ofApp::update(){

    // Test String.
    if (frame % 30 == 0)
    {
        letters.push_back(Letter(ofRandom(ofGetWidth()), -20, 10));
    }
    frame += 1;

    // Determine the amount of time that has elapsed since the last update call frame.
    float new_time = ofGetElapsedTimef();
    float dt       = new_time - time;
    time           = new_time;

    vector<list<Letter>::iterator> dead_letters;

    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter).update(dt);
        if ((*iter).isDead())
        {
            dead_letters.push_back(iter);
        }
    }

    // Remove all dead letters from the update list and deallocate them.
    for (auto iter = dead_letters.begin(); iter != dead_letters.end(); ++iter)
    {
        letters.erase(*iter);
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(ofColor(255));

    // Draw all of the letters to the screen.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter).draw();
    }
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

}
