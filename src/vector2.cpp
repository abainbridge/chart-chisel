#include "vector2.h"

#include <math.h>


Vector2 Vector2::operator + (Vector2 const &b) const
{
	return Vector2(x + b.x, y + b.y);
}


Vector2 Vector2::operator - (Vector2 const &b) const
{
	return Vector2(x - b.x, y - b.y);
}


Vector2 Vector2::operator * (double const b) const
{
	return Vector2(x * b, y * b);
}


Vector2 const &Vector2::Normalize()
{
	double lenSqrd = x*x + y*y;
	if (lenSqrd > 0.0f)
	{
		double invLen = 1.0 / sqrt(lenSqrd);
		x *= invLen;
		y *= invLen;
	}
	else
	{
		x = 0.0f;
        y = 1.0f;
	}

	return *this;
}


void Vector2::SetLen(double len)
{
	double scaler = len / Len();
	x *= scaler;
	y *= scaler;
}


double Vector2::Len() const
{
    return sqrt(x * x + y * y);
}
