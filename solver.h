#pragma once

#include "helper.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_PARTICLES 10000

// Constants
#define KERNEL 0.06				// 0.04 (default)			
#define DEFAULT_MASS 0.05		// 0.01 (default)
#define REST_DENSITY 1000.0		// 1000.0 (default)
#define GAS_CONSTANT 1.0		// 1.0 (default)
#define HSQ KERNEL * KERNEL		
#define VISCOSITY 6.5			// 6.5 (default)
#define DT 0.003				// 0.003 (default)

#define GRAVITY_X 0.0			// I often use -9.8 here to corner the water and have to use fewer particles to see results
#define GRAVITY_Y -4.5			// -9.8 (default)

// Kernels
#define POLY6 315.0/(64.0 * PI * (KERNEL*KERNEL*KERNEL*KERNEL*KERNEL*KERNEL*KERNEL*KERNEL*KERNEL))
#define SPIKY_GRAD -45.0 / (PI*(KERNEL*KERNEL*KERNEL*KERNEL*KERNEL*KERNEL))
#define VISC_LAP 45.0 / (PI*(KERNEL*KERNEL*KERNEL*KERNEL*KERNEL*KERNEL))

// Simulation
#define BOUNDARY 0.0005			// 0.005 (default)
#define BOUND_DAMPINING -0.8	// -0.5 (default)

// Grid 
#define GRID_XY(X,Y) (Y*GRID_WIDTH) + X
#define GRID_WIDTH 100				
#define GRID_SIZE GRID_WIDTH * GRID_WIDTH 	// Number of cells
#define GRID_TRANSFORM 1.0/GRID_WIDTH		// This is a basis, so multiply by cell number to get the location of the cell

// Etc.
#define SELF_DENSITY_WO_MASS POLY6 * (KERNEL*KERNEL*KERNEL*KERNEL*KERNEL*KERNEL)

enum particleType
{
	DYNAMIC,
	STATIC
};

struct Particle
{
	// Use for dynamic particles
	Particle(vec2f position, vec2f velocity, int id)
	{
		identifier = id;
		
		pType = DYNAMIC;

		mass = DEFAULT_MASS;
		density = REST_DENSITY;
		pressure = 0.f;

		acceleration = 0.f;

		this->position = position;
		this->velocity = velocity;
	}

	// Use for static 
	Particle(vec2f position, int id)
	{
		identifier = id;

		pType = STATIC;

		mass = DEFAULT_MASS;
		density = REST_DENSITY;
		pressure = 0.f;

		acceleration = 0.f;

		this->position = position;
	}

	unsigned int identifier;

	// Dynamic or static
	particleType pType;
	
	float mass;
	float density;
	float pressure;

	vec2f position;
	vec2f velocity;
	vec2f evelocity;
	vec2f acceleration;
};


struct Cell
{
	Cell(vec2f position)
	{
		this->position = position;
		velocity = 0.f;
	}

	vec2f position;
	vec2f velocity;
};

class Solver
{
public:
	// Constructor & Deconstructor //
	Solver();
	~Solver();

	/// Methods ///
	// Procedures
	void initialize();
	void update();
	void addParticle(vec2f pos, vec2f vel = vec2f(0.0f, 0.0f));
	void addParticle(float mass, vec2f pos, vec2f vel = vec2f(0.0f, 0.0f));

	void addWall(vec2f pos);

	// Functions
	

	/// Variables ///
	Particle* particles[MAX_PARTICLES];
	Cell* grid[GRID_SIZE];
	
	float worldSize_width;
	float worldSize_height;

	int currentParticles = 0;

private:
	/// Methods ///
	// Procedures
	void integration();
	void calculateDensityPressure();
	void calculateForces();

	// Functions
	Cell* getCell(vec2f pos);

	/// Variables ///
	vec2f gravity;
	float timeStep;
	
};
