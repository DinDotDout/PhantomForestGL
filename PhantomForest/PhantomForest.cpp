#include "pch.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <fstream>

#include "BmpLoader.h"
#include "Camera.h"

using namespace std;

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);
void SpecialKeyboardUp(int key, int x, int y);
void MouseMotion(int x, int y);
void Mouse(int button, int state, int x, int y);
void Timer(int value);
void Idle();
void Init();

void LoadTexture(const char * imagepath);
void DrawBracito(bool derecho);
void DrawBoo(bool rey);
void DrawTree();
void DrawEscenario();
void DrawMoon();

bool admin = false;

GLfloat fRotacionGlobal;
float movY = -2.0f;
bool arriba = 1;
bool velChange = 0;
GLfloat f = 0.0f;
float sumx = 0.5f;
GLfloat invisible = 0.0f;
float suminv = 0.5f;

//variables de teclado y raton
Camera g_camera;
bool g_key[256];
bool g_key_left;
bool g_key_right;
bool g_key_up;
bool g_key_down;
bool g_mouse_left_down = false;
bool g_mouse_right_down = false;
int g_viewport_width = 0;
int g_viewport_height = 0;
int pos_cam = 0;
float x_cam, y_cam, z_cam;
bool g_fps_mode = true;


//Variables de movimiento
float g_translation_speed = 10.0f;
const float g_rotation_speed = 60.0f * ((acos(-1.0f)) / 180.0f);
const float g_rotation_speed_mouse = 5.0f * (acos(-1.0f)) / 180.0f;

//variables de entorno
int escala;
const int numArboles = 100;
float escalaf[numArboles];
float posAr_x[numArboles];
float posAr_z[numArboles];



//Opciones de luz

GLfloat light_ambient_moon[] = { 0.0f, 0.0f, 0.0f, 0.0f }; //nos aseguramos que la luna no haga luz ambiente
GLfloat light_diffuse_moon[] = { 0.1f, 0.0f, 0.0f, 1.0f };
GLfloat light_position_moon[] = { 20.0f, 80.0f, 20.0f, 1.0f };

GLfloat light_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat light_spot_direction[] = { 0.0f, 0.0f, -1.0f };
GLfloat moonlight[] = { 0.0f, -20.0f, 0.0f,1.0f };
bool Luces = false;

//Opciones de niebla
bool fog = true;
GLuint filter;
GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
GLuint fogfilter = 0;                    // Which Fog To Use
GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };      // Fog Color

//Time
int new_t;
int old_t = 0;
float deltaTime = 0;

GLuint textureID[1];


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Rotacion: W,A,S,D Movimiento: Flechas");
	glutSetCursor(GLUT_CURSOR_NONE);

	LoadTexture("grass1.bmp");

	Init();

	old_t = glutGet(GLUT_ELAPSED_TIME);

	glutIgnoreKeyRepeat(1);

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(SpecialKeyboard);
	glutSpecialUpFunc(SpecialKeyboardUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	glutIdleFunc(Idle);

	glutTimerFunc(1, Timer, 0);
	glutMainLoop();

	return 0;
}

void Init(void) {

	glEnable(GL_LIGHTING);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);          // We'll Clear To The Color Of The Fog ( Modified )
	glFogi(GL_FOG_MODE, fogMode[fogfilter]);        // Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.05f);              // How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
	//glFogf(GL_FOG_START, 0.0f);             // Fog Start Depth
	//glFogf(GL_FOG_END, 1.0f);               // Fog End Depth
	glEnable(GL_FOG);                   // Enables GL_FOG

	old_t = glutGet(GLUT_ELAPSED_TIME);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position_moon);
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient_moon);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse_moon);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 180);

	for (int i = 0; i < numArboles; i++) { //Randomizador de posicion y escalado de arboles
		escala = rand() % 8 + 7;
		escalaf[i] = escala / 10.0f;

		posAr_x[i] = (float)(rand() % 80 + 10);
		posAr_z[i] = (float)(rand() % 80 + 10);
	}
}


