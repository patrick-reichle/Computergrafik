// Include-File für die Text Ein-/Ausgabe
#include <iostream>
using namespace std;

// Include-File für die GLUT-Library
#include "glut.h"

#include "vec.h"



/////////////////////////////////////////////////////////////////////////////////////////
//
// Hier wird einiges initialisiert. Sie dürfen sich gerne den Code anschauen und
// versuchen ihn zu verstehen. Das sollte Ihnen nicht allzu schwer fallen. Es werden
// jedoch einige Techniken benutzt (Texturen, ...) die wir in der Vorlesung noch
// nicht besprochen haben, die wir aber aus techn. Gründen hier benötigen.
//
// Weiter unten finden Sie einen Vermerk, ab dem Sie den Code jedoch vollständig
// verstehen sollten!!!


// Auflösungen der gesamten Textur
// !!!ACHTUNG!!! nicht alle Texturauflösungen funktionieren!
// Stichwort ungefähr: POT2 Problematik
#define TEX_RES_X 400
#define TEX_RES_Y 400
// Anzahl der Pixel der Textur
#define TEX_RES TEX_RES_X*TEX_RES_Y
// Achsenlänge der Textur (Achsen sind asymmetrisch von -TexRes#/2 bis TexRes#/2-1)

// Konvertiert Indices von (x,y) Koordinaten in ein lineares Array
#define TO_LINEAR(x, y) (((x)) + TEX_RES_X*((y)))

#define PI 3.1415926535

#define max(a ,b) (((a) > (b)) ? (a) : (b))
#define min(a ,b) (((a) < (b)) ? (a) : (b))

/////////////////////////////////////////////////////////////

// globaler Speicher für Texturdaten
char g_Buffer[3*TEX_RES];
// Textur ID Name
GLuint g_TexID = 0;

// Auflösung des Hauptfensters (kann sich durch User ändern)
int g_WinWidth = 400;
int g_WinHeight = 400;


// Kugel
int M= 100;
int R= 50;

float rK= 1.0;
float gK= 1.0;
float bK= 1.0;

// Licht
float xL= 0;
float yL= 1000;
float zL= 0;
float angleYZ;
float angleXZ;
float radL= 1000;

// Augpunkt
int A= -1;


//
/////////////////////////////////////////////////////////////////////////////////////////
//


// Eine überaus primitive Punktklasse
class Point {
public:

    Point (int x=0, int y=0) {
	this->x = x;
        this->y = y;
    }

    int x, y;
};

// Eine überaus primitive Farbklasse
class Color {
public:

    Color (float r=1.0f, float g=1.0f, float b=1.0f) {
	this->r = r;
	this->g = g;
	this->b = b;
    }

    float r, g, b;

    void print(char * name = "c") {
	cout << name << " = (" << r << ", " << g << ", " << b << ")" << endl;
    }
};

//
/////////////////////////////////////////////////////////////////////////////////////////
//


// Funktion organisiert die Textur.
// Kümmern Sie sich nicht weiter um diese Funktion, da
// sie momentan nur ein notwendiges Übel darstellt!
void manageTexture () {

    glEnable (GL_TEXTURE_2D);

    if (g_TexID==0)
    glGenTextures (1, &g_TexID);

    glBindTexture (GL_TEXTURE_2D, g_TexID);

    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, TEX_RES_X, TEX_RES_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, g_Buffer);
	
    glBindTexture (GL_TEXTURE_2D, 0);

    glDisable (GL_TEXTURE_2D);
}

void mapTexture() {

    glBindTexture (GL_TEXTURE_2D, g_TexID);
    glEnable (GL_TEXTURE_2D);
    glBegin (GL_QUADS);
    glColor3f (1, 0, 0);
    glTexCoord2f (0, 0);
    glVertex2f (-g_WinWidth/2, -g_WinHeight/2);
    glTexCoord2f (1, 0);
    glVertex2f (g_WinWidth/2, -g_WinHeight/2);
    glTexCoord2f (1, 1);
    glVertex2f (g_WinWidth/2, g_WinHeight/2);
    glTexCoord2f (0, 1);
    glVertex2f (-g_WinWidth/2, g_WinHeight/2);
    glEnd ();

    glBindTexture (GL_TEXTURE_2D, 0);
    glDisable (GL_TEXTURE_2D);
}

// Callback Funktion um die Fenstergrößen anzupassen.
// Auch diese Funktion ist ein notwendiges Übel! Kümmern
// Sie sich im Moment nicht weiter darum.
void reshape(int w, int h) {

    g_WinWidth = w;
    g_WinHeight = h;

    glViewport(0, 0, w, h);					// Establish viewing area to cover entire window.

    glMatrixMode(GL_PROJECTION);			// Start modifying the projection matrix.
    glLoadIdentity();						// Reset project matrix.
    glOrtho(-w/2, w/2, -h/2, h/2, 0, 1);	// Map abstract coords directly to window coords.

    glutPostRedisplay ();
}

