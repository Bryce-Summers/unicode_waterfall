#include "Collidable.h"
#include "OBB.h"


Collidable::Collidable()
{
}


Collidable::~Collidable()
{
}


// -- General Collision

bool Collidable::detect_collision_concave(Collidable * other)
{
    // This collision detection check doesn't work without convex objects.
    if (isConvex() || other-> isConvex())
    {
        return false;
    }

    // Determine an exhastive list of directions that we will test separations on.
    vector<ofVec2f> directions;

    this  -> getSeparatingAxisesNormals(directions);
    other -> getSeparatingAxisesNormals(directions);

    
    // Precompute center points and distances.
    ofVec2f center_a = this  -> getCenterPoint();
    ofVec2f center_b = other -> getCenterPoint();

    float separation_distance = center_a.distance(center_b);

    // Test all axis for a separation hyperplane.
    for (auto iter = directions.begin(); iter != directions.end(); ++iter)
    {
        ofVec2f direction = *iter;
        float ra = this -> getRadiusAlongDirection(direction);
        float rb = this -> getRadiusAlongDirection(direction);

        // ASSUMPTION: ra >= 0 and rb >= 0

        // With enough separation, convex objects are gurranteed not to intersect.
        if (ra + rb < separation_distance)
        {
            return false;
        }
    }
    
    return true;
}

bool Collidable::detect_collision_with_rectangle(ofRectangle rect)
{
    float x = rect.getLeft();
    float y = rect.getTop();
    
    float hw = rect.getWidth()/2;
    float hh = rect.getHeight()/2;

    OBB other = OBB(ofVec2f(x, y), hw, hh, 0);

    return this -> detect_collision_concave(&other);
}

float Collidable::getProjectedLength(ofVec2f pt, ofVec2f center, ofVec2f direction)
{
    ofVec2f offset = pt - center;
    float projectedLength = offset.dot(direction);
    return ABS(projectedLength);
}