// Sample code for Übung 2

#include <math.h>
#include "mat.h"

#include "cube.h"
#include "bresenham.h"


// might be you have to swith to
// #include "glut.h" depending on your GLUT installation


////////////////////////////////////////////////////////////
//
// system relevant global variables
//

// window width and height (choose an appropriate size)
const int g_iWidth  = 400;
const int g_iHeight = 400;

// global variable to tune the timer interval
int g_iTimerMSecs;

//
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// private, global variables ... replace by your own ones
//
// some global state variables used to describe ...

float fFocus;

CVec4f eye;

CVec4f viewDir;

CVec4f viewUp;

Color red(1,0,0);
Color green(0,1,0);
Color blue(0,0,1);

////////////////////////////////////////////////////////////

// function to initialize our own variables
void init () 
{
    fFocus = 50;

    float e[4] = {0,0,0,1};
    eye = CVec4f(e);

    float vd[4] = {0,0,-1,0};
    viewDir = CVec4f(vd);

    float vu[4] = {0,1,0,0};
    viewUp = CVec4f(vu);

   
    // init timer interval
    g_iTimerMSecs = 10;
}

// function to initialize the view to ortho-projection
void initGL () 
{
    glViewport (0, 0, g_iWidth, g_iHeight);	// Establish viewing area to cover entire window.

    glMatrixMode (GL_PROJECTION);			// Start modifying the projection matrix.
    glLoadIdentity ();						// Reset project matrix.
    glOrtho (-g_iWidth/2, g_iWidth/2, -g_iHeight/2, g_iHeight/2, 0, 1);	// Map abstract coords directly to window coords.

    // tell GL that we draw to the back buffer and
    // swap buffers when image is ready to avoid flickering
    glDrawBuffer (GL_BACK);

    // tell which color to use to clear image
    glClearColor (0,0,0,1);
}




void drawLine(CVec4f a, CVec4f b, Color c) {
    bhamLine(Point(a(0),a(1)),Point(b(0),b(1)), c);
}

void drawProjectedZ(CVec4f p[8], Color c) {
    drawLine(p[0],p[1],c);
    drawLine(p[0],p[2],c);
    drawLine(p[1],p[3],c);
    drawLine(p[2],p[3],c);

    drawLine(p[0],p[4],c);
    drawLine(p[1],p[5],c);
    drawLine(p[2],p[6],c);
    drawLine(p[3],p[7],c);

    drawLine(p[4],p[5],c);
    drawLine(p[4],p[6],c);
    drawLine(p[5],p[7],c);
    drawLine(p[6],p[7],c);
}

CVec4f cross(CVec4f a, CVec4f b) {
    float v[4] = {a(1) * b(2) - a(2) * b(1),
                  a(2) * b(0) - a(0) * b(2),
                  a(0) * b(1) - a(1) * b(0),1};
    return CVec4f(v);
}
    

CVec4f norm(CVec4f vec) {
    float len = sqrt(pow(vec(0), 2) + pow(vec(1), 2) + pow(vec(2), 2));
    return vec * (1 / len);
}
CVec4f rotateX(CVec4f p, float alpha) {
    float r[4][4] = { {1, 0, 0, 0}, {0, cos(alpha), -sin(alpha), 0}, {0, sin(alpha),  cos(alpha), 0}, {0, 0, 0, 1} };
    return CMat4f(r) * p;
}

CVec4f rotateY(CVec4f p, float alpha) {
    float r[4][4] = { {cos(alpha), 0, sin(alpha), 0}, {0, 1, 0, 0}, {-sin(alpha), 0, cos(alpha), 0}, {0, 0, 0, 1} };
    return CMat4f(r) * p;
}

CVec4f rotateZ(CVec4f p, float alpha) {
    float r[4][4] = { {cos(alpha), -sin(alpha), 0, 0}, {sin(alpha), cos(alpha), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} };
    return CMat4f(r) * p;
}

//rotate a around axis b
CVec4f rotate(CVec4f a, CVec4f b, float psi) {
    b = norm(b);
    float c = cos(psi);
    float c1 = 1 -c;
    float s = sin(psi);

    float t1[4][4] = { {1, 0, 0, -a(0)},
                       {0, 1, 0, -a(1)},
                       {0, 0, 1, -a(2)},
                       {0, 0, 0,     1} };

    float t2[4][4] = { {1, 0, 0, a(0)},
                       {0, 1, 0, a(1)},
                       {0, 0, 1, a(2)},
                       {0, 0, 0,    1} };
                      
    float m[4][4] = {{b(0)*b(0)*c1 + c,      b(0)*b(1)*c1 - b(2)*s, b(0)*b(2)*c1 + b(1)*s, 0},
                     {b(0)*b(1)*c1 + b(2)*s, b(1)*b(1)*c1 + c     , b(1)*b(2)*c1 - b(0)*s, 0},
                     {b(0)*b(2)*c1 - b(1)*s, b(1)*b(2)*c1 + b(0)*s, b(2)*b(2)*c1 + c,      0},
                     {0,0,0,1}};
    
    
    a = CMat4f(t2) * CMat4f(m) * CMat4f(t1) * a;
    return a;
}

CVec4f projectZ(float fFocus, CVec4f p) {
    CVec4f projection;
    projection(0) = p(0) * (fFocus / (fFocus-p(2)));
    projection(1) = p(1) * (fFocus / (fFocus-p(2)));
    projection(2) = 0;
    projection(3) = 1;
    return projection;
}



