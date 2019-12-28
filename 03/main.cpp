// Sample code for Übung 2

#include "vec.h"
#include "mat.h"
#include <math.h>

// might be you have to swith to
// #include "glut.h" depending on your GLUT installation
#include "glut.h"

class Point {
public:

	Point (int x=0, int y=0) {
		this->x = x;
		this->y = y;
	}

	int x, y;
};

class Color {
public:

	Color (float r=1.0f, float g=1.0f, float b=1.0f) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	float r, g, b;
};
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
float rotationX, rotationY, rotationZ;

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
    rotationX = 0; 
    rotationY = 0;
    rotationZ = 0;

    float e[4] = {0,0,1,1};
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


Point inv_trafo(Point p, bool mirror_x, bool mirror_y, bool mirror_xy) {
    Point result = Point(p.x, p.y);
    if (mirror_xy) {
    	int t = result.x;
    	result.x = result.y;
    	result.y = t;
    }
    if (mirror_x) {
    	result.x = -result.x;
    }
    if (mirror_y) {
    	result.y = -result.y;
    }

    return result;
}

void bhamLine (Point p1, Point p2, Color c) 
{
    glBegin (GL_POINTS);
    glColor3f (c.r, c.g, c.b);
    Point start(0, 0);
    Point end(p2.x - p1.x, p2.y - p1.y);
    bool mirror_x = false;
    bool mirror_y = false;
    bool swap_xy = false;

    if (end.x < 0) {
    	end.x = -end.x;
    	mirror_x = true;
    }
    if (end.y < 0) {
    	end.y = -end.y;
	mirror_y = true;
    }

    if (end.y > end.x) {
    	int t = end.x;
    	end.x = end.y;
    	end.y = t;
    	swap_xy = true;
    }

    int x = start.x;
    int y = start.y;
    int dx = end.x - x;
    int dy = end.y - y;
    int d = 2 * dy - dx;
    int dNE = 2 * (dy - dx);
    int dE = 2 * dy;

    while (x < end.x) {
    	if (d >= 0) {
    	    d += dNE;
	    x++;
            y++;
	}
	else {
	    d += dE;
	    x++;
	}
	Point inv = inv_trafo(Point(x, y), mirror_x, mirror_y, swap_xy);
	inv.x += p1.x;
	inv.y += p1.y;
	glVertex2i(inv.x, inv.y);
    }
	
    glEnd ();
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
    vec(0) = vec(0) / len;
    vec(1) = vec(1) / len;
    vec(2) = vec(2) / len;
    return vec;
}
CVec4f rotateX(CVec4f p, float alpha) {
    float r[4][4] = {{1,0,0,0},{0,cos(alpha),-sin(alpha),0},{0,sin(alpha),cos(alpha),0},{0,0,0,1}};
    return CMat4f(r) * p;
}

CVec4f rotateY(CVec4f p, float alpha) {
    float r[4][4] = { { cos(alpha), 0, sin(alpha), 0 },{ 0, 1, 0, 0 },{ -sin(alpha), 0, cos(alpha), 0 } ,{ 0, 0, 0, 1 } };
    return CMat4f(r) * p;
}

CVec4f rotateZ(CVec4f p, float alpha) {
    float r[4][4] = { { cos(alpha), -sin(alpha), 0, 0 },{ sin(alpha), cos(alpha), 0, 0 },{ 0, 0, 1, 0 },{ 0, 0, 0, 1 } };
    return CMat4f(r) * p;
}

//rotate a around axis b
CVec4f rotate(CVec4f a, CVec4f b, float psi) {
    float c = cos(psi);
    float c1 = 1 -c;
    float s = sin(psi);
    float m[4][4] = {{b(0)*b(0)*c1 + c, b(0)*b(1)*c1 - b(2)*s, b(0)*b(2)*c1 + b(1)*s,0},
                     {b(0)*b(1)*c1+b(2)*s, b(1)*b(1)*c1 + c, b(1)*b(2)*c1-b(0)-b(0)*s,0},
                     {b(0)*b(2)*c1-b(1)*s, b(1)*b(2)*c1 + b(0)*s, b(2)*b(2)*c1+c,0},
                     {0,0,0,1}};
    a = a - eye;
    a = CMat4f(m) * a;
    a = a + eye;
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
    
    float v[4] = {viewDir(1) * viewUp(2) - viewDir(2) * viewUp(1),
                  viewDir(2) * viewUp(0) - viewDir(0) * viewUp(2),
                  viewDir(0) * viewUp(1) - viewDir(1) * viewUp(0),1};
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
    //p = rotateX(p, rotationX);
    //p = rotateY(p, rotationY);
    //p = rotateZ(p, rotationZ);
    p = M*p;
    return projectZ(fFocus, p);
}

void drawQuader(CVec4f p[8], float fFocus, Color c) {
    for (int i = 0; i < 8; i++) {
        p[i] = projectZallg(getTransform(eye,viewDir,viewUp),fFocus, p[i]); 

    }
    drawProjectedZ(p, c);
}

