#pragma once

#include "ofMain.h"
#include "OrientedHyperplane.h"

class LineSegment : public OrientedHyperplane
{
public:
    LineSegment(ofVec2f pt1, ofVec2f pt2);
    ~LineSegment();


private:
    ofVec2f pt1;
    ofVec2f pt2;


// -- Super Class interface.
public:

    virtual ofVec2f getPoint1();
    virtual ofVec2f getPoint2();

    virtual ofVec2f getDirection();

    // 0 <= time <= 1.
    // For out purposes, collisions at endpoints are important for OBB - OBB intersections.
    virtual bool timeInBounds(float time);
};