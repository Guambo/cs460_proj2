#include <GL/glut.h>
#include <cmath>  // abs()
#include <assert.h>
#include <iterator>
#include <iostream>
#include <vector>

using namespace std;

vector<pair<float, float>> controlPoints;
vector<pair<float, float>> clippingRectanglePoints; // should be a total of 5 (last point is a duplicate point)
vector<pair<float, float>> clippedPolygonPoints; // Contain all output vertexes after performing SutherlandHodgmanClipping()
pair<float,float> dummyVertex(-1,-1);
pair<float,float> previousControlPoint (dummyVertex);

int menuEntryNum = -1;
int menuId;

bool redrawLineSegments = true;
bool redrawClippingRectangle = false;
bool redrawClippedPolygon = false;
bool refillClippedPolygon = true;
bool viewportDefined = false;
bool drawDynamicLine = false;
bool pressedLeftButton = false;
bool pressedRightButton = false;
bool lineStippleEnabled = false;

// Menu Option 0
void DrawRectangularClippingWindow(int lenX, int lenY, int centerX, int centerY) {
	// Draw a clipping rectangular that is lenX x lenY pixels
	// Draw from minX, minY to maxX, maxY
	// Use line stipple to draw
	int offsetX = lenX * 0.5;
	int offsetY = lenY * 0.5;

	int minX = centerX - offsetX;
	int minY = centerY - offsetY;

	int maxX = centerX + offsetX;
	int maxY = centerY + offsetY;

	if (clippingRectanglePoints.size() == 0) {
		// add points to vector
		clippingRectanglePoints.emplace_back(make_pair<float,float>(minX,minY));
		clippingRectanglePoints.emplace_back(make_pair<float,float>(minX,maxY));
		clippingRectanglePoints.emplace_back(make_pair<float,float>(maxX,maxY));
		clippingRectanglePoints.emplace_back(make_pair<float,float>(maxX,minY));
		clippingRectanglePoints.emplace_back(make_pair<float,float>(minX,minY));
	}

	glEnable(GL_LINE_STIPPLE);
		glLineStipple(4, 0xAAAA); // Define line stipple type
			glBegin(GL_LINES);
				glVertex2f(minX, minY);
				glVertex2f(minX, maxY);

				glVertex2f(minX, maxY);
				glVertex2f(maxX, maxY);

				glVertex2f(maxX, maxY);
				glVertex2f(maxX, minY);

				glVertex2f(maxX, minY);
				glVertex2f(minX, minY);
			glEnd();
	glDisable(GL_LINE_STIPPLE);

	glFlush();
}

// Menu Option 1
void SutherlandHodgmanClipping() {
	// Perform the Sutherland-Hodgman algorithm
	// For each edge in the clipping rectangle
	for (unsigned int i = 0; i < clippingRectanglePoints.size()-1; i++) {
		// Clip polygon on edge created from
		// clippingRectanglePoints i to i+1
		pair<float, float> vertex1 = clippingRectanglePoints[i];
		pair<float, float> vertex2 = clippingRectanglePoints[i+1];

		// For each control point in the polygon...
		for (unsigned int j = 0; j < controlPoints.size(); j++) {
			pair<float, float> currentPoint = controlPoints[i];
			pair<float, float> previousPoint = controlPoints[(i + controlPoints.size() - 1) % controlPoints.size()];
		}
	}
}

// Menu Option 2
void DoRegionFilling() {
	// Fill the clipped polygon using a polygon filling algorithm to fill a red color
	// ex: boundary filling, flood-filling, scan-line
	// Hint: use glReadPixels and store values in a buffer, then use glDrawPixels (see hint 4 in overview)
	;
}

// Menu Option 3
void DefineViewport(int lenX, int lenY, int minX, int minY) {
	// Create a viewport that is lenX x lenY
	// Then perform a window-to-viewport mapping on the clipped polygon (see question 4 as an example)

	int maxX = minX + lenX;
	int maxY = minY + lenY;

	// WindowToViewportMapping()

	viewportDefined = true;

	;
}

// start - Passive functions to be called in passiveMotionFunc
// They are performed by dragging the top right corner of the viewport or window
// Or moving the window horizontally/vertically

// Performed passively once we define a viewport
void WindowToViewportMapping() {
	// Map clipped polygon to predefined viewport

	;
}