void Display(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0); //Pintamos el fondo negro
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHT0);
	if (Luces) { //Si encendemos las luces ponemos una luz ambiente general
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	}
	else {
		glDisable(GL_LIGHT0);
	}

	glLoadIdentity();
	//LIGHT1 es una camara tipo linterna que apuntara donde mire la persona
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_spot_direction);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 60);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 70);

	if (pos_cam == 0) { //Camara primera persona
		glFogf(GL_FOG_DENSITY, 0.05f);
		g_camera.Refresh();
	}
	else if (pos_cam == 1) { //Vista aerea
		glDisable(GL_LIGHT1); //No hay linterna en vista aerea
		glFogf(GL_FOG_DENSITY, 0.01f);
		glMatrixMode(GL_MODELVIEW);
		glOrtho(-1.0, 1.0f, -1.0, 1.0f, -1.0, 1.0f);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		glRotatef(90, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, -100.0f, 0.0f);
	}

	DrawEscenario();

	//Arboles en posiciones aleatorias dentro de cierto rango y de tamaños aleatorios
	glPushMatrix();
	glTranslatef(-50.0f, 0.0f, -50.0f);
	for (int i = 0; i < numArboles; i++) {
		glPushMatrix();
		glTranslatef(posAr_x[i], 0.0f, posAr_z[i]);
		//Translate
		glScalef(escalaf[i], escalaf[i], escalaf[i]);
		DrawTree();
		glPopMatrix();
	}

	// Boos en arboles
	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glTranslatef(posAr_x[i], 0.0f, posAr_z[i]);
		glRotatef(fRotacionGlobal, 0.0f, 1.0f, 0.0f);
		glTranslatef(3.0f, 2.0f, 0.0f);
		DrawBoo(false);
		glPopMatrix();
	}

	for (int i = 6; i < 12; i++) {
		glPushMatrix();
		glTranslatef(posAr_x[i], 0.0f, posAr_z[i]);
		glRotatef(-fRotacionGlobal, 0.0f, 1.0f, 0.0f);
		glTranslatef(3.0f, 2.0f, 0.0f);
		glRotatef(180, 0.0f, 1.0f, 0.0f);
		DrawBoo(false);
		glPopMatrix();
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 20.0f, 0.0f);
	glScalef(15.0f, 15.0f, 15.0f);
	DrawBoo(true);
	glPopMatrix();
	DrawMoon();

	glutSwapBuffers(); //Intercambio de buffers
}

