#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <iostream>
#include <vector>

using namespace std;

float canvasSize[] = { 10.0f, 10.0f };
int rasterSize[] = { 800, 600 };

float color[3];
float lineWidth = 1.0f;
float mousePos[2];

float pointSize;

enum struct selection { none, point, line, triangle, quadrilateral, polygon}; 
selection select = selection::none;

struct Point
{
	float xCoord;
	float yCoord;
	float color[3];
	float size;
};
Point point;
std::vector<Point> points;

class Line
{
public:
	float color[3];
	float width;
	std::vector<Point> linePoints;
};
Line line;
std::vector<Line> lineList;

class Triangle
{
public:
	float vertexArray[6];
	float color[3];
};
Triangle triangle;
int numberOfVertices;
std::vector<Triangle> triangleList;

//Quad structure
class Quad
{
public:
	float quadOrigin[2];
	float quadEnd[2];
	float color[3];
};
Quad quad;
std::vector<Quad> quadList;
int numberOfQuadInputs = 0;

class Poly
{
public:
	float color[3];
	std::vector<Point> polygonVertices;
};
Poly polygon;
std::vector<Poly> polygonList;

void CreatePoint(Point point)
{
	glColor3f(point.color[0], point.color[1], point.color[2]);
	glPointSize(point.size);
	glBegin(GL_POINTS);
	glVertex2f(point.xCoord, point.yCoord);
	glEnd();
}

void drawCurrentLine(Line line)
{
	glLineWidth(lineWidth);
	glColor3fv(color);
	glBegin(GL_LINE_STRIP);
	for (Point& linePoints : line.linePoints)
	{
		glVertex2f(linePoints.xCoord, linePoints.yCoord);
	}
	if (select == selection::line)
	{
		glVertex2f(mousePos[0], mousePos[1]);
	}
	glEnd();
}

void drawLine(Line line)
{
	glLineWidth(line.width);
	glBegin(GL_LINE_STRIP);
	glColor3fv(line.color);
	
	for (Point& linePoints : line.linePoints)
	{
		glVertex2f(linePoints.xCoord, linePoints.yCoord);
	}
	glEnd();
}

void drawTriangle(Triangle triangle)
{
	glColor3f(triangle.color[0], triangle.color[1], triangle.color[2]);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 3; i++)
	{
		glVertex2fv(triangle.vertexArray + i * 2);
	}
	glEnd();
}

void drawQuad(Quad quad)
{
	glColor3f(quad.color[0], quad.color[1], quad.color[2]);
	glBegin(GL_QUADS);
	glVertex2f(quad.quadOrigin[0], quad.quadOrigin[1]);
	glVertex2f(quad.quadEnd[0], quad.quadOrigin[1]);
	glVertex2f(quad.quadEnd[0], quad.quadEnd[1]);
	glVertex2f(quad.quadOrigin[0], quad.quadEnd[1]);
	glEnd();
}

void resetQuadInProgress()
{
	numberOfQuadInputs = 0;
}

void resetTriangleInProgress()
{
	numberOfVertices = 0;
}

void finishDrawingCurrentPolygon()
{
	polygon.color[0] = color[0];
	polygon.color[1] = color[1];
	polygon.color[2] = color[2];
	polygonList.push_back(polygon);
	polygon.polygonVertices.clear();
}

void finishDrawingCurrentLine()
{

	line.color[0] = color[0];
	line.color[1] = color[1];
	line.color[2] = color[2];
	line.width = lineWidth;
	lineList.push_back(line);
	line.linePoints.clear();
}

void init(void)
{
	for (int i = 0; i < 6; i++)
	{
		triangle.vertexArray[i] = 0.0f;
	}
	quad.quadOrigin[0] = quad.quadOrigin[1] = 0.0f;
	quad.quadEnd[0] = quad.quadEnd[1] = 0.0f;
	mousePos[0] = mousePos[1] = 0.0f;
	color[0] = 0.0f;
	color[1] = color[2] = 0.0f;
}

void drawCursor()
{
	glColor3f(1.0f, 0.0f, 1.0f);
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glVertex2fv(mousePos);
	glEnd();
	glPointSize(1.0f);
}

void drawAllPoints()
{
	for (Point& point : points)
	{
		CreatePoint(point);
	}
}

void drawAllLines()
{
	for (Line& line : lineList)
	{
		drawLine(line);
	}
}

