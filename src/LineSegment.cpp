#include "LineSegment.h"


LineSegment::LineSegment(ofVec2f pt1, ofVec2f pt2)
{
    this -> pt1 = pt1;
    this -> pt2 = pt2;
}


LineSegment::~LineSegment()
{
}

ofVec2f LineSegment::getPoint1()
{
    return this -> pt1;
}

ofVec2f LineSegment::getPoint2()
{
    return this -> pt2;
}

ofVec2f LineSegment::getDirection()
{
    return this -> getPoint2() - this -> getPoint1();
}

bool LineSegment::timeInBounds(float time)
{
    float epsilon = .01;
    return -0 - epsilon <= time && time <= 1.0 + epsilon;
}