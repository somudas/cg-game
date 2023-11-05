#include <bits/stdc++.h>
#include <GL/glut.h>

using namespace std;

// constants
#define TWO_PI  6.28318530718 
#define PI 		3.14159265359 

// macros
#define begin(x) 	glBegin(x)
#define end() 		glEnd(); glFlush();
#define PLOT(x, y)  glVertex2f(x, y)
#define FPS 		50

// global vars
float windowHeight = 800;
float windowWidth  = 800;
float fillR, fillG, fillB;
bool fillBit;
float x = 200;
bool godMode = true;
float health = 100;
float ammo = 500;
float score = 0;
float maxY = 730;
bool endGame = false;


// obstacles
vector<pair<int, int>> bullets;
vector<pair<int, int>> obstacles;
vector<int> obs_health;
int seg = 250;
int freq = 8;
int gap = 80;

void output(int x, int y, float r, float g, float b, const char *string) {
  glColor3f( r, g, b );
  glRasterPos2f(x, y);
  int len, i;
  len = (int)strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
  }
}

void fillc(float r, float g, float b) { 
	fillBit = true; 
	fillR = r / 255.0;
	fillG = g / 255.0;
	fillB = b / 255.0;
}

void fillc(float x) {
	fillBit = true; 
	fillR = x / 255.0;
	fillB = fillG = fillR;
}
 
void noFill(void) { fillBit = false; }

void clear(void) {
	glClear(GL_COLOR_BUFFER_BIT);
}

void fillHeader(void) {
	if(fillBit) begin(GL_POLYGON);	
	else begin(GL_LINE_LOOP);	
	glColor3f(fillR, fillG, fillB);
}

void ellipse(float x, float y, float r1, float r2) {
	fillHeader();
	for(float a = 0; a<TWO_PI; a+=0.4) {
		float xx = x + r1 * cos(a);
		float yy = y + r2 * sin(a);
		glVertex2f(xx, yy);
	}
	end();
}

void rect(float x, float y, float w, float h) {
	fillHeader();
	PLOT(x, y);
	PLOT(x+w, y);
	PLOT(x+w, y-h);
	PLOT(x, y-h);
	end();
}

void display(void);

void init(void) {
	glPointSize(2.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, 0, windowHeight);
}

void timer_callback(int) {
	glutPostRedisplay();
    glutTimerFunc(1000/FPS,timer_callback,0);
}

float t;
void generate() {
	if(rand()%100 > 3) return;
	float x = rand() % int(windowWidth);
	float y = maxY - 40;
	obstacles.push_back({x, y});
	obs_health.push_back(100);
}

void update() {
	for(auto &[x, y]: obstacles) {
		y -= 5;
	}
	vector<int> idx;
	vector<pair<int, int>> tmp;
	set<int> rem;
	auto dist = [&](int x1, int y1, int x2, int y2) {
		return sqrt((x1 -x2) * (x1 -x2) + (y1 - y2) * (y1- y2));
	};
	for(int i=0;i<bullets.size();i++) rem.insert(i);
	for(int i=0;i<obstacles.size();i++) {
		
		auto [x, y] = obstacles[i];
		if(y < 0) {
			health -= 5;
		}else{
			for(int j=0;j<bullets.size();j++) {
				auto [px, py] = bullets[j];
				if(px >= x and px<=( x + 35) and py<=y and py>= (y - 55)){
					if(rem.count(j)) {
						rem.erase(j);
						obs_health[i]-=20;
						score += 20;
					}
				} 
			}
			if(obs_health[i] > 0){
				tmp.push_back({x, y});
				idx.push_back(obs_health[i]);
			}
		}
	}
	vector<pair<int, int>> nb;
	for(int i: rem) {
		nb.push_back(bullets[i]);
	}
	bullets = nb;
	obs_health = idx;
	obstacles = tmp;
}

void display_callback(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	display();
	glutSwapBuffers();
	generate();	
}

void shoot() {
	if(ammo==0) return;
	if(godMode){
		bullets.push_back({x + 5, 50});	
		bullets.push_back({x + 40, 50});	
	}
	bullets.push_back({x + 22, 70});
	ammo--;
}

