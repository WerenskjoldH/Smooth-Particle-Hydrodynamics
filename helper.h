#pragma once

#include <math.h>

#define PI 3.14159265359

struct vec2f
{
	float x;
	float y;

	vec2f(float x = 0, float y = 0) : x(x), y(y) { }
	
	vec2f& operator=(const vec2f& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	vec2f operator=(const float f)
	{
		x = f;
		y = f;
		return vec2f(x, y);
	}

	vec2f operator+(const vec2f& v)
	{
		return vec2f(v.x + x, v.y + y);
	}

	vec2f& operator+=(const vec2f& v)
	{
		this->x += v.x;
		this->y += v.y;
		return *this;
	}

	vec2f& operator-=(const vec2f& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		return *this;
	}

	vec2f operator-(const vec2f& v)
	{
		return vec2f(x - v.x, y - v.y);
	}

	vec2f operator*(const float f)
	{
		return vec2f(f * x, f * y);
	}

	float operator*(vec2f v)
	{
		return v.x * x + v.y * y;
	}

	vec2f operator/(float f)
	{
		return vec2f(x / f, y / f);
	}

	vec2f unit()
	{
		return vec2f(x, y) * (1 / (sqrt((double)(x*x + y*y))));
	}

	float mag()
	{
		return (float)sqrt((double)(x*x + y*y));
	}
};

static vec2f unitVecFromDeg(float deg)
{
	vec2f v;
	v.x = cos(deg * (PI / 180.0));
	v.y = sin(deg * (PI / 180.0));
	return v;
}