void drawTriangleOutLine()
{
	if (numberOfVertices > 0 && numberOfVertices < 3)
	{
		glColor3fv(color);
		glBegin(GL_LINE_STRIP);
		glVertex2fv(triangle.vertexArray);
		glVertex2fv(mousePos);
		glEnd();

		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < numberOfVertices; i++)
		{
			glVertex2fv(triangle.vertexArray + i * 2);

		}
		glVertex2fv(mousePos);
		glEnd();
	}
}

void drawAllTriangles()
{
	for (Triangle& triangle : triangleList)
	{
		drawTriangle(triangle);
	}
}

void drawQuadOutline()
{
	if (numberOfQuadInputs == 1)
	{
		glColor3fv(color);
		glBegin(GL_LINE_STRIP);
		glVertex2f(quad.quadOrigin[0], quad.quadOrigin[1]);//(x,y)
		glVertex2f(mousePos[0], quad.quadOrigin[1]);//(x',y)
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex2f(quad.quadOrigin[0], quad.quadOrigin[1]);//(x,y)
		glVertex2f(quad.quadOrigin[0], mousePos[1]);//(x,y')
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex2f(mousePos[0], quad.quadOrigin[1]);//(x',y)
		glVertex2f(mousePos[0], mousePos[1]);//(x',y')
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex2f(quad.quadOrigin[0], mousePos[1]);//(x,y')
		glVertex2f(mousePos[0], mousePos[1]);//(x',y')
		glEnd();
	}
}

void drawAllQuads()
{
	for (Quad& quad : quadList)
	{
		drawQuad(quad);
	}
}

void drawCurrentPolygon(Poly polygon)
{
	glColor3fv(color);
	if (polygon.polygonVertices.size() < 2)
	{
		glBegin(GL_LINE_STRIP);
		for (Point& point : polygon.polygonVertices)
		{
			glVertex2f(point.xCoord, point.yCoord);
		}
		if (select == selection::polygon)
		{
			glVertex2fv(mousePos);
		}
		glEnd();
	}
	else if (polygon.polygonVertices.size() >= 2)
	{
		glColor3fv(color);
		glBegin(GL_POLYGON);
		for (Point& point : polygon.polygonVertices)
		{
			glVertex2f(point.xCoord, point.yCoord);
		}
		if (select == selection::polygon)
		{
			glVertex2fv(mousePos);
		}
		glEnd();
	}
}

void drawPolygon(Poly polygon)
{
	glColor3fv(polygon.color);
	glBegin(GL_POLYGON);
	for (Point& point : polygon.polygonVertices)
	{
		glVertex2f(point.xCoord, point.yCoord);
	}
	glEnd();
}

void drawAllPolygons()
{
	for (Poly& polygon : polygonList)
	{
		drawPolygon(polygon);
	}
}

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3fv(color);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	drawAllPoints();

	drawCurrentLine(line);
	drawAllLines();

	glLineWidth(1.0f);

	if (select == selection::triangle)
	{
		drawTriangleOutLine();
	}
	drawAllTriangles();

	if (select == selection::quadrilateral)
	{
		drawQuadOutline();
	}
	drawAllQuads();
	
	drawCurrentPolygon(polygon);
	drawAllPolygons();

	drawCursor();

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	rasterSize[0] = w;
	rasterSize[1] = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, canvasSize[0], 0.0, canvasSize[1]);
	glViewport(0, 0, rasterSize[0], rasterSize[1]);

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mousePos[0] = (float)x / rasterSize[0] * canvasSize[0];
		mousePos[1] = (float)(rasterSize[1] - y) / rasterSize[1] * canvasSize[1];

		if (select == selection::quadrilateral)
		{
			if (numberOfQuadInputs == 1)
			{
				quad.quadEnd[0] = mousePos[0];
				quad.quadEnd[1] = mousePos[1];

				quad.color[0] = color[0];
				quad.color[1] = color[1];
				quad.color[2] = color[2];
				quadList.push_back(quad);

				numberOfQuadInputs = 0;
				glutPostRedisplay();
				return;
			}
			else if (numberOfQuadInputs == 0)
			{
				quad.quadOrigin[0] = mousePos[0];
				quad.quadOrigin[1] = mousePos[1];
			}

			numberOfQuadInputs++;
		}
		else if (select == selection::line)
		{
			point.xCoord = mousePos[0];
			point.yCoord = mousePos[1];
			line.linePoints.push_back(point);
			
		}
		else if (select == selection::triangle)
		{
			if (numberOfVertices >= 3)
			{
				numberOfVertices = 0;
			}

			triangle.vertexArray[numberOfVertices * 2 + 0] = mousePos[0];
			triangle.vertexArray[numberOfVertices * 2 + 1] = mousePos[1];
			numberOfVertices++;

			if (numberOfVertices == 3)
			{
				triangle.color[0] = color[0];
				triangle.color[1] = color[1];
				triangle.color[2] = color[2];
				triangleList.push_back(triangle);
			}
		}
		else if (select == selection::polygon)
		{
			point.xCoord = mousePos[0];
			point.yCoord = mousePos[1];
			polygon.polygonVertices.push_back(point);
		
		}
		else if (select == selection::point)
		{
			point.xCoord = mousePos[0];
			point.yCoord = mousePos[1];
			point.color[0] = color[0];
			point.color[1] = color[1];
			point.color[2] = color[2];
			point.size = pointSize;
			points.push_back(point);
			
		}

		glutPostRedisplay();
	}
}

