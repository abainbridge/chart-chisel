#ifndef INCLUDED_VECTOR2_H
#define INCLUDED_VECTOR2_H


class Vector2
{
public:
	double x, y;

	Vector2()										{ x = y = 0.0; }
	Vector2(double _x, double _y)					{ x=_x; y=_y; }

	void Set(double _x, double _y)					{ x=_x; y=_y; }

	Vector2 operator + (Vector2 const &b) const;
	Vector2 operator - (Vector2 const &b) const;
	Vector2 operator * (double const b) const;

	void	operator = (Vector2 const &b)			{ x=b.x; y=b.y; }
	void	operator *= (double const b)			{ x*=b; y*=b; }
	void	operator += (Vector2 const &b)			{ x+=b.x; y+=b.y; }
	void	operator -= (Vector2 const &b)			{ x-=b.x; y-=b.y; }

	Vector2 const &Normalize();
	void	SetLen(double len);
	
    double	Len() const;
	double	LenSquared() const						{ return x*x + y*y; }
};


// Operator * between double and Vector2
inline Vector2 operator * (double scale, Vector2 const &v)
{
	return v * scale;
}


#endif
