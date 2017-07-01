#include "Collidable.h"
#include "OBB.h"


Collidable::Collidable()
{
}


Collidable::~Collidable()
{
}


// -- General Collision

bool Collidable::detect_collision_convex(Collidable * other)
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

    OBB other = OBB(x, y, hw, hh, 0);

    return this -> detect_collision_convex(&other);
}

float Collidable::getProjectedLength(ofVec2f pt, ofVec2f center, ofVec2f direction)
{
    ofVec2f offset = pt - center;
    float projectedLength = offset.dot(direction);
    return ABS(projectedLength);
}

bool Collidable::computeFuturePenetrationLocation(
    Collidable * other,
    ofVec2f direction,
    CollideInfo * output)
{
    /* 1. Extract all of the penetration points and segments.
    ** 2. For all points on this body (body1) cast a ray in the direction.
    **    For all points on body2, cast a ray in the opposite direction.
    ** 3. Store the minnimum. Average minnimums that are close.
    **
    **  Find location 1 and 2. Most likely a corner point and point in the middle of and edge.
    */

    Collidable * collide1 = this;   
    Collidable * collide2 = other;

    vector<ofVec2f> points1;
    vector<ofVec2f> points2;
    getAllPenetrationPoints(&points1);
    getAllPenetrationPoints(&points2);
    
    vector<LineSegment> segments1;
    vector<LineSegment> segments2;
    pointsToSegments(points1, &segments1);
    pointsToSegments(points2, &segments2);

    vector<Ray> rays1;
    vector<Ray> rays2;
    pointsToRays(points1, direction, &rays1);
    pointsToRays(points2, direction, &rays2);

    bool found_result = false;

    output -> time_till_collision = std::numeric_limits<float>::max();

    // Cast rays from 1 onto the segments of 2.
    found_result = found_result || findMinIntersection(rays1, segments2, output);

    // Cast rays from 2 onto the segments of 1.
    found_result = found_result || findMinIntersection(rays2, segments1, output);

    return found_result;
}

void pointsToRays(vector<ofVec2f> & points, ofVec2f direction,  vector<Ray> * output)
{
    for (ofVec2f pt : points)
    {
        output -> push_back(Ray(pt, direction));
    }
}

void Collidable::pointsToSegments(vector<ofVec2f> & points, vector<LineSegment> * output)
{
    int len = points.size();
    for (int i = 0; i < len; i++)
    {
        LineSegment seg = LineSegment(points[i], points[(i + 1) % len]);
        output -> push_back(seg);
    }
}

bool findMinIntersection(vector<Ray> & rays, vector<LineSegment> & segments, CollideInfo * info)
{


    // 6.30.2017 FIXME:
    // do the minnimization. cast every ray onto all segments.
    // FIXME: only cast onto segments pointing on the front side towards the rays.





    // Start out with no time found.
    float min_time = info -> time_till_collision;
    for (Ray ray : rays)
    {
        for (LineSegment seg : segments)
        {
            bool getIntersectionTime(float * time_out, OrientedHyperplane * other);
        }
    }

    return min_time > 0;
}