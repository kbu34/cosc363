/*----------------------------------------------------------
* COSC363  Ray Tracer
*
* The sphere class
* This is a subclass of Object, and hence implements the
* methods intersect() and normal().
------------------------------------------------------------ - */

#include "Cylinder.h"
#include <math.h>

/**
* Sphere's intersection method.  The input is a ray.
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir)
{
    float a = (((dir.x) * (dir.x)) + ((dir.z) * (dir.z)));
    float b = 2 * (dir.x * (p0.x - center.x) + dir.z * (p0.z - center.z));
    float c = ((p0.x - center.x) * (p0.x - center.x)) + ((p0.z - center.z) * (p0.z - center.z)) - (radius * radius);

    float delta = (b * b) - (4 * a * c);

    if (fabs(delta) < 0.001) return -1.0;
    if (delta < 0.0) return -1.0;

    float t1 = (-b + sqrt(delta)) / (2 * a);
    float t2 = (-b - sqrt(delta)) / (2 * a);

    if (fabs(t1) < 0.001)
    {
        if (t2 > 0) return t2;
        else t1 = -1.0;
    }
    if (fabs(t2) < 0.001) t2 = -1.0;

    if (t1 < t2) {
        if (t1 <= 0) {
            return t2;
        }
        glm::vec3 inter = p0 + (t1 * dir);
        if (inter.y > (center.y + height) || inter.y < center.y) {
            glm::vec3 secInter = p0 + (t2 * dir);
            if (secInter.y < (center.y + height) && secInter.y > center.y) {
                return t2;
            }
            else {
                return -1.0;
            }
        }
    }
    else {
        if (t2 <= 0) {
            return t1;
        }
        glm::vec3 inter = p0 + (t2 * dir);
        if (inter.y > (center.y + height) || inter.y < center.y) {
            glm::vec3 secInter = p0 + (t1 * dir);
            if (secInter.y < (center.y + height) && secInter.y > center.y) {
                return t1;
            }
            else {
                return -1.0;
            }
        }
    }

    return (t1 < t2) ? t1 : t2;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3((p.x - center.x) / radius, 0.0, (p.z - center.z) / radius);
    n = glm::normalize(n);
    return n;
}