// Funktion löscht den Bildschirm mit der angegebenen Farbe
// Usage z.B.: clearImage (Color (1,1,1))
// löscht den Bildschirm in Weiß.
// Ohne Farbangabe ist der Standard Weiß
void clearTexture (Color c=Color()) {
    for (int i=0; i<TEX_RES; i++) {
    	g_Buffer[3*i  ] = 255.0*c.r;
	g_Buffer[3*i+1] = 255.0*c.g;
	g_Buffer[3*i+2] = 255.0*c.b;
    }
}

// Funktion malt einen Punkt an die angegebenen Koordinaten
// Usage z.B.: addPoint (Point(10, 5), Color (1,0,0))
// malt einen Punkt an die Koordinate (10, 5)
// Ohne Farbangabe ist die standard Malfarbe Schwarz
//
// Nutzen Sie diese Funktion ...
void setPoint (Point p, Color c= Color(0,0,0)) {
    if (p.x < 0 || p.y < 0 || p.x > TEX_RES_X -1 || p.y > TEX_RES_Y -1) {
	cerr << "Illegal point co-ordinates (" << p.x << ", " << p.y << ")\n" << flush;
	return;
    }
	
    g_Buffer[3*TO_LINEAR (p.x, p.y)  ] = 255.0*c.r;
    g_Buffer[3*TO_LINEAR (p.x, p.y)+1] = 255.0*c.g;
    g_Buffer[3*TO_LINEAR (p.x, p.y)+2] = 255.0*c.b;
}

void setQuad(Point p, Color c) {
    for (int x= p.x; x < p.x+10; x++)
	for (int y= p.y; y < p.y+10; y++)
	    setPoint(Point(x,y),c);
}

//
/////////////////////////////////////////////////////////////////////////////////////////
//



Color phong(CVec3f HitPos, CVec3f ViewDir) {
	
// Phong-Parameter
	
    //Reflexionskoeffizienten fuer die Kugelfarben
    CVec3f Ka;		// ambient
    Ka(0)= rK;
    Ka(1)= gK;
    Ka(2)= bK;

    CVec3f Kd;		// diffuse
    Kd(0)= rK;
    Kd(1)= gK;
    Kd(2)= bK;

    CVec3f Ks;		// specular
    Ks(0)= 0.5;
    Ks(1)= 0.5;
    Ks(2)= 0.5;

    int exp= 28;	       // shininess

    //andere Reflexionskoeffizienten
    //////////////////
/*  // Messing
    CVec3f Ka;
    Ka(0)= 0.33;
    Ka(1)= 0.22;
    Ka(2)= 0.03;

    CVec3f Kd;
    Kd(0)= 0.78;
    Kd(0)= 0.57;
    Kd(0)= 0.11;

    CVec3f Ks;
    Ks(0)= 0.99;
    Ks(1)= 0.94;
    Ks(2)= 0.81;

    int exp= 28;
*/
/*  //////////////////  
    // Bronze
    CVec3f Ka;
    Ka(0)= 0.21;
    Ka(1)= 0.13;
    Ka(2)= 0.05;

    CVec3f Kd;
    Kd(0)= 0.71;
    Kd(0)= 0.43;
    Kd(0)= 0.18;

    CVec3f Ks;
    Ks(0)= 0.39;
    Ks(1)= 0.27;
    Ks(2)= 0.17;
	
    int exp= 26;
    //////////////////
*/

    // Beleuchtungsstaerken
    ///////////////////////
    CVec3f Ia;		// ambient
    Ia(0)= 0.1;
    Ia(1)= 0.1;
    Ia(2)= 0.1;
	
    CVec3f Id;		// diffuse
    Id(0)= 1.0;
    Id(1)= 1.0;
    Id(2)= 1.0;
	
    CVec3f Is;		// specular
    Is(0)= 1.0;
    Is(1)= 1.0;
    Is(2)= 1.0;
	///////////////////////
    CVec3f LightPos;
    LightPos(0) = xL;
    LightPos(1) = yL;
    LightPos(2) = zL;
    CVec3f LightDir = LightPos - HitPos;
    CVec3f N;
    N(0) = HitPos(0);
    N(1) = HitPos(1);
    N(2) = HitPos(2) - M;
    N = N.norm();
    LightDir = LightDir.norm();
    ViewDir = ViewDir.norm();

    CVec3f R = N *2*N.dot(LightDir) - LightDir;
    CVec3f H = LightDir + ViewDir * ((LightDir+ViewDir).length());
    H = H.norm();
    R = R.norm();
    float d = max(0, LightDir.dot(N));
    
    double s = max(0,powf(R.dot(N),exp));
    float r,g,b;
    CVec3f k = Ks+Kd;
    r = (Kd(0)*d + Ks(0)*s) / k(0) + Ka(0)*Ia(0);
    g = (Kd(1)*d + Ks(1)*s) / k(1) + Ka(1)*Ia(1);
    b = (Kd(2)*d + Ks(2)*s) / k(2) + Ka(2)*Ia(2);
    r = min(r, 1);
    g = min(g,1);
    b = min(b,1);
    return Color(r,g,b);
}

