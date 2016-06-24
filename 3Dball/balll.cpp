#include "ball.h"

struct vec{
	GLfloat x, y;
} speed, force;

//GLfloat M[16];

GLfloat colPos[4][3];
GLfloat pos[3] = {-3, -10, 1};
static float lastTime = 0.0f;
int col = 0;
int score;

void BallInit()
{
	speed.x = 0;
	speed.y = 0;
	
	colPos[0][0] = 4; colPos[0][1] = 8; colPos[0][2] = 2;
	colPos[1][0] = -4; colPos[1][1] = 8; colPos[1][2] = 2;
	colPos[2][0] = 4; colPos[2][1] = 0; colPos[2][2] = 2;
	colPos[3][0] = -4; colPos[3][1] = 0; colPos[3][2] = 2.8;
}

void BallStart()
{
	pos[0] = -3;
	pos[1] = -10;
	pos[2] = 1;
	speed.x = 0;
	speed.y = -7;
	force.x = 0;
	force.y = -5;
	score = 0;
}

GLfloat Distance(GLfloat p[3], GLfloat x1, GLfloat y1)
{
	GLfloat res = 0;
	res += (p[0] - x1) * (p[0] - x1);
	res += (p[1] - y1) * (p[1] - y1);
	return sqrt(res);
}

GLfloat Distance(GLfloat p[3], GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	if (p[0] + 1 < fminf(x1, x2) || p[0] - 1 > fmaxf(x1, x2) || p[1] + 1 < fminf(y1, y2) || p[1] - 1> fmaxf(y1, y2))
		return 10000;
	GLfloat A = y2 - y1;
	GLfloat B = x1 - x2;
	GLfloat C = y1 * (x2 - x1) - x1 * (y2 - y1);
	GLfloat res = abs(A * p[0] + B * p[1] + C);
	res /= sqrt(A * A + B * B);
	//printf("%f\n", res);
	return res;
}

