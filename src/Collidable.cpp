#include "CollideHeaders.h"

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
    if (!isConvex() || !other-> isConvex())
    {
        return false;
    }

    // Determine an exhastive list of directions that we will test separations on.
    vector<ofVec2f> directions;

    this  -> getSeparatingAxisesNormals(&directions);
    other -> getSeparatingAxisesNormals(&directions);

    
    // Precompute center points and distances.
    ofVec2f center_a = this  -> getCenterPoint();
    ofVec2f center_b = other -> getCenterPoint();

    float separation_distance = center_a.distance(center_b);

    // Test all axis for a separation hyperplane.
    for (auto iter = directions.begin(); iter != directions.end(); ++iter)
    {
        ofVec2f direction = *iter;
        float ra = this  -> getRadiusAlongDirection(direction);
        float rb = other -> getRadiusAlongDirection(direction);

        // ASSUMPTION: ra >= 0 and rb >= 0

        // With enough separation, convex objects are gurranteed not to intersect.
        if (ra + rb < separation_distance)
        {
            return false;
        }
        else
        {
            continue;
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

bool Collidable::computeFuturePenetrationLocation(Collidable * other, ofVec2f direction, CollideInfo * output)
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
    other -> getAllPenetrationPoints(&points2);
    
    vector<LineSegment> segments1;
    vector<LineSegment> segments2;
    pointsToSegments(points1, &segments1);
    pointsToSegments(points2, &segments2);

    vector<Ray> rays1;
    vector<Ray> rays2;
    pointsToRays(points1, direction, &rays1);
    pointsToRays(points2, direction, &rays2);

    bool found_result = false;

    // Start the collisions at infinity.
    output -> time_till_collision = std::numeric_limits<float>::max();

    // Cast rays from 1 onto the segments of 2.
    found_result = found_result || findMinIntersection(rays1, segments2, output, true);

    // Cast rays from 2 onto the segments of 1.
    found_result = found_result || findMinIntersection(rays2, segments1, output, false);

    return found_result;
}

void  Collidable::pointsToRays(vector<ofVec2f> & points, ofVec2f direction,  vector<Ray> * output)
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

bool  Collidable::findMinIntersection(vector<Ray> & rays, vector<LineSegment> & segments, CollideInfo * info, bool from_location1)
{
    // 6.30.2017 FIXME:
    // do the minnimization. cast every ray onto all segments.
    // FIXME: only cast onto segments pointing on the front side towards the rays.


    ofVec2f * from_location; // Location on ray collider.
    ofVec2f * to_location;   // Lcoation on segments collider.

    if (from_location1)
    {
        from_location = &(info -> location1);
        to_location   = &(info -> location2);
    }
    else
    {
        from_location = &(info -> location2);
        to_location   = &(info -> location1);
    }

    bool found = false;

    // Start out with no time found.
    float min_time = info -> time_till_collision;
    float epsilon = .01;
    float new_time;
    for (Ray ray : rays)
    {
        for (LineSegment seg : segments)
        {
            // Update Intersection info if found.

            if (ray.getIntersectionTime(&new_time, &seg) && new_time < min_time + epsilon)
            {                
                // Collision are of roughly equal distance,
                // So it is probably an edge - edge collision,
                // So we average the points together to get the collision point
                // Closer to the center of mass.
                if (abs(min_time - new_time) < epsilon)
                {
                    *from_location = (ray.getPoint1() + *from_location) / 2;
                    *to_location   = (ray.getPointAtTime(new_time) + *to_location) / 2;
                }
                else // Point - Segment collision
                {
                    *from_location = ray.getPoint1();
                    *to_location   = ray.getPointAtTime(new_time);
                    info -> collision_normal = seg.getNormal(ray.getPoint1());
                }

                min_time = new_time;
                found = true;
            }
        }
    }

    info -> time_till_collision = min_time;
    return found;
}

void Collidable::separateFromOther(Collidable * other, ofVec2f direction, float separation_distance)
{
    ofVec2f pt_this  = this  -> getCenterPoint();
    ofVec2f pt_other = other -> getCenterPoint();

    float desired_distance = this -> getRadiusAlongDirection(direction) +
                             other -> getRadiusAlongDirection(direction) + separation_distance;
    float current_distance = abs((pt_this - pt_other).dot(direction));
    
    // The distance we need to move to be at minnimum the desired distance away.
    float delta_distance = max(0.0f, desired_distance - current_distance);  

    ofVec2f offset = delta_distance * direction;
    ofVec2f new_position = this -> getCenterPoint() + offset;
    this -> setCenterPoint(new_position);
}