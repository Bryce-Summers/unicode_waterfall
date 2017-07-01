#include "OBB.h"

OBB::OBB(float x, float y,
  float width_half,
  float height_half,
  float angle_start)
{
    this -> radius_right    = width_half;
    this -> radius_up       = height_half;

    this -> setToPosition(x, y);
    this -> setToAngle(angle_start);
}


OBB::~OBB()
{
}

void OBB::updatePositionRotation(float x, float y, float angle)
{
    setToAngle(angle);
    setToPosition(x, y);
}

void OBB::setToAngle(float angle)
{
    float rx = cos(angle);
    float ry = sin(angle);

    float ux = cos(angle + PI/2);
    float uy = sin(angle + PI/2);

    right_direction = ofVec2f(rx, ry);
    up_direction    = ofVec2f(ux, uy);
}

void OBB::setToPosition(float x, float y)
{
    this -> center_position = ofVec2f(x, y);
}


void OBB::getSeparatingAxisesNormals(vector<ofVec2f> results)
{
    results.push_back(right_direction);
    results.push_back(up_direction);
}

// Returns the farthest distance from the center point on this object
// to any other point on the object as measured by the project onto the given direction.
// Used in separating axis theorem calculations.
float OBB::getRadiusAlongDirection(ofVec2f direction)
{
    // Maximum component projected radiuses.
    float proj_up   = abs(direction.dot(right_direction))*radius_right;
    float proj_down = abs(direction.dot(up_direction))*radius_up;

    return proj_up + proj_down;
}

ofVec2f OBB::getCenterPoint()
{
    return center_position;
}

// Returns true iff this collidable is capable of movement.
bool OBB::isDynamic()
{
    return dynamic;
}

bool OBB::isConvex()
{
    return true;
}

// Returns the Axis aligned bounding box that tightly fits this oriented box.
ofRectangle OBB::getBoundingBox()
{
    // Expand a rectangle with each of the 4 points.
    
    AABB box = AABB();

    ofPolyline pline;
    this -> toPolyline(pline);

    for(auto iter = pline.begin(); iter != pline.end(); ++iter)
    {
        box.expandByPt(*iter);
    }

    return box.getBoundingBox();
}

void OBB::toPolyline(ofPolyline & output)
{
    ofVec2f right = right_direction*radius_right;
    ofVec2f up    = up_direction*radius_right;
    ofVec2f left  = -right;
    ofVec2f down  = -up;

    ofVec2f p0 = center_position + left + up;
    ofVec2f p1 = center_position + right + up;
    ofVec2f p2 = center_position + right + down;
    ofVec2f p3 = center_position + left + down;

    output.addVertex(p0);
    output.addVertex(p1);
    output.addVertex(p2);
    output.addVertex(p3);

    output.setClosed(true);
}

// Official draw function, may be out of synch.
void OBB::draw()
{
    ofPolyline pline;
    this -> toPolyline(pline);

    ofSetColor(0, 0, 0, 255);
    pline.draw();    
  
}

void OBB::draw(float x, float y, float angle)
{

    ofVec2f center = ofVec2f(x, y);

    float rx = cos(angle);
    float ry = sin(angle);

    float ux = cos(angle + PI / 2);
    float uy = sin(angle + PI / 2);

    ofVec2f right = ofVec2f(rx, ry) * radius_right;
    ofVec2f up = ofVec2f(ux, uy)    * radius_up;
    ofVec2f left = -right;
    ofVec2f down = -up;

    ofVec2f p0 = center_position + left + up;
    ofVec2f p1 = center_position + right + up;
    ofVec2f p2 = center_position + right + down;
    ofVec2f p3 = center_position + left + down;

    ofPolyline pline;
    pline.addVertex(p0);
    pline.addVertex(p1);
    pline.addVertex(p2);
    pline.addVertex(p3);

    pline.setClosed(true);

    ofSetColor(0, 0, 0, 255);
    pline.draw();
}