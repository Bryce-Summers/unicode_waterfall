#include "ConvexHull.h"



ConvexHull::ConvexHull(ofPolyline pline)
{
    this -> pline = ofPolyline();

    for (ofPoint p : pline)
    {
        this -> pline.addVertex(p);
    }
    this -> pline.setClosed(true);
    center_position = this -> pline.getCentroid2D();
}


ConvexHull::~ConvexHull()
{
}

void ConvexHull::getSeparatingAxisesNormals(vector<ofVec2f> * results)
{
    int len = pline.size();
    for (int i = 0; i < len; i++)
    {
        ofPoint a = pline[i];
        ofPoint b = pline[(i + 1) % len];

        ofVec3f direction = b - a;
        ofVec2f normal = ofVec2f(-direction.y, direction.x).normalized();
        results -> push_back(normal);
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

void ConvexHull::setCenterPoint(ofVec2f pos)
{
    // Udpates the center point and manually translates the polyline.
    // FIXME: Alternatively I could mutate the polyline vertex values by a transform matrix when
    //        methods are called.
    ofVec2f offset = pos - this -> center_position;
    this -> center_position = pos;
    
    int len = pline.size();
    for (int i = 0; i < len; i++)
    {
        this -> pline[i] += offset;
    }
    
    return;
}

void ConvexHull::getAllPenetrationPoints(vector<ofVec2f> * output)
{
    for (ofPoint pt : pline)
    {
        output -> push_back(ofVec2f(pt.x, pt.y));
    }
}