CS460 Assignment 2
---
---
Author: Nicholas Vallejos
---
---

Compiling
---
Open bash terminal, navigate to project directory and type 'make.'
This will create an executable called 'a.out'

Running
---
Type './a.out' to run my project.

Instructions on how to use my Program
---
After running the program, to open the menu please press 'Middle Mouse Click'.

My menu has 4 options:
- 'Clipping Rectangle'
- 'Sutherland Clipping'
- 'Region Filling'
	- It is important to note that once 'Region Filling' is selected, you then have to use your mouse and 'left-click' to select a region to fill with color.
- 'Create Viewport'

What I implemented
---
- Creating a clipping window (rectangle)
- Draw a polygon
- Clip the polygon with respect to the clipping rectangle (Uses Sutherland-Hodgman Algorithm)
- Fill the clipped polygon with color red
- Create a viewport
- Scaling effect by changing viewport size dynamically

What I didn't implement
---
- Zomm-in Zoom-out effect
- Panning effect
