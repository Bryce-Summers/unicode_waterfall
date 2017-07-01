#pragma once

// This order is meant to define Collidable first,
// So that it is availible as a base class for the various sub classes.

// Subclasses like OBB and ABB need to be inlcuded first, because they then
// include Collidable.h to define it first, before it tries to include and define the subclasses
// that are specified in its base class level functions.

#include "OBB.h"
#include "AABB.h"
#include "ConvexHull.h"
#include "CollideInfo.h"
#include "Collidable.h"