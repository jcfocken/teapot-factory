#include <iostream>
#include <glut.h>
#include <math.h>
#include <stdio.h>
using namespace std;

//--Globals ---------------------------------------------------------------

# define MAX_PARTICLES 1000

const float PI = 3.141593f;

float blueRotation = 0;
float greenRotation = 10;
float redRotation = 8;
float rPos[6];
int cameraMode = 0;


//--Holds position and direction of camera
typedef struct
{
	float PosX;
	float PosY;
	float PosZ;	
	float RotX;
	float RotY;
	float viewDirX;
	float viewDirY;
	float viewDirZ;
	float cameraUpX;
	float cameraUpY;
	float cameraUpZ;	
}CameraPos;
CameraPos camera1;
CameraPos camera2;

//--Holds position, direction and color of a particle
typedef struct
{
	float posX;
	float posY;
	float posZ;
	float veloX;
	float veloY;
	float veloZ;
	float colorRed;
	float colorGreen;
	float colorBlue;
	int life;	
} Particle;
int numParticles = 0;
Particle particles[MAX_PARTICLES];

//--Colors----------------------------------------------------------------
static GLfloat no_emission[] = {0.0, 0.0, 0.0, 0.0};
static GLfloat bulb_emission[] = {1, 1, 1, 0.0};
static GLfloat grey[4] = {0.2, 0.2, 0.2, 1.};
static GLfloat white[4] = {1.0, 1.0, 1.0, 1.};
static GLfloat red[4] = {1.0, 0.0, 0.0, 1.};


//--This function returns the inner radius of a gear (R) given the number of 
//--teeth.  The outer radius is always R+2.
float getRadius(int numTeeth)
{
    return 1.4*numTeeth/PI;
}

//--Converts degrees to radians
float DegreesToRadians(float degrees)
{
return degrees * PI / 180;
};

//--Updates the rPos array with the rocket's position and direction
void updateRocketPos()
{
	rPos[0] = 20*(cos(DegreesToRadians(blueRotation)));
	rPos[1] = 30 + 20*(sin(DegreesToRadians(blueRotation)));
	rPos[2] = 10;
	
	rPos[3] = sin(DegreesToRadians(blueRotation));
	rPos[4] = -cos(DegreesToRadians(blueRotation));
	rPos[5] = 0;
}	

//--Add a new particle, with random velocity and colour,
//--at the rocket's position to the particle array at index
void createParticle(int index)
{
	particles[index].posX = rPos[0];
	particles[index].posY = rPos[1];
	particles[index].posZ = rPos[2];
	
	particles[index].veloX = (((float(rand() % 100))/100)-0.5)+rPos[3];
	particles[index].veloY = (((float(rand() % 100))/100))*rPos[4];
	particles[index].veloZ = ((float(rand() % 100))/100)-0.5;
	
	particles[index].colorRed = (float(rand() % 100))/100;
	particles[index].colorGreen = (float(rand() % 100))/100;
	particles[index].colorBlue = (float(rand() % 100))/100;
	
	particles[index].life =50;
	numParticles++;
}

//--Moves particles according to their velocity, accelerates them downwards
//--and decreases their time to live
void updateParticles()
{
	int i;
	for(i = 0; i <= numParticles; i++){
		particles[i].posX += particles[i].veloX;
		particles[i].posY += particles[i].veloY;
		particles[i].posZ += particles[i].veloZ;
		
		//bounce particles of the ground
		if (particles[i].posY <= 0) {
			particles[i].veloY *= -0.6;
		}
		
		//this simulates gravity
		particles[i].veloY -= .02;
		
		particles[i].life -= 1	;

		//old particles die and create new ones
		if (particles[i].life == 0){
			createParticle(i);
			numParticles--;
		}		
	}
}

void drawParticles()
{
	int i;	
	GLfloat particle_emission[4];
	for(i = 0; i <= numParticles; i++){		
		glPushMatrix();
			glColor3f(particles[i].colorRed, particles[i].colorGreen, particles[i].colorBlue);
			particle_emission[0] = particles[i].colorRed; 
			particle_emission[1] = particles[i].colorGreen;
			particle_emission[2] = particles[i].colorBlue;
			particle_emission[3] = 0.0;
			glTranslatef (particles[i].posX, particles[i].posY, particles[i].posZ);			
			glBegin(GL_LINES);
				glMaterialfv(GL_FRONT, GL_EMISSION, particle_emission);
				glVertex3f(0, 0, 0);
				glVertex3f(0, .2, 0);	
				glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
			glEnd();
		glPopMatrix();
	}
}

