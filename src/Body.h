#pragma once

/* Body class. (Abstract base class)
 * Written by Bryce Summers on 06 - 29 - 2017
 *
 * Specifies objects that have Collidable geometry
 * coupled with physical attributes such as velocity and mass.
 *
 */
//
// 
//

#include "Collidable.h"

class Body
{
public:
    Body();
    ~Body();

    virtual bool isDynamic() = 0;
    virtual Collidable * getCollidable() = 0;

};

