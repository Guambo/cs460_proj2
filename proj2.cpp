#include <GL/glut.h>
#include <iterator>
#include <iostream>
#include <vector>

using namespace std;

//-------------------------------- Global Variables ----------------------------------//
vector<pair<float, float>> controlPoints;			// Dynamic line drawn Polygon vertexes
vector<pair<float, float>> clippingRectanglePoints; // Clipping window vertexes
vector<pair<float, float>> clippedPolygonPoints; 	// Contain all output vertexes after performing SutherlandHodgmanClipping()
vector<pair<float, float>> viewportPolygonPoints;	// Contain all window-to-viewport mapped vertexes from clippedPolygonPoints
pair<float,float> dummyVertex(-1,-1);				// For dynamic line drawing
pair<float,float> previousControlPoint (dummyVertex);	// For dynamic line drawing

// Parameters passed into CreateViewport function (line 556)
float viewportMaxSizeX = 40;
float viewportMaxSizeY = 80;
float viewportMinX = 0;
float viewportMinY = 0;

// World Coordinates
float xMin;
float yMin;
float xMax;
float yMax;

// Viewport Coordinates
float uMin;
float vMin;
float uMax;
float vMax;

// Max window size
float maxWindowSizeX = 1000;
float maxWindowSizeY = 500;

// Menu variables
int menuEntryNum = -1;
int menuId;

// Booleans used in Redraw() and Redraw2()
bool redrawLineSegments = true;
bool redrawClippedPolygon = false;
bool redrawClippingRectangle = false;
bool clippedPolygonFilled = false;
bool viewportDefined = false;

// Booleans used in mouseFunc() and passiveMotionFunc()
bool drawDynamicLine = false;
bool pressedLeftButton = false;
bool pressedRightButton = false;
bool allowRegionFill = false;

//-------------------------------- Function declarations --------------------------------//
void WindowToViewportMapping();
void DrawRectangularClippingWindow(int lenX, int lenY, int centerX, int centerY);
void DoRegionFilling(int x, int y);

//-------------------------------- Draw Functions ----------------------------------//
void DrawViewportPolygion() {
	if (viewportPolygonPoints.empty()) {
		return;
	}

	glColor3f(1.0, 1.0, 1.0);
	for (unsigned int i = 0; i < clippedPolygonPoints.size()-1; i++) {
		glBegin(GL_LINE_LOOP);
			glVertex2f(clippedPolygonPoints[i].first, clippedPolygonPoints[i].second);
			glVertex2f(clippedPolygonPoints[i+1].first, clippedPolygonPoints[i+1].second);
		glEnd();
	}
}

// Draws the clipped polygon
void DrawClippedPolygon() {
	if (clippedPolygonPoints.empty()) {
		cout << "clippedPolygonPoints.empty" << endl;
		return;
	}

	glColor3f(1.0, 1.0, 1.0);
	for (unsigned int i = 0; i < clippedPolygonPoints.size()-1; i++) {
		glBegin(GL_LINE_LOOP);
			glVertex2f(clippedPolygonPoints[i].first, clippedPolygonPoints[i].second);
			glVertex2f(clippedPolygonPoints[i+1].first, clippedPolygonPoints[i+1].second);
		glEnd();
	}
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
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_LINES);
				glVertex2f(controlPoints.at(i).first, controlPoints.at(i).second);
				glVertex2f(controlPoints.at(i+1).first, controlPoints.at(i+1).second);
			glEnd();
		}
	}
}

void Redraw() {
	if (clippedPolygonFilled) {
		return;
	}

	//cout << "Clearing screen Redraw" << endl;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (viewportDefined) {
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0.0, maxWindowSizeX, maxWindowSizeY, 0.0);

		DrawViewportPolygion();

		glPopMatrix();
		glLoadIdentity();
	}
	else {
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0);	// set coordinate system to match window size

		if (redrawClippingRectangle) {
			DrawRectangularClippingWindow(200, 200, glutGet(GLUT_WINDOW_WIDTH) * 0.5, glutGet(GLUT_WINDOW_HEIGHT) * 0.5);
		}
		if (redrawClippedPolygon) {
			DrawClippedPolygon();
		}
		else if (redrawLineSegments) {
			DrawLineSegments();
		}

		glPopMatrix();
		glLoadIdentity();
	}

	glFlush();
}