void drawBullet(int x, int y) {
	float len = 10;
	float d = 2;
	glColor4f(1.0, 0.5, 0.0, 0.1);
	glBegin(GL_POLYGON);
	glVertex2f(x, y + len);
	glVertex2f(x + 6, y + len);
	glVertex2f(x + 6, y);
	glVertex2f(x, y);
	glEnd();

	glColor4f(1.0, 1.0, 0.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex2f(x + 2, y + len - d);
	glVertex2f(x + 4, y + len - d);
	glVertex2f(x + 4, y + d);
	glVertex2f(x + 2, y + d);
	glEnd();
}

void showBullets() {
	vector<pair<int, int>> nb;
	for(auto &[x, y]: bullets) {
		if(y < maxY-30){
			drawBullet(x, y);
			y += 8;
			nb.push_back({x, y});
		}
	}
	bullets = nb;
}

float angle;
bool isShoot=false;
void gun() {
	fillc(0, 0, 255);
	
	rect(x, 30, 50, 30);
	glColor3f(1.0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x, 30);
	glVertex2f(x, 0);
	glVertex2f(x-30, 0);
	glEnd();
	
	glColor3f(1.0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x+50, 30);
	glVertex2f(x+50, 0);
	glVertex2f(x+80, 0);
	glEnd();
	float dy = 0;
	if(isShoot) dy = 15;
	fillc(255,255,0);
	rect(x+22, 70-dy, 5, 50);
	rect(x+5, 50-dy, 5, 30);
	rect(x+40, 50-dy, 5, 30);
}

void showHealth() {
	float w = 4;
	float p = health / 100.0;
	fillc(255 * (1 - p), 255*p, 0);
	rect(20, 780, 400, 30);
	fillc(0);
	rect(20+w, 780-w, 400 - 2*w, 30 - 2*w);
	fillc(255 * (1 - p), 255*p, 0);
	rect(20+w, 780-w, (400 - 2*w) * p, 30 - 2*w);
}

void showAmmo() {
	float x = 500;
	fillc(255,255,0);
	rect(x+22, 790, 5, 50);
	rect(x+5, 770, 5, 30);
	rect(x+40, 770, 5, 30);
	const char* str = to_string(int(ammo)).c_str();
	output(x - 40, 740, 1.0, 1.0, 1.0, str);
}

void showScore() {
	float x = 750, y = 780;
	glColor3f(1.0, 1.0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x+20, y-20);
	glVertex2f(x, y-40);
	glVertex2f(x-20, y-20);
	glEnd();
	string s = to_string(int(score));
	output(x-30-10*s.size(), y-30, 1.0, 1.0, 1.0, to_string(int(score)).c_str());
}

void showStats() {
	showHealth();
	showAmmo();
	showScore();
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINES);
	glVertex2f(0, maxY);
	glVertex2f(windowWidth, maxY);	
	glEnd();
}

void enemy(float x, float y, float health) {
	glColor3f((1-health/100), 0.5, health);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x+30, y);
	int i=0;
	for(float xx = x+36; xx >= x; xx -= 6, i++) {
		if(i%2==0)
			glVertex2f(xx, y-50);
		else
			glVertex2f(xx, y-20);
	}
	glVertex2f(x, y-50);
	glEnd();
	fillc(0);
	ellipse(x+7, y-7, 5, 5);
	ellipse(x+23, y-7, 5, 5);
}

void showEnemy() {
	for(int i=0;i<obstacles.size(); i++) {
		auto [x, y] = obstacles[i];
		int h = obs_health[i];
		enemy(x, y, h);
	}
}

void scorePage() {
	fillc(0);
	rect(0, windowHeight, windowHeight, windowHeight);
	output(windowHeight/2-50, windowWidth/2, 1.0, 1.0, 1.0, "GAME OVER");
	output(windowHeight/2-30, windowWidth/2 - 50, 1.0, 1.0, 1.0, "your Score");
	output(windowHeight/2 - to_string(int(score)).size() * 10, windowWidth/2 - 100, 1.0, 1.0, 1.0, to_string(int(score)).c_str());
	glFlush();
}

void display(void) {
	// health = 0;
	if(health<=0) {
		endGame = true;
		scorePage();
		return;
	}
	fillc(0);
	rect(0, windowHeight, windowWidth, windowHeight); 
	fillc(0);
	gun();
	update();
	showBullets();
	showEnemy();
	showStats();
	isShoot = 0;
}


void keyPressed(int key, int px, int py) {
	if(key == GLUT_KEY_LEFT) {
		x = max((float)30.0, x - 12);
	}
	else if(key == GLUT_KEY_RIGHT) {
		x = min(windowWidth - (float)80, x + 12);
	}
}

void genericKey(unsigned char key, int px, int py) {
	if(key == ' ') {
		isShoot = 1;
		shoot();
	}
}

int main(int argc, char** argv) {
	// boilerplate code
	glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(350,40);
    glutCreateWindow("2d Shooter game");
    glutDisplayFunc(display_callback);
    glutTimerFunc(0,timer_callback,0);
    init();
	glutSpecialFunc(keyPressed);
	glutKeyboardFunc(genericKey);
    glutMainLoop();

	return 0;
}
	
