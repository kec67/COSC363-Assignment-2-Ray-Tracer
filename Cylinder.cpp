/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir).
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{

    float a = (dir.x *dir.x) + (dir.z * dir.z);

    float b = 2 * ((dir.x *(posn.x - center.x) + (dir.z * (posn.z - center.z))));
    float c = ((posn.x - center.x) * (posn.x - center.x)) + ((posn.z - center.z) * (posn.z - center.z)) - (radius * radius);

    float delta = b*b -(4 * a * c) ;

    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta)) / (2*a);

    float t2 = (-b + sqrt(delta)) / (2*a);

    float t;

    if(fabs(t1) < 0.001)
    {
        if (t2 > 0) t = t2;
        else t = -1.0;
    }
    if(fabs(t2) < 0.001 ) t = -1.0;
    t = (t1 < t2)? t1: t2;

    float m = posn.y + dir.y * t;

    return (center.y <= m) && (m <= height + center.y)? t : -1.0;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 d = p - center;
        glm::vec3 n = glm::vec3 (d.x, 0, d.z);
        n = glm::normalize(n); //normalize
        return n;
}