//--Draws a disk with a hole
void drawDisk()
{	
	GLUquadric *q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluDisk(q, 5, 10, 100, 1);
}

//--Draws a gear with a specific number of teeth
void drawGear(int numTeeth, float thickness)
{ 
    float radius = getRadius(numTeeth);
    float angInc1 = 0.5/radius;
    float angInc2 = 0.8/radius;

    float angle = 0.0;

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.,0.,-1.);
    glVertex3f(0.0, 0.0, 0.0);
    for(int i = 0; i < numTeeth; i++)
    {
        glVertex3f(radius*cos(angle), radius*sin(angle), 0.0);
        angle += angInc1;
        glVertex3f((radius+2.0)*cos(angle), (radius+2.0)*sin(angle), 0.0);
        angle += angInc2;
        glVertex3f((radius+2.0)*cos(angle), (radius+2.0)*sin(angle), 0.0);
        angle += angInc1;
        glVertex3f(radius*cos(angle), radius*sin(angle), 0.0);
        angle += 2.0*angInc1;
    }
    glVertex3f(radius, 0.0, 0.0);
    glEnd();
    
    angle = 0.0;
    
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.,0.,1.);
    glVertex3f(0.0, 0.0, thickness);
    for(int i = 0; i < numTeeth; i++)
    {
        glVertex3f(radius*cos(angle), radius*sin(angle), thickness);
        angle += angInc1;
        glVertex3f((radius+2.0)*cos(angle), (radius+2.0)*sin(angle), thickness);
        angle += angInc2;
        glVertex3f((radius+2.0)*cos(angle), (radius+2.0)*sin(angle), thickness);
        angle += angInc1;
        glVertex3f(radius*cos(angle), radius*sin(angle), thickness);
        angle += 2.0*angInc1;
    }
    glVertex3f(radius, 0.0, thickness);
    glEnd();
    
    angle = 0.0;
    

	//this is the strip connecting the two 2D gears
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i < numTeeth; i++)
    {	
		glNormal3f(cos(angle),sin(angle),0.);
        glVertex3f(radius*cos(angle), radius*sin(angle), 0.0);
        glVertex3f(radius*cos(angle), radius*sin(angle), thickness);
        angle += angInc1;
        glNormal3f(cos(angle),sin(angle),0.);
        glVertex3f((radius+2.0)*cos(angle), (radius+2.0)*sin(angle), 0.0);
        glVertex3f((radius+2.0)*cos(angle), (radius+2.0)*sin(angle), thickness);
        angle += angInc2;
        glNormal3f(cos(angle),sin(angle),0.);
        glVertex3f((radius+2.0)*cos(angle), (radius+2.0)*sin(angle), 0.0);
        glVertex3f((radius+2.0)*cos(angle), (radius+2.0)*sin(angle), thickness);
        angle += angInc1;
        glNormal3f(cos(angle),sin(angle),0.);
        glVertex3f(radius*cos(angle), radius*sin(angle), 0.0);
        glVertex3f(radius*cos(angle), radius*sin(angle), thickness);
        angle += 2.0*angInc1;
    }
    glVertex3f(radius*cos(angle), radius*sin(angle), 0.0);
    glVertex3f(radius*cos(angle), radius*sin(angle), thickness);
    glEnd();
}

//--Draws one of the rooms walls
void drawWall()
{
	glNormal3f(0., 0., 1.);
	glBegin(GL_QUADS);		
		glVertex3f(-100, 0, 0);
		glVertex3f(100, 0, 0);
		glVertex3f(100, 50, 0);
		glVertex3f(-100, 50, 0);
	glEnd();
	
}
	
//--Draws the checkered floor plane
void drawFloor()
{
    glNormal3f(0., 1., 0.);

	for(int x = -100; x < 100; x += 10){
		for(int y = -100; y < 100; y += 10){			
			glBegin(GL_QUADS);
				if ((x/10+y/10)%2 == 0) glColor3f(1., 1., 1.);
				else glColor3f(0., 0., 0.);
				glVertex3f(x, 0, y);
				glVertex3f(x+10, 0, y);
				glVertex3f(x+10, 0, y+10);
				glVertex3f(x, 0, y+10);
			glEnd();
		}
	}
}

