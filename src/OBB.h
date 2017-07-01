#pragma once

#include "UWF.h"

/* Oriented Bounding Box (OBB)
 * Written by Bryce Summers on Jun.29.2017
 *
 * Purpose: Allows for accurate collision detection for rotating boxes, such as textures.
 */

class OBB : public Collidable
{
public:
    OBB(float x, float y,
       float width_half = 0,
       float height_half = 0,
       float angle_start = 0);
    ~OBB();

    // -- Data Fields.
    ofVec2f center_position;
    float radius_right;// x direction.
    float radius_up;   // y direction.

    ofVec2f right_direction;
    ofVec2f up_direction;

    bool dynamic;

// Mutation fields.
public:
    // Orients this OBB to the given angle specified in radians.
    void setToAngle(float angle);
    void setToPosition(float x, float y);
    void updatePositionRotation(float x, float y, float angle);

public:
    virtual void draw();
    void draw(float x, float y, float angle);
    // -- Relevant convex information methods from Collidable super class.

    virtual void getSeparatingAxisesNormals(vector<ofVec2f> results);

    // Returns the farthest distance from the center point on this object
    // to any other point on the object as measured by the project onto the given direction.
    // Used in separating axis theorem calculations.
    virtual float getRadiusAlongDirection(ofVec2f direction);

    virtual ofVec2f getCenterPoint();

    // Returns true iff this collidable is capable of movement.
    virtual bool isDynamic();
    virtual bool isConvex();

    // Returns the axis aligned bounding box for this collidable object.
    // Assumed to be in standard form, which the position at its top left corner.
    virtual ofRectangle getBoundingBox();


private:
    
    // Returns the polyline of the collision boundary in world space.
    void toPolyline(ofPolyline & output);

};
