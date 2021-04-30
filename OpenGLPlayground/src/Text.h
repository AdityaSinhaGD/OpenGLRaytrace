#ifndef _TEXT_H_
#define _TEXT_H_

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

struct Text
{
private:
	void* font;
	float color[3];

public:
	Text();
	void setFont(void* p_font);
	void setColor(float r, float g, float b);
	void draw(	const float x, const float y, 
				char* textContent, 
				const int winW, const int winH, 
				const int characterSpace = 3);
};

inline Text::Text() 
{
	font = GLUT_BITMAP_8_BY_13; //GLUT_BITMAP_9_BY_15;
	color[0] = color[1] = color[2] = 0.0f;
}

inline void Text::setFont(void* f)
{
	font =f;
}

inline void Text::setColor(float r, float g, float b)
{
	color[0]=r; 
	color[1]=g; 
	color[2]=b;
}

inline void Text :: draw(const float x, const float y, 
						 char* textContent, 
						 const int winW, const int winH, 
						 const int characterSpace)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0, winW, 0, winH);
	glScalef(1.0f, -1.0f, 1.0f);				// y =0 is at bottom, +Y is point to down
	glTranslatef(0, (float)-winH, 0); 			// move the origin from the bottom left corner, to the upper left corner
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(color[0], color[1], color[2]);

	// rendering the string
	char* c = NULL;
	float x1 = x;
	for( c = textContent; *c != '\0'; ++c) {
		glRasterPos2f(x1,y);
    	glutBitmapCharacter(font, *c);
		x1 += (float)(glutBitmapWidth(font,*c) + characterSpace);
	}

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}
#endif