//--Draws the floor and the 4 walls bounding the room
void drawRoom()
{
	drawFloor();
	
	glColor3f(.8, .8, .8);
	
	glPushMatrix();
		glTranslatef(0, 0, -100);
		drawWall();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(0, 0, 100);
		glRotatef(180, 0, 1, 0);
		drawWall();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(100, 0, 0);
		glRotatef(-90, 0, 1, 0);
		drawWall();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-100, 0, 0);
		glRotatef(90, 0, 1, 0);
		drawWall();
	glPopMatrix();
}

//draws a rocket of specified length
void drawRocket(int length)
{

	GLUquadric* quadric;
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricOrientation( quadric,GLU_OUTSIDE);
	gluQuadricDrawStyle(quadric, GLU_FILL );
	
	glColor3f(1., 1., 1.);	

	gluCylinder(quadric, 1, 1, length, 20, 1);
	
	glPushMatrix();
		glColor3f(1., 0., 0.);	
		glTranslatef(0, 0, length);
		gluCylinder(quadric, 1, 0, 2, 20, 1);
	glPopMatrix();
		
}

//--Draws the arm holding the rocket
void drawRocketArm()
{
	int depth = 10;
	int radius = 20;
	int lengthRocket = 6;
	
	GLUquadric* quadric;
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricOrientation( quadric,GLU_OUTSIDE);
	gluQuadricDrawStyle(quadric, GLU_FILL );
	
	glColor3f(0.3, 1., 0.3);	
	gluCylinder(quadric, 1, 1, depth, 20, 1);
	
	glBegin (GL_QUADS);
		glNormal3f(0., 0., 1.);
		glVertex3f(-1, -1, 10);
		glVertex3f(-1, 1, 10);
		glVertex3f(radius, 1, 10);
		glVertex3f(radius, -1, 10);
	glEnd();
	
	glPushMatrix();
		glTranslatef(radius, -lengthRocket/2, depth);
		glRotatef(-90, 1, 0, 0);
		drawRocket(lengthRocket);
	glPopMatrix();

	
}