void CollisionDetect()
{
	//防止多次碰撞
	if (col) {
		col--;
		return;
	}
	vec newSpeed = { 0 };
	//右下方挡板
	if (Distance(pos, 5 * 2, -10 * 2, 0.75 * 2, -12 * 2) < 1.8) {
		GLfloat ang = atan(2 / 4.5);
		col = DELAY;
		if (abs(ang - atan(speed.y / speed.x)) < 0.00001)
			return;
		if (speed.y < 0.00001) {
			newSpeed.y += -speed.y * cos(2 * ang);
			if (speed.x < 0.00001)
				newSpeed.x += speed.x + speed.y * sin(2 * ang);
		}
		if (speed.x > -0.00001) {
			newSpeed.x += speed.x * cos(2 * ang);
			if (speed.y > -0.00001)
				newSpeed.y += speed.y + speed.x * sin(2 * ang);
		}
		speed.x = newSpeed.x * 0.7;
		speed.y = newSpeed.y * 0.7;
		//printf("%f   %f\n", speed.x, speed.y);
		return;
	}
	
	//左下方挡板
	if (Distance(pos, -5 * 2, -10 * 2, -0.75 * 2, -12 * 2) < 1.8) {
		col = DELAY;
		//printf("%f   %f\n", speed.x, speed.y);
		GLfloat ang = atan(2 / 4.5);
		if (abs(ang - atan(speed.y / speed.x)) < 0.00001)
			return;
		if (speed.y < 0.00001) {
			newSpeed.y += -speed.y * cos(2 * ang);
			if (speed.x > -0.00001)
				newSpeed.x += speed.x - speed.y * sin(2 * ang);
		}
		if (speed.x < 0.00001) {
			newSpeed.x += -speed.x * cos(2 * ang);
			if (speed.y > -0.00001)
				newSpeed.y += speed.y - speed.x * sin(2 * ang);
		}
		speed.x = newSpeed.x * 0.6;
		speed.y = newSpeed.y * 0.6;
		//printf("%f   %f\n", speed.x, speed.y);
		
		return;
	}

	//两侧挡板
	if (pos[0] < -7.3 || pos[0] > 7.3) {
		printf("%f    %f\n", pos[0], pos[1]);

		col = DELAY;
		newSpeed.x = -speed.x * 0.8;
		newSpeed.y = speed.y;

		speed = newSpeed;

		return;
	}
	//顶部
	if (pos[1] > 20 && Distance(pos, 0, 20) > 6) {
		//printf("1----------    %f       %f\n", speed.x, speed.y);
		col = DELAY;
		GLfloat ang1 = atan(pos[0] / (pos[1] - 20));
		GLfloat ang2 = atan(speed.x / speed.y);
		GLfloat sp = sqrt(speed.x * speed.x + speed.y * speed.y) * 0.9;
		/*if (speed.x > 0) {
			speed.x = abs(sp * sin(ang2 + 2 * ang1));
			if (pos[0] > 0)
				speed.y = abs(sp * cos(ang2 + 2 * ang1));
			else
				speed.y = -abs(sp * cos(ang2 + 2 * ang1));
		}
		else {
			speed.x = -abs(sp * sin(ang2 + 2 * ang1));
			if (pos[0] < 0)
				speed.y = abs(sp * cos(ang2 + 2 * ang1));
			else
				speed.y = -abs(sp * cos(ang2 + 2 * ang1));
		}*/
		//speed.y = -sp * cos(ang2 + 2 * ang1);

		if (speed.x > 0 && speed.y > 0)
			speed.x = abs(sp * sin(ang2 + 2 * ang1));
		else
			speed.x = -abs(sp * sin(ang2 + 2 * ang1));

		if (speed.x < 0 && pos[0] < 0 && pos[1] < 23)
			speed.x = 0.5;
		else if (speed.x > 0 && pos[0] > 0 && pos[1] < 23)
			speed.x = -0.5;
		speed.y = -abs(sp * cos(ang2 + 2 * ang1));
		//printf("2----------    %f      %f\n", speed.x, speed.y);
	}

	//弹球
	if (shot) {
		printf("%f   %f\n", pos[0], pos[1]);
		if ((pos[0] > 1.3 && pos[0] < 5 && pos[1] > -24.3 && pos[1] < -20) && rightan ||
			(pos[0] > -5 && pos[0] < -1.3 && pos[1] > -24.3 && pos[1] < -20) && leftan) {
			//printf("!!!!!!!\n");
			col = DELAY;
			speed.y = 40 * 1.5 / (abs(pos[0]));
			speed.x = -pos[0];
		}
		shot = false;
		return;
	}

	//物体

	for (int i = 0; i < 4; i++) {
		if (Distance(pos, colPos[i][0], colPos[i][1]) < colPos[i][2]) {
			col = DELAY;
			score++;
			GLfloat ang1 = atan((pos[0] - colPos[i][0])/(pos[1] - colPos[i][1]));
			GLfloat ang2 = atan(speed.y / speed.x);
			GLfloat sp = sqrt(speed.x * speed.x + speed.y * speed.y);
			speed.x = -sp * cos(2 * ang2 - ang2) * 1.05;
			speed.y = sp * sin(2 * ang2 - ang1) * 1.05;
			return;
		}
	}
	
}

void BallMovement()
{
	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	float deltaTime = (currentTime - lastTime) / 1000;
	lastTime = currentTime;
	CollisionDetect();
	pos[0] += speed.x * deltaTime;
	pos[1] += speed.y * deltaTime;
	speed.x += force.x * deltaTime;
	speed.y += force.y * deltaTime;
	//printf("%f ------   %f   %f\n", deltaTime, speed.x, speed.y);
}

void BallDisplay()
{
	BallMovement();
	DrawBall();
}

void DrawBall()
{
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	glutSolidSphere(0.7, 20, 20);
	glPopMatrix();
}