//Cogemos las texturas y las asignamos
void LoadTexture(const char * imagepath) {
	unsigned int width, height;
	void * data = loadBMPRaw(imagepath, width, height);
	// Create one OpenGL texture
	glGenTextures(1, textureID);
	glEnable(GL_TEXTURE_2D);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID[0]);
	// Give the image to OpenGL
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	delete[] data;
}
void DrawMoon() {//Dibujamos la luna con menor niebla, además tendra un "aura" roja alrededor
	glPushMatrix();
	//glDisable(GL_FOG);
	glFogf(GL_FOG_DENSITY, 0.01f);
	glTranslatef(20.0f, 80.0f, 20.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glutSolidSphere(7, 60, 60);
	glColor4f(0.6f, 0.0f, 0.0f, 0.3f);
	glutSolidSphere(7.5, 60, 60);
	//glEnable(GL_FOG);
	glFogf(GL_FOG_DENSITY, 0.05f);
	glPopMatrix();
}

void DrawBracito(bool derecho) {
	glPushMatrix();
	if (derecho) glRotatef(-f * 40, 0.0f, 0.0f, 1.0f);
	else glRotatef(f * 40, 0.0f, 0.0f, 1.0f);
	glScalef(1.6f, 0.8f, 1.0f);
	glutSolidSphere(0.3f, 30, 30);
	glPopMatrix();
}

//Funcion que pinta el fantasma
void DrawBoo(bool rey) {
	glPushMatrix();
	glScalef(0.5, 0.5, 0.5); //Escalamos para poder trabajar con valores mas sencillos
	glTranslatef(0.0f, 1.0f + f, 0.0f);
	//Ojos
	glPushMatrix();//Los ojos son esferas aplastadas
	glTranslatef(0.16f, 0.2f, -1.0f);
	glScalef(0.4f, 1.0f, 0.01f);
	if (rey) {
		glColor3f(0.0f, 0.0f, 0.0f); //El rey no tiene transparencias
	}
	else {
		glColor4f(0.0f, 0.0f, 0.0f, invisible);
	}
	glutSolidSphere(0.2f, 20, 5);
	glTranslatef(-0.8f, 0.0f, 0.0f);
	glutSolidSphere(0.2f, 20, 5);
	glPopMatrix();
	if (rey) { //Si es rey dibujamos la corona con una tetera y una bola
		glPushMatrix();
		glTranslatef(0.0f, 1.65f, 0.0f);
		glColor3f(1.0f, 0.1f, 0.1f);
		glutSolidSphere(0.15f, 20, 20);
		glTranslatef(0.0f, -0.45f, 0.0f);
		glColor3f(1.0f, 1.0f, 0.1f);
		glutSolidTeapot(0.5);
		glPopMatrix();
	}
	//Cuerpo
	if (rey) {
		glColor3f(1.0f, 1.0f, 1.0f); //El rey no tiene transparencias
	}
	else {
		glColor4f(1.0f, 1.0f, 1.0f, invisible);
	}
	glutSolidSphere(1.0f, 60, 60);
	//Animacion brazos
	glPushMatrix();
	glTranslatef(0.8f, 0.0f, 0.0f);
	DrawBracito(true);
	glTranslatef(-1.6f, 0.0f, 0.0f);
	DrawBracito(false);
	glPopMatrix();
	//Cola en forma de cono
	glTranslatef(0.0f, -0.5f, 0.15f);
	glutSolidCone(0.5, 1.5, 20, 20);
	glPopMatrix();


}

//Funcion para pintar arboles grandes
void DrawTree() {
	//trunk
	glPushMatrix();
	glColor3f(0.3f, 0.2f, 0.1f);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.5, 10, 20, 5);
	//Hojas
	glColor3f(0.0f, 0.1f, 0.0f);
	glTranslatef(0.0f, 0.0f, 4.0f); //Translates en ejes Z debido al rotate
	glutSolidCone(4, 5, 20, 10);
	glColor3f(0.05f, 0.15f, 0.0f);
	glTranslatef(0.0f, 0.0f, 2.0f);
	glutSolidCone(3.5, 3.5, 20, 10);
	glColor3f(0.0f, 0.1f, 0.0f);
	glTranslatef(0.0f, 0.0f, 1.8f);
	glutSolidCone(2.8, 3, 20, 10);
	glColor3f(0.05f, 0.15f, 0.0f);
	glTranslatef(0.0f, 0.0f, 1.8f);
	glutSolidCone(2, 2.2, 20, 10);
	glColor3f(0.0f, 0.1f, 0.0f);
	glTranslatef(0.0f, 0.0f, 1.5f);
	glutSolidCone(1.4, 3, 20, 10);
	glPopMatrix();
}

