#include "Ray.h"



Ray::Ray(ofVec2f pos, ofVec2f direction)
{
    this -> position  = pos;
    this -> direction = direction.normalized();
}


Ray::~Ray()
{
}

ofVec2f Ray::getPoint1()
{
    return this -> position;
}

ofVec2f Ray::getPoint2()
{
    return this -> position + this -> direction;
}

// Returns true iff this ray intersects the given line segment.
bool Ray::detect_intersection(LineSegment seg)
{
    // Based on BDS.js Ray.coffee
    float side1 = this -> line_side_test(seg.getPoint1());
    float side2 = this -> line_side_test(seg.getPoint2());

    ofVec2f normal = seg.getNormal(this -> position);
    
    // Ray shoots through the line, rather than away form it.
    // the line normal and ray direction should be going in opposition.
    bool correct_direction = normal.dot(this -> direction) < 0;

    // intersection if the ray goes towards the line segment and is on the proper side.
    return side1*side2 <= 0 && correct_direction;
}

// Returns the distance to the given segment.
// Returns -1 if the given line segment is not intersected.
float Ray::dist_to_intersection(LineSegment seg)
{
    // Reduces this to the standard oriented hyperplane intersection test.
    float output;
    if (this -> getIntersectionTime(&output, &seg))
    {
        return output;
    }

    return -1;
}

ofVec2f Ray::getDirection()
{
    return this -> direction;
}

bool Ray::timeInBounds(float time)
{
    return time >= 0;
}

ofVec2f Ray::getPointAtTime(float time)
{
    return this -> position + this -> direction*time;
}