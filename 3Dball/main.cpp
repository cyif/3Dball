// glutEx1.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define BITMAP_ID 0x4D42
#include "glew.h"
#include "wglew.h"
#include "gl/glut.h"
#include "ball.h"
#pragma comment(lib, "glew32.lib")
#define TEXH 64
#define TEXW 64


float fTranslate;
float fRotate;
float fScale = 1.0f;	// set inital scale value to 1.0f
GLuint texture[4];
bool bPersp = true;
bool bAnim = false;
bool bWire = false;
bool shot = false;
GLubyte tex[TEXH][TEXW][3];
GLUquadricObj *quadratic = gluNewQuadric();
int modee = 2;
int wHeight = 0;
int wWidth = 0;
int wenLi = 0;
int leftan = 0;
int rightan = 0;
float white[] = { 0.8f, 0.8f, 0.8f, 0.8f };
float red[] = { 1.0f, 0.0f, 0.0f, 0.0f };
float green[] = { 0.0f, 1.0f, 0.0f, 0.0f };
float defaultdiff[] = { 0.8f, 0.8f, 0.8f, 1.0f };
float defaultspe[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float defaultamb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
void huazhuozi();
void drawCube(int x);
void makeImage(){
	int i, j, c;
	for (i = 0; i < TEXH; i++){
		for (j = 0; j < TEXW; j++){
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			if (c == 255){
				tex[i][j][0] = (GLubyte)255;
				tex[i][j][1] = (GLubyte)0;
				tex[i][j][2] = (GLubyte)0;
			}
			else {
				tex[i][j][0] = (GLubyte)c;
				tex[i][j][1] = (GLubyte)c;
				tex[i][j][2] = (GLubyte)c;
			}
		}
	}
}

// �����ʾ�����飬������������ı�ʾ��
// ����: ͨ��ָ�룬����filename ָ����bitmap�ļ������ݡ�
// ͬʱҲ����bitmap��Ϣͷ.����֧��-bitλͼ��
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;	// �ļ�ָ��
	BITMAPFILEHEADER bitmapFileHeader;	// bitmap�ļ�ͷ
	unsigned char	*bitmapImage;		// bitmapͼ������
	int	imageIdx = 0;		// ͼ��λ������
	unsigned char	tempRGB;	// ��������

	// �ԡ�������+����ģʽ���ļ�filename 
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL) return NULL;
	// ����bitmap�ļ�ͼ
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// ��֤�Ƿ�Ϊbitmap�ļ�
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file\n");
		return NULL;
	}

	// ����bitmap��Ϣͷ
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// ���ļ�ָ������bitmap����
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// Ϊװ��ͼ�����ݴ����㹻���ڴ�
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
	// ��֤�ڴ��Ƿ񴴽��ɹ�
	if (!bitmapImage) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	// ����bitmapͼ������
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// ȷ�϶���ɹ�
	if (bitmapImage == NULL) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	//����bitmap�б���ĸ�ʽ��BGR�����潻��R��B��ֵ���õ�RGB��ʽ
	for (imageIdx = 0;
		imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	// �ر�bitmapͼ���ļ�
	fclose(filePtr);
	return bitmapImage;
}

//��������ĺ�����
void texload(int i, char *filename)
{

	BITMAPINFOHEADER bitmapInfoHeader;                                 // bitmap��Ϣͷ
	unsigned char*   bitmapData;                                       // ��������

	bitmapData = LoadBitmapFile(filename, &bitmapInfoHeader);
	glBindTexture(GL_TEXTURE_2D, texture[i]);
	// ָ����ǰ����ķŴ�/��С���˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0, 	    //mipmap���(ͨ��Ϊ����ʾ���ϲ�) 
		GL_RGB,	//����ϣ���������к졢�̡�������
		bitmapInfoHeader.biWidth, //��������������n�����б߿�+2 
		bitmapInfoHeader.biHeight, //����߶ȣ�������n�����б߿�+2 
		0, //�߿�(0=�ޱ߿�, 1=�б߿�) 
		GL_RGB,	//bitmap���ݵĸ�ʽ
		GL_UNSIGNED_BYTE, //ÿ����ɫ���ݵ�����
		bitmapData);	//bitmap����ָ��  
}

