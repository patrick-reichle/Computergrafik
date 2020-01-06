#include "glut.h"
#include "point.h"
#include "color.h"

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
