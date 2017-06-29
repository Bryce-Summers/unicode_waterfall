#include "Obstacle.h"


Obstacle::Obstacle(ofPolyline bounds, Grid * grid)
{
    this -> pline = bounds;

    this -> pline.setClosed(true);

    grid -> add_to_collision_grid(this);
}


Obstacle::~Obstacle()
{
}

void Obstacle::draw()
{
    ofSetColor(0, 0, 0, 255);
    ofFill();
    this -> pline.draw();
}

bool Obstacle::detect_collision_with_rectangle(ofRectangle rect)
{
    
    return getBoundingBox().intersects(rect);
}

// Returns true iff this collidable is capable of movement.
bool Obstacle::isDynamic()
{
    return false;
}

// Updates this object's internal physics values to point 
// it away from the direction of collision.
bool Obstacle::resolve_collision(ofVec3f direction)
{
    // Static objects like obstacles do not move.
    return false;
}

ofRectangle Obstacle::getBoundingBox()
{
    return pline.getBoundingBox();
}