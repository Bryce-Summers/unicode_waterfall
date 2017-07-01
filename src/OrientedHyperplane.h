#pragma once

#include "ofMain.h"

class OrientedHyperplane
{
public:
    OrientedHyperplane();
    ~OrientedHyperplane();

    // -- Subclass interface.

    virtual ofVec2f getPoint1() = 0;
    virtual ofVec2f getPoint2() = 0;

    virtual ofVec2f getDirection() = 0;

    // Returns true iff the given time parameterized by 0 = pt1, 1 = pt1 + direction
    // is considered to be in this hyperplane.
    // for instance lines always return true,
    // segments that include their endpoints return true iff 0 <= time <= 1, 
    // and direction is the entire offset from pt0 to pt1.
    virtual bool timeInBounds(float time) = 0;



// -- General Interface.
public:

    /*
     * Returns < 0 if pt is to the 'left' of this oriented hyperplane.
     * Returns = 0 if pt is on this hyperplane.
     * Returns > 0 if pt is to the 'right' of this hyperplane.
     */
    float line_side_test(ofVec2f pt);


    // Returns the normal of this hyperplane.
    // The orientation is chosen with most positive projected direction 
    // from points on this hyperplane towards the given target pt.
    // FIXME: if we ever extend this to linesegment to 3d point, we'll need to extend
    //        this function to choose the best normal from a circle of possibilities.
    ofVec2f getNormal(ofVec2f target);

    // Hyper Plane Intersection.
    // returns true iff successful.
    // populates output value if it returned true.
    bool getIntersectionTime(float * time_out, OrientedHyperplane * other);
    bool getIntersectionPt(ofVec2f * pt_out, OrientedHyperplane * other);

};
