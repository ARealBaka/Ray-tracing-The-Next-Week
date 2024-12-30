#ifndef RAY_H
#define RAY_H
#include"vec3.h"
class ray
{

public:
	ray(){}
	ray(const point3& origin,const vec3& direction,double time):ori(origin),dir(direction), tm(time){}
	ray(const point3& origin, const vec3& direction):ori(origin), dir(direction), tm(0){}
	const point3& origin() const{ return ori; }
	const vec3& direction() const{ return dir; }
	const double& time() const { return tm; }

	point3 at(double t) const
	{
		return ori + dir * t;
	}


private:
	point3 ori;
	vec3 dir;
	//time range from 0 to 1
	double tm;
};

#endif // !RAY_H