void DrawEscenario() {
	//Suelo
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glColor3f(0.4f, 0.6f, 0.4f);
	glBindTexture(GL_TEXTURE_2D, textureID[0]);
	glBegin(GL_QUADS);
	for (GLfloat i = -200.0f; i < 200.0f; i++) {
		for (GLfloat j = -200.0f; j < 200.0f; j++) {
			glNormal3f(0, 1, 0);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(i, 0, j);

			glNormal3f(0, 1, 0);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(i + 1, 0, j);

			glNormal3f(0, 1, 0);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(i + 1, 0, j + 1);

			glNormal3f(0, 1, 0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(i, 0, j + 1);
		}
	}
	glEnd();
	glPopMatrix();
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glScalef(4.0f, 1.0f, 4.0f);
	//Techo
	glPushMatrix();
	glColor3d(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-50.0f, 100.0f, -50.0f);
	glVertex3f(50.0f, 100.0f, -50.0f);
	glVertex3f(50.0f, 100.0f, 50.0f);
	glVertex3f(-50.0f, 100.0f, 50.0f);
	glEnd();
	glPopMatrix();
	//Paredes
	//1
	glPushMatrix();
	glTranslatef(-50.0f, 0.0f, -50.0f);
	glBegin(GL_POLYGON);
	glVertex3f(0.0f, 100.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 100.0f, 0.0f);
	glEnd();
	glPopMatrix();
	//2
	glPushMatrix();
	glTranslatef(50.0f, 0.0f, -50.0f);
	glBegin(GL_POLYGON);
	glVertex3f(0.0f, 100.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glVertex3f(0.0f, 100.0f, 100.0f);
	glEnd();
	glPopMatrix();
	//3
	glPushMatrix();
	glTranslatef(50.0f, 0.0f, 50.0f);
	glBegin(GL_POLYGON);
	glVertex3f(0.0f, 100.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 100.0f, 0.0f);
	glEnd();
	glPopMatrix();
	//4
	glPushMatrix();
	glTranslatef(-50.0f, 0.0f, 50.0f);
	glBegin(GL_POLYGON);
	glVertex3f(0.0f, 100.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glVertex3f(0.0f, 100.0f, -100.0f);
	glEnd();
	glPopMatrix();
	glPopMatrix();
}

void Reshape(int w, int h) {
	g_viewport_width = w;
	g_viewport_height = h;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)w / (GLfloat)h, 0.1, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

/*
	Opciones de teclado
*/
void Keyboard(unsigned char key, int x, int y)
{
	if (key == 27) {
		exit(0);
	}

	if (key == 'l' || key == 'L') {
		Luces = !Luces;
	}

	if (key == ' ') {
		g_fps_mode = !g_fps_mode;

		if (g_fps_mode) {
			glutSetCursor(GLUT_CURSOR_NONE);
			glutWarpPointer(g_viewport_width / 2, g_viewport_height / 2);
		}
		else {
			glutSetCursor(GLUT_CURSOR_DESTROY);
		}
	}

	//Cambio de velocidad
	if (key == 'v' || key == 'V') {
		velChange = !velChange;
		if (velChange) {
			g_translation_speed = 20.0f;
		}
		else {
			g_translation_speed = 10.0f;
		}
	}

	if (key == 'f' || key == 'F') {
		fog = !fog;
		if (fog) {
			glEnable(GL_FOG);
		}
		else {
			glDisable(GL_FOG);
		}
	}

	if (key == 'g' || key == 'G') {
		admin = !admin;
		if (admin) {
			glDisable(GL_FOG);
			Luces = true;
		}
		else {
			glEnable(GL_FOG);
			Luces = false;
		}
	}

	g_key[key] = true;
}

void KeyboardUp(unsigned char key, int x, int y)
{
	g_key[key] = false;
}

void Timer(int value)
{
	//Rotacion vertical de la camara
	if (g_key_up) {
		g_camera.RotatePitch(+g_rotation_speed * deltaTime);
	}
	else if (g_key_down) {
		g_camera.RotatePitch(-g_rotation_speed * deltaTime);
	}
	//Rotacion lateral de la camara
	if (g_key_left) {
		g_camera.RotateYaw(+g_rotation_speed * deltaTime);
	}
	else if (g_key_right) {
		g_camera.RotateYaw(-g_rotation_speed * deltaTime);
	}

	//Movimiento vertical camara
	if (g_key['q'] || g_key['Q']) {
		g_camera.Fly(-g_translation_speed * deltaTime);
	}
	else if (g_key['e'] || g_key['E']) {
		g_camera.Fly(g_translation_speed * deltaTime);
	}
	else if (g_mouse_left_down) {
		g_camera.Fly(-g_translation_speed * deltaTime);
	}
	else if (g_mouse_right_down) {
		g_camera.Fly(+g_translation_speed * deltaTime);
	}

	//Movimiento izquierda-derecha
	if (g_key['d'] || g_key['D']) {
		g_camera.Strafe(+g_translation_speed * deltaTime);
	}
	else if (g_key['a'] || g_key['A']) {
		g_camera.Strafe(-g_translation_speed * deltaTime);
	}
	//Movimiento adelante-atras
	if (g_key['w'] || g_key['W']) {
		g_camera.Move(+g_translation_speed * deltaTime);
	}
	else if (g_key['s'] || g_key['S']) {
		g_camera.Move(-g_translation_speed * deltaTime);
	}

	//Comprobamos que no salimos de la zona

	if (!admin) {
		float x_pos, y_pos, z_pos;
		g_camera.GetPos(x_pos, y_pos, z_pos);
		if (x_pos > 140.0f) {
			x_pos = -140;
		}
		else if (x_pos < -140.0f) {
			x_pos = 140;
		}
		if (y_pos > 60.0f) {
			y_pos = 60.0f;
		}
		else if (y_pos < 1.0f) {
			y_pos = 1.0f;
		}
		if (z_pos > 140.0f) {
			z_pos = -140.0f;
		}
		else if (z_pos < -140.0f) {
			z_pos = 140.0f;
		}

		g_camera.SetPos(x_pos, y_pos, z_pos);
	}

}
//Interaccion de teclas especiales (flechas y F's)
void SpecialKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		g_key_left = true;
		break;
	case GLUT_KEY_RIGHT:
		g_key_right = true;
		break;
	case GLUT_KEY_UP:
		g_key_up = true;
		break;
	case GLUT_KEY_DOWN:
		g_key_down = true;
		break;
	}

	switch (key) {
	case GLUT_KEY_F1:
		pos_cam = 0;
		break;
	case GLUT_KEY_F2:
		pos_cam = 1;
		break;
	}
}

void SpecialKeyboardUp(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		g_key_left = false;
		break;
	case GLUT_KEY_RIGHT:
		g_key_right = false;
		break;
	case GLUT_KEY_UP:
		g_key_up = false;
		break;
	case GLUT_KEY_DOWN:
		g_key_down = false;
		break;
	}
}
//Gestion  de los botones del mouse
void Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			g_mouse_left_down = true;
		}
		else if (button == GLUT_RIGHT_BUTTON) {
			g_mouse_right_down = true;
		}
	}
	else if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON) {
			g_mouse_left_down = false;
		}
		else if (button == GLUT_RIGHT_BUTTON) {
			g_mouse_right_down = false;
		}
	}
}

