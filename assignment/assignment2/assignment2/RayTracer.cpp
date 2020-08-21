/*==================================================================================
* COSC 363  Computer Graphics (2020)
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab07.pdf, Lab08.pdf for details.
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cylinder.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "TextureBMP.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

vector<SceneObject*> sceneObjects;


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(1);						//Background colour = (0,0,0)
	glm::vec3 lightPos1(-100, 160, -3);					//Light's position
    glm::vec3 lightPos2(150, 140, -10);
	glm::vec3 color(0);
    glm::vec3 ambient(0.2);
	SceneObject* obj;
    float z1(-90);
    float z2(-200);

    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found

    if (ray.index == 4)
    {
        //Stripe pattern
        int stripeWidth = 5;
        int iz = (ray.hit.z) / stripeWidth;
        int k = iz % 2; //2 colors

        //chequered pattern
        int ix = (ray.hit.x) / stripeWidth;
        int u = ix % 2;
        if (k != 0 && u != 0) {
            color = glm::vec3(0.4, 0.8, 0.5);
            if (ix < 0) {
                color = glm::vec3(0.3, 0.1, 0.5);
            }
        }
        else if (k == 0 && u != 0) {
            color = glm::vec3(0.3, 0.1, 0.5);
            if (ix < 0) {
                color = glm::vec3(0.4, 0.8, 0.5);
            }
        }
        else if (k == 0) {
            color = glm::vec3(0.4, 0.8, 0.5);
            if (ix < 0) {
                color = glm::vec3(0.3, 0.1, 0.5);
            }
        }
        else {
            color = glm::vec3(0.3, 0.1, 0.5);
            if (ix < 0) {
                color = glm::vec3(0.4, 0.8, 0.5);
            }
        }
        if (ray.hit.x > -5 && ray.hit.x < 0) {
            color = glm::vec3(0.3, 0.1, 0.5);
            if (k != 0) {
                color = glm::vec3(0.4, 0.8, 0.5);
            }
        }
        obj->setColor(color);
    }
    if (ray.index >= 5 && ray.index < 11) {
        color = glm::vec3(0.1, 0.2, 0.1);
        obj->setColor(color);
    }


    color = obj->lighting(lightPos1, -ray.dir, ray.hit, 0.0, 0.0);						//Object's colour
    color = color + (obj->lighting(lightPos2, -ray.dir, ray.hit, 0.0, 0.0)) - (ambient * obj->getColor());  //add two lights' colours together

    glm::vec3 lightVec1 = (lightPos1) - ray.hit;
    glm::vec3 lightVec2 = (lightPos2) - ray.hit;
    Ray shadowRay1(ray.hit, lightVec1);
    Ray shadowRay2(ray.hit, lightVec2);

    shadowRay1.closestPt(sceneObjects);
    shadowRay2.closestPt(sceneObjects);

    //shadow calculation done using two light sources
    if (shadowRay1.index > -1 && shadowRay1.dist < glm::length(lightVec1)) {
        if (sceneObjects[shadowRay1.index]->isRefractive() || sceneObjects[shadowRay1.index]->isTransparent()) {
            ambient = glm::vec3(0.95);
        }
        color = obj->lighting(lightPos2, -ray.dir, ray.hit, 0.0, 0.0);
    }
    if (shadowRay2.index > -1 && shadowRay2.dist < glm::length(lightVec2)) {
        if (sceneObjects[shadowRay2.index]->isRefractive() || sceneObjects[shadowRay2.index]->isTransparent()) {
            ambient = glm::vec3(0.95);
        }
        color = (obj->lighting(lightPos1, -ray.dir, ray.hit, 0.0, 0.0));
    }
    if ((shadowRay1.index > -1 && shadowRay1.dist < glm::length(lightVec1)) && (shadowRay2.index > -1 && shadowRay2.dist < glm::length(lightVec2))) {
        if (sceneObjects[shadowRay2.index]->isRefractive() || sceneObjects[shadowRay2.index]->isTransparent()) {
            ambient = glm::vec3(0.95);
        }
        color = ambient * obj->getColor();
    }


    if (obj->isReflective() && step < MAX_STEPS)
    {
        float rho = obj->getReflectionCoeff();
        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
        Ray reflectedRay(ray.hit, reflectedDir);
        glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
        color = color + (rho * reflectedColor);
    }

    //colour calculation for transparent objects
    if (obj->isTransparent() && step < MAX_STEPS)
    {
        glm::vec3 transparentDir = ray.dir;
        Ray transparentRay(ray.hit, transparentDir);
        glm::vec3 transparentColor = trace(transparentRay, step + 1);
        color = color + transparentColor;
    }

    //colour calculation for refractive objects
    if (obj->isRefractive() && step < MAX_STEPS)
    {
        float rfi = obj->getRefractiveIndex();
        float eta = 1 / rfi;
        float cof = obj->getRefractionCoeff();
        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 g = glm::refract(ray.dir, normalVec, eta);
        Ray refrRay(ray.hit, g);
        refrRay.closestPt(sceneObjects);
        glm::vec3 m = obj->normal(refrRay.hit);
        glm::vec3 h = glm::refract(g, -m, 1.0f / eta);
        Ray realRay(refrRay.hit, h);
        glm::vec3 refractedColor = trace(realRay, step + 1);
        color = color + (cof * refractedColor);
    }
    float t = (float(ray.hit.z) - z1) / (z2 - z1);
    color = ((1 - t) * color) + (t * glm::vec3(1));
	return color;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for(int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

		    Ray ray = Ray(eye, dir);

		    glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}



//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

    Sphere *sphere1 = new Sphere(glm::vec3(-15.0, 2, -110.0), 12.0);
    sphere1->setColor(glm::vec3(0, 0.5, 0.65));   //Set colour to blue
    //sphere1->setSpecularity(false);
    sphere1->setReflectivity(true, 0.999);
    sceneObjects.push_back(sphere1);		 //Add sphere to scene objects

    //Sphere *sphere2 = new Sphere(glm::vec3(1.0, -6.5, -85.0), 6.0);
    //sphere2->setColor(glm::vec3(1, 0.3, 0.5));
    //sphere2->setReflectivity(true, 0.1);
    //sceneObjects.push_back(sphere2);		 //Add sphere to scene objects

    Sphere *sphere3 = new Sphere(glm::vec3(1.0, -6.5, -95.0), 6.0);
    sphere3->setColor(glm::vec3(0.75, 0.05, 0.1));
    sphere3->setRefractivity(true, 0.99, 1.01);
    sphere1->setReflectivity(true, 0.2);
    sceneObjects.push_back(sphere3);		 //Add sphere to scene objects

    Sphere *sphere4 = new Sphere(glm::vec3(10.0, -10.0, -65.0), 5.0);
    sphere4->setColor(glm::vec3(0, 0.2, 0));
    sphere4->setTransparency(true);
    sphere4->setReflectivity(true, 0.07);
    sceneObjects.push_back(sphere4);		 //Add sphere to scene objects

    Cylinder *cylinder1 = new Cylinder(glm::vec3(-13.0, -15, -85), 6.5, 5.0);
    cylinder1->setColor(glm::vec3(1.0, 0.0, 1.0));
    cylinder1->setReflectivity(true, 0.1);
    sceneObjects.push_back(cylinder1);


    Plane *plane = new Plane (glm::vec3(-75., -15, -40), //Point A
                              glm::vec3(75., -15, -40), //Point B
                              glm::vec3(75., -15, -200), //Point C
                              glm::vec3(-75., -15, -200)); //Point D
    plane->setSpecularity(false);
    sceneObjects.push_back(plane);

    //box construction
    Plane* boxBottom = new Plane(glm::vec3(-1, -14, -100),
        glm::vec3(20, -14, -100),
        glm::vec3(20, -14, -130),
        glm::vec3(-1, -14, -130));
    boxBottom->setSpecularity(false);
    sceneObjects.push_back(boxBottom);

    Plane* boxTop = new Plane(glm::vec3(-1, -2, -100),
        glm::vec3(20, -2, -100),
        glm::vec3(20, -2, -130),
        glm::vec3(-1, -2, -130));
    boxTop->setSpecularity(false);
    sceneObjects.push_back(boxTop);

    Plane* boxBack = new Plane(glm::vec3(-1, -14, -130),
        glm::vec3(20, -14, -130),
        glm::vec3(20, -2, -130),
        glm::vec3(-1, -2, -130));
    boxBack->setSpecularity(false);
    sceneObjects.push_back(boxBack);

    Plane* boxLeft = new Plane(glm::vec3(-1, -14, -130),
        glm::vec3(-1, -14, -100),
        glm::vec3(-1, -2, -100),
        glm::vec3(-1, -2, -130));
    boxLeft->setSpecularity(false);
    sceneObjects.push_back(boxLeft);

    Plane* boxRight = new Plane(glm::vec3(20, -2, -100),
        glm::vec3(20, -14, -100),
        glm::vec3(20, -14, -130),
        glm::vec3(20, -2, -130));
    boxRight->setSpecularity(false);
    sceneObjects.push_back(boxRight);

    //front has tiny bit of reflectivity for the consistency of the scene
    Plane* boxFront = new Plane(glm::vec3(-1, -14, -100),
        glm::vec3(20, -14, -100),
        glm::vec3(20, -2, -100),
        glm::vec3(-1, -2, -100));
    boxFront->setSpecularity(false);
    boxFront->setReflectivity(true, 0.05);
    sceneObjects.push_back(boxFront);

    Cylinder* cylinder2 = new Cylinder(glm::vec3(1.0, -15.0, -60.0), 2.0, 1.5);
    cylinder2->setColor(glm::vec3(0.1, 0.2, 0.5));
    cylinder2->setShininess(5);
    //cylinder2->setTransparency(true);
    cylinder2->setReflectivity(true, 0.08);
    sceneObjects.push_back(cylinder2);


}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
