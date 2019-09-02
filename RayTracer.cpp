/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer 
* Assignment 2
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/glut.h>
#include "Cylinder.h"
#include "Cone.h"
#include "Plane.h"
#include "TextureBMP.h"
using namespace std;

TextureBMP textureSphere, textureWall;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 800;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;


vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{

    glm::vec3 backgroundCol(0.2,0.2,0.2);

    glm::vec3 light(-10, 40, -3); //light source 1

    glm::vec3 light2(50, 50, 100); //light source 2

    glm::vec3 colorSum;

    glm::vec3 ambientCol(0.2);   //Ambient color of light

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour


    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); //normal vector


    glm::vec3 lightVector = light - ray.xpt;  //light vector for ray 1
    glm::vec3 normlightVector = glm::normalize(lightVector);

    glm::vec3 lightVector2 = light2 - ray.xpt;  //light vector for ray 2
    glm::vec3 normlightVector2 = glm::normalize(lightVector2);

    float lightDist = glm::length(lightVector);
    float lightDist2 = glm::length(lightVector2);

    float lDotn = glm::dot(normlightVector, normalVector);
    float lDotn2 = glm::dot(normlightVector2, normalVector);

    glm::vec3 reflVector = glm::reflect(-normlightVector, normalVector);   //reflection vector
    glm::vec3 reflVector2 = glm::reflect(-normlightVector2, normalVector);

    glm::vec3 viewVector = -ray.dir; //view vector

    float rDotv = glm::dot(reflVector, viewVector);
    float rDotv2 = glm::dot(reflVector2, viewVector);



    //------floor texture----
    if(ray.xindex == 3){
         int modx = (int)((ray.xpt.x + 50) /8) % 2;
         int modz = (int)((ray.xpt.z + 200) /8) % 2;

        if((modx && modz) || (!modx && !modz)){
            materialCol = glm::vec3(0.3,0.4,0.6);}
        else{
            materialCol = glm::vec3(0.6,0.7,1);
        }
    }


    //------sphere texture-----
    if(ray.xindex == 6){
        float s = normalVector.x /(2 * M_PI) + 0.7;
        float t = normalVector.y /(2 * M_PI) + 0.5;
        materialCol = textureSphere.getColorAt(s, t);

    }

    //----wall texture------
    if(ray.xindex == 7){
        float s = (ray.xpt.x + 60) / 110;
        float t = (ray.xpt.y + 20) / 70;
        return  textureWall.getColorAt(s, t);
    }


    //---cone texture-----
    if(ray.xindex == 5)
    {
        if ((int(ray.xpt.x+ray.xpt.y-12) % 2 == 0)){
            materialCol = glm::vec3(0,0.4,0.9);
        }else{
            materialCol = glm::vec3(0.5,0,0.0);
        }
    }


    //----shadow----
    Ray shadow(ray.xpt, normlightVector); // shadow 1
    shadow.closestPt(sceneObjects);

    Ray shadow2(ray.xpt,normlightVector2);    //shadow 2
    shadow2.closestPt(sceneObjects);



    float specularTerm;
    if(rDotv < 0) {
        specularTerm = 0.0;
    }
    else {
        specularTerm = pow(rDotv, 20);
    }

    float specularTerm2;
    if(rDotv2 < 0) {
        specularTerm2 = 0.0;
    }
    else {
        specularTerm2 = pow(rDotv2, 20);
    }


    if (lDotn2 <= 0)  {
        colorSum = ambientCol*materialCol;
    }

    if (lDotn <= 0)  {
        colorSum = ambientCol*materialCol;
    }

    //-----shadow of light 1------
    if ((shadow.xindex>-1) && (shadow.xdist <lightDist)) {
        colorSum = ambientCol*materialCol;

        if(shadow.xindex == 2){
                    colorSum += (lDotn * materialCol + specularTerm) * glm::vec3(0.6) + sceneObjects[2]->getColor()*glm::vec3(0.1);
                }

        if(shadow.xindex == 1){
                    colorSum += (lDotn * materialCol + specularTerm) * glm::vec3(0.6) + sceneObjects[1]->getColor()*glm::vec3(0.1);
                }

    }

//    //------shadow of light 2------
//    else if ((shadow2.xindex>-1) && (shadow2.xdist <lightDist2)) {
//        colorSum = ambientCol*materialCol;

//        if(shadow2.xindex == 2){                                                                                        //green transparent sphere
//                    colorSum += (lDotn2 * materialCol + specularTerm2) * glm::vec3(0.6) + sceneObjects[2]->getColor()*glm::vec3(0.1);
//                }