//��������ĺ�����
void init()
{
	glGenTextures(4, texture);                                         // ��һ��������Ҫ���ɱ�ʾ���ĸ���, �ڶ������Ƿ��ر�ʾ��������
	texload(0, "Monet.bmp");
	texload(1, "Crack.bmp");
	texload(3, "Spot.bmp");
	//���������Զ�������

	makeImage(); //�ο�opengl red book����������ͺ����Ĳ��衣
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //�������ش洢ģʽ��������ȡ��ͼ�����ݵ��ж��뷽ʽ.
	glTexImage2D(GL_TEXTURE_2D, 0, 3, TEXW, TEXH, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	bool multiTexturingSupported = strstr((const char*)extensions, "GL_ARB_multitexture ") != NULL;
	GLint maxTextureUnits = 0;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &maxTextureUnits);
	printf("Texture Units available = %d\n", maxTextureUnits);
	glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)wglGetProcAddress("glMultiTexCoord1fARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
	glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress("glMultiTexCoord3fARB");
	glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)wglGetProcAddress("glMultiTexCoord4fARB");
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");



}

void Draw_Leg();

void Draw_Triangle() // This function draws a triangle with RGB colors
{
	glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultdiff);
	glMaterialfv(GL_FRONT, GL_AMBIENT, defaultamb);
	glMaterialfv(GL_FRONT, GL_SPECULAR, defaultspe);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 50);
	glPushMatrix();
	glScalef(2, 2, 1);
	huazhuozi();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-4, 8, 0.5);
	glScalef(2, 2, 2);
	drawCube(1);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(4, 8, 1);
	glutSolidSphere(1, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(4, 0, 0);
	gluCylinder(quadratic, 1.5, 0, 2, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-4, 0, 0);
	gluCylinder(quadratic, 2, 1, 2, 20, 20);
	glTranslatef(0, 0, 2);
	GLfloat R = 1;//Բ�뾶
	GLfloat Pi = 3.1415926536f;
	int n = 20;
	glBegin(GL_POLYGON);
	for (int i = 0; i<n; i++)
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 0.0f);
	glEnd();
	glPopMatrix();

	BallDisplay();
}


void updateView(int width, int height)
{
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	float whRatio = (GLfloat)width / (GLfloat)height;
	if (bPersp) {
		gluPerspective(45.0f, whRatio, 0.1f, 100.0f);
		//glFrustum(-3, 3, -3, 3, 3,100);
	}
	else {
		glOrtho(-3, 3, -3, 3, -100, 100);
	}

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void reshape(int width, int height)
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	wHeight = height;
	wWidth = width;

	updateView(wHeight, wWidth);
}

void idle()
{
	glutPostRedisplay();
}

float eye[] = { 0, 0, 8 };
float center[] = { 0, 0, 0 };

void key(unsigned char k, int x, int y)
{
	switch (k)
	{
	case '1': {
		BallStart();
		break;
	}
	case 'q': {exit(0); break; }
	case 'p': {bPersp = !bPersp; break; }

	case ' ': {bAnim = !bAnim; break; }
	case 'o': {bWire = !bWire; break; }

	case 'a': {
		eye[0] -= 0.2f;
		center[0] -= 0.2f;
		break;
	}
	case 'd': {
		eye[0] += 0.2f;
		center[0] += 0.2f;
		break;
	}
	case 'w': {
		eye[1] -= 0.2f;
		center[1] -= 0.2f;
		break;
	}
	case 's': {
		eye[1] += 0.2f;
		center[1] += 0.2f;
		break;
	}
	case 'z': {
		eye[2] -= 0.2f;
		center[2] -= 0.2f;
		break;
	}
	case 'c': {
		eye[2] += 0.2f;
		center[2] += 0.2f;
		break;
	}
	case 'i':{
		wenLi = (wenLi + 1) % 3;
		break;
	}
	case 'u':{
		modee = (modee + 1) % 3;
		break;
	}
	case 't':{
		center[0] = 0;
		center[1] = 0;
		center[2] = 0;
		break;
	}
	case 'j':{
		shot = true;
		leftan = 100;
		break;
	}
	case 'l':{
		shot = true;
		rightan = 100;
		break;
	}
	}
	updateView(wHeight, wWidth);
}