// timer callback function
void timer (int value) 
{
    ///////
    // update your variables here ...
    //

    ///////

    // the last two lines should always be
    glutPostRedisplay ();
    glutTimerFunc (g_iTimerMSecs, timer, 0);	// call timer for next iteration
}

// display callback function
void display1 (void) 
{

    glClear (GL_COLOR_BUFFER_BIT);

    ///////
    // display your data here ...
    //
    float point0Arr0[4] = {  5.0,  5.0,  0.0, 1.0 };
    float point0Arr1[4] = { 20.0,  5.0,  0.0, 1.0 };
    float point0Arr2[4] = {  5.0, 20.0,  0.0, 1.0 };
    float point0Arr3[4] = { 20.0, 20.0,  0.0, 1.0 };
    float point0Arr4[4] = {  5.0,  5.0, -7.0, 1.0 };
    float point0Arr5[4] = { 20.0,  5.0, -7.0, 1.0 };
    float point0Arr6[4] = {  5.0, 20.0, -7.0, 1.0 };
    float point0Arr7[4] = { 20.0, 20.0, -7.0, 1.0 };

    CVec4f points0[8] = {
        CVec4f(point0Arr0),
        CVec4f(point0Arr1),
        CVec4f(point0Arr2),
        CVec4f(point0Arr3),
        CVec4f(point0Arr4),
        CVec4f(point0Arr5),
        CVec4f(point0Arr6),
        CVec4f(point0Arr7)
    };

    float point1Arr0[4] = { -40.0, -40.0,  10.0, 1.0 };
    float point1Arr1[4] = {  40.0, -40.0,  10.0, 1.0 };
    float point1Arr2[4] = { -40.0,  40.0,  10.0, 1.0 };
    float point1Arr3[4] = {  40.0,  40.0,  10.0, 1.0 };
    float point1Arr4[4] = { -40.0, -40.0, -10.0, 1.0 };
    float point1Arr5[4] = {  40.0, -40.0, -10.0, 1.0 };
    float point1Arr6[4] = { -40.0,  40.0, -10.0, 1.0 };
    float point1Arr7[4] = {  40.0,  40.0, -10.0, 1.0 };

    CVec4f points1[8] = {
        CVec4f(point1Arr0),
        CVec4f(point1Arr1),
        CVec4f(point1Arr2),
        CVec4f(point1Arr3),
        CVec4f(point1Arr4),
        CVec4f(point1Arr5),
        CVec4f(point1Arr6),
        CVec4f(point1Arr7)
    };

    float point2Arr0[4] = { 50.0, -10.0,  0.0, 1.0 };
    float point2Arr1[4] = { 80.0, -10.0,  0.0, 1.0 };
    float point2Arr2[4] = { 50.0,  20.0,  0.0, 1.0 };
    float point2Arr3[4] = { 80.0,  20.0,  0.0, 1.0 };
    float point2Arr4[4] = { 50.0, -10.0, -5.0, 1.0 };
    float point2Arr5[4] = { 80.0, -10.0, -5.0, 1.0 };
    float point2Arr6[4] = { 50.0,  20.0, -5.0, 1.0 };
    float point2Arr7[4] = { 80.0,  20.0, -5.0, 1.0 };

    CVec4f points2[8] = {
        CVec4f(point2Arr0),
        CVec4f(point2Arr1),
        CVec4f(point2Arr2),
        CVec4f(point2Arr3),	
        CVec4f(point2Arr4),
        CVec4f(point2Arr5),
        CVec4f(point2Arr6),
        CVec4f(point2Arr7)
    };
    drawQuader(points1, fFocus,red);
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

    ///////
    // display your data here ...
    //
	
    //
    ///////

    // In double buffer mode the last
    // two lines should alsways be
    glFlush ();
    glutSwapBuffers (); // swap front and back buffer
}

void keyboard (unsigned char key, int x, int y) 
{
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
            viewDir = rotateX(viewDir, 0.01);
            viewUp = rotateX(viewUp, 0.01);
            eye = rotateX(eye, 0.01);
            break;
        case 'Y':
            viewDir = rotateY(viewDir, 0.01);
            viewUp = rotateY(viewUp, 0.01);
            eye = rotateY(eye, 0.01);
            break;
        case 'Z':
            viewDir = rotateZ(viewDir, 0.01);
            viewUp = rotateZ(viewUp, 0.01);
            eye = rotateZ(eye, 0.01);
            break;
        case 'x':
            viewDir = rotateX(viewDir, -0.01);
            viewUp = rotateX(viewUp, -0.01);
            eye = rotateX(eye, -0.01);
            break;
        case 'y':
            viewDir = rotateY(viewDir, -0.01);
            viewUp = rotateY(viewUp, -0.01);
            eye = rotateY(eye, -0.01);
            break;
        case 'z':
            viewDir = rotateZ(viewDir, -0.01);
            viewUp = rotateZ(viewUp, -0.01);
            eye = rotateZ(eye, -0.01);
            break;
        case 'A':
            viewDir = rotate(viewDir, viewUp, 0.01);
            break;
        case 'B':
            viewUp = rotate(viewUp, viewDir, 0.01);
            break;
        case 'C':
            break;
        case 'a':
            break;
        case 'b':
            break;
        case 'c':
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