//        if(shadow2.xindex == 1){                                                                                         // refractive sphere
//                    colorSum += (lDotn2 * materialCol + specularTerm2) * glm::vec3(0.6) + sceneObjects[1]->getColor()*glm::vec3(0.1);
//                }
//    }

    else {

        colorSum = ambientCol*materialCol + lDotn*materialCol  + specularTerm + specularTerm2;
    }







    if((ray.xindex == 0 || ray.xindex == 6 )&& step < MAX_STEPS)                     //reflection
     {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1); //Recursion!
        colorSum = colorSum + (0.8f*reflectedCol);
     }




    if(ray.xindex == 2 ) {                                          //transparent object
        Ray transparentRay(ray.xpt, ray.dir);
        glm::vec3 transparentCol = trace(transparentRay, 1);
        colorSum =  colorSum + (0.8f * transparentCol) ;

}

    if( ray.xindex == 1 && step < MAX_STEPS)                    //refraction
    {
        float eta = 1/1.15;
        glm::vec3 n = sceneObjects[1]->normal(ray.xpt);
        glm::vec3 g = glm::refract(ray.dir, n, eta);
        Ray refrRay1(ray.xpt, g);
        refrRay1.closestPt(sceneObjects);
        glm::vec3 m = sceneObjects[refrRay1.xindex]->normal(refrRay1.xpt);
        glm::vec3 h = glm::refract(g, -m, 1.0f/eta);
        Ray refrRay2(refrRay1.xpt, h);
        refrRay2.closestPt(sceneObjects);
        glm::vec3 refraccol = trace(refrRay2, 1);
        colorSum =  specularTerm + specularTerm2 + refraccol;


    }

    return colorSum;


}


void drawCube(float x, float y, float z, float l, float w, float h, glm::vec3 color)
{
//    glm::vec3 A = glm::vec3(x,y,z);
//    glm::vec3 B = glm::vec3(x+l,y,z);
//    glm::vec3 C = glm::vec3(x+l,y+h,z);
//    glm::vec3 D = glm::vec3(x,y+h,z);
//    glm::vec3 E = glm::vec3(x+l,y,z-w);
//    glm::vec3 F = glm::vec3(x+l,y+h,z-w);
//    glm::vec3 G = glm::vec3(x,y+h,z-w);
//    glm::vec3 H = glm::vec3(x,y,z-w);

    glm::vec3 A = glm::vec3(x,y,z);                          //----cube with shear transformation-----
    glm::vec3 B = glm::vec3(x+l,y,z);
    glm::vec3 C = glm::vec3(x+l+1,y+h,z);
    glm::vec3 D = glm::vec3(x+1,y+h,z);
    glm::vec3 E = glm::vec3(x+l,y,z-w);
    glm::vec3 F = glm::vec3(x+l+1,y+h,z-w);
    glm::vec3 G = glm::vec3(x+1,y+h,z-w);
    glm::vec3 H = glm::vec3(x,y,z-w);



    Plane *plane1 = new Plane(A,B,C,D,color);

    Plane *plane2 = new Plane(B,E,F,C,color);

    Plane *plane3 = new Plane(E,H,G,F,color);

    Plane *plane4 = new Plane(D,G,H,A,color);

    Plane *plane5 = new Plane(D,C,F,G,color);

    Plane *plane6 = new Plane(H,E,B,A,color);

    sceneObjects.push_back(plane1);
    sceneObjects.push_back(plane2);
    sceneObjects.push_back(plane3);
    sceneObjects.push_back(plane4);
    sceneObjects.push_back(plane5);
    sceneObjects.push_back(plane6);

}

