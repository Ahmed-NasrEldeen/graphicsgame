#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <stdlib.h>
#include <glut.h>
#include <Windows.h>
#include <string.>
#include <iostream>
#include <cstdlib>



int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1000;

GLdouble x = 0;
GLdouble y = 1;
GLdouble z = 20;
GLdouble speed = 0.005;

bool turnRight = 0;
bool turnLeft = 0;
bool xPos = 0;
bool xNeg = 0;
bool zPos = 0;
bool zNeg = 1;
bool flag = true;
bool started;

bool eaten;
int applex = rand() % 36 - 18;
int applez = rand() % 36 - 18;

int score = 0;
GLdouble distance = 20;
class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(0, 30, -10);
Vector At(0, 0, -30);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;

// Textures
GLTexture tex_ground;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void drawWalls()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing
	//glBindTexture(GL_TEXTURE_2D, wall_texture.texture[0]);	// Bind the ground texture

	glPushMatrix();

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(40, 0);
	glVertex3f(-20, 2, -20);
	glTexCoord2f(40, 2);
	glVertex3f(-20, 2, 20);
	glTexCoord2f(0, 40);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.


	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) +-> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(40, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(40, 2);
	glVertex3f(20, 2, -20);
	glTexCoord2f(0, 40);
	glVertex3f(-20, 2, -20);
	glEnd();
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);


	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(20, 0, -20);
	glTexCoord2f(40, 0);
	glVertex3f(20, 2, -20);
	glTexCoord2f(40, 2);
	glVertex3f(20, 2, 20);
	glTexCoord2f(0, 0.1);
	glVertex3f(20, 0, 20);
	glEnd();
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(20, 0, 20);
	glTexCoord2f(40, 0);
	glVertex3f(20, 2, 20);
	glTexCoord2f(40, 2);
	glVertex3f(-20, 2, 20);
	glTexCoord2f(0, 2);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);



	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void hud(float x, float y, float z, char* string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos3f(x, y, z);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}
void randomizedApple() {

	if (eaten) {
		applex = rand() % 36 - 18;
		applez = rand() % 36 - 18;
		eaten = 0;
	}
	glPushMatrix();
	glTranslatef(applex, 0, applez);
	glScalef(0.005, 0.005, 0.005);
	model_tree.Draw();
	glPopMatrix();

}
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//***********//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//***********//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	//***********//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//***********//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderGround2()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -25);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -25);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, -45);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, -45);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void drawobstacle() {
	glPushMatrix();
	glTranslatef(16.0, 1, 0.0);
	glScalef(8, 2, 1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 1, -16.0);
	glScalef(1, 2, 8);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-16.0, 1, 0.0);
	glScalef(8, 2, 1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 1, 16.0);
	glScalef(1, 2, 8);
	glutSolidCube(1.0);
	glPopMatrix();

}



////////////////////////////////////////////////////////////////////////////
void drawobstacle2() {
	glPushMatrix();
	glTranslatef(16.0, 1, 0.0);
	glScalef(8, 2, 1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 1, -16.0);
	glScalef(1, 2, 8);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-16.0, 1, 0.0);
	glScalef(8, 2, 1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 1, 16.0);
	glScalef(1, 2, 8);
	glutSolidCube(1.0);
	glPopMatrix();

}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);


	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Display score and info
	hud(15, 10, 10, "Carol is cute");
	// Draw Wall
	drawWalls();

	// Draw Ground
	RenderGround();
	RenderGround2();

	//Draw Apples
	randomizedApple();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	//DrawObstacle
	drawobstacle();

	// Draw sample character(Box)
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(x, 1, z);
	glutSolidCube(0.2);
	glPopMatrix();

	//sky box
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();

	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

// Animation Function
void Anim()
{
	if (x >= 20 || x <= -20 || z >= 20 || z <= -20)
	{
		score--;
		x = 0;
		y = 1;
		z = 20;
		xPos = 0;
		xNeg = 0;
		zPos = 0;
		zNeg = 1;
	}
	if (z >= applez - 0.5 && z <= applez + 0.5 && x >= applex - 0.5 && x <= applex + 0.5)
	{
		score++;
		eaten = 1;
	}
	// movement control of character
	if (turnRight)
	{

		/*glPushMatrix();
		glRotated(90, 0, 1, 0);
		At;
		glPopMatrix();*/

		if (xPos)
		{
			xPos = 0;
			zPos = 1;

		}
		else if (zPos)
		{
			zPos = 0;
			xNeg = 1;

		}
		else if (xNeg)
		{
			xNeg = 0;
			zNeg = 1;

		}
		else if (zNeg)
		{
			zNeg = 0;
			xPos = 1;
		}


		turnRight = 0;
	}
	if (turnLeft)
	{
		if (xPos)
		{
			xPos = 0;
			zNeg = 1;

		}
		else if (zPos)
		{
			zPos = 0;
			xPos = 1;
		}
		else if (xNeg)
		{
			xNeg = 0;
			zPos = 1;
		}
		else if (zNeg)
		{
			zNeg = 0;
			xNeg = 1;

		}
		turnLeft = 0;
	}
	if (xPos)
	{
		if (x <= 20) {
			x += speed;
		}
		/*Eye.x = x;
		At.z = Eye.z;
		At.x = Eye.x + distance;*/
	}
	else if (zPos)
	{
		if (z <= 20) {
			z += speed;
		}
		/*Eye.z = z;
		At.x = Eye.x;
		At.z = Eye.z + distance;*/
	}
	else if (xNeg)
	{
		if (x >= -20) {
			x -= speed;
		}
		/*Eye.x = x;
		At.z = Eye.z;
		At.x = Eye.x - distance;*/
	}
	else if (zNeg)
	{
		if (z >= -20) {
			z -= speed;
		}
		//Eye.z = z;
		////Eye.y = y + 0.5;
		//At.x = Eye.x;
		//At.z = Eye.z - distance;
	}
	glutPostRedisplay();
}

void specialKey(int k, int x, int y)
{
	if (k == GLUT_KEY_LEFT)
	{
		turnLeft = 1;
	}

	if (k == GLUT_KEY_RIGHT)
	{
		turnRight = 1;
	}
	if (k == GLUT_KEY_DOWN)
	{

	}
	if (k == GLUT_KEY_UP)
	{

	}
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	//glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glutIdleFunc(Anim);
	glutSpecialFunc(specialKey);
	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}