#include<Windows.h>
#include <glut.h>
#include <cmath>
#include <iostream>
#include <string>
#define PI  3.14159265358979323846264338327950288419716939937510582097494459230781640628620899863
using namespace std;

/*F1 : reset
F2 : FullScreen
F3 : Exit
UpArrow : Increase Speed
DownArrow : Decrese Speed
RightArrow : Move Pad To The Right
LeftArrow : Move Pad To The Left*/

//Ball
struct Ball {
	int segments ;
	float x , y ;
	float velocityX;
	float velocityY;
	float raduis ;
	float ballMaxX, ballMinX, ballMaxY, ballMinY;

};

//Jumping Pad
struct Pad {
	float x_Right;
	float x_Left;
	float y_top;
	float y_bot;


};

//Screen
struct Screen {

	float screenTop, screenBot, screenLeft, screenRight;
	float ratio;
};

//Speed
struct Speed {
	float x;
	float y;
};


//Blocks
int numOfRows = 4, numOfBlocks = 6;
int startPoint = -6, height = 3;
float extra = 0.5;
float blockHeight = 1;
float Rows[4][2][6] = { 
{	{ -9 ,-5, -1, 3 ,7,11 } ,
	{ -12,-8,-4,0,4,8 }
	
},

{
	{ -9.5, -5 - 0.5, -1 - 0.5, 2.5, 6.5,10.5 },
	{ -11.5, -7.5, -3.5, 0.5, 4.5, 8.5 }

},


{
	
	{ -9, -5, -1, 3, 7, 11 },
	{ -12, -8, -4, 0, 4, 8 }
	
},
{
	{ -9.5, -5 - 0.5, -1 - 0.5, 2.5, 6.5,10.5 },

	{ -11.5, -7.5, -3.5, 0.5, 4.5, 8.5 }
	
}

};




//Create globals
Ball ball;
Pad pad;
Screen screen;
Speed speed;
bool gravity = true;
bool won = false;
bool lost = false;
string direction;
float maxBallSpeedX = 0.076;
float maxBallSpeedY = 0.38;
float minSpeedX = 0.03;
float minSpeedY = 0.15;

void initGame();
void display();
void reshape(int w, int h);
void setBallScreenBoundary();
void drawBall();
void drawJumpingPad();
void setBackGround(float red, float green, float blue);
void timer(int);
void detectCollisionWithBlocks();
void detectCollisionWithJumpingPad();
void detectCollisionWithScreen();
void removeElementFromArray(int row ,int block);
void keyboard(int, int, int);
int main(int argc, char** argv)
{

	glutInit(&argc, argv);


	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	glutInitWindowSize(800, 640);
	glutInitWindowPosition(GetSystemMetrics(SM_CXSCREEN) / 6, GetSystemMetrics(SM_CYSCREEN) / 6);

	glutCreateWindow("Breakout");


	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard);
	glutTimerFunc(0, timer, 0);
	initGame();

	setBackGround(0.03, 0.01, 0.01);

	glutMainLoop();
}
void initGame() {
	//Ball
	ball.x = 0;
	ball.y = -4;
	ball.segments = 50;
	ball.raduis = 0.5;

	//Pad
	pad.x_Right = 3;
	pad.x_Left = -3;
	pad.y_top = -7.5;
	pad.y_bot = -8;

	//Speed
	speed.x = minSpeedX;
	speed.y = minSpeedY;


	//Game Settings
	gravity = true;
	lost = false;
	direction = "";

}
void reshape(int w, int h) {
	if (h == 0)
		h = 1;

	screen.ratio = (float)w / h;


	glViewport(0, 0, w, h);



	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w >= h) {


		screen.screenLeft = -10 * screen.ratio;
		screen.screenRight = 10 * screen.ratio;

		screen.screenBot = -10;
		screen.screenTop = 10;

	}
	else {
		screen.screenLeft = -10;
		screen.screenRight = 10;

		screen.screenBot = -10 / screen.ratio;
		screen.screenTop = 10 / screen.ratio;
	}
	
	gluOrtho2D(screen.screenLeft, screen.screenRight, screen.screenBot, screen.screenTop);

	glMatrixMode(GL_MODELVIEW);

	setBallScreenBoundary();

}
void setBallScreenBoundary() {
	ball.ballMinX = screen.screenLeft + ball.raduis;
	ball.ballMaxX = screen.screenRight - ball.raduis;
	ball.ballMinY = screen.screenBot + ball.raduis;
	ball.ballMaxY = screen.screenTop - ball.raduis;
}

