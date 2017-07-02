#pragma once

#include "ofMain.h"
#include "Collidable.h"

class AABB : Collidable
{
public:
    AABB();
    ~AABB();

private:
    // I like the min - max version, because it makes expanding a box more intuitive.
    ofVec2f min;
    ofVec2f max;

// Mutation Functions.
public:

void expandByPt(ofVec2f pt);


// Methods from Collidable.
public:

    // Returns the farthest distance from the center point on this object
    // to any other point on the object as measured by the project onto the given direction.
    // Used in separating axis theorem calculations.
    // REQUIRES: direction should be of length 1.
    virtual float getRadiusAlongDirection(ofVec2f direction);

    virtual ofVec2f getCenterPoint();
    virtual void setCenterPoint(ofVec2f pos);

    // Returns true iff this collidable is capable of movement.
    virtual bool isConvex();

    // Returns the axis aligned bounding box for this collidable object.
    // Assumed to be in standard form, which the position at its top left corner.
    virtual ofRectangle getBoundingBox();
    
    // Draws rectangle in world space.
    virtual void draw();

    // Returns a list of all points that may be tested for penetrations.
    virtual void getAllPenetrationPoints(vector<ofVec2f> * output);
    virtual void getSeparatingAxisesNormals(vector<ofVec2f> * results);

};