void drawCube2(float x, float y, float z, float l, float w, float h, glm::vec3 color)
{



    glm::vec3 A = glm::vec3(x,y,z);                    //-----cube with rotation----
    glm::vec3 B = glm::vec3(x+l,y-2,z);
    glm::vec3 C = glm::vec3(x+l,y+h-2,z);
    glm::vec3 D = glm::vec3(x,y+h,z);
    glm::vec3 E = glm::vec3(x+l+3,y,z-w);
    glm::vec3 F = glm::vec3(x+l+3,y+h,z-w);
    glm::vec3 G = glm::vec3(x+3,y+h+2,z-w);
    glm::vec3 H = glm::vec3(x+3,y+2,z-w);


    Plane *plane1 = new Plane(A,B,C,D,color);

    Plane *plane2 = new Plane(B,E,F,C,color);

    Plane *plane3 = new Plane(E,H,G,F,color);

    Plane *plane4 = new Plane(D,G,H,A,color);

    Plane *plane5 = new Plane(D,C,F,G,color);

    Plane *plane6 = new Plane(H,E,B,A,color);

    sceneObjects.push_back(plane1);
    sceneObjects.push_back(plane2);
    sceneObjects.push_back(plane3);
    sceneObjects.push_back(plane4);
    sceneObjects.push_back(plane5);
    sceneObjects.push_back(plane6);

}
void drawTetrahedron(float x, float y, float z, float l, float w, float h, glm::vec3 color)
{

    glm::vec3 A = glm::vec3(x+1,y-1,z);
    glm::vec3 B = glm::vec3(x+l,y,z);
    glm::vec3 C = glm::vec3(x+l/2,y+h,z);
    glm::vec3 D = glm::vec3(x,y,z-w);

    Plane *triangle1 = new Plane(A,B,C,C,color);
    Plane *triangle2 = new Plane(B,D,C,C,color);
    Plane *triangle3 = new Plane(D,A,C,C,color);

    sceneObjects.push_back(triangle1);
    sceneObjects.push_back(triangle2);
    sceneObjects.push_back(triangle3);

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

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

            glm::vec3 dir1(xp + 0.25 * cellX, yp + 0.25 * cellY, -EDIST);	//direction of the primary ray

            glm::vec3 dir2(xp + 0.75 * cellX, yp + 0.25 * cellY, -EDIST);
            glm::vec3 dir3(xp + 0.25 * cellX, yp + 0.75 * cellY, -EDIST);
            glm::vec3 dir4(xp + 0.75 * cellX, yp + 0.75 * cellY, -EDIST);



            Ray ray1 = Ray(eye, dir1);      //Create a ray originating from the camera in the direction 'dir'
            Ray ray2 = Ray(eye, dir2);
            Ray ray3 = Ray(eye, dir3);
            Ray ray4 = Ray(eye, dir4);


            ray1.normalize();               //Normalize the direction of the ray to a unit vector
            ray2.normalize();
            ray3.normalize();
            ray4.normalize();

            glm::vec3 col1 = trace (ray1, 1); //Trace the primary ray and get the colour value
            glm::vec3 col2 = trace (ray2, 1);
            glm::vec3 col3 = trace (ray3, 1);
            glm::vec3 col4 = trace (ray4, 1);

            float col_r = (col1.r + col2.r + col3.r + col4.r) / 4;
            float col_g = (col1.g + col2.g + col3.g + col4.g) / 4;
            float col_b = (col1.b + col2.b + col3.b + col4.b) / 4;

            glColor3f(col_r, col_g, col_b);

//            //-- Without anti-aliasing
//            glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);
//            Ray ray = Ray(eye, dir);
//            ray.normalize();
//            glm::vec3 col = trace (ray, 1);
//            glColor3f(col.r, col.g, col.b);

            //-- Draw each cell with its color value
            glVertex2f(xp, yp);
            glVertex2f(xp + cellX, yp);
            glVertex2f(xp + cellX, yp + cellY);
            glVertex2f(xp, yp + cellY);


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

    textureSphere = TextureBMP((char* const)"aurora.bmp");
    textureWall = TextureBMP((char* const) "nightsky.bmp");

	//-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -95.0), 12.0, glm::vec3(0.3, 0, 0.8));      //largest sphere

    Sphere *sphere2 = new Sphere(glm::vec3(-10.0, -11.0, -70.0), 3.0, glm::vec3(0, 0, 0));   //sphere that refracts

    Sphere *transparentSphere = new Sphere(glm::vec3(8.0, -12.0, -75.0), 3, glm::vec3(0, 0.5, 0)); //transparent sphere

    Sphere *sphere3 = new Sphere(glm::vec3(-12.0, 6, -70.0), 3.5, glm::vec3(0.5, 0.5, 0.5));      //sphere with texture


    Cylinder *cylinder = new Cylinder(glm::vec3(16, -20, -130), 3, 12.0, glm::vec3(0.2,0.9,1));

    Cone *cone = new Cone(glm::vec3(3, -20.0, -85.0), 2, 10.0, glm::vec3(1,0.8,0.5));



    Plane *plane = new Plane(glm::vec3(-50., -20, -40), //Point A       //plane for floor
                             glm::vec3(50., -20, -40), //Point B
                             glm::vec3(50., -20, -200), //Point C
                             glm::vec3(-50., -20, -200), //Point D
                             glm::vec3(0.6, 0.6, 1)); //Colour



    Plane *planeWall = new Plane(glm::vec3(-50., -20, -200), //Point A     //plane for wall
                             glm::vec3(50., -20, -200), //Point B
                             glm::vec3(50., 50, -200), //Point C
                             glm::vec3(-50., 50, -200), //Point D
                             glm::vec3(0, 0.6, 1)); //Colour



	//--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1);

    sceneObjects.push_back(sphere2);

    sceneObjects.push_back(transparentSphere);



    sceneObjects.push_back(plane);

    sceneObjects.push_back(cylinder);
    sceneObjects.push_back(cone);
    sceneObjects.push_back(sphere3);

    sceneObjects.push_back(planeWall);


//    glm::mat4 im = glm::mat4(1.0f);
//    float cdr = 3.1415/180;
//    glm::mat4 am = glm::translate(im, glm::vec3(12,-15,-80));
//    glm::mat4 bm = glm::rotate(am, 30 * cdr, glm::vec3(0,1,0));
//    glm::mat4 cm = glm::translate(bm, glm::vec3(-12, 15, 80));

    drawCube(12, -15.0, -80.0, 4, 4, 4, glm::vec3(0.8, 0.6, 0.3));
    drawCube2(11, 5.0, -95.0, 3.5, 3.5, 3.5, glm::vec3(1, 1, 1));
    drawTetrahedron(-5, -15.0, -75.0, 5, 5,5, glm::vec3(0.9, 0.5, 0.5));

}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
