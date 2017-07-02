#pragma once

#include "ofMain.h"

#include "CollideHeaders.h"

/*
 * Convex Polyline defined region.
 * Written by Bryce Summers on Jun.29th.2017
 */

class ConvexHull : public Collidable
{
public:
    ConvexHull(ofPolyline pline);
    ~ConvexHull();

private:
    ofPolyline pline;
    ofVec2f center_position;

public:

    // -- Methods from Collidable.

    // Returns the farthest distance from the center point on this object
    // to any other point on the object as measured by the project onto the given direction.
    // Used in separating axis theorem calculations.
    virtual float getRadiusAlongDirection(ofVec2f direction);

    virtual ofVec2f getCenterPoint();
    virtual void setCenterPoint(ofVec2f pos);

    // Returns true iff this collidable is capable of movement.
    virtual bool isConvex();

    // Returns the axis aligned bounding box for this collidable object.
    // Assumed to be in standard form, which the position at its top left corner.
    virtual ofRectangle getBoundingBox();

    virtual void draw();

    virtual void getAllPenetrationPoints(vector<ofVec2f> * output);
    virtual void getSeparatingAxisesNormals(vector<ofVec2f> * results);
};