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

enum struct selection { none, point, line, triangle, quadrilateral, polygon}; 
selection select = selection::none;

struct Vector2
{
	float xCoord;
	float yCoord;
	Vector2(float xCoord, float yCoord)
	{
		this->xCoord = xCoord;
		this->yCoord = yCoord;
	}
};

std::vector<Vector2> points;

class Line
{
public:
	std::vector<Vector2> linePoints;
};
Line line;
//std::vector<Line> lineList;


#pragma region Quads
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
#pragma endregion

class Poly
{
public:
	std::vector<Vector2> polygonVertices;
};

Poly polygon;

class Triangle
{
public:
	float vertexArray[6];
	float color[3];
};

Triangle triangle;
int numberOfVertices;
std::vector<Triangle> triangleList;

void CreatePoint(Vector2 point)
{
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glVertex2f(point.xCoord, point.yCoord);
	glEnd();
}

void CreateLine(Line line)
{
	glBegin(GL_LINE_STRIP);
	for (Vector2& linePoints : line.linePoints)
	{
		glVertex2f(linePoints.xCoord, linePoints.yCoord);
	}
	if (select == selection::line)
	{
		glVertex2f(mousePos[0], mousePos[1]);
	}
	//glVertex2fv(mousePos);
	glEnd();
}

void CreateTriangle(Triangle triangle)
{
	glColor3f(triangle.color[0], triangle.color[1], triangle.color[2]);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++)
	{
		glVertex2fv(triangle.vertexArray + i * 2);
	}
	glEnd();
}

void CreateQuad(Quad quad)
{
	glColor3f(quad.color[0], quad.color[1], quad.color[2]);
	glBegin(GL_QUADS);
	glVertex2f(quad.quadOrigin[0], quad.quadOrigin[1]);
	glVertex2f(quad.quadEnd[0], quad.quadOrigin[1]);
	glVertex2f(quad.quadEnd[0], quad.quadEnd[1]);
	glVertex2f(quad.quadOrigin[0], quad.quadEnd[1]);
	glEnd();
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

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3fv(color);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (numberOfQuadInputs == 1)
	{
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

	for (Quad& quad : quadList)
	{
		CreateQuad(quad);
	}

	if (line.linePoints.size() >= 1)
	{
		CreateLine(line);	
	}

	for (Vector2& point : points)
	{
		CreatePoint(point);
	}

	if (numberOfVertices > 0 && numberOfVertices < 3)
	{
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

	if (triangleList.size() >= 1)
	{
		for (Triangle& triangle : triangleList)
		{
			CreateTriangle(triangle);
		}
	}

	if (polygon.polygonVertices.size() < 2)
	{
		glBegin(GL_LINE_STRIP);
		for (Vector2& point : polygon.polygonVertices)
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
		glBegin(GL_POLYGON);
		for (Vector2& point : polygon.polygonVertices)
		{
			glVertex2f(point.xCoord, point.yCoord);
		}
		if (select == selection::polygon)
		{
			glVertex2fv(mousePos);
		}
		glEnd();
	}

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
			line.linePoints.push_back(Vector2(mousePos[0], mousePos[1]));
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
			polygon.polygonVertices.push_back(Vector2(mousePos[0], mousePos[1]));
		}
		else if (select == selection::point)
		{
			points.push_back(Vector2(mousePos[0], mousePos[1]));
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
	case 27:
		exit(0);
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
		points.clear();
		triangleList.clear();
		polygon.polygonVertices.clear();
		glutPostRedisplay();
		break;

	case 1:
		select = selection::point;
		break;

	case 2:
		select = selection::line;
		break;

	case 3:
		select = selection::triangle;
		break;

	case 4:
		select = selection::quadrilateral;
		break;

	case 5:
		select = selection::polygon;
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

	glutCreateMenu(menu);
	glutAddMenuEntry("Clear", 0);
	glutAddSubMenu("Objects", objectMenu);
	glutAddSubMenu("Color", colorMenu);
	glutAddSubMenu("LineWidth", lineWidthMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char* argv[])
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(rasterSize[0], rasterSize[1]);
	glutCreateWindow("Mouse Event - draw a quad");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(motion);
	createMenu();
	glutMainLoop();
	return 0;
}