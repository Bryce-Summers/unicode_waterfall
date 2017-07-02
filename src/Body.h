#pragma once

/* Body class. (Abstract base class)
 * Written by Bryce Summers on 06 - 29 - 2017
 *
 * Specifies objects that have Collidable geometry
 * coupled with physical attributes such as velocity and mass.
 *
 */

#include "ofMain.h"
#include "CollideHeaders.h"

class Body
{
public:
    Body();// Values should be set by the subclasses if needed.
    ~Body();

// Dynamics quantities.

protected:
   
    float mass;
    float restitution_coef;

    // -- Translational Physics dynamics.
    ofVec2f position; // Location of the center of mass.
    ofVec2f velocity; // Translational velocity at the center of mass.
    ofVec2f acceleration; // Translational acceleration at center of mass.

    // -- Rotational Physics Dynamics.

    // Planar orientation in radians.
    float angle_speed;
    float angle;
    bool dynamic;


public:
    virtual Collidable * getCollidable() = 0;

    // Updates this body's position from its collidable object.
    virtual void updatePositionFromCollidable() = 0;

    // Global Translational Velocity.
    bool isDynamic();
    ofVec2f getTranslationalVelocity();
    float   getAngularSpeed();
    float   getMass();
    ofVec2f getCenterOfMass();
    float   getRestitutionCoef();

    // Returns the local velocity of a point on this rigid body at the given location
    // = translational velocity + rotational velocity
    ofVec2f getVelocityAtPt(ofVec2f location);

    // Adds the given velocity to this body by projecting it onto the
    // translational and angular components.
    // NOTE: This is not virtual, because we can do the projection using the other virtual methods.
    void addVelocityAtPt(ofVec2f velocity, ofVec2f location);

    ofVec2f getCurrentAngularVelocityAtPt(ofVec2f location);

    void addTranslationalVelocity(ofVec2f velocity);
    void addRotationalVelocity(ofVec2f velocity, ofVec2f location);

    // Unpenetrates this body and changes its velocity based on 
    // the current collision withthe given other body
    // ENSURES: does nothing if this is static, rather than dynamic.
    void resolve_collision(Body * other);

    void updateDynamics(CollideInfo & info, Body * other);

    // IN/OUT v1, v2, updates the given velocities from the results of the 1D collision
    // between this and the other rigid body.
    void solve1DRigidBodyCollision(Body * other, float * v1, float * v2);

    // Reverts this body to its last position and angle.
    // Only required if the body is dynamic.
    virtual void revertToPrevious() = 0;
};

