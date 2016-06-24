#ifndef BALL_H
#define BALL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define DELAY 5

extern int leftan, rightan;
extern int col;
extern bool shot;
extern int score;
extern int leftan;
extern int rightan;


void BallInit();
void ChangeSpeed();
void DrawBall();
void BallMovement();
void BallDisplay();
void BallStart();
void CollisionDetect();
void getScore();

GLfloat Distance(GLfloat[], GLfloat x1, GLfloat y1);
GLfloat Distance(GLfloat[], GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

#endif