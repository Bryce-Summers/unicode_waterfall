#include "OrientedHyperplane.h"

OrientedHyperplane::OrientedHyperplane()
{

}


OrientedHyperplane::~OrientedHyperplane()
{
}

float OrientedHyperplane::line_side_test(ofVec2f pt)
{
    ofVec2f p1 = this -> getPoint1();
    ofVec2f p2 = this -> getPoint2();
    return ((p2.x - p1.x)*(pt.y - p1.y) - (p2.y - p1.y)*(pt.x - p1.x));
}

ofVec2f OrientedHyperplane::getNormal(ofVec2f target)
{
    ofVec2f pt1 = this -> getPoint1();
    ofVec2f pt2 = this -> getPoint2();

    ofVec2f offset = pt2 - pt1;
    offset.normalize();
    ofVec2f normal = ofVec2f(-offset.y, offset.x);

    // Flip the normal if it is travelling away from the target.
    ofVec2f toTarget = target - pt1;
    float proj = toTarget.dot(normal);
    if (proj < 0)
    {
        normal = - normal;
    }

    return normal;
}

bool OrientedHyperplane::getIntersectionPt(ofVec2f * pt_out, OrientedHyperplane * other)
{
    float time_out;
    if (this -> getIntersectionTime(&time_out, other))
    {
        ofVec2f pt1       = this -> getPoint1();
        ofVec2f direction = this -> getDirection();
        ofVec2f isect_pt = pt1 + time_out*direction;
        *pt_out = isect_pt;
        return true;
    }

    return false;
}

bool OrientedHyperplane::getIntersectionTime(float * time_out, OrientedHyperplane * other)
{
    // -- Find the intersection point.

    /*
    u = ((bs.y - as.y) * bd.x - (bs.x - as.x) * bd.y) / (bd.x * ad.y - bd.y * ad.x)
    v = ((bs.y - as.y) * ad.x - (bs.x - as.x) * ad.y) / (bd.x * ad.y - bd.y * ad.x)
    Factoring out the common terms, this comes to :
    dx = bs.x - as.x
    dy = bs.y - as.y
    det = bd.x * ad.y - bd.y * ad.x
    u = (dy * bd.x - dx * bd.y) / det
    v = (dy * ad.x - dx * ad.y) / det
    */

    // Extract the relevant points.
    // s = source, d is direction.
    ofVec2f as = this  -> getPoint1();
    ofVec2f bs = other -> getPoint1();
    ofVec2f ad = this  -> getDirection();
    ofVec2f bd = other -> getDirection();

        
    float dx = bs.x - as.x;
    float dy = bs.y - as.y;
    float det = bd.x * ad.y - bd.y * ad.x;

    // Collinear.
    if (det == 0)
    {
        return false; // FIXME: Change this if collinearity needs to be handled.
    }

    float u = (dy * bd.x - dx * bd.y) / det;
    float v = (dy * ad.x - dx * ad.y) / det;

    // Elliminate all collisions that are in the negative portion of one of the rays.

    // The point needs to be at a proper position for both hyperplanes.
    // This allows us to genarically intersect lines, segments, and rays.
    if (!(this -> timeInBounds(u)) || !(other -> timeInBounds(v)))
    {
        return false;
    }

    // Successful output.
    *time_out = u;
    return true;
}