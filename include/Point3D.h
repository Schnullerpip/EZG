#pragma once
struct Point3D
{
	float x, y, z;
	Point3D(float x, float y, float z) :x(x), y(y), z(z) {}

	Point3D operator+(Point3D& p) const
	{
		return Point3D(x + p.x, y + p.y, z + p.z);
	}

	Point3D& operator+=(Point3D& p)
	{
		x += p.x;
		y += p.y;
		z += p.z;
		return *this;
	}

	Point3D operator*(float f) const
	{
		return Point3D(x*f, y*f, z*f);
	}

	Point3D& operator*=(Point3D p) {
		x*p.x;
		y*p.y;
		z*p.z;
		return *this;
	}

	Point3D& operator*=(float f) {
		x*=f;
		y*=f;
		z*=f;
		return *this;
	}

	~Point3D() {}
};

