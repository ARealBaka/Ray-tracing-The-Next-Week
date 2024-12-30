#ifndef AABB_H
#define AABB_H
#include"interval.h"
#include"vec3.h"
#include"global.h"
#include"ray.h"
class AABB
{
public:
	AABB():min(point3(infinity, infinity, infinity)),max(point3(-infinity, -infinity,-infinity)) {}
	//两点确定一个包围盒，但要注意两点确定的包围盒未必能完整包围物体，比如物体是一个平行四边形，具体物体的包围盒实现还是要具体问题具体分析
	AABB(const point3& A, const point3& B)
	{
		min = point3(std::fmin(A[0], B[0]), std::fmin(A[1], B[1]), std::fmin(A[2], B[2]));
		max = point3(std::fmax(A[0], B[0]), std::fmax(A[1], B[1]), std::fmax(A[2], B[2]));
	}	
	AABB(interval x, interval y,interval z):min(point3(x.min,y.min,z.min)),max(x.max,y.max,z.max){}
	AABB(AABB x,AABB y)
	{
		min = point3(std::fmin(x.min.x(), y.min.x()), std::fmin(x.min.y(), y.min.y()), std::fmin(x.min.z(), y.min.z()));
		max = point3(std::fmax(x.max.x(), y.max.x()), std::fmax(x.max.y(), y.max.y()), std::fmax(x.max.z(), y.max.z()));
	}
	bool is_hitted(const ray& r)const
	{
		vec3 dir_inv(1 / r.direction().x(), 1 / r.direction().y(), 1 / r.direction().z());
		vec3 t1 = (min - r.origin()) * dir_inv;
		vec3 t2 = (max - r.origin()) * dir_inv;
		interval x, y, z;
		x = interval(std::fmin(t1.x(), t2.x()), std::fmax(t1.x(), t2.x()));
		y = interval(std::fmin(t1.y(), t2.y()), std::fmax(t1.y(), t2.y()));
		z = interval(std::fmin(t1.z(), t2.z()), std::fmax(t1.z(), t2.z()));
		double t_min = std::fmax(std::fmax(x.min, y.min), z.min);
		double t_max = std::fmin(std::fmin(x.max, y.max), z.max);
		return t_min < t_max;
	}
	point3 min, max;

};


#endif