void Redraw2() {
	if (clippedPolygonFilled) {
		return;
	}

	if (redrawClippingRectangle) {
		DrawRectangularClippingWindow(200, 200, glutGet(GLUT_WINDOW_WIDTH) * 0.5, glutGet(GLUT_WINDOW_HEIGHT) * 0.5);
	}
	if (redrawClippedPolygon) {
		DrawClippedPolygon();
	}
	else if (redrawLineSegments) {
		DrawLineSegments();
	}
}

//-------------------------------- Sutherland Hodgman Clipping ----------------------------------//
// SutherlandHodgmanClipping helper function
pair<float, float> computeIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
		float x = ((x1*y2 - y1*x2)*(x3 - x4) - (x1-x2)*(x3*y4 - y3*x4)) / ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));
		float y = ((x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4 - y3*x4)) / ((x1-x2)*(y3-y4) - (y1-y2)*(x3 - x4));

		pair<float, float> intersectionPoint (x,y);

		return intersectionPoint;
}

// Called in DoPolygonClipping() (Menu Option 2)
void SutherlandHodgmanClipping() {
	if (controlPoints.empty()) {
		return;
	}

	// Perform the Sutherland-Hodgman algorithm
	// For each edge in the clipping rectangle
	for (unsigned int i = 0; i < clippingRectanglePoints.size()-1; i++) {
		// Clip polygon on edge created from
		// clippingRectanglePoints i to i+1
		pair<float, float> clippingV1 = clippingRectanglePoints[i];
		pair<float, float> clippingV2 = clippingRectanglePoints[i+1];

		// Cache individual coordinates
		float x1 = clippingV1.first;
		float y1 = clippingV1.second;

		float x2 = clippingV2.first;
		float y2 = clippingV2.second;

		float differenceX = x2 - x1;
		float differenceY = y2 - y1;

		clippedPolygonPoints.clear();

		// For each control point in the polygon...
		for (unsigned int j = 0; j < controlPoints.size(); j++) {
			int prevIndex = (j + controlPoints.size() - 1) % controlPoints.size();

			pair<float, float> currentPoint = controlPoints[j];
			pair<float, float> previousPoint = controlPoints[prevIndex];

			// Cache coordinates
			float currentX = currentPoint.first;
			float currentY = currentPoint.second;

			float previousX = previousPoint.first;
			float previousY = previousPoint.second;

			// Calculate currentPointClipped and previousPointClipped w.r.t clipping edge vertexes clippingV1 and clippingV2
			float currentPointClipped = ((differenceX)*(currentY - y1)) - ((differenceY)*(currentX - x1));
			float previousPointClipped = ((differenceX)*(previousY - y1)) - ((differenceY)*(previousX - x1));

			// Account for three scenarios:
			// 1. clipped points are inside the clipping region (value < 0)
			//	  a) both clipped points
			//	  b) only 1 clipped point
			// 2. clipped points lie on clipping edge (value = 0)
			//	  a) both clipped points
			//	  b) only 1 clipped point
			// 3. clipping points are not inside clipping region (value > 0)

			if (currentPointClipped < 0 && previousPointClipped < 0) {
				// Only currentPoint is added
				clippedPolygonPoints.push_back({ currentX, currentY });
			}
			else if (currentPointClipped >= 0 && previousPointClipped < 0) {
				// Only point of intersection with clipping edge is added
				pair<float, float> intersectionPoint = computeIntersection(x1, y1, x2, y2, previousX, previousY, currentX, currentY);

				clippedPolygonPoints.push_back(intersectionPoint);
			}
			else if (currentPointClipped < 0 && previousPointClipped >= 0) {
				// Point of intersection with clipping edge is added
				// Current point is also added
				pair<float, float> intersectionPoint = computeIntersection(x1, y1, x2, y2, previousX, previousY, currentX, currentY);

				clippedPolygonPoints.push_back(intersectionPoint);
				clippedPolygonPoints.push_back(currentPoint);
			}
		}

		// Copy the vertexes in clippedPolygonPoints into controlPoints
		controlPoints.clear();
		for (unsigned int j = 0; j < clippedPolygonPoints.size(); j++) {
			controlPoints.push_back(clippedPolygonPoints[j]);
		}
	}
	// Finally, add the first point to the end of the clippingPolygonPoints vector
	// This is so that when it is drawn, it will connect the last vertex in the list with the first vector
	clippedPolygonPoints.push_back(clippedPolygonPoints[0]);
}