void redraw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);				// ������0��0��0�����ӵ����� (0,5,50)��Y������

	if (bWire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	GLfloat white[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat light_pos[] = { 5, 5, 5, 1 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glEnable(GL_LIGHT0);

	//	glTranslatef(0.0f, 0.0f,-6.0f);			// Place the triangle at Center
	glRotatef(fRotate, 0, 1.0f, 0);			// Rotate around Y axis
	glRotatef(-90, 1, 0, 0);
	glScalef(0.2, 0.2, 0.2);
	Draw_Triangle();						// Draw triangle

	if (bAnim) fRotate += 0.5f;
	getScore();
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(480, 480);
	int windowHandle = glutCreateWindow("Simple GLUT App");
	init();
	BallInit();
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

void drawCube(int x){

	glBegin(GL_QUADS);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glEnd();
}
void huazhuozi(){
	GLfloat R = 5;//Բ�뾶
	GLfloat Pi = 3.1415926536f;
	int n = 30;
	int h = 1;
	float zhuomian[6][3] = { -5, 10, 0,
		5, 10, 0,
		5, -10, 0,
		0.5, -12, 0,
		-0.5, -12, 0,
		-5, -10, 0
	};
	glBegin(GL_POLYGON);//����
	for (int i = 0; i < 6; i++){
		glVertex3f(zhuomian[i][0], zhuomian[i][1], zhuomian[i][2]);
	}
	glEnd();
	glBegin(GL_POLYGON);//����
	for (int i = 0; i < 6; i++){
		glVertex3f(zhuomian[i][0], zhuomian[i][1], zhuomian[i][2] - 1);
	}
	glEnd();
	for (int j = 0; j < 5; j++){//����
		glBegin(GL_POLYGON);
		int i = j + 1;
		glVertex3f(zhuomian[i % 6][0], zhuomian[i % 6][1], zhuomian[i % 6][2]);
		glVertex3f(zhuomian[i % 6][0], zhuomian[i % 6][1], zhuomian[i % 6][2] - 1);
		i++;
		glVertex3f(zhuomian[i % 6][0], zhuomian[i % 6][1], zhuomian[i % 6][2] - 1);
		glVertex3f(zhuomian[i % 6][0], zhuomian[i % 6][1], zhuomian[i % 6][2]);
		glEnd();
	}
	glPushMatrix();
	glTranslatef(0, 10, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i<n; i++)
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 0.0f);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 10, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i<n; i++)
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), -1.0f);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 10, 0);
	for (int j = 0; j < n / 2; j++){//banyuan����
		glBegin(GL_POLYGON);
		int i = j;
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 0.0f);
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), -1.0f);
		i = i + 1 % n;
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), -1.0f);
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 0.0f);
		glEnd();
	}
	glPopMatrix();
	//���߿�
	float zhuobiankuang[6][3] = { -5 + 0.5, 10, 0,
		5 - 0.5, 10, 0,
		5 - 0.5, -10, 0,
		0.5, -12, 0,
		-0.5, -12, 0,
		-5 + 0.5, -10, 0
	};

	glMaterialfv(GL_FRONT, GL_AMBIENT, green);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green);
	glPushMatrix();
	glTranslatef(4.5, 0, 0.5);
	glScalef(1, 20, 1);
	drawCube(1);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-4.5, 0, 0.5);
	glScalef(1, 20, 1);
	drawCube(1);
	glPopMatrix();
	int r = R - 1;
	glPushMatrix();
	glTranslatef(0, 10, 0);
	for (int j = 0; j < n / 2; j++){//banyuan����
		int i = j;
		glBegin(GL_POLYGON);
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 1.0f);
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 0.0f);
		i = i + 1 % n;
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 0.0f);
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 1.0f);
		glEnd();
		i = j;
		glBegin(GL_POLYGON);
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 1.0f);
		glVertex3f(r*cos(2 * Pi / n*i), r*sin(2 * Pi / n*i), 1.0f);
		i = i + 1 % n;
		glVertex3f(r*cos(2 * Pi / n*i), r*sin(2 * Pi / n*i), 1.0f);
		glVertex3f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i), 1.0f);
		glEnd();
		i = j;
		glBegin(GL_POLYGON);
		glVertex3f(r*cos(2 * Pi / n*i), r*sin(2 * Pi / n*i), 1.0f);
		glVertex3f(r*cos(2 * Pi / n*i), r*sin(2 * Pi / n*i), 0.0f);
		i = i + 1 % n;
		glVertex3f(r*cos(2 * Pi / n*i), r*sin(2 * Pi / n*i), 0.0f);
		glVertex3f(r*cos(2 * Pi / n*i), r*sin(2 * Pi / n*i), 1.0f);
		glEnd();
	}
	glPopMatrix();
	double ang = atan(2 / 4.5);
	glMaterialfv(GL_FRONT, GL_AMBIENT, red);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
	glMaterialfv(GL_FRONT, GL_SPECULAR, red);
	glBegin(GL_POLYGON);
	glVertex3f(5, -10, 0);
	glVertex3f(5, -10, 1);
	glVertex3f(2.75, -11, 1);
	glVertex3f(2.75, -11, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(5, -10, 1.01);
	glVertex3f(5 - 0.5*sin(ang), -10 + 0.5*cos(ang), 1.01);
	glVertex3f(2.75 - 0.5*sin(ang), -11 + 0.5*cos(ang), 1.01);
	glVertex3f(2.75, -11, 1.01);
	glEnd();
	glBegin(GL_POLYGON);//�ڲ�
	glVertex3f(5 - 0.5*sin(ang), -10 + 0.5*cos(ang), 1.01);
	glVertex3f(5 - 0.5*sin(ang), -10 + 0.5*cos(ang), 0.01);
	glVertex3f(2.75 - 0.5*sin(ang), -11 + 0.5*cos(ang), 0.01);
	glVertex3f(2.75 - 0.5*sin(ang), -11 + 0.5*cos(ang), 1.01);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-5, -10, 0);
	glVertex3f(-5, -10, 1);
	glVertex3f(-2.75, -11, 1);
	glVertex3f(-2.75, -11, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-5, -10, 1.01);
	glVertex3f(-5 + 0.5*sin(ang), -10 + 0.5*cos(ang), 1.01);
	glVertex3f(-2.75 + 0.5*sin(ang), -11 + 0.5*cos(ang), 1.01);
	glVertex3f(-2.75, -11, 1.01);
	glEnd();
	glBegin(GL_POLYGON);//�ڲ�
	glVertex3f(-5 + 0.5*sin(ang), -10 + 0.5*cos(ang), 1.01);
	glVertex3f(-5 + 0.5*sin(ang), -10 + 0.5*cos(ang), 0.01);
	glVertex3f(-2.75 + 0.5*sin(ang), -11 + 0.5*cos(ang), 0.01);
	glVertex3f(-2.75 + 0.5*sin(ang), -11 + 0.5*cos(ang), 1.01);
	glEnd();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green);
	double xishu = 0.2;
	glPushMatrix();
	if (rightan){
		rightan--;
		glTranslatef(2.75 - 1 / sin(ang)*0.4*sin(ang) - 0.25*cos(ang), -11 + 1 / sin(ang)*0.4 * cos(ang) - 0.25*sin(ang), 0.5);
		glRotatef(90 + 180 / Pi*ang, 0, 0, 1);

	}
	else{
		glTranslatef(2.75 - 4.5*xishu - 0.25*sin(ang), -11 - 2 * xishu + 0.25*cos(ang), 0.5);
		//glTranslatef(2.75  - 0.25*sin(ang), -11 + 0.25*cos(ang), 0.5);
		glRotatef(180 / Pi*ang, 0, 0, 1);
	}
	glScalef(2 / sin(ang)*0.4, 0.5, 1);
	drawCube(1);

	glPopMatrix();
	glPushMatrix();
	if (leftan){
		leftan--;
		glTranslatef(-(2.75 - 1 / sin(ang)*0.4*sin(ang) - 0.25*cos(ang)), -11 + 1 / sin(ang)*0.4 * cos(ang) - 0.25*sin(ang), 0.5);
		glRotatef(-90 - 180 / Pi*ang, 0, 0, 1);

	}
	else{
		glTranslatef(-(2.75 - 4.5*xishu - 0.25*sin(ang)), -11 - 2 * xishu + 0.25*cos(ang), 0.5);
		//glTranslatef(2.75  - 0.25*sin(ang), -11 + 0.25*cos(ang), 0.5);
		glRotatef(-180 / Pi*ang, 0, 0, 1);
	}
	glScalef(2 / sin(ang)*0.4, 0.5, 1);
	drawCube(1);

	glPopMatrix();
}

void getScore()
{
	static char buffer[256];
	sprintf(buffer, "Score: %d", score);

	char *c;
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 480, 0, 480, -1, 1);  
	glMatrixMode(GL_MODELVIEW);  
	glPushMatrix();              
	glLoadIdentity();            
	glRasterPos2f(10, 10);
	for (c = buffer; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();              
	glMatrixMode(GL_MODELVIEW);  
	glPopMatrix();             
	glEnable(GL_DEPTH_TEST);
}
