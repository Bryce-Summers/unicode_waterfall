#pragma once

#include "ofMain.h"

/*
 * Base Class for collision capable objects.
 * Written by Bryce Summers on Jun.27.2017
 *
 * Features:
 *  Specifies interfaces for performing convex, convex separation axis based collision detection.
 *  Provides interface for static and dynamic objects.
 *
 */

class Collidable
{
public:
    Collidable();
    virtual ~Collidable();

    // Draw function for debugging. Visualizes the bounding box.
    virtual void draw() = 0;


    // -- Collision functions.

    // -- Separating axis theorem convex polyhedra functions.
    
    // Returns true iff this and the other object are convex and
    // are statically colliding with each other.
    bool detect_collision_concave(Collidable * other);
    bool detect_collision_with_rectangle(ofRectangle rect); // Reduces rectangle to OBB.


    virtual void getSeparatingAxisesNormals(vector<ofVec2f> results) = 0;

    // Returns the farthest distance from the center point on this object
    // to any other point on the object as measured by the project onto the given direction.
    // Used in separating axis theorem calculations.
    // REQUIRES: direction should be of length 1.
    virtual float getRadiusAlongDirection(ofVec2f direction) = 0;

    virtual ofVec2f getCenterPoint() = 0;

    // Returns true iff this collidable is capable of movement.
    virtual bool isConvex() = 0;

    // Updates this object's internal physics values to point 
    // it away from the direction of collision.
    //virtual bool resolve_collision(ofVec3f direction) = 0;

    // Returns the axis aligned bounding box for this collidable object.
    // Assumed to be in standard form, which the position at its top left corner.
    virtual ofRectangle getBoundingBox() = 0;

protected:
    float getProjectedLength(ofVec2f pt, ofVec2f center, ofVec2f direction);

};