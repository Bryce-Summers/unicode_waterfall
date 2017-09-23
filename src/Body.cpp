#include "Body.h"

Body::Body(Grid * grid)
{
    this -> grid = grid;

    // Default Values.
    restitution_coef = 1.0;
    mass = 1.0;
    angle = 0;
    angle_speed = 0;
    dynamic = false;
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

    /*
    // Ignore static collisions for now.
    if(!other -> isDynamic())
    {
        return;
    }*/

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

    if (this -> position.x < 10)
    {
        //cout << "left" << endl;
    }

    Collidable * c1 = this  -> getCollidable();
    Collidable * c2 = other -> getCollidable();

    //float separation_dist = 1;

    // 1. separate, then update synch this body's position with the collidable.
    //c1 -> separateFromOther(c2, -movement_direction, separation_dist);
    //this -> revertToPrevious();
    //bool c1_moved = separatePenetratingBody(other);

    /*
    if(other -> isDynamic())
    {
        other -> revertToPrevious();
    }
    */

    //this -> updatePositionFromCollidable();

    // Revert positions.

    /*
    this  -> velocity = ofVec2f(0, 0);
    other -> velocity = ofVec2f(0, 0);
    */

    
    // 2, 3.
    CollideInfo info;
    /*
    if(c1_moved)
    {
        ofVec2f movement_direction = this -> velocity.normalized();
        c1 -> computeFuturePenetrationLocation(c2, movement_direction, &info);
        this -> moveBody(info.location1, info.location2 - movement_direction);
    }
    //else
    {
        // Compute theoretical point of collision.
        ofVec2f movement_direction = other -> velocity.normalized();
        c2 -> computeFuturePenetrationLocation(c1, movement_direction, &info);
        //other -> moveBody(info.location1, info.location2 - movement_direction);
    } */

    this -> revertToPrevious();

    ofVec2f movement_direction = this -> velocity.normalized();
    bool penetration_found = c1 -> computeFuturePenetrationLocation(c2, movement_direction, &info);

    other -> revertToPrevious();
 
    
    // Try the second one.
    if(!penetration_found)
    {
        // Revert back to test for collisions the other way around.
        this -> revertToPrevious();
        movement_direction = other -> velocity.normalized();
        penetration_found = c2 -> computeFuturePenetrationLocation(c1, movement_direction, &info);
        this -> revertToPrevious();
    }

    // this eliiminates elements that double reverse into restricted areas.
    this  -> erasePrevious();
    other -> erasePrevious();


    /* This is not working.
    // If everything fails, use a default collision specification.
    if (!penetration_found)
    {
        info.location1 = this -> position;
        info.location2 = other -> position;
    
        info.collision_normal = info.location2 - info.location1;
        info.time_till_collision = info.collision_normal.length();
        info.collision_normal /= info.time_till_collision;
        cout << "Third Option" << endl;
    }
    */
    

    // Udates both body's dynamics.
    this -> updateDynamics(info, other);
    
    // Resolve any new collisions the other may be experiencing.
    grid -> resolve_collisions(other);

}

void Body::revertToPrevious()
{
    ofVec2f pp = previous_position;
    float pa = previous_angle;

    // -- We swap the previous information,
    //    so that we can do bidirectional collision casting.
    previous_position = this -> position;
    previous_angle    = previous_angle;

    this -> position = pp;
    this -> angle    = pa;
}

void Body::erasePrevious()
{
    this -> previous_position = this -> position;
    this -> previous_angle    = this -> angle;
}

bool Body::separatePenetratingBody(Body * other)
{

    ofVec2f pt1 = this -> getCenterOfMass();
    ofVec2f pt2 = other -> getCenterOfMass();

    ofVec2f towardsOther = pt2 - pt2;
    towardsOther.normalize();

    float proj_1 = this -> getTranslationalVelocity().dot(towardsOther);
    float proj_2 = this -> getTranslationalVelocity().dot(-towardsOther);

    if (proj_1 < proj_2 || !other -> isDynamic())
    {
        //this -> revertToPrevious();
        // Move the object far back, because it will be collided soon.
        this -> moveBody(this -> position, this -> position - this -> velocity.normalized()*100);
        return true;
    }
    else
    {
        //other -> revertToPrevious();
        other -> moveBody(other -> position, other -> position - other -> velocity.normalized() * 100);
        return false;
    }
}

void Body::moveBody(ofVec2f old_location, ofVec2f new_location)
{
    grid -> remove_from_collision_grid(this);
    this -> position += new_location - old_location;
    this -> updateCollidableFromPosition();
    grid -> add_to_collision_grid(this);
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
    ofVec2f body1_away_from_body2 = body1 -> getCenterOfMass() - body2 -> getCenterOfMass();
    body1_away_from_body2.normalize()*4;
    ofVec2f body2_away_from_body1 = -body1_away_from_body2;
    body1 -> addVelocityAtPt(-old_projected_velocity1 + new_projected_velocity1 + body1_away_from_body2, position1);
    body2 -> addVelocityAtPt(-old_projected_velocity2 + new_projected_velocity2 + body2_away_from_body1, position2);
    
    body1 -> velocity.x *=10;

    // body 1 to second body.
    ofVec2f offset = position2 - position1;
    offset.normalize();

    body1 -> position -= offset*2;

    //offset *= 200;

    // Apply forces directly against the bodies.
    //body1 -> velocity -= offset;
    //body2 -> velocity +=  offset;
    
    

        
    if (!body2 -> isDynamic())
    {
       //body1 -> position.y -= 1;
    }
    
    if (body1 -> velocity.length() < 100)
    {
        body1 -> velocity.normalize();
        body1 -> velocity *= 100;
    }

    if (body2 -> isDynamic() && body2 -> velocity.length() < 100)
    {
        body2 -> velocity.normalize();
        body2 -> velocity *= 100;
    }

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
    float u1 = v1;
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
    /* 1st Attempt. Didn't go so well, because the objects don't bounce.
    ofVec2f translation_direction = this -> getTranslationalVelocity().getNormalized();
    
    ofVec2f delta_translate = velocity.dot(translation_direction) * translation_direction;

    ofVec2f delta_angle = velocity - delta_translate;

    this -> addTranslationalVelocity(delta_translate);
    this -> addRotationalVelocity(delta_angle, location);
    */

    // https://physics.stackexchange.com/questions/87484/effect-of-incoming-force-on-linear-vs-angular-velocity
    // Second attempt, now with forces acting as torque.

    ofVec2f center_to_location = location - this -> getCenterOfMass();
    center_to_location.normalize();

    ofVec2f line_of_action = velocity.normalized();

    // Decompose the vector from the center to the point of collision
    // Into the parts that are 
    ofVec2f par  = center_to_location.dot(line_of_action)*line_of_action;
    ofVec2f perp = center_to_location - par;

    ofVec2f translationalVelocity = velocity *.9;//.dot(par)*par;
    ofVec2f rotationalVelocity    = velocity *.1;// - translationalVelocity;//.dot(perp)*perp;
    this -> addTranslationalVelocity(translationalVelocity);
    this -> addRotationalVelocity(rotationalVelocity, location);
    
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

// adds the given velocity as a rotational component to this body at a location
// in world space.
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
    
    if (perp.dot(velocity) > 0)
    {
        delta_angular_speed = -delta_angular_speed;
    }

    angle_speed += delta_angular_speed;

}

void Body::deactivateCollider()
{
    this -> collidable = false;
}

void Body::activateCollider()
{
    this -> collidable = true;
}

bool Body::isCollidable()
{
    return collidable;
}