void ResizeViewport() {
	// Resize viewport by dragging topright corner of viewport
	// Scale the clipped polygon as well

	// ScaleClippedPolygon();

	;
}

void ResizeWindow() {
	// Change the window size
	// Dynamically perform window-to-viewport mapping as you resize window
	// Achieve the ZOOM-IN and ZOOM-OUT effect

	// ScaleClippedPolygon();

	;
}

void ScaleClippedPolygon() {
	// Scale the clipped polygon
	;
}

void TranslateWindow() {
	// Move the window horizontally or vertically
	// Map the clipped polygon from window to viewport acheiving the PANNING effect

	// WindowToViewportMapping();

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

void Redraw() {
	if (redrawLineSegments) {
		DrawLineSegments();
	}
	if (redrawClippingRectangle) {
		DrawRectangularClippingWindow(200, 200, glutGet(GLUT_WINDOW_WIDTH) * 0.5, glutGet(GLUT_WINDOW_WIDTH) * 0.5);
	}
	if (redrawClippedPolygon) {
		// SutherlandHodgmanClipping();
		if (refillClippedPolygon) {
			//DoRegionFilling();
		}
	}
}
// end - Passive functions to be called in passiveMotionFunc


// start - dynamic line drawing
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

		// Redraw previous drawings
		Redraw();

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
// end - dynamic line drawing

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
			break;
		case '1':
			glLineStipple(1, 0xAAAA);
			cout << "stipple factor 1" << endl;
			break;
		case '2':
			glLineStipple(2, 0xAAAA);
			cout << "stipple factor 2" << endl;
			break;
		case '3':
			glLineStipple(3, 0xAAAA);
			cout << "stipple factor 3" << endl;
			break;
		case '4':
			glLineStipple(4, 0xAAAA);
			cout << "stipple factor 4" << endl;
			break;
		case 'w':
			glLineWidth(5);
			cout << "line width changed" << endl;
			break;
		case 'W':
			glLineWidth(10);
			cout << "line width changed" << endl;
			break;
		case 'r':
			glLineWidth(1);
			cout << "line width reset to 1" << endl;
			break;
		default:
			break;
	}

	// Redraw previous drawings
	Redraw();

	glPopMatrix();
	glLoadIdentity();

	glFlush(); // render when button pressed;
}

// start - menu functions
// Grabs the value of the menu and sets global variable menuEntryNum to menu 'num'
void menu(int num) {
	menuEntryNum = num;
}

// Create the menu's
void createMenu(void) {
	menuId = glutCreateMenu(menu);
	glutAddMenuEntry("Clipping Rectangle", 0);	// DrawRectangularClippingWindow()
	glutAddMenuEntry("Sutherman Clipping", 1); 	// SutherlandHodgmanClipping()
	glutAddMenuEntry("Region Filling", 2);	// 	DoRegionFilling()
	glutAddMenuEntry("Create Viewport", 3);	// 	DefineViewport() -> calls WindowToViewportMapping()
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

void DoMenuSelection() {
	switch (menuEntryNum) {
		case 0:
			DrawRectangularClippingWindow(200, 200, glutGet(GLUT_WINDOW_WIDTH) * 0.5, glutGet(GLUT_WINDOW_WIDTH) * 0.5);
			redrawClippingRectangle = true;
			break;
		case 1:
			SutherlandHodgmanClipping();
			redrawClippedPolygon = true;
			break;
		case 3:
			DoRegionFilling();
			refillClippedPolygon = true;
			break;
		case 4:
			DefineViewport(40, 80, 5, 5);
			break;
		default:
			break;
	}
}
// end - menu functions

void display() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	// start - changing window coordinate system
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0);	// set coordinate system to match window size

	Redraw();

	// Performing menu actions
	DoMenuSelection();

	// if (drawCircle)
	// 	MidpointCircleAlgorithm(glutGet(GLUT_WINDOW_WIDTH) * 0.5, glutGet(GLUT_WINDOW_HEIGHT) * 0.5, 100);

	glPopMatrix();
	glLoadIdentity();
	// end - returning to previous window coordinate system

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

	// Create the menus
	createMenu();

	// Called once everytime window is resized
    glutDisplayFunc(display);	// Called when window is created/resized

	// Called every frame
	glutMouseFunc(mouseFunc);
	glutPassiveMotionFunc(passiveMotionFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMainLoop();	// Enter event processing loop

	return 0;
}