//--Updates the camera1 view direction-------------------------------------
void updateCamera1()
{
	//--converts the camera rotation into a direction
	camera1.viewDirX = sin(DegreesToRadians(camera1.RotX)) - 
		fabs(sin(DegreesToRadians(camera1.RotY))) * sin(DegreesToRadians(camera1.RotX));
	camera1.viewDirY = sin(DegreesToRadians(camera1.RotY));
	camera1.viewDirZ = cos(DegreesToRadians(camera1.RotX)) - 
		fabs(sin(DegreesToRadians(camera1.RotY))) * cos(DegreesToRadians(camera1.RotX));
	
	//sets the up direction to be along the x-axis and z-axis rather than
	//y-axis if the camera looks straight up or down
	if (camera1.viewDirY == 1) {
		camera1.cameraUpX = sin(DegreesToRadians(camera1.RotX));
	} else if (camera1.viewDirY == -1) {
		camera1.cameraUpX = -sin(DegreesToRadians(camera1.RotX));
	} else {
		camera1.cameraUpX = 0;
	}
	
	if (camera1.viewDirY == 1 || camera1.viewDirY == -1) {
		camera1.cameraUpY = 0;
	} else {
		camera1.cameraUpY = 1;
	}

	if (camera1.viewDirY == 1) {
		camera1.cameraUpZ = -cos(DegreesToRadians(camera1.RotX));
	} else if (camera1.viewDirY == -1) {
		camera1.cameraUpZ = cos(DegreesToRadians(camera1.RotX));
	} else {
		camera1.cameraUpZ = 0;
	}
}	
//--Updates the camera2 position and view direction
void updateCamera2()
{
	camera2.PosX = 50 + 7.5 * (sin(DegreesToRadians(blueRotation)));
	camera2.PosY = 25;
	camera2.PosZ = -30 + 7.5 * (cos(DegreesToRadians(blueRotation)));
	camera2.viewDirX = -cos(DegreesToRadians(2*blueRotation));
	camera2.viewDirY = 0;
	camera2.viewDirZ = -sin(DegreesToRadians(2*blueRotation));
	camera2.cameraUpX = 0;
	camera2.cameraUpY = 1;
	camera2.cameraUpZ = 0;	
}
//--Display:
//--This is the main display module containing function calls for generating
//--the scene.
void display(void) 
{ 
	CameraPos camera;
	//the position of the spotlight
	GLfloat spot_posX = 0.0;
	GLfloat spot_posY = 25.0;
	GLfloat spot_posZ = -30.0;
	
	GLfloat zero_position[] = { 0.0, 0.0, 0.0, 1.0 }; 
    GLfloat spot_direction[] = { 0.0, 0.0, -1.0 }; 
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (cameraMode == 0) {
		updateCamera1();
		camera = camera1;
	} else {
		updateCamera2();
		camera = camera2;
	}
		
	gluLookAt(camera.PosX, camera.PosY, camera.PosZ, 
		  camera.PosX - camera.viewDirX, camera.PosY + camera.viewDirY,
		  camera.PosZ + camera.viewDirZ,
		  camera.cameraUpX, camera.cameraUpY, camera.cameraUpZ);
	
	drawRoom();	
	drawParticles();
	
	
	//draw the bouncing ball
	glPushMatrix();
		glTranslatef(95 * (sin(DegreesToRadians(blueRotation))), 15 + 10 * (sin(DegreesToRadians(redRotation))), 50);	
		glMaterialfv(GL_FRONT, GL_EMISSION, red);
		glutSolidSphere(5,20, 20);	
		glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
		glLightfv(GL_LIGHT0,GL_POSITION, zero_position);
	glPopMatrix();
	
	//positions spotlight
	glPushMatrix();
		glTranslatef(spot_posX, spot_posY, spot_posZ);
		glRotatef(-90, 0, 1, 0);		 
		glLightfv(GL_LIGHT1, GL_POSITION, zero_position);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
		glutSolidCone(3, 5, 50, 1);
		glMaterialfv(GL_FRONT, GL_EMISSION, bulb_emission);
		glutSolidSphere(1, 20, 20);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
	glPopMatrix();	
	
	float shadowPosX = 99.9; // shadows start disappearing into wall if this is 100
	float shadowMat[16] = {shadowPosX, spot_posY, spot_posZ, 1,
						   0, shadowPosX - spot_posX, 0, 0,
						   0, 0, shadowPosX - spot_posX, 0,
						   -shadowPosX * spot_posX, -shadowPosX * spot_posY,
								-shadowPosX * spot_posZ, -spot_posX};	
	
	//Recreates the rotating table and balls and projects them onto the wall
	glDisable(GL_LIGHTING);
	glPushMatrix();
		glMultMatrixf(shadowMat);
		glColor3f(0.2, 0.2, 0.2);		
		glTranslatef(50, 20, -30);
		glRotatef(blueRotation, 0, 1, 0);
		glPushMatrix();
			glTranslatef(0, 6 + 4 * (sin(DegreesToRadians(redRotation))), -7.5);
			glutSolidSphere(2, 20, 20);			
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0, 1.2, 7.5);
			glRotatef(blueRotation, 0, 1, 0);
			glutSolidTeapot(1.5);
		glPopMatrix();
		glRotatef(90, 1, 0, 0); 
		drawDisk();
	glPopMatrix();
		
	//Displays rotating table and objects	
	glEnable(GL_LIGHTING);
	glPushMatrix();
		glTranslatef(50, 20, -30);
		glRotatef(blueRotation, 0, 1, 0);
		glPushMatrix();
			glColor3f(0.0, 1.0, 0.0); 
			glTranslatef(0, 6 + 4 * (sin(DegreesToRadians(redRotation))), -7.5);
			glutSolidSphere(2, 20, 20);			
		glPopMatrix();			
		glPushMatrix();
			glColor3f(0.0, 0.0, 1.0); 
			glTranslatef(0, 1.2, 7.5);
			glRotatef(blueRotation, 0, 1, 0);
			glutSolidTeapot(1.5);			
		glPopMatrix();
		glRotatef(90, 1, 0, 0);
		glColor3f(0.3, 0.8, 1.0); 
		drawDisk();
    glPopMatrix(); 
	
	glTranslatef(0, 30, 0);
		
	glPushMatrix();
	  glRotatef(blueRotation, 0, 0, 1.0); 
	  drawRocketArm();
	  glColor3f(0.0, 0.0, 1.0); 
	  drawGear(25, 1.0);         //A gear with 25 teeth
    glPopMatrix();

    glPushMatrix();
      glColor3f(0.0, 1.0, 0.0); 
      glTranslatef(getRadius(25) + getRadius(13) + 2.5, 0, 0);
      glRotatef(greenRotation, 0, 0, 1.0); 
      drawGear(13, 1.0);         //A gear with 13 teeth
    glPopMatrix();
  
    glPushMatrix();
      glColor3f(1.0, 0.0, 0.0); 
      glTranslatef(getRadius(25) + getRadius(13) + 2.5, 0, 0);
	  glTranslatef(0, getRadius(13) + getRadius(10) + 2.5, 0);
	  glRotatef(redRotation, 0, 0, 1.0); 
      drawGear(10, 1.0);         //A gear with 10 teeth
    glPopMatrix(); 
	
	glFlush(); 
} 

