#pragma once

#include "ofMain.h"
#include "LineSegment.h"
#include "Ray.h"
#include "CollideInfo.h"
#include "OBB.h"

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
    bool detect_collision_convex(Collidable * other);
    bool detect_collision_with_rectangle(ofRectangle rect); // Reduces rectangle to OBB.

    // REQUIRES: this object does not currently collide with the other.
    //           output should be default.
    bool computeFuturePenetrationLocation(Collidable * other, ofVec2f direction, CollideInfo * output);

    void pointsToSegments(vector<ofVec2f> & points, vector<LineSegment> * output);
    void pointsToRays(vector<ofVec2f> & points, ofVec2f direction, vector<Ray> * output);

    // Casts the rays onto the segments, updates info if a closer intersection is found.
    // If 2 roughly equally short locations are found they are averaged together.
    // returns true iff an intersection was found at a shorter time than the 1 info had when inputted.
    // REQUIRES: info should have a dummy time value set.
    bool findMinIntersection(vector<Ray> & rays, vector<LineSegment> & segments, CollideInfo * info, bool fromLocation1);

    // Returns a list of all points that may be tested for penetrations.
    virtual void getAllPenetrationPoints(vector<ofVec2f> * output) = 0;
    virtual void getSeparatingAxisesNormals(vector<ofVec2f> * results) = 0;

    // Moves this object in the given direction until their is a separating hyperplane between this
    // and the other object and the separation distance is as given.
    void separateFromOther(Collidable * other, ofVec2f direction, float separation_distance);

    // Returns the farthest distance from the center point on this object
    // to any other point on the object as measured by the project onto the given direction.
    // Used in separating axis theorem calculations.
    // REQUIRES: direction should be of length 1.
    virtual float getRadiusAlongDirection(ofVec2f direction) = 0;

    virtual ofVec2f getCenterPoint() = 0;
    virtual void setCenterPoint(ofVec2f pos) = 0; 

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