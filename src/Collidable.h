#pragma once

#include "ofMain.h"

/*
 * Base Class for collision capable objects.
 * Written by Bryce Summers on Jun.27.2017
 *
 * Features:
 *  - add/remove capability from Grid object.
 *  - non-aligned rectangle - rectangle collision.
 *  - Polygon - rectangle collision.
 *
 * Since this is meant to be used for letters, we can collide arbitrary obstacles off of axis aligned bounding boxes.
 */

class Collidable
{
public:
    Collidable();
    virtual ~Collidable();

// -- Collision functions.

// Returns whether this object is in collision with the given rectangle.
// FIXME: Think about orientation.
virtual bool detect_collision_with_rectangle(ofRectangle rect) = 0;

// Returns true iff this collidable is capable of movement.
virtual bool isDynamic() = 0;

// Updates this object's internal physics values to point 
// it away from the direction of collision.
virtual bool resolve_collision(ofVec3f direction) = 0;

};