CMat4f getTransform(CVec4f viewOrigin, CVec4f viewDir, CVec4f viewUp) {
    viewUp = norm(viewUp);
    viewDir = norm(viewDir);
    
    CVec4f x = cross(viewDir, viewUp);
    float t[4][4] = {{1,0,0,-viewOrigin(0)},
                     {0,1,0,-viewOrigin(1)},
                     {0,0,1,-viewOrigin(2)},
                     {0,0,0,1}};


    float m[4][4] = {{x(0),x(1),x(2),0},
                     {viewUp(0),viewUp(1),viewUp(2),0},
                     {-viewDir(0),-viewDir(1),-viewDir(2),0},
                     {0,0,0,1}};

    return CMat4f(m) * CMat4f(t);

}

CVec4f projectZallg(CMat4f M, float fFocus, CVec4f p) {
    p = M*p;
    return projectZ(fFocus, p);
}

void drawQuader(CVec4f p[8], float fFocus, Color c) {
    CVec4f q[8];
    for (int i = 0; i < 8; i++) {
        q[i] = projectZallg(getTransform(eye,viewDir,viewUp),fFocus, p[i]); 

    }
    drawProjectedZ(q, c);
}

// timer callback function
void timer (int value) 
{
    // the last two lines should always be
    glutPostRedisplay ();
    glutTimerFunc (g_iTimerMSecs, timer, 0);	// call timer for next iteration
}

// display callback function
void display1 (void) 
{

    glClear (GL_COLOR_BUFFER_BIT);
    drawQuader(points0, fFocus, green);
    //drawQuader(points1, fFocus, red);
    drawQuader(points2, fFocus, blue);
    //drawQuader(q1, fFocus, red);
    //
    ///////
    // In double buffer mode the last
    // two lines should alsways be
    glFlush ();
    glutSwapBuffers (); // swap front and back buffer
}

// display callback function
void display2 (void) 
{
    glClear (GL_COLOR_BUFFER_BIT);

    // In double buffer mode the last
    // two lines should alsways be
    glFlush ();
    glutSwapBuffers (); // swap front and back buffer
}

void keyboard (unsigned char key, int x, int y) 
{
    CVec4f axis;
    switch (key) {
	case 'q':
	case 'Q':
	    exit (0); // quit program
	    break;
	case '1':
	    glutDisplayFunc (display1);
	    //glutPostRedisplay ();	// not needed since timer triggers redisplay
	    break;
	case '2':
	    glutDisplayFunc (display2);
	    //glutPostRedisplay ();	// not needed since timer triggers redisplay
	    break;
        case 'f':
            fFocus--;
            break;
        case 'F':
            fFocus++;
            break;
        case 'X':
            viewDir = rotateX(viewDir, 0.1);
            viewUp  = rotateX(viewUp, 0.1);
            eye     = rotateX(eye, 0.1);
            break;
        case 'Y':
            viewDir = rotateY(viewDir, 0.1);
            viewUp  = rotateY(viewUp, 0.1);
            eye     = rotateY(eye, 0.1);
            break;
        case 'Z':
            viewDir = rotateZ(viewDir, 0.1);
            viewUp  = rotateZ(viewUp, 0.1);
            eye     = rotateZ(eye, 0.1);
            break;
        case 'x':
            viewDir = rotateX(viewDir, -0.1);
            viewUp  = rotateX(viewUp, -0.1);
            eye     = rotateX(eye, -0.1);
            break;
        case 'y':
            viewDir = rotateY(viewDir, -0.1);
            viewUp  = rotateY(viewUp, -0.1);
            eye     = rotateY(eye, -0.1);
            break;
        case 'z':
            viewDir = rotateZ(viewDir, -0.1);
            viewUp  = rotateZ(viewUp, -0.1);
            eye     = rotateZ(eye, -0.1);
            break;
        case 'A':
            viewDir = rotate(viewDir, viewUp, 0.1);
            break;
        case 'B':
            viewUp = rotate(viewUp, viewDir, 0.1);
            break;
        case 'C':
            axis = norm(cross(viewDir, viewUp));
            viewDir = rotate(viewDir, axis, 10*M_PI / 180);
            viewUp  = rotate(viewUp, axis, 10*M_PI / 180);
            break;
        case 'a':
            viewDir = rotate(viewDir, viewUp, -0.1);
            break;
        case 'b':
            viewUp = rotate(viewUp, viewDir, -0.1);
            break;
        case 'c':
            axis = norm(cross(viewDir, viewUp));
            viewDir = rotate(viewDir, axis, -0.1);
            viewUp  = rotate(viewUp, axis, -0.1);
            break;
        case 'U':
            eye(0)+=5;
            break;
        case 'V':
            eye(1)+=5;
            break;
        case 'W':
            eye(2)+=5;
            break;
        case 'u':
            eye(0)-=5;
            break;
        case 'v':
            eye(1)-=5;
            break;
        case 'w':
            eye(2)-=5;
            break;
        case 'r':
        case 'R':
            init();
            break;
	default:
	    // do nothing ...
	    break;
    };
}


int main (int argc, char **argv) 
{
    glutInit (&argc, argv);
    // we have to use double buffer to avoid flickering
    // TODO: lookup "double buffer", what is it for, how is it used ...
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutCreateWindow ("Übung 3");

    init ();	// init my variables first
    initGL ();	// init the GL (i.e. view settings, ...)

    // assign callbacks
    glutTimerFunc (10, timer, 0);
    glutKeyboardFunc (keyboard);
    glutDisplayFunc (display1);
    // you might want to add a resize function analog to
    // Übung1 using code similar to the initGL function ...

    // start main loop
    glutMainLoop ();

    return 0;
}
