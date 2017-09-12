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
#include "grid.h"

class Grid;

class Body
{
public:
    Body(Grid * grid);// Values should be set by the subclasses if needed.
    ~Body();

// Dynamics quantities.
private:
    bool collidable = true;

protected:
   
    Grid * grid;

    float mass;
    float restitution_coef;


    // -- Translational Physics dynamics.
    ofVec2f position; // Location of the center of mass.
    ofVec2f velocity; // Translational velocity at the center of mass.
    ofVec2f acceleration; // Translational acceleration at center of mass.

    ofVec2f previous_position;
    float   previous_angle;

    // -- Rotational Physics Dynamics.

    // Planar orientation in radians.
    float angle_speed;
    float angle;
    bool dynamic;

public:
    bool isCollidable();
    void deactivateCollider();
    void activateCollider();

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

    // ENSURES: does nothing if this is static, rather than dynamic.
    // Update's this body's dynamics vectors.
    // Reverts it to its previous position.
    void resolve_collision(Body * other);

    void updateDynamics(CollideInfo & info, Body * other);

    // IN/OUT v1, v2, updates the given velocities from the results of the 1D collision
    // between this and the other rigid body.
    void solve1DRigidBodyCollision(Body * other, float * v1, float * v2);

    // Reverts this body to its last position and angle.
    // Only required if the body is dynamic.
    void revertToPrevious();
    virtual void updateCollidableFromPosition() = 0;

private:
    // Moves the body that has rammed the other backwards in time in preparation for a collision point search.
    // Returns true if this is the body that has been moved.
    // REturns false if the other body has been moved.
    bool separatePenetratingBody(Body * other);
    // Moves this body such that the pt at the old location has been translated to the new_location.
    void moveBody(ofVec2f old_location, ofVec2f new_location);
};