void motion(int x, int y)
{
	// mouse events are handled by OS, eventually. When using mouse in the raster window, it assumes top-left is the origin.
	// Note: the raster window created by GLUT assumes bottom-left is the origin.
	mousePos[0] = (float)x / rasterSize[0] * canvasSize[0];
	mousePos[1] = (float)(rasterSize[1] - y) / rasterSize[1] * canvasSize[1];

	quad.quadEnd[0] = mousePos[0];
	quad.quadEnd[1] = mousePos[1];

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'f':

		finishDrawingCurrentPolygon();
		finishDrawingCurrentLine();
		glutPostRedisplay();
		break;

	case 27:
		exit(0);
		break;

	default:
		break;
	}

}

void menu(int value)
{
	switch (value)
	{
	case 0:
		quadList.clear();
		line.linePoints.clear();
		lineList.clear();
		points.clear();
		triangleList.clear();
		polygon.polygonVertices.clear();
		polygonList.clear();
		glutPostRedisplay();
		break;

	case 1:
		select = selection::point;
		break;

	case 2:
		select = selection::line;
		resetTriangleInProgress();
		resetQuadInProgress();
		finishDrawingCurrentPolygon();
		break;

	case 3:
		select = selection::triangle;
		resetQuadInProgress();
		finishDrawingCurrentLine();
		finishDrawingCurrentPolygon();
		break;

	case 4:
		select = selection::quadrilateral;
		finishDrawingCurrentPolygon();
		resetTriangleInProgress();
		finishDrawingCurrentLine();
		break;

	case 5:
		select = selection::polygon;
		resetTriangleInProgress();
		resetQuadInProgress();
		finishDrawingCurrentLine();
		break;

	case 6:
		color[0] = 1.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		glutPostRedisplay();
		break;

	case 7:
		color[0] = 0.0f;
		color[1] = 1.0f;
		color[2] = 0.0f;
		glutPostRedisplay();
		break;

	case 8:
		color[0] = 0.0f;
		color[1] = 0.0f;
		color[2] = 1.0f;
		glutPostRedisplay();
		break;

	case 9:
		lineWidth = 1.0f;
		glutPostRedisplay();
		break;

	case 10:
		lineWidth = 5.0f;
		glutPostRedisplay();
		break;

	case 11:
		lineWidth = 10.0f;
		glutPostRedisplay();
		break;

	case 12:
		pointSize = 5.0f;
		glutPostRedisplay();
		break;

	case 13:
		pointSize = 10.0f;
		glutPostRedisplay();
		break;

	case 14:
		pointSize = 15.0f;
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

void createMenu()
{
	int objectMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Point", 1);
	glutAddMenuEntry("Line", 2);
	glutAddMenuEntry("Triangle", 3);
	glutAddMenuEntry("Quad", 4);
	glutAddMenuEntry("Polygon", 5);

	int colorMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Red", 6);
	glutAddMenuEntry("Green", 7);
	glutAddMenuEntry("blue", 8);

	int lineWidthMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Small", 9);
	glutAddMenuEntry("Medium", 10);
	glutAddMenuEntry("Thick", 11);

	int pointSizeMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Small", 12);
	glutAddMenuEntry("Medium", 13);
	glutAddMenuEntry("Large", 14);

	glutCreateMenu(menu);
	glutAddMenuEntry("Clear", 0);
	glutAddSubMenu("Objects", objectMenu);
	glutAddSubMenu("Color", colorMenu);
	glutAddSubMenu("LineWidth", lineWidthMenu);
	glutAddSubMenu("PointSize", pointSizeMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char* argv[])
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(rasterSize[0], rasterSize[1]);
	glutCreateWindow("OpenGL Assignment 1");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(motion);
	createMenu();
	glutMainLoop();
	return 0;
}