//--Timer call back
void gearTimer (int value)
{
	int i;
	float blueChange = 2;
	float greenChange = -blueChange * 25.0 / 13.0;
	float redChange = -greenChange * 13.0 / 10.0;
	
	updateRocketPos();
	
	//creates 5 new particles during each callback
	for(i=0;i<5;i++){		 
		if (numParticles < MAX_PARTICLES) createParticle(numParticles);
	}	
	updateParticles();
	
	blueRotation = blueRotation + blueChange;
	greenRotation = greenRotation + greenChange;
	redRotation = redRotation + redChange;

	glutPostRedisplay();
	value ++;
	glutTimerFunc(50, gearTimer, value);

}
	
//------------ Special key event callback ---------------------------------
//--Rotates the camera on input from the arrow keys
void special(int key, int x, int y)
{
    if(key == GLUT_KEY_LEFT) camera1.RotX -= 10;
    else if(key == GLUT_KEY_RIGHT) camera1.RotX += 10;
    else if(key == GLUT_KEY_UP) camera1.RotY += 10;
    else if(key == GLUT_KEY_DOWN) camera1.RotY -= 10;
    else if(key == GLUT_KEY_HOME) cameraMode += 1;
    
    if (camera1.RotX >= 360) camera1.RotX -= 360;
    if (camera1.RotX < 0) camera1.RotX += 360;
    if (camera1.RotY >= 90) camera1.RotY = 90;
    if (camera1.RotY < -90) camera1.RotY = -90;
    if (cameraMode > 1) cameraMode = 0;
    
    glutPostRedisplay();
} 

//-------------Keyboard event callback-------------------------------------
//--Moves the camera about on input from the wasd keys
void keyboard(unsigned char key, int x, int y)
{
	int speed = 5;
	switch(key)
	{
		case'w':
		case'W':
			camera1.PosX -= sin(DegreesToRadians(camera1.RotX)) * cos(DegreesToRadians(camera1.RotY)) * speed;
			camera1.PosY += sin(DegreesToRadians(camera1.RotY)) * speed;
			camera1.PosZ += cos(DegreesToRadians(camera1.RotX)) * cos(DegreesToRadians(camera1.RotY)) * speed;
			break;
		case's':
		case'S':
			camera1.PosX += sin(DegreesToRadians(camera1.RotX)) * cos(DegreesToRadians(camera1.RotY)) * speed;
			camera1.PosY -= sin(DegreesToRadians(camera1.RotY)) * speed;
			camera1.PosZ -= cos(DegreesToRadians(camera1.RotX)) * cos(DegreesToRadians(camera1.RotY)) * speed;
			break;
		case'a':
		case'A':
			camera1.PosX += cos(DegreesToRadians(camera1.RotX)) * speed;
			camera1.PosZ += sin(DegreesToRadians(camera1.RotX)) * speed;
			break;
		case'd':
		case'D':
			camera1.PosX -= cos(DegreesToRadians(camera1.RotX)) * speed;
			camera1.PosZ -= sin(DegreesToRadians(camera1.RotX)) * speed;
			break;
	}
	glutPostRedisplay();
	
	printf("Pos: X=%f, Y=%f, Z=%f \n", camera1.PosX, camera1.PosY, camera1.PosZ);
}

//-------Window reshape callback-------------------------------
//--Keeps the window filled with the scene and ensures the window and 
//--camera aspect ratio is the same
void reshape(int width, int height)
{
	float aspect = (float)width/(float)height;	
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, aspect, 5, 500);
}

void configLights()
{
	GLfloat mat_shininess[] = { 50.0 };
	
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
	glEnable(GL_LIGHTING);	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);  
	   
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    
	glLightfv(GL_LIGHT0, GL_AMBIENT, red);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, red);
	glLightfv(GL_LIGHT0, GL_SPECULAR, red);	
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, .1);
		
	glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);      
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 50.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,0.01);	
}
//----------------------------------------------------------------------
void initialize(void)
{
	configLights();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

//  ------- Main: Initialize glut window and register call backs -------
int main(int argc, char **argv) 
{ 
	glutInit(&argc, argv);            
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);  
	glutInitWindowSize(600,600);
	glutInitWindowPosition(200,200);
	glutCreateWindow("Teapot Factory");
	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, gearTimer, 0);  
	glutSpecialFunc(special); 
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0; 
}

