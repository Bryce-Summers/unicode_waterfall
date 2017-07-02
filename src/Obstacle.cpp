#include "Obstacle.h"


Obstacle::Obstacle(ofPolyline pline, Grid * grid)
{
    this -> pline = pline;
    this -> pline.setClosed(true);

    // FIXME: compute the convex hull of the obstacle.
    ofPolyline bounds = convexHull(pline);
    this -> collidable = new ConvexHull(bounds);
    //this -> collidable = this -> collidable -> getAABB();
    
    this -> pline.setClosed(true);

    grid -> add_to_collision_grid(this);
}

Obstacle::~Obstacle()
{
    //delete collidable;
}

void Obstacle::draw()
{
    ofSetColor(0, 0, 0, 255);
    ofFill();
    this -> pline.draw();

    this-> collidable -> draw();
}

ofPolyline Obstacle::convexHull(ofPolyline pline)
{
    cout << "Obstacle::convexhull, implement me!" << endl;
    return pline;
}

void Obstacle::updatePositionFromCollidable()
{
    this -> position = collidable -> getCenterPoint();
}

// Obstacles don't revert, because they are inert.
void Obstacle::revertToPrevious()
{
    return;
}