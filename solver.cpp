#include "solver.h"

Solver::Solver()
{

}

Solver::~Solver()
{
	free(particles);
	free(grid);
}

void Solver::initialize()
{
	worldSize_width = 1.0f;
	worldSize_height = 1.0f;

	for (int x = 0; x < GRID_WIDTH; x++)
		for (int y = 0; y < GRID_WIDTH; y++)
			grid[GRID_XY(x, y)] = new Cell(vec2f(x, y));

	gravity.x = GRAVITY_X;
	gravity.y = GRAVITY_Y;
}

void Solver::update()
{
	calculateDensityPressure();
	
	calculateForces();
	
	integration();
}

void Solver::calculateDensityPressure()
{
	for (int i = 0; i < currentParticles; i++)
	{
		particles[i]->density = 0.0f;

		for (int j = 0; j < currentParticles; j++)
		{
			vec2f r = particles[i]->position - particles[j]->position;
			float r2 = r * r;

			if (r2 >= HSQ || r2<1e-12)
			{
				continue;
			}

			particles[i]->density += particles[j]->mass * POLY6 * pow(HSQ - r2, 3);
		}

		particles[i]->density += particles[i]->mass * SELF_DENSITY_WO_MASS;
		particles[i]->pressure = (pow(particles[i]->density / REST_DENSITY, 7) - 1) * GAS_CONSTANT;
	}
}

void Solver::calculateForces()
{
	for (int i = 0; i < currentParticles; i++)
	{
		particles[i]->acceleration = 0;

		if (particles[i]->pType == STATIC)
			continue;

		for (int j = 0; j < currentParticles; j++)
		{
			if (i == j)
				continue;

			vec2f r0 = particles[i]->position - particles[j]->position;
			float r2 = r0.x * r0.x + r0.y * r0.y;

			if (r2 < HSQ && r2 > 1e-12)
			{
				float r = sqrt(r2);

				float V = particles[j]->mass / particles[j]->density / 2;
				float Kr = KERNEL - r;
				float Kp = SPIKY_GRAD * Kr * Kr;

				float tempForce = V*(particles[i]->pressure + particles[j]->pressure) * Kp;
				particles[i]->acceleration -= r0 * tempForce / r;

				vec2f rV = particles[j]->evelocity - particles[i]->evelocity;

				float Kv = VISC_LAP * (KERNEL - r);
				tempForce = V * VISCOSITY * Kv;
				particles[i]->acceleration += rV * tempForce;
			}
		}
		particles[i]->acceleration = particles[i]->acceleration / particles[i]->density;
	}
}

void Solver::integration() 
{
	for (int i = 0; i < currentParticles; i++)
	{
		if (particles[i]->pType == STATIC)
			continue;

		particles[i]->velocity += particles[i]->acceleration * DT + gravity * DT;
		particles[i]->position += particles[i]->velocity * DT;

		if (particles[i]->position.x >= worldSize_width)
		{
			particles[i]->velocity.x = particles[i]->velocity.x * BOUND_DAMPINING;
			particles[i]->position.x = worldSize_width - BOUNDARY;
		}

		if (particles[i]->position.x < 0.f)
		{
			particles[i]->velocity.x = particles[i]->velocity.x * BOUND_DAMPINING;
			particles[i]->position.x = BOUNDARY;
		}

		if (particles[i]->position.y >= worldSize_height)
		{
			particles[i]->velocity.y = particles[i]->velocity.y * BOUND_DAMPINING;
			particles[i]->position.y = worldSize_height - BOUNDARY;
		}

		if (particles[i]->position.y < 0.f)
		{
			particles[i]->velocity.y = particles[i]->velocity.y * BOUND_DAMPINING;
			particles[i]->position.y = BOUNDARY;
		}

		Cell* c = getCell(particles[i]->position);
		c->velocity = (c->velocity + particles[i]->velocity) / 2.f;

		particles[i]->evelocity = (particles[i]->evelocity + particles[i]->velocity) / 2.f;
	}
}

void Solver::addParticle(vec2f pos, vec2f vel)
{
	if (currentParticles <= MAX_PARTICLES)
	{
		particles[currentParticles] = new Particle(pos, vel, currentParticles);
		particles[currentParticles]->mass = DEFAULT_MASS;
		currentParticles++;
	}
}

void Solver::addParticle(float mass, vec2f pos, vec2f vel)
{
	if (currentParticles <= MAX_PARTICLES)
	{
		particles[currentParticles] = new Particle(pos, vel, currentParticles);
		particles[currentParticles]->mass = mass;
		currentParticles++;
	}
}

void Solver::addWall(vec2f pos)
{
	if (currentParticles <= MAX_PARTICLES)
	{
		particles[currentParticles] = new Particle(pos, currentParticles);
		particles[currentParticles]->mass = DEFAULT_MASS * 10;
		currentParticles++;
	}
}

Cell* Solver::getCell(vec2f pos)
{
	return grid[GRID_XY((int)(pos.x*GRID_WIDTH), (int)(pos.y*GRID_WIDTH))];
}