void drawBall() {
    glBegin(GL_TRIANGLE_FAN);
	glVertex2f(ball.x, ball.y);
	float angle;

	for (int i = 0; i <= ball.segments; i++) {
		angle = i * 2.0 * PI / ball.segments;
		glVertex2f(ball.x + cos(angle) * ball.raduis, ball.y + sin(angle) * ball.raduis);
	}


	glEnd();
}
void drawJumpingPad() {

	glBegin(GL_QUADS);
	
	glVertex2f(pad.x_Right, pad.y_top);

	glVertex2f(pad.x_Left, pad.y_top);

	glVertex2f(pad.x_Left, pad.y_bot);
	glVertex2f(pad.x_Right, pad.y_bot);

	glEnd();

}
void drawRows() {
	int bot =startPoint;
	for (int row = 0; row< numOfRows; row++) {
		bot = bot + height;
		for (int block = 0; block < numOfBlocks; block++) {


			if (Rows[row][0][block] != 100) {
				glBegin(GL_POLYGON);


				glVertex2f(Rows[row][1][block] + extra, bot);
				glVertex2f(Rows[row][0][block] - extra, bot);

				glVertex2f(Rows[row][0][block], bot + extra);

				glVertex2f(Rows[row][0][block] - extra, bot + blockHeight);
				glVertex2f(Rows[row][1][block] + extra, bot + blockHeight);

				glVertex2f(Rows[row][1][block], bot + extra);

				glVertex2f(Rows[row][1][block] + extra, bot);

				glEnd();

			}
		}
	}
	
}
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();



	// Drawing the ball using ballx and bally as center points.

	glColor3f(1, 0.7, 0.7);
	drawBall();

	// Drawing Blocks.
	glColor3f(0.9, 0.9, 0.5);
	drawRows();

	// Drawing the jumping pad.
	glColor3f(0.5, 0.6, 0.4);
	drawJumpingPad();

	

	glutSwapBuffers();
}
void setBackGround(float red, float green, float blue) {

	glClearColor(red, green, blue, 1);
}
void timer(int v) {
	glutPostRedisplay();
	glutTimerFunc(1, timer, 0);
	detectCollisionWithJumpingPad();
	detectCollisionWithScreen();
	detectCollisionWithBlocks();
	if (gravity) {
		ball.y = ball.y - speed.y;

	}
	else {
		ball.y = ball.y + speed.y;

	}
	if (direction == "right") {
		ball.x = ball.x + speed.x;
	}
	else if (direction == "left") {
	
		ball.x = ball.x - speed.x;

	}


}

void detectCollisionWithScreen() {

	if (ball.x > ball.ballMaxX) {

		direction = "left";

	}


	else if (ball.x < ball.ballMinX) {

		direction = "right";

	}

	else if (ball.y >= ball.ballMaxY) {
		gravity = true;

	}
	else if (ball.y <= ball.ballMinY) {

		lost = true;
	}

}
void detectCollisionWithJumpingPad() {
	

	// if the ball passed the top of the jumping pad.
	if (ball.y - ball.raduis <= pad.y_top - 0.3) {
	
		lost = true;
		
	}
	else if (ball.y - ball.raduis <= pad.y_top && ball.x+ball.raduis >= pad.x_Left && ball.x <= pad.x_Right - 4.0) {

		gravity = false;
		

		direction = "left";
	}
	/// right part
	else if (ball.y - ball.raduis <= pad.y_top && ball.x >= pad.x_Left + 4.0 && ball.x - ball.raduis <= pad.x_Right) {
		gravity = false;
				
		direction = "right";

	}
	// in the middle
	else if (ball.y - ball.raduis <= pad.y_top && ball.x >= pad.x_Left && ball.x <= pad.x_Right) {

		gravity = false;


	}

}
void detectCollisionWithBlocks() {
	int bot = startPoint;
	for (int row = 0; row < numOfRows; row++) {
		bot = bot + height;
		for (int block = 0; block < numOfBlocks; block++) {


			// the ball hits the block form the bottom.
	    if (ball.x - ball.raduis >= Rows[row][1][block]-extra && ball.x + ball.raduis <= Rows[row][0][block]+extra) {
			if (ball.y + ball.raduis >= bot && ball.y < bot+0.2 && !gravity) {
				gravity = true;
				removeElementFromArray(row, block);

			}
			// the ball hits the block from the top.
			else if (ball.y - ball.raduis <= bot + blockHeight && ball.y>(bot+ blockHeight)-0.2 && gravity  ) {
				gravity = false;
				removeElementFromArray(row, block);

			}
			 
		}

		}
	}

}
void removeElementFromArray(int row ,int block) {
	//set the element to 100 and ignore it in display funciton.
	Rows[row][0][block] = 100;
	Rows[row][1][block] = 100;
	

}

void keyboard(int key, int x, int y) {

	
	//left
	if (key == 100) {
		// move to the left if half of jumping pad untill half the pad is out of the screen.
		if (pad.x_Right > screen.screenLeft + (pad.x_Right - pad.x_Left) / 2) {

			pad.x_Left = pad.x_Left - 1.0;
			pad.x_Right = pad.x_Right - 1.0;

		}
		

	}
	//up
	else if (key == 101) {
		if (speed.x < maxBallSpeedX) {
		
				speed.x = speed.x + (speed.x) * 0.05;

		}
		if(speed.y < maxBallSpeedY) {
				speed.y = speed.y + (speed.y) * 0.05;
			

		}
	}
	//right
	else if (key == 102) {
		// move to the right if half of jumping pad untill half the pad is out of the screen.
		if (pad.x_Left < screen.screenRight-(pad.x_Right- pad.x_Left)/2) {
			pad.x_Left = pad.x_Left + 1.0;
			pad.x_Right = pad.x_Right + 1.0;
		}
	}
	//down
	else if (key == 103) {
		float tmpSpeedX = speed.x - (speed.x) * 0.05;
		float tmpSpeedY = speed.y - (speed.y) * 0.05;
		cout << tmpSpeedX << "  " << minSpeedX << endl;
		if (tmpSpeedX > minSpeedX){
		
			speed.x = tmpSpeedX;

		}
		if(tmpSpeedY > minSpeedY) {
	
			speed.y = tmpSpeedY;

		}


	}

	//Restart game.
	else if (key == 1) {

		initGame();
	}
	//F2 Fullscreen.
	else if (key == 2) {

		glutFullScreen();
	}

	//F3 Exit game.
	else if (key == 3) {
		exit(0);
	}



}
