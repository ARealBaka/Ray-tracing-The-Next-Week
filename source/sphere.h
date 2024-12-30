#ifndef SEPHERE_H
#define SEPHERE_H
#include"object.h"
#include"AABB.h"
class sphere:public object
{
public:

	sphere(){}
    //static sphere
    sphere(point3 center1, double r, std::shared_ptr<material>m) :radius(r), center(ray(center1, vec3(0, 0, 0))), mat(m), bounding_box(center1 + point3(-r, -r, -r), center1 + point3(r, r, r)){}
	sphere(point3 center1,point3 center2,double r, std::shared_ptr<material>m):center(ray(center1,center2-center1)),radius(r), mat(m) 
    {
        AABB from(center1 + point3(-r, -r, -r), center1 + point3(r, r, r));
        AABB to(center2 + point3(-r, -r, -r), center2 + point3(r, r, r));
        bounding_box = AABB(from, to);
    }

    //pos指的是在位于原点的单位圆上的坐标
    void get_sphere_uv(point3 pos, float& u, float& v)const
    {
        float theta = std::acos(-pos.y());
        float phi = std::atan2(-pos.z(), pos.x()) + pi;
        u = phi / (2 * pi);
        v = theta / pi;
        return;
    }


    bool intersect(const ray& r, double r_tmin, double r_tmax, intersection& rec)const override
	{
        vec3 oc = center.at(r.time()) - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;
        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;
        auto sqrtd = std::sqrt(discriminant);
        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (root <= r_tmin || r_tmax <= root) {
            root = (h + sqrtd) / a;
            if (root <= r_tmin || r_tmax <= root)
                return false;
        }
        rec.t = root;
        rec.p = r.at(rec.t);
        //设置法线方向始终指向与入射光线相反方向
		vec3 outward_normal = (rec.p - center.at(r.time())) / radius;
        if (dot(r.direction(), outward_normal) > 0)
        {
            rec.normal = -outward_normal;
            rec.normal_face_outward = false;
        } 
        else
        {
            rec.normal = outward_normal;
            rec.normal_face_outward = true;
        } 
        rec.mat = mat;
        get_sphere_uv(outward_normal, rec.u, rec.v);
        return true;
	}

    AABB bbox()const override{  return bounding_box; }
private:
	double radius;
	ray center;
    std::shared_ptr<material> mat;
    AABB bounding_box;
};



#endif
