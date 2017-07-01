#include "CollideHeaders.h"

AABB::AABB()
{
    min.x = std::numeric_limits<float>::infinity();
    min.y = std::numeric_limits<float>::infinity();
    
    max.x = -std::numeric_limits<float>::infinity();
    max.y = -std::numeric_limits<float>::infinity();
}


AABB::~AABB()
{
}

// Expands the bounds of this AABB
// Useful for tight bounding box formation.
// FIXME: This is legible for the programmer,
//        but may be less efficient than directly iterating
//        over a collection of points.
void AABB::expandByPt(ofVec2f pt)
{
    min.x = MIN(pt.x, min.x);
    min.y = MIN(pt.y, min.y);

    max.x = MAX(pt.x, max.x);
    max.y = MAX(pt.y, max.y);
}


void AABB::getSeparatingAxisesNormals(vector<ofVec2f> results)
{
    // 2 unique normal axises. Takes symmetry into consideration.
    results.push_back(ofVec2f(1.0, 0));
    results.push_back(ofVec2f(0, 1.0));
}

// Returns the farthest distance from the center point on this object
// to any other point on the object as measured by the project onto the given direction.
// Used in separating axis theorem calculations.
// REQUIRES: direction should be of length 1.
float AABB::getRadiusAlongDirection(ofVec2f direction)
{
    ofVec2f pt0 = min;
    ofVec2f pt1 = ofVec2f(min.x, max.y);
    ofVec2f pt2 = ofVec2f(max.x, max.y);
    ofVec2f pt3 = ofVec2f(max.x, min.y);

    ofVec2f center = getCenterPoint();

    float max_val = 0;
    max_val = MAX(max_val, getProjectedLength(pt0, center, direction));
    max_val = MAX(max_val, getProjectedLength(pt1, center, direction));
    max_val = MAX(max_val, getProjectedLength(pt2, center, direction));
    max_val = MAX(max_val, getProjectedLength(pt3, center, direction));

    return max_val;
}

ofVec2f AABB::getCenterPoint()
{
    ofVec2f center = (min + max)/2;
    return center;
}

// Returns true iff this collidable is capable of movement.
bool AABB::isConvex()
{
    return true;
}

// Returns the axis aligned bounding box for this collidable object.
// Assumed to be in standard form, which the position at its top left corner.
ofRectangle AABB::getBoundingBox()
{
    float x = min.x;
    float y = min.y;
    float w = max.x - min.x;
    float h = max.y - min.y;

    return ofRectangle(x, y, w, h);
}


void AABB::draw()
{
    ofRectangle bounds = this -> getBoundingBox();
    ofSetColor(0, 0, 0, 255);
    ofRect(bounds);
}


// Returns a list of all points that may be tested for penetrations.
void AABB::getAllPenetrationPoints(vector<ofVec2f> * output)
{
    // Counter - Clockwise.
    ofVec2f pt0 = min;
    ofVec2f pt1 = ofVec2f(min.x, max.y);
    ofVec2f pt2 = max;
    ofVec2f pt3 = ofVec2f(max.x, min.y);

    output -> push_back(pt0);
    output -> push_back(pt1);
    output -> push_back(pt2);
    output -> push_back(pt3);
}

void AABB::setCenterPoint(ofVec2f pos)
{
    ofVec2f current_center_point = (min + max)/2;
    ofVec2f offset = pos - current_center_point;

    min += offset;
    max += offset;
}