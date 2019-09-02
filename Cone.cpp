/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (pos, dir).
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float vx = posn.x - center.x;
    float vy = height - posn.y + center.y;
    float vz = posn.z - center.z;
    float tan = radius / height;

    float a = (dir.x * dir.x) + (dir.z * dir.z) -  ((tan * tan) * (dir.y * dir.y));
    float b = 2 * (dir.x * vx + dir.z * vz) + 2 * (tan * tan) * (dir.y * vy);
    float c = (vx * vx) + (vz * vz) - (tan * tan) * (vy * vy);


    float delta = b*b -(4 * a * c) ;

    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;




    float t1 = (-b - sqrt(delta)) / (2 * a);

    float t2 = (-b + sqrt(delta)) / (2 * a);

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
glm::vec3 Cone::normal(glm::vec3 p)
{
    glm::vec3 d = p - center;
    float r = sqrt(d.x * d.x + d.z * d.z);
    glm::vec3 n = glm::vec3 (d.x, r * (radius / height), d.z);
    n = glm::normalize(n); //normalize
    return n;
}