//Gestion de los movimientos del raton para rotar la camara
void MouseMotion(int x, int y)
{
	static bool just_warped = false;

	if (just_warped) {
		just_warped = false;
		return;
	}

	if (g_fps_mode) {
		int dx = x - g_viewport_width / 2;
		int dy = y - g_viewport_height / 2;

		if (dx) {
			g_camera.RotateYaw(-g_rotation_speed_mouse * dx * deltaTime);
		}

		if (dy) {
			g_camera.RotatePitch(-g_rotation_speed_mouse * dy * deltaTime);
		}

		glutWarpPointer(g_viewport_width / 2, g_viewport_height / 2);

		just_warped = true;
	}
}

void Idle()
{
	//Obtiene el delta time en segundos para hacer los movimientos de forma similar independientemente de los fps
	new_t = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (float)(new_t - old_t) / 1000.0000f;
	old_t = new_t;

	// Incrementamos el angulo
	fRotacionGlobal += 60 * deltaTime;

	// Si es mayor que dos pi la decrementamos
	if (fRotacionGlobal > 360) fRotacionGlobal -= 360;

	// Movimiento vertical de los boo con la funcion sinus
	sumx += 2 * deltaTime;
	f = sin(sumx);

	invisible += suminv * deltaTime / 16;
	if (invisible <= 0.2f) {
		suminv += 7.0f*deltaTime;
	}
	else {
		suminv -= 2.0f*deltaTime;
	}

	glutTimerFunc(1, Timer, 0);
	// Indicamos que es necesario repintar la pantalla
	glutPostRedisplay();
}