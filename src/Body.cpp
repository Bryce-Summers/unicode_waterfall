#include "Body.h"

Body::Body()
{
}


Body::~Body()
{
}

void Body::resolve_collision(Body * other)
{
    // Switch the order if necessary to guranteed that b1 is a dynamic object.
    if (!this -> isDynamic())
    {
        return;
    }

    /*ASSUMPTION: this body has moved along velocity and penetrated b2.
    * We resolve the collision as follows:
    *
    * velocity and angular momentum come from the body.
    * position and orientation come from the Collidable.
    *
    * 0. Extract all of the relevant physical and geometric quantities.
    * 1. Move b1 backwards along its velocity until there is a separating
    *    axis perpendicular to the velocity. (IDEA: Or revert bodies to previous timestep.
    * 2. Cast rays from points on this to sides of other and point of other to sides of this.
    * 3. Find the shortest ray - collision point.
    * 4. Apply an equal and opposite force on each of the bodies to change their velocities and angular momentums.
    */

    // 0.
    
    // Used as the direction of collision.
    ofVec2f velocity = this -> getTranslationalVelocity();

    Collidable * c1 = this  -> getCollidable();
    Collidable * c2 = other -> getCollidable();

    float separation_dist = 1;

    // 1. separate, then update synch this body's position with the collidable.
    c1 -> separateFromOther(c2, -velocity, separation_dist);
    this -> updatePositionFromCollidable();

    // 2, 3.
    CollideInfo info;
    c1 -> computeFuturePenetrationLocation(c2, velocity, &info);

    this -> updateDynamics(info, other);

}

void Body::updateDynamics(CollideInfo & info, Body * body2)
{
    Body * body1 = this;
    
    // Extract all relevant quantities,
    // Then convert this into a single dimensional collision problem.
    ofVec2f position1 = info.location1;
    ofVec2f position2 = info.location2;
    ofVec2f velocity1 = body1 -> getVelocityAtPt(position1);
    ofVec2f velocity2 = body2 -> getVelocityAtPt(position2);

    ofVec2f normal = info.collision_normal;
    
    float v1 = velocity1.dot(normal);
    float v2 = velocity2.dot(normal);

    ofVec2f old_projected_velocity1 = v1*normal;
    ofVec2f old_projected_velocity2 = v2*normal;

    this -> solve1DRigidBodyCollision(body2, &v1, &v2);

    ofVec2f new_projected_velocity1 = v1*normal;
    ofVec2f new_projected_velocity2 = v2*normal;

    // Negate the original component and add the new projected componants.
    body1 -> addVelocityAtPt(-old_projected_velocity1 + new_projected_velocity1, position1);
    body2 -> addVelocityAtPt(-old_projected_velocity2 + new_projected_velocity2, position2);
    
}

// IN/OUT v1, v2, updates the given velocities from the results of the 1D collision
// between this and the other rigid body.
void Body::solve1DRigidBodyCollision(Body * other, float * v1_in_out, float * v2_in_out)
{
    // Resolve collision by satisfying the following Physical laws:
    // 1. conservation of momentum and 
    // 2. conservation of kinetic energy.
    float m1 = this  -> getMass();
    float m2 = other -> getMass();
    float v1 = *v1_in_out;
    float v2 = *v2_in_out;

    // Collision with a static object results in a full bounce.
    if (!(other -> isDynamic()))
    {
        *v1_in_out = -v1;
        return;
    }

    // Temporary renaming of velocity both are computed using the original values.
    float u1 = v2;
    float u2 = v2;

    // 0.0 - 1.0, what percentage of the momentum is conserved.
    float coef = this -> getRestitutionCoef();

    v1 = (coef*m2*(u2 - u1) + m1*u1 + m2*u2) / (m1 + m2);
    v2 = (coef*m1*(u1 - u2) + m2*u2 + m1*u1) / (m2 + m1);

    *v1_in_out = v1;
    *v2_in_out = v2;
}

bool Body::isDynamic()
{
    return this -> dynamic;
}

// Global Translational Velocity.
ofVec2f Body::getTranslationalVelocity()
{
    return this -> velocity;
}

float   Body::getAngularSpeed()
{
    return this -> angle_speed;
}

float   Body::getMass()
{
    return this -> mass;
}

ofVec2f Body::getCenterOfMass()
{
    return this -> position;
}

float   Body::getRestitutionCoef()
{
    return this -> restitution_coef;
}

// Returns the local velocity of a point on this rigid body at the given location
// = translational velocity + rotational velocity
ofVec2f Body::getVelocityAtPt(ofVec2f location)
{
    ofVec3f perp_velocity = getCurrentAngularVelocityAtPt(location);

    return this -> getTranslationalVelocity() + perp_velocity;
}

// Adds the given velocity to this body by projecting it onto the
// translational and angular components.
// NOTE: This is not virtual, because we can do the projection using the other virtual methods.
void Body::addVelocityAtPt(ofVec2f velocity, ofVec2f location)
{   
    ofVec2f translation_direction = this -> getTranslationalVelocity().getNormalized();
    
    ofVec2f delta_translate = velocity.dot(translation_direction) * translation_direction;

    ofVec2f delta_angle = velocity - delta_translate;

    this -> addTranslationalVelocity(delta_translate);
    this -> addRotationalVelocity(delta_angle, location);

}

ofVec2f Body::getCurrentAngularVelocityAtPt(ofVec2f location)
{
    ofVec2f center_to_location = location - getCenterOfMass();

    float radius = center_to_location.length();
    float circumference = 2 * radius*PI;

    ofVec2f perp = ofVec2f(-center_to_location.y, center_to_location.x).normalize();

    // distance/angle * (angle / time)
    float perp_speed = (circumference / (2 * PI)) * this -> angle_speed;
    ofVec2f perp_velocity = perp * perp_speed; // Signed by angle_velocity.
    
    return perp_velocity;
}

void Body::addTranslationalVelocity(ofVec2f velocity)
{
    this -> velocity += velocity;
}

void Body::addRotationalVelocity(ofVec2f velocity, ofVec2f location)
{
    ofVec2f center_to_location = location - getCenterOfMass();
        
    float radius = center_to_location.length();
    float circumference = 2 * radius*PI;

    float angle_per_dist = 1.0/radius;//circumference / 2*PI;

    // Want angle / time, have distance/time, distance, angle/dist
    float delta_angular_speed = angle_per_dist*velocity.length();

    // If negative angular direction, invert delta speed.
    ofVec2f perp = ofVec2f(-center_to_location.y, center_to_location.x).normalize();
    
    if (perp.dot(velocity) < 0)
    {
        delta_angular_speed = -delta_angular_speed;
    }

    angle_speed += delta_angular_speed;
        
}