#define _CRT_SECURE_NO_WARNINGS

// Every _1_ second(s) we calculate fps
#define TIME_CHUNK 1

#include <ctime>

#include "stdlib.h"
#include "math.h"
#include <iostream>

#include <GL\glut.h>

#include "helper.h"
#include "solver.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT WINDOW_WIDTH
/// Main Program Variables ///

char *window_title;

float window_width = WINDOW_WIDTH;
float window_height = WINDOW_HEIGHT;

// Paused at start
int simPause = 1;
int drawCells = 1;

vec2f mousePos;

Solver* sphSolver;

/// FPS Counter Variables ///

std::clock_t last_time = clock();
unsigned int frames_rendered = 0;

void initialFormation(int whSize)
{
	int particleCtr = 0;

	for (int x = 0; x < whSize; x++)
		for (int y = 0; y < whSize; y++)
		{
			sphSolver->addParticle(vec2f(x*2.f * GRID_TRANSFORM, (y*2.f + GRID_WIDTH / 2.f) * GRID_TRANSFORM));
			particleCtr++;
		}

	for (int x = 0; x < whSize / 2; x++)
		for (int y = 0; y < whSize / 2; y++)
		{
			sphSolver->addParticle(vec2f((x*2.f + GRID_WIDTH / 4.f) * GRID_TRANSFORM, y*2.f * GRID_TRANSFORM));
			particleCtr++;
		}

	std::cout << "Number of Particles: " << particleCtr << std::endl;
}

void init()
{
	sphSolver = new Solver();
	sphSolver->initialize();

	initialFormation(20);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, sphSolver->worldSize_width, 0.0, sphSolver->worldSize_height);

	window_title = (char*)malloc(sizeof(char) * 50);

	glutIgnoreKeyRepeat(false);

	std::cout << std::endl << "\tKeybinds: " << std::endl;
	std::cout << "n	- Disable cell visualization" << std::endl;
	std::cout << "Space   - Resume/Pause" << std::endl;
	std::cout << "x	- Place wall particle" << std::endl;
	std::cout << "LMB	- Heavy particle" << std::endl;
	std::cout << "RMB	- Ring of particles" << std::endl;

	std::cout << "Simulation is paused - Press 'space' to unpause" << std::endl;
}

void render()
{
	if(drawCells)
		for (int x = 0; x < GRID_SIZE; x++)
		{
			Cell* c = sphSolver->grid[x];
			glColor3ub((c->velocity.unit().x + 1.f) / 2.f * 255.f, (c->velocity.unit().y + 1.f) / 2.f * 255.f, 144.f);

			glBegin(GL_POLYGON);
			glVertex2f(c->position.x * GRID_TRANSFORM, c->position.y * GRID_TRANSFORM);
			glVertex2f((c->position.x + 1) * GRID_TRANSFORM, c->position.y * GRID_TRANSFORM);
			glVertex2f((c->position.x + 1) * GRID_TRANSFORM, (c->position.y + 1) * GRID_TRANSFORM);
			glVertex2f(c->position.x * GRID_TRANSFORM, (c->position.y + 1) * GRID_TRANSFORM);
			glEnd();
		}

	// Set state values for particles
	glColor3f(0.372549f, 0.623529f, 0.623529f);
	glPointSize(10.f);

	for(int x = 0; x < sphSolver->currentParticles; x++)
	{
		// Draw
		glBegin(GL_POINTS);
		glVertex2d(sphSolver->particles[x]->position.x, sphSolver->particles[x]->position.y);
		glEnd();
	}	

}

void display()
{
	glViewport(0, 0, window_width, window_height);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0f, 0.0f, 1.0f);


	if(simPause == 0)
		sphSolver->update();


	render();

	glutSwapBuffers();

	
	if ((clock() - last_time) / CLOCKS_PER_SEC >= TIME_CHUNK)
	{
		memset(window_title, 0, 50);
		sprintf(window_title, "SPH Solver - Hunter Werenskjold - FPS: %d", frames_rendered / TIME_CHUNK);
		glutSetWindowTitle(window_title);

		frames_rendered = 0;
		last_time = clock();
	}
	frames_rendered++;
	
}

// Redisplays the window's normal plane
void idle()
{
	glutPostRedisplay();
}

// Prevents reshaping of the window
void reshape(int width, int height) {
	glutReshapeWindow(window_width, window_height);
}

vec2f lastWallParticle;
// Processes keyboard inputs
void process_keyboard(unsigned char key, int x, int y)
{
	if (key == ' ')
	{
		simPause = 1 - simPause;
	}

	if (key == 'x')
	{
		if (sqrtf(pow(mousePos.x - lastWallParticle.x, 2.f) + pow(mousePos.y - lastWallParticle.y, 2.f)) >= 30)
		{
			sphSolver->addWall(vec2f((mousePos.x / window_width) * sphSolver->worldSize_width, (((-1 * mousePos.y + window_height) / window_height)) * sphSolver->worldSize_height));
			lastWallParticle = mousePos;
		}
	}

	if (key == 'n')
	{
		drawCells = 1 - drawCells;
	}

}

// Processes passive mouse movements
void process_mouse_movement(int x, int y)
{
	mousePos.x = x;
	mousePos.y = y;
}

// Processes mouse inputs
void process_mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		sphSolver->addParticle(DEFAULT_MASS * 2 ,vec2f((mousePos.x / window_width) * sphSolver->worldSize_width, (((-1 * mousePos.y + window_height) / window_height)) * sphSolver->worldSize_height));
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		for (float i = 0; i < 360; i = i + 40)
		{
			sphSolver->addParticle(vec2f((mousePos.x / window_width) * sphSolver->worldSize_width + 0.02 * unitVecFromDeg(i).x, (((-1 * mousePos.y + window_height) / window_height)) * sphSolver->worldSize_height + 0.02 * unitVecFromDeg(i).y));
		}
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("SPH Solver - H Werenskjold - FPS: ");

	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(process_keyboard);
	glutPassiveMotionFunc(process_mouse_movement);
	glutMouseFunc(process_mouse);
	glutMainLoop();

	// Free memory below here

	return 0;
}
