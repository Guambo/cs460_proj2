#include <GL/glut.h>
#include <cmath>  // abs()
#include <assert.h>
#include <iterator>
#include <iostream>
#include <vector>

using namespace std;

vector<pair<float, float>> controlPoints;
pair<float,float> dummyVertex(-1,-1);
pair<float,float> previousControlPoint (dummyVertex);
bool drawDynamicLine = false;
bool pressedLeftButton = false;
bool pressedRightButton = false;
bool lineStippleEnabled = false;

void DrawClippingRectangularWindow(int lenX, int lenY, int minX, int minY, int maxX, int maxY) {
	// Draw a clipping rectangular that is lenX x lenY pixels
	// Draw from minX, minY to maxX, maxY
	;
}

// Menu Option 1
void DoPolygonClipping() {
	// Perform the Sutherland-Hodgman algorithm
	;
}

void DoRegionFilling() {
	// Fill the clipped polygon using a polygon filling algorithm to fill a red color
	// ex: boundary filling, flood-filling, scan-line
	// Hint: use glReadPixels and store values in a buffer, then use glDrawPixels (see hint 4 in overview)
	;
}

void DefineViewport(int lenX, int lenY) {
	// Create a viewport that is lenX x lenY
	// Then perform a window-to-viewport mapping on the clipped polygon (see question 4 as an example)

	// WindowToViewportMapping()

	;
}

void WindowToViewportMapping() {
	// Map clipped polygon to predefined viewport

	;
}

void ResizeViewport() {
	// Resize viewport by dragging topright corner of viewport
	// Scale the clipped polygon as well
	;
}

void ScaleClippedPolygon() {
	// Scale the clipped polygon
	;
}

void ChangeWindowSize() {
	// Change the window size
	// Dynamically perform window-to-viewport mapping as you resize window
	// Achieve the ZOOM-IN and ZOOM-OUT effect
	;
}

void TranslateWindow() {
	// Move the window horizontally or vertically
	// Map the clipped polygon from window to viewport acheiving the PANNING effect
	;
}

// Used to redraw lines in vector of controlPoints
void DrawLineSegments() {
	if (controlPoints.size() > 0) {
		// For every 2 vertexes/elements in controlPoints, draw a line
		for (unsigned int i = 0; i < controlPoints.size()-1; i++) {
			// If the current vertex or the next vertex in controlPoints has an x-coordinate = -1, skip it
			// In other words, this if-statement indicates the end of a line in our 'controlPoints' vector
			if (controlPoints.at(i).first == -1 || controlPoints.at(i+1).first == -1) {
				continue;
			}
			glBegin(GL_LINES);
				glVertex2f(controlPoints.at(i).first, controlPoints.at(i).second);
				glVertex2f(controlPoints.at(i+1).first, controlPoints.at(i+1).second);
			glEnd();
		}
	}
}

// Mouse button handler
void mouseFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			if (pressedRightButton) {
				pressedRightButton = false;
				controlPoints.clear();
			}
			previousControlPoint = make_pair<float,float>(x,y);
			controlPoints.push_back(previousControlPoint);	// Add a vertex to control points list
			drawDynamicLine = true;
			pressedLeftButton = true;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			// Check if we have pressed left mouse button down at least once
			if (pressedLeftButton) {
				controlPoints.push_back(make_pair<float,float>(x,y)); // Add final vertex for this line to vector
				controlPoints.push_back(dummyVertex);	// Add dummyVertex (-1,-1) to indicate end of line in our vector

				// Reset some global variable values
				previousControlPoint = dummyVertex;
				drawDynamicLine = false;
				pressedLeftButton = false;
				pressedRightButton = true;
			}
		}
	}
}

// Mouse movement handler
void passiveMotionFunc(int x, int y) {
	if (drawDynamicLine) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    	glClear(GL_COLOR_BUFFER_BIT);

		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0);	// set coordinate system to match window size

		// Draw line segments stored in vector
		DrawLineSegments();

		// if (drawCircle)
		// 	MidpointCircleAlgorithm(glutGet(GLUT_WINDOW_WIDTH) * 0.5, glutGet(GLUT_WINDOW_HEIGHT) * 0.5, 100);

		// Now draw the dynamic line with mouse cursor position as the end vertex
		glBegin(GL_LINES);
			glVertex2f(previousControlPoint.first, previousControlPoint.second);
			glVertex2f(x, y);
		glEnd();

		glPopMatrix();
		glLoadIdentity();

		glFlush(); // render now
	}
}

// keyboard shortcuts
void keyboardFunc(unsigned char key, int x, int y) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0);	// set coordinate system to match window size

	// 'key' values based on ASCII values
	switch (key) {
		case 's':
			lineStippleEnabled = !lineStippleEnabled;
			if (lineStippleEnabled) {
				glEnable(GL_LINE_STIPPLE);
				cout << "line stipple enabled" << endl;
			}
			else {
				glDisable(GL_LINE_STIPPLE);
				cout << "line stipple disabled" << endl;
			}
			DrawLineSegments();
			break;
		case '1':
			glLineStipple(1, 0xAAAA);
			DrawLineSegments();
			cout << "stipple factor 1" << endl;
			break;
		case '2':
			glLineStipple(2, 0xAAAA);
			DrawLineSegments();
			cout << "stipple factor 2" << endl;
			break;
		case '3':
			glLineStipple(3, 0xAAAA);
			DrawLineSegments();
			cout << "stipple factor 3" << endl;
			break;
		case '4':
			glLineStipple(4, 0xAAAA);
			DrawLineSegments();
			cout << "stipple factor 4" << endl;
			break;
		case 'w':
			glLineWidth(5);
			DrawLineSegments();
			cout << "line width changed" << endl;
			break;
		case 'W':
			glLineWidth(10);
			DrawLineSegments();
			cout << "line width changed" << endl;
			break;
		case 'r':
			glLineWidth(1);
			DrawLineSegments();
			cout << "line width reset to 1" << endl;
			break;
		default:
			DrawLineSegments();
			break;
	}

	// if (drawCircle)
	// 	MidpointCircleAlgorithm(glutGet(GLUT_WINDOW_WIDTH) * 0.5, glutGet(GLUT_WINDOW_HEIGHT) * 0.5, 100);

	glPopMatrix();
	glLoadIdentity();

	glFlush(); // render when button pressed;
}

void display() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0);	// set coordinate system to match window size

	// Draw line segments stored in vector if any
	DrawLineSegments();

	// if (drawCircle)
	// 	MidpointCircleAlgorithm(glutGet(GLUT_WINDOW_WIDTH) * 0.5, glutGet(GLUT_WINDOW_HEIGHT) * 0.5, 100);

	glPopMatrix();
	glLoadIdentity();

	glFlush();	// Render now
}

int main(int argc, char** argv) {
	// Called once process starts
	cout << "Keys:\n(s) - toggle line stipple on/off\n(1,2,3,4) - Change stipple factor\n(w,W) - Change line width\n(r) - reset line width" << endl;

	// Initialize window
	glutInit(&argc, argv);
    glutCreateWindow("Hello CS460/560");
    glutInitWindowSize(500, 500);
	glutReshapeWindow(500, 500);
    glutInitWindowPosition(200,200);

	// Called once everytime window is resized
    glutDisplayFunc(display);	// Called when window is created/resized

	// Called every frame
	glutMouseFunc(mouseFunc);
	glutPassiveMotionFunc(passiveMotionFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMainLoop();	// Enter event processing loop

	return 0;
}