CVec3f intersect (CVec3f EyePos, CVec3f ViewDir) {
    CVec3f hit;
    EyePos(2) -= M;
    float a = ViewDir.dot(ViewDir);
    float b = ViewDir.dot(EyePos) * 2;
    float c = EyePos.dot(EyePos) - (R*R);
    float d = b*b - 4*a*c;
    if (d < 0) {
        hit(2) = -1;
    } else {
        float t;
        if (d == 0) {
            t = -b / 2*a; 
        } else {
            float t1 = (-b + sqrt(d)) / 2*a;
            float t2 = (-b - sqrt(d)) / 2*a;
            t = min(t1,t2);
        }
        hit = EyePos + ViewDir*t;
    }
    return hit;
}
void rayCast() {
    clearTexture();

    CVec3f e,v;
    
    e(0)= 0;
    e(1)= 0;
    e(2)= A;
	
    v(2)= -A;

    for (int x= 0; x < TEX_RES_X; x++) {
    	for (int y= 0; y < TEX_RES_Y; y++) {
    	    v(0)= -1 + 2*x / static_cast<float>(TEX_RES_X -1);
	    v(1)= -1 + 2*y / static_cast<float>(TEX_RES_Y -1);
	    	
	    CVec3f hit= intersect(e,v);
			
	    Color c = Color(0,0,0); // Hintergrund weiß
	    if (hit(2) != -1) {
		c= phong(hit,v);
	    }
			
	    setPoint(Point(x,y),c);
	}
    }
    cout << "raycast done." << endl;
}

// Die Callback Funktion die für das eigentliche Malen
// zuständig ist. Im Wesentlichen brauchen Sie sich nur
// um den Bereich zwischen den Kommentaren zu kümmern,
// alles andere ist wiederum ein notwendiges Übel!
void display (void) {

    manageTexture();
	
    glClear (GL_COLOR_BUFFER_BIT);
	
    mapTexture();

    glFlush ();
    glutSwapBuffers();
}

void init() {
    angleXZ= 1.5 * PI;
    angleYZ= 0;

    rayCast();
}

void calcL() {
    xL= radL * sin(angleYZ) * cos(angleXZ);
    zL= radL * sin(angleYZ) * sin(angleXZ);
    yL= radL * cos(angleYZ);
    cout << "L= (" << xL << ", " << yL << ", " << zL << ")" << endl;
}

// nach Kugelkorrdinaten:
// (x) PI <= XZ <= 2Pi (X)
// (y) 0 <= YZ <= PI (Y)
void keyboard(unsigned char c, int x, int y) {
    switch (c) {
        case 'q':
        case 'Q':
            exit(0);
            break;
	    case 'x':
            if (angleXZ > PI) {
	        	angleXZ-= .1;
	        	calcL();
       	    }
	        break;
    	case 'X':
            if (angleXZ < 2 * PI) {
	        	angleXZ+= .1;
        		calcL();
	         }
	       break;
    	case 'y':
            if (angleYZ > 0) {
	        	angleYZ-= .1;
	        	calcL();
	        }
	        break;
	    case 'Y':
            if (angleYZ < PI) {
	          angleYZ+= .1;
		      calcL();
	        }
	        break;
	    /////////////////////////////////
		/////////////////////////////////

		
		/////////////////////////////////

        case 'r':
            if (rK > 0) rK -= 0.1;
            cout << "red = "<< rK << endl;
            break;
        case 'R':
            if (rK < 1) rK += 0.1;
            cout << "red = "<< rK << endl;
            break;
        case 'g':
            if (gK > 0) gK -= 0.1;
            cout << "green = "<< gK << endl;
            break;
        case 'G':
            if (gK < 1) gK += 0.1;
            cout << "green = "<< gK << endl;
            break;
        case 'b':
            if (bK > 0) bK -= 0.1;
            cout << "blue = "<< bK << endl;
            break;
        case 'B':
            if (bK < 1) bK += 0.1;
            cout << "blue = "<< bK << endl;
            break;
        case 'd':
            if (R > 0) R--;
            break;
        case 'D':
            R++;
            break;
	default:
            cout << "Please use 'x', 'X', 'y' and 'Y' for moving the light source." << endl; 
	    break;
    }
    rayCast();	
    glutPostRedisplay();
}

// Diese Funktion kennen Sie ja bereits ...
int main (int argc, char **argv) {
	
    init();

    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize (g_WinWidth, g_WinHeight);

    glutCreateWindow ("Übung 7: Phong");
	
    glutKeyboardFunc(keyboard);
    glutReshapeFunc (reshape);	// zuständig für Größenänderungen des Fensters
    glutDisplayFunc (display);	// zuständig für das wiederholte Neuzeichnen des Bildschirms
	
    glutMainLoop ();

    glDeleteTextures (1, &g_TexID); // löscht die oben angelegte Textur

    return 0;
}