//-------------------------------- Color Boundary Fill Algorithm ----------------------------------//
// This is a 4-fill recursive boundary fill algorithm
// Assumption: parameters 'boundaryColor' and 'fillColor' contain only 3 elements
// Caslled in DoRegionFilling() (Menu Option 3)
void boundaryFill(int x, int y, float * fillColor, float * boundaryColor) {
	float pixelColor[3];
	// First read the color of the pixel at (x,y)
	// Store in pixelColor buffer
	glReadPixels(x, y, 1.0, 1.0, GL_RGB, GL_FLOAT, pixelColor);

	// Check if pixelColor does not equal the boundary color and fill color...
	if ((pixelColor[0] != boundaryColor[0] || pixelColor[1] != boundaryColor[1] || pixelColor[2] != boundaryColor[2]) && (pixelColor[0] != fillColor[0] || pixelColor[1] != fillColor[1] || pixelColor[2] != fillColor[2])) {
		// Draw red pixel at (x,y)
		glColor3f(fillColor[0], fillColor[1], fillColor[2]);
		glBegin(GL_POINTS);
			glVertex2i(x, y);
		glEnd();
		boundaryFill(x + 1, y, fillColor, boundaryColor);
		boundaryFill(x - 1, y, fillColor, boundaryColor);
		boundaryFill(x, y + 1, fillColor, boundaryColor);
		boundaryFill(x, y - 1, fillColor, boundaryColor);
	}
}

//-------------------------------- Window to Viewport Mapping ----------------------------------//
void WindowToViewportMapping() {
	// Map clipped polygon to predefined viewport
	for (unsigned int i = 0; i < clippedPolygonPoints.size(); i++) {
		float x = clippedPolygonPoints[i].first;
		float y = clippedPolygonPoints[i].second;

		float transformedX = (x - xMin) * ((uMax - uMin)/(xMax -xMin)) + uMin;
		float transformedY = (y - yMin) * ((vMax - vMin)/(yMax -yMin)) + vMin;

		viewportPolygonPoints.push_back({transformedX, transformedY});
	}

	// Draw polygon and viewport
	viewportDefined = true;
	clippedPolygonFilled = false;
	Redraw();
	glutReshapeWindow(uMax, vMax);
	glutPostRedisplay();
}


//-------------------------------- Mouse Functions ----------------------------------//
// Mouse button handler
void mouseFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			if (allowRegionFill) {
				DoRegionFilling(x,y);
				clippedPolygonFilled = true;
				allowRegionFill = false;
				cout << "Region filled with color red." << endl;
			}
			else {
				if (pressedRightButton) {
					pressedRightButton = false;
					controlPoints.clear();

					// RESET PROJECT 2 VARIABLES
					clippedPolygonPoints.clear();
					viewportPolygonPoints.clear();
					clippingRectanglePoints.clear();
					redrawClippedPolygon = false;
					redrawLineSegments = true;
					clippedPolygonFilled = false;
					viewportDefined = false;
				}
				previousControlPoint = make_pair<float,float>(x,y);
				controlPoints.push_back(previousControlPoint);	// Add a vertex to control points list
				drawDynamicLine = true;
				pressedLeftButton = true;
			}
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
		//cout << "Clearing screen passiveMotionFunc" << endl;
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0);	// set coordinate system to match window size

		Redraw2();

		// Now draw the dynamic line with mouse cursor position as the end vertex
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
			glVertex2f(previousControlPoint.first, previousControlPoint.second);
			glVertex2f(x, y);
		glEnd();

		glPopMatrix();
		glLoadIdentity();

		glFlush(); // render now
	}
}

//-------------------------------- Menu Functions ----------------------------------//
// Grabs the value of the menu and sets global variable menuEntryNum to menu 'num'
void menu(int num) {
	menuEntryNum = num;
}
// Create the menu entries
void createMenu(void) {
	menuId = glutCreateMenu(menu);
	glutAddMenuEntry("Create Clipping Rectangle", 0);	// Calls -> DrawRectangularClippingWindow()
	glutAddMenuEntry("Sutherland-Hodgman Clipping", 1); 	// Calls DoPolygonClipping -> calls SutherlandHodgmanClipping()
	glutAddMenuEntry("Region Filling", 2);	// 	Enables region filling on mouse left-click -> calls DoRegionFilling()
	glutAddMenuEntry("Create Viewport", 3);	// 	CreateViewport() -> calls WindowToViewportMapping()
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

// Menu Option 1
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
			glColor3f(1, 1, 1);
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

	//glFlush();
}

