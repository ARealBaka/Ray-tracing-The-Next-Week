#ifndef OBJ_H
#define OBJ_H

#include"AABB.h"
//在这里声明一个material类很关键，使得std::shared_ptr<material>mat;不会报错
class material;

class intersection
{
public:
	point3 p;
	vec3 normal;
	bool normal_face_outward;
	double t;
	float u, v;
	std::shared_ptr<material>mat;
}; 

class object
{
public:
	material* mat;
	virtual ~object() = default;

	virtual bool intersect(const ray& r, double r_tmin, double r_tmax, intersection& rec) const = 0;

	virtual AABB bbox()const = 0;
};
//实现不改变物体具体坐标的条件下移动物体（translate只包含平移）
class translate :public object
{
public:
	translate(){}
	translate(shared_ptr<object>ptr,const vec3&off):obj(ptr),offset(off)
	{
		bounding_box = AABB(obj->bbox().min + offset, obj->bbox().max + offset);
	}

	bool intersect(const ray& r, double r_tmin, double r_tmax, intersection& rec) const override
	{
		//注意射线的时间也要保留好
		ray ray_backward_offset = ray(r.origin() - offset, r.direction(),r.time());
		if (!obj->intersect(ray_backward_offset, r_tmin, r_tmax, rec))
			return false;
		rec.p += offset;
		return true;
	}
	AABB bbox()const override { return bounding_box; }

private:
	shared_ptr<object>obj;
	vec3 offset;
	AABB bounding_box;
};

class rotate_y :public object
{
public:
	rotate_y(){}
	rotate_y(shared_ptr<object>ptr, double agl):angle(agl),obj(ptr)
	{
		auto radians = degrees_to_radians(angle);
		cos_theta = std::cos(radians);
		sin_theta = std::sin(radians);
		//旋转后包围盒也要更新
		bounding_box = ptr->bbox();
		point3 min(infinity, infinity, infinity), max(-infinity, -infinity, -infinity);
		//i -> x   j -> y   k-> z
		for(int i=0;i<2;i++)
			for(int j=0;j<2;j++)
				for (int k = 0; k < 2; k++)
				{
					auto x = i * bounding_box.min[0] + (1 - i) * bounding_box.max[0];
					auto y = j * bounding_box.min[1] + (1 - j) * bounding_box.max[1];
					auto z = k * bounding_box.min[2] + (1 - k) * bounding_box.max[2];
					auto rotated_x = cos_theta * x + sin_theta * z;
					auto rotated_z = cos_theta * z - sin_theta * x;
					point3 rotated_coor(rotated_x, y, rotated_z);
					for (int l = 0; l < 3; l++)
					{
						min[l] = std::fmin(min[l], rotated_coor[l]);
						max[l] = std::fmax(max[l], rotated_coor[l]);
					}
				}
		bounding_box = AABB(min, max);
	}
	bool intersect(const ray& r, double r_tmin, double r_tmax, intersection& rec) const override
	{
		//旋转射线
		auto rotated_origin = point3(cos_theta * r.origin().x() - sin_theta * r.origin().z(), r.origin().y(), cos_theta * r.origin().z() + sin_theta * r.origin().x());
		auto rotated_dir = vec3(cos_theta * r.direction().x() - sin_theta * r.direction().z(), r.direction().y(), cos_theta * r.direction().z() + sin_theta * r.direction().x());
		rotated_dir = unit_vector(rotated_dir);
		//注意保留射线时间
		ray rotated_ray = ray(rotated_origin, rotated_dir,r.time());
		if (!obj->intersect(rotated_ray, r_tmin, r_tmax, rec))
			return false;
		//复原碰撞点和normal
		rec.p = point3(cos_theta * rec.p.x() + sin_theta * rec.p.z(), rec.p.y(), cos_theta * rec.p.z() - sin_theta * rec.p.x());
		rec.normal = vec3(cos_theta * rec.normal.x() + sin_theta * rec.normal.z(), rec.normal.y(), cos_theta * rec.normal.z() - sin_theta * rec.normal.x());
		rec.normal = unit_vector(rec.normal);
		return true;
	}

	AABB bbox()const override { return bounding_box; }
private:
	double angle;
	shared_ptr<object>obj;
	AABB bounding_box;
	double sin_theta;
	double cos_theta;
};

#endif // !OBJ_H

