#include<Windows.h>
#include <glut.h>
#include <iostream>
#include <string>
#include <vector>
#define PI  3.14159265358979323846264338327950288419716939937510582097494459230781640628620899863
using namespace std;

/*F1 : reset
F2 : FullScreen
F3 : Exit
UpArrow : Increase Speed
DownArrow : Decrese Speed
RightArrow : Move Pad To The Right
LeftArrow : Move Pad To The Left
Mouse Left Click: Start/Pause The Game*/

//Ball
struct Ball {
	int segments;
	float x, y;
	float velocityX;
	float velocityY;
	float raduis;
	float ballMaxX, ballMinX, ballMaxY, ballMinY;

};

//Jumping Pad
struct Pad {
	float x_Right;
	float x_Left;
	float y_top;
	float y_bot;
	float middlePoint;

};

//Screen
struct Screen {

	float screenTop, screenBot, screenLeft, screenRight;
	float ratio;
	float widthInPixels, heightInPixels;
};

//Speed
struct Speed {
	float x;
	float y;
};


//Blocks

struct Blocks {
	vector<vector<float>> Rows;
	vector<vector<int>> RemovedBlocks;
	float startRowPosition;
	float rawsPaddingDistance;
	float columnPaddingDistance;
	float blockHeight;

};





//Create globals
Ball ball;
Pad pad;
Screen screen;
Speed speed;
Blocks blocks;
bool gravity = true;
bool won = false;
bool lost = false;
bool pause = true;
bool fullScreenMode = false;
string direction;
float maxBallSpeedX = 0.076;
float maxBallSpeedY = 1.0;
float minSpeedX = 0.02;
float minSpeedY = 0.08;
string onScreenText = "";

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
void removeBlockFromVector(size_t row, size_t blockNum);
vector<vector<int>> initRemovedBlocksVector(int numOfRows);
bool isBlockRemoved(vector<int> RemovedBlocks, int block);
void keyboard(int, int, int);
void mouseMotion(int x, int y);
void mouseClick(int, int ,int, int);
void drawText(float x, float y, float r, float g, float b, void* font, char* string);
void checkGameStatus();
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
	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(mouseMotion);
	glutTimerFunc(0, timer, 0);
	initGame();

	setBackGround(0.03, 0.01, 0.01);

	glutMainLoop();
}
void initGame() {
	//Ball
	ball.x = 0;
	ball.y = -6.0;
	ball.segments = 50;
	ball.raduis = 0.5;

	//Pad
	pad.x_Right = 3.0;
	pad.x_Left = -3.0;
	pad.middlePoint = (pad.x_Right - pad.x_Left) / 2;
	pad.y_top = -7.5;
	pad.y_bot = -8.0;

	//Speed
	speed.x = minSpeedX;
	speed.y = minSpeedY;

	//Blocks
	blocks.startRowPosition = -7.0 ;
	blocks.rawsPaddingDistance = 2.5;
	blocks.columnPaddingDistance = 0.1;
	blocks.blockHeight = 0.5;
	

	blocks.Rows = {
{	
	{ -11, 2, 10 } , //{first block position , each block width , number of blocks}
	
},

{
	{ -11, 2 , 10 },
	
},

{
	{ -11, 2, 10 }
},
{
	{ -11, 2 , 10 },
}
,
{
	{ -11, 2, 10 }
}
,
{
	{ -11, 2, 10 }
}

	};
	blocks.RemovedBlocks = initRemovedBlocksVector(blocks.Rows.size());
	

	//Game Settings
	gravity = true;
	won = false;
	lost = false;
	pause = true;
	direction = "";
	onScreenText = "Right Click To Start";

}
vector<vector<int>> initRemovedBlocksVector(int numOfRows) {
	//create vector of vectors and intilize each with one element equal to -1.
	vector<vector<int>> initilizedVector(numOfRows , vector<int>(1, -1));
	return initilizedVector;
		

}
void reshape(int w, int h) {
	if (h == 0)
		h = 1;

	screen.ratio = (float)w / h;


	screen.widthInPixels = w;
	screen.heightInPixels = h;

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
	float bot = blocks.startRowPosition;
	int drawnBlocksCounter = 0;
	for (size_t row = 0; row < blocks.Rows.size(); row++) {
		bot = bot + blocks.rawsPaddingDistance;
		float blockStartPosition = blocks.Rows[row][0];
		float blockWidth = blocks.Rows[row][1];
		for (size_t block = 0; block < blocks.Rows[row][2]; block++) {
			
				if (!isBlockRemoved(blocks.RemovedBlocks[row],block)) {

					/* RemovedBlocks doesn't contains the block*/
					drawnBlocksCounter += 1;

					glBegin(GL_POLYGON);
					glVertex2f(blockStartPosition, bot);
					glVertex2f(blockStartPosition + blockWidth, bot);
					glVertex2f(blockStartPosition + blockWidth, bot + blocks.blockHeight);
					glVertex2f(blockStartPosition, bot + blocks.blockHeight);

					glEnd();
				}
				
				blockStartPosition = blockStartPosition + blockWidth + blocks.columnPaddingDistance; //move to the begining of the next block

			
		}
		
	}
	//no blocks drawn on screen.
	if (drawnBlocksCounter == 0) {
		won = true;
	 
	}

}
void drawText(float x, float y, float r, float g, float b, void *font, char* string)
{
	/*FONTS:
	GLUT_BITMAP_8_BY_13
	GLUT_BITMAP_9_BY_15
	GLUT_BITMAP_TIMES_ROMAN_10
	GLUT_BITMAP_TIMES_ROMAN_24
	GLUT_BITMAP_HELVETICA_10
	GLUT_BITMAP_HELVETICA_12
	GLUT_BITMAP_HELVETICA_18*/

	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, string[i]);
	}
}
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	//glLoadIdentity();


	//Drawing the ball using ballx and bally as center points.
	glColor3f(1.0, 0.7, 0.7);
	drawBall();

	//Drawing Blocks.
	glColor3f(1.0, 0.5, 0.31);
	drawRows();

	//Drawing the jumping pad.
	glColor3f(1.0, 0.65, 0.52);
	drawJumpingPad();

	//Drawing text on screen.
	const char* cstr = onScreenText.c_str();
	drawText(-12, 9.5, 1, 1, 1, GLUT_BITMAP_8_BY_13, (char*)cstr);

	glutSwapBuffers();
}
void setBackGround(float red, float green, float blue) {

	glClearColor(red, green, blue, 1);
}
void timer(int v) {
	glutPostRedisplay();
	glutTimerFunc(1, timer, 0);
	if (!pause) {
		checkGameStatus();
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
	if (ball.y - ball.raduis <= pad.y_top -speed.y) {

		//lost = true;

	}
	else if (ball.y - ball.raduis <= pad.y_top && ball.x + ball.raduis >= pad.x_Left && ball.x <= pad.x_Right - 4.0 && direction!="left") {

		gravity = false;


		direction = "left";
	}
	/// right part
	else if (ball.y - ball.raduis <= pad.y_top && ball.x >= pad.x_Left + 4.0 && ball.x - ball.raduis <= pad.x_Right && direction!="right") {
		gravity = false;

		direction = "right";

	}
	// in the middle
	else if (ball.y - ball.raduis <= pad.y_top && ball.x >= pad.x_Left && ball.x <= pad.x_Right) {

		gravity = false;


	}

}
void detectCollisionWithBlocks() {
	float bot = blocks.startRowPosition;

	for (size_t row = 0; row < blocks.Rows.size(); row++) {
		bot = bot + blocks.rawsPaddingDistance;
		float blockStartPosition = blocks.Rows[row][0];
		float blockWidth = blocks.Rows[row][1];
		for (size_t block = 0; block < blocks.Rows[row][2]; block++) {


			if (!isBlockRemoved(blocks.RemovedBlocks[row], block) &&ball.x + ball.raduis >= blockStartPosition  && ball.x - ball.raduis <= blockStartPosition+blockWidth ) {
				// the ball hits the block form the bottom.
				if (ball.y + ball.raduis >= bot && ball.y < bot +speed.y  && !gravity) {
					gravity = true;
					removeBlockFromVector(row, block);

				}
				// the ball hits the block from the top.
				else if (ball.y - ball.raduis <= bot + blocks.blockHeight && ball.y > (bot + blocks.blockHeight) - speed.y && gravity) {
					gravity = false;
					removeBlockFromVector(row, block);

				}



			}
			blockStartPosition = blockStartPosition + blockWidth + blocks.columnPaddingDistance; //move to the begining of the next block

		}
	}

}
void removeBlockFromVector(size_t row, size_t blockNum) {
	//Add the specified block to RemovedBlocks vector.
		blocks.RemovedBlocks[row].push_back(blockNum);

}
bool isBlockRemoved(vector<int> RemovedBlocks, int block) {
	//Search specific row from RemovedBlocks vector for block , return true if found. 
	if (find(RemovedBlocks.begin(), RemovedBlocks.end(), block) != RemovedBlocks.end()) {
		/* RemovedBlocks contains the block*/
		return true;
	}
	  /* RemovedBlocks doesn't contains the block*/
	   return false;


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
		if (speed.y < maxBallSpeedY) {
			speed.y = speed.y + (speed.y) * 0.05;


		}
	}
	//right
	else if (key == 102) {
		// move to the right if half of jumping pad untill half the pad is out of the screen.
		if (pad.x_Left < screen.screenRight - (pad.x_Right - pad.x_Left) / 2) {
			pad.x_Left = pad.x_Left + 1.0;
			pad.x_Right = pad.x_Right + 1.0;
		}
	}
	//down
	else if (key == 103) {
		
		
		if (speed.x > minSpeedX) {

			speed.x = speed.x - (speed.x) * 0.05;

		}
		if (speed.y > minSpeedY) {

			speed.y = speed.y - (speed.y) * 0.05;

		}


	}

	//Restart game.
	else if (key == 1) {

		initGame();
	}
	//F2 Fullscreen.
	else if (key == 2) {
		fullScreenMode = !fullScreenMode;
		if (fullScreenMode) {
			glutFullScreen();
		}
		else {
			glutReshapeWindow(800, 640);
			glutPositionWindow(GetSystemMetrics(SM_CXSCREEN) / 6,
				GetSystemMetrics(SM_CYSCREEN) / 6);
		}
	}

	//F3 Exit game.
	else if (key == 3) {
		exit(0);
	}



}
void mouseMotion(int x, int y) {


	if (x != 0) {
	   //get x game position respect to x pixel position.
	   float xPositionRatio = screen.widthInPixels / x;
	   float xWorldCoordinate = ((screen.screenRight - screen.screenLeft) / xPositionRatio) - screen.screenRight;

	   //update pad position to mouse position.
	   pad.x_Left = xWorldCoordinate - pad.middlePoint;
	   pad.x_Right = xWorldCoordinate + pad.middlePoint;
	}
	
}
void mouseClick(int button ,int state,int x, int y) {
	//Unpause the game.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (pause) {
			pause = false;
			onScreenText = "";
		}
		else {
			pause = true;
			onScreenText = "Right Click To Start";
		}
		
	}
}
void checkGameStatus() {
	
	if (lost||won) {

		onScreenText = "[F1] To Retry";
		pause = true;
	}
	
}