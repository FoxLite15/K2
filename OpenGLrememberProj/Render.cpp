#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

class CustomCamera : public Camera
{
public:
	double camDist;
	double fi1, fi2;

	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}
	void SetUpCamera()
	{
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;

class CustomLight : public Light
{
public:
	CustomLight()
	{
		pos = Vector3(1, 1, 3);
	}

	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}

	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}

GLuint texId;

void initRender(OpenGL* ogl)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	RGBTRIPLE* texarray;

	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);
	free(texCharArray);
	free(texarray);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	glEnable(GL_NORMALIZE);
	glEnable(GL_LINE_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void drawDisk(double x, double y, double z, double radius, int numSegments, float red, float green, float blue, bool flag) {
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(red, green, blue);
	glVertex3d(x, y, z);
	for (int i = 0; i <= numSegments; i++) {
		double angle = i / (double)numSegments * M_PI + M_PI / -1.3868;
		double dx = radius * cos(angle);
		double dy = radius * sin(angle);
		glVertex3d(x + dx, y + dy, z);
	}
	glEnd();
	if (flag == TRUE) {
		glBegin(GL_QUAD_STRIP);
		glColor3f(0, 0, 1);
		for (int i = 0; i <= numSegments; i++) {
			double angle = i / (double)numSegments * M_PI + M_PI / -1.3868;
			double dx = radius * cos(angle);
			double dy = radius * sin(angle);
			glVertex3d(x + dx, y + dy, z);
			glVertex3d(x + dx, y + dy, z - 2);
		}
		glEnd();
	}
}

void Render(OpenGL* ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	glShadeModel(GL_SMOOTH);
	drawDisk(21.5, 3, 0, 3.9, 30, 1, 0, 0, 0);
	drawDisk(21.5, 3, 2, 3.9, 30, 0, 1, 0, 1);

	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3d(6, 6, 0);
	glTexCoord2f(1, 0);
	glVertex3d(10, 10, 0);
	glTexCoord2f(1, 1);
	glVertex3d(15, 8, 0);
	glTexCoord2f(0, 0.5);
	glVertex3d(15, 4, 0);
	glTexCoord2f(0, 0.5);
	glVertex3d(9, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3d(6, 6, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2f(1, 1);
	glVertex3d(15, 8, 0);
	glTexCoord2f(0.5, 1);
	glVertex3d(19, 14, 0);
	glTexCoord2f(0.5, 0.1);
	glVertex3d(24, 6, 0);
	glTexCoord2f(0.0, 0.1);
	glVertex3d(19, 0, 0);
	glTexCoord2f(0.0, 0.5);
	glVertex3d(15, 4, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3d(6, 6, 2);
	glTexCoord2f(1, 0);
	glVertex3d(10, 10, 2);
	glTexCoord2f(1, 1);
	glVertex3d(15, 8, 2);
	glTexCoord2f(0, 0);
	glVertex3d(15, 4, 2);
	glTexCoord2f(0, 0);
	glVertex3d(9, 1, 2);
	glTexCoord2f(0, 0);
	glVertex3d(6, 6, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2f(1, 1);
	glVertex3d(15, 8, 2);
	glTexCoord2f(0.5, 1);
	glVertex3d(19, 14, 2);
	glTexCoord2f(0, 0.5);
	glVertex3d(24, 6, 2);
	glTexCoord2f(0, 0.5);
	glVertex3d(19, 0, 2);
	glTexCoord2f(0, 0);
	glVertex3d(15, 4, 2);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0, 1);
	glTexCoord2f(0, 0); glVertex3d(6, 6, 0);
	glTexCoord2f(0, 1); glVertex3d(10, 10, 0);
	glTexCoord2f(1, 1); glVertex3d(10, 10, 2);
	glTexCoord2f(1, 0); glVertex3d(6, 6, 2);

	glTexCoord2f(0, 0); glVertex3d(10, 10, 0);
	glTexCoord2f(0, 1); glVertex3d(15, 8, 0);
	glTexCoord2f(1, 1); glVertex3d(15, 8, 2);
	glTexCoord2f(1, 0); glVertex3d(10, 10, 2);

	glTexCoord2f(0, 0); glVertex3d(15, 8, 0);
	glTexCoord2f(0, 1); glVertex3d(19, 14, 0);
	glTexCoord2f(1, 1); glVertex3d(19, 14, 2);
	glTexCoord2f(1, 0); glVertex3d(15, 8, 2);

	glTexCoord2f(0, 0); glVertex3d(19, 14, 0);
	glTexCoord2f(0, 1); glVertex3d(24, 6, 0);
	glTexCoord2f(1, 1); glVertex3d(24, 6, 2);
	glTexCoord2f(1, 0); glVertex3d(19, 14, 2);

	glTexCoord2f(0, 0); glVertex3d(19, 0, 0);
	glTexCoord2f(0, 1); glVertex3d(15, 4, 0);
	glTexCoord2f(1, 1); glVertex3d(15, 4, 2);
	glTexCoord2f(1, 0); glVertex3d(19, 0, 2);

	glTexCoord2f(0, 0); glVertex3d(15, 4, 0);
	glTexCoord2f(0, 1); glVertex3d(9, 1, 0);
	glTexCoord2f(1, 1); glVertex3d(9, 1, 2);
	glTexCoord2f(1, 0); glVertex3d(15, 4, 2);

	glTexCoord2f(0, 0); glVertex3d(9, 1, 0);
	glTexCoord2f(0, 1); glVertex3d(6, 6, 0);
	glTexCoord2f(1, 1); glVertex3d(6, 6, 2);
	glTexCoord2f(1, 0); glVertex3d(9, 1, 2);


	/*glMatrixMode(GL_PROJECTION); 
	glPushMatrix();			    
	glLoadIdentity();
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();	
	glLoadIdentity();		  
	glDisable(GL_LIGHTING);*/

	//"T - вкл/выкл текстур"
	//"L - вкл/выкл освещение"
	//"F - Свет из камеры"
	//"G - двигать свет по горизонтали"
	//G+ЛКМ двигать свет по вертекали"


}