// Menu Option 2
void DoPolygonClipping() {
	if (!clippedPolygonPoints.empty()) {
		return;
	}

	// Perform Sutherland-Hodman Clipping algorithm
	// Clipping points stored in clippedPolygonPoints vector
	SutherlandHodgmanClipping();

	if (clippedPolygonPoints.empty()) {
		return;
	}

	redrawLineSegments = false;
	redrawClippedPolygon = true;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0);	// set coordinate system to match window size

	DrawRectangularClippingWindow(200, 200, glutGet(GLUT_WINDOW_WIDTH) * 0.5, glutGet(GLUT_WINDOW_HEIGHT) * 0.5);
	DrawClippedPolygon();

	glPopMatrix();
	glLoadIdentity();

	glFlush();
}

// Menu Option 3
void DoRegionFilling(int x, int y) {
	// Fill the clipped polygon using boundary filling algorithm

	float fillColor[] = {1.0, 0.0, 0.0};
	float boundaryColor[] = {1.0, 1.0, 1.0};

	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT));	// set coordinate system to match window size

	// Call recursive function boundaryFill()
	boundaryFill(x, y, fillColor, boundaryColor);

	glPopMatrix();
	glLoadIdentity();

	glFlush();
}

// Menu Option 4
void CreateViewport(int lenX, int lenY, int minX, int minY) {
	// Create a viewport that is lenX x lenY
	// Then perform a window-to-viewport mapping on the clipped polygon (see question 4 as an example)

	// World coordinates
	xMin = 0;
	yMin = 0;
	xMax = maxWindowSizeX;
	yMax = maxWindowSizeY;

	// Viewport Coordinates
	uMin = minX;
	vMin = minY;
	uMax = uMin + lenX;
	vMax = vMin + lenY;

	// Begin mapping clippedPolygonPoints to viewport
	WindowToViewportMapping();
}

// Menu Entry Selection handler
void DoMenuSelection() {
	switch (menuEntryNum) {
		case 0:
			redrawClippingRectangle = true;
			viewportDefined = false;
			Redraw();
			cout << "Drew a rectangle clipping box" << endl;
			break;
		case 1:
			if (!controlPoints.empty()) {
				DoPolygonClipping();
				redrawClippedPolygon = true;
				cout << "Clipped polygon." << endl;
			}
			else {
				cout << "Please draw a polygon using left mouse button down." << endl;
			}
			break;
		case 2:
			if (!clippedPolygonFilled && !clippedPolygonPoints.empty() && !drawDynamicLine) {
				allowRegionFill = true;
				cout << "Please select the region to be filled." << endl;
			}
			else {
				if (clippedPolygonFilled) {
					cout << "Polgyon is already colored." << endl;
				}
				else {
					cout << "Please create a clipped polygon first." << endl;
				}
			}
			break;
		case 3:
			if (!clippedPolygonPoints.empty()) {
				CreateViewport(viewportMaxSizeX, viewportMaxSizeY, viewportMinX, viewportMinY);
			}
			else {
				cout << "Cannot create viewport. Please create a clipped polygon." << endl;
			}
			break;
		default:
			break;
	}
	// Reset after done using menu
	menuEntryNum = -1;
}

//-------------------------------- Display and Main ----------------------------------//

void display() {
	if (!drawDynamicLine) {
		//cout << "Redraw - display" << endl;
		// Performing menu actions
		DoMenuSelection();
		Redraw();
	}
}

int main(int argc, char** argv) {
	// Initialize window
	glutInit(&argc, argv);
    glutCreateWindow("Project 2");
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(500, 500);
	glutReshapeWindow(maxWindowSizeX, maxWindowSizeY);
    glutInitWindowPosition(200,200);

	// Initialize menu
	createMenu();

	// Display
    glutDisplayFunc(display);

	// Mouse functions
	glutMouseFunc(mouseFunc);
	glutPassiveMotionFunc(passiveMotionFunc);

	// Enter event processing loop
	glutMainLoop();

	return 0;
}
