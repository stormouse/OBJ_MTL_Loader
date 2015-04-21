
#include "OBJModel.h"
#include <GL/freeglut.h>
#define GLOBAL_LIGHT 0


GLint screenHeight = 512;
GLint screenWidth = 512;
bool	mouse_isDown = false;
float	x_angle = 0;
float	y_angle = 0;
int		mx, my;

GLuint boxTexture = -1;
OBJModel *box, *woodbox;

void TurnOn(int i)
{
	if(i==GLOBAL_LIGHT)
		glEnable(GL_LIGHT0);
}
void TurnOff(int i)
{
	if (i == GLOBAL_LIGHT)
		glDisable(GL_LIGHT0);
}
void DrawBasicCube(void)
{
	glColor3d(1.0, 1.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, boxTexture);
	//top
	glBegin(GL_POLYGON);
	glTexCoord2d(0,0);
	glVertex3d(0.0, 1.0, 0.0);
	glTexCoord2d(0,1);
	glVertex3d(1.0, 1.0, 0.0);
	glTexCoord2d(1,1);
	glVertex3d(1.0, 1.0, 1.0);
	glTexCoord2d(1,0);
	glVertex3d(0.0, 1.0, 1.0);
	glEnd();

	//bottom
	glBegin(GL_POLYGON);
	glTexCoord2d(0,0);
	glVertex3d(0.0, 0.0, 0.0);
	glTexCoord2d(0,1);
	glVertex3d(0.0, 0.0, 1.0);
	glTexCoord2d(1,1);
	glVertex3d(1.0, 0.0, 1.0);
	glTexCoord2d(1,0);
	glVertex3d(1.0, 0.0, 0.0);
	glEnd();

	//left
	glBegin(GL_POLYGON);
	glTexCoord2d(0,0);
	glVertex3d(0.0, 0.0, 0.0);
	glTexCoord2d(0,1);
	glVertex3d(0.0, 1.0, 0.0);
	glTexCoord2d(1,1);
	glVertex3d(0.0, 1.0, 1.0);
	glTexCoord2d(1,0);
	glVertex3d(0.0, 0.0, 1.0);
	glEnd();

	//right
	glBegin(GL_POLYGON);
	glTexCoord2d(0,0);
	glVertex3d(1.0, 0.0, 1.0);
	glTexCoord2d(0,1);
	glVertex3d(1.0, 1.0, 1.0);
	glTexCoord2d(1,1);
	glVertex3d(1.0, 1.0, 0.0);
	glTexCoord2d(1,0);
	glVertex3d(1.0, 0.0, 0.0);
	glEnd();

	//front
	glBegin(GL_POLYGON);
	glTexCoord2d(0,0);
	glVertex3d(0.0, 0.0, 0.0);
	glTexCoord2d(0,1);
	glVertex3d(1.0, 0.0, 0.0);
	glTexCoord2d(1,1);
	glVertex3d(1.0, 1.0, 0.0);
	glTexCoord2d(1,0);
	glVertex3d(0.0, 1.0, 0.0);
	glEnd();

	//back
	glBegin(GL_POLYGON);
	glTexCoord2d(0,0);
	glVertex3d(0.0, 1.0, 1.0);
	glTexCoord2d(0,1);
	glVertex3d(1.0, 1.0, 1.0);
	glTexCoord2d(1,1);
	glVertex3d(1.0, 0.0, 1.0);
	glTexCoord2d(1,0);
	glVertex3d(0.0, 0.0, 1.0);
	glEnd();

}
void GlobalLight(bool debug = false)
{
	GLfloat lightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0);

	if (debug)
	{
		glColor3d(1.0, 1.0, 0.2);
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glutSolidSphere(0.05, 32, 32);
		glEnable(GL_LIGHTING);
		glPopAttrib();
	}
}

void ImportTextures()
{
	box = new OBJModel("box_tri.obj");
	woodbox = new OBJModel("perfect.obj");
}

void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

	TurnOn(GLOBAL_LIGHT);
	ImportTextures();

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glViewport(0, 0, screenWidth, screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1, 6.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0);

	glEnable(GL_SMOOTH);
	//Global Transition//
	glPushMatrix();
	{
		glTranslated(0.0, 0.0, -3.0);
		glRotated(x_angle, 1.0, 0.0, 0.0);
		glRotated(y_angle, 0.0, 1.0, 0.0);

		//Global Light Transition//
		glPushMatrix();
		{
			glTranslated(0.0, 1, 0.0);
			GlobalLight(true);
		}
		glPopMatrix();

		//Cube Transition//
		glPushMatrix();
		{	
			glTranslated(-0.4, 0, 0);
			glScalef(0.25, 0.25, 0.25);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			box->Draw(0);
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();
		//Cube Over//

		//WoodBox Transition//
		glPushMatrix();
		{
			glTranslated(0.4, 0, 0);
			glScalef(0.4, 0.4, 0.4);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			woodbox->Draw(0);
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();
		//WoodBox Over//
	}glPopMatrix();
	//Global Over//

	glFlush();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	float aspect = (float)w / (float)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, aspect, 0.1, 6.0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void myMouseDown(int button, int state, int x, int y)
{
	if (GLUT_DOWN == state)
	{
		if (GLUT_LEFT_BUTTON == button)
		{
			mx = x; my = y;
			mouse_isDown = true;
		}
	}
	else mouse_isDown = false;
}

void myMouseMove(int x, int y)
{
	int dx, dy;
	dx = x - mx;
	dy = y - my;
	mx = x;
	my = y;
	if (mouse_isDown)
	{
		y_angle += dx*0.2f;
		x_angle += dy*0.2f;
	}
	display();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Texturing");

	//register callback functions
	glutDisplayFunc(display);
	glutMouseFunc(myMouseDown);
	glutMotionFunc(myMouseMove);

	//initialize and main loop
	init();
	glutMainLoop();

	return 0;
}