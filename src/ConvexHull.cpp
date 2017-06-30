#include "ConvexHull.h"



ConvexHull::ConvexHull(ofPolyline pline)
{
    this -> pline = pline;
    center_position = this -> pline.getCentroid2D();
}


ConvexHull::~ConvexHull()
{
}

void ConvexHull::getSeparatingAxisesNormals(vector<ofVec2f> results)
{
    int len = pline.size();
    for (int i = 0; i < len; i++)
    {
        ofPoint a = pline[i];
        ofPoint b = pline[(i + 1) % len];

        ofVec3f direction = b - a;
        ofVec2f normal = ofVec2f(-direction.y, direction.x);
        results.push_back(normal);
    }
    
    return;
}

// Returns the farthest distance from the center point on this object
// to any other point on the object as measured by the project onto the given direction.
// Used in separating axis theorem calculations.
float ConvexHull::getRadiusAlongDirection(ofVec2f direction)
{
    // Search for the maximum projected radius among all points.
    float max_value = 0;

    for (auto iter = pline.begin(); iter != pline.end(); ++iter)
    {
        ofVec2f pt = *iter;
        
        float projected_radius = getProjectedLength(pt, center_position, direction);
        max_value = max(max_value, projected_radius);
    }

    return max_value;
}

ofVec2f ConvexHull::getCenterPoint()
{
    return center_position;
}

// Returns true iff this collidable is capable of movement.
bool ConvexHull::isConvex()
{
    return true;
}

// Returns the axis aligned bounding box for this collidable object.
// Assumed to be in standard form, which the position at its top left corner.
ofRectangle ConvexHull::getBoundingBox()
{
    AABB box = AABB();

    for(auto iter = pline.begin(); iter != pline.end(); ++iter)
    {
        box.expandByPt(*iter);
    }

    return box.getBoundingBox();
}

void ConvexHull::draw()
{
    ofSetColor(0, 0, 0, 255);
    this -> pline.draw();
}