#include"object.h"
#ifndef QUAD_H
#define QUAD_H

class quad :public object
{
public:
	quad(){}
	quad(const point3& q, const vec3& u, const vec3& v,shared_ptr<material>m):Q(q),U(u),V(v),mat(m)
	{
		AABB bbox1(q, q + u + v);
		AABB bbox2(q + u, q + v);
		bounding_box = AABB(bbox1, bbox2);
		//�ı��ο��ܳ��ְ�Χ��ĳ���߳���Ϊ0�������Ϊ�������������Ҫ���⴦��
		if (std::fabs(bounding_box.min[0] - bounding_box.max[0]) < 0.001)
		{
			bounding_box.min[0] -= 0.05;
			bounding_box.max[0] += 0.05;
		}
		if (std::fabs(bounding_box.min[1] - bounding_box.max[1]) < 0.001)
		{
			bounding_box.min[1] -= 0.05;
			bounding_box.max[1] += 0.05;
		}
		if (std::fabs(bounding_box.min[2] - bounding_box.max[2]) < 0.001)
		{
			bounding_box.min[2] -= 0.05;
			bounding_box.max[2] += 0.05;
		}

		//Ax+By+Cz = D 
		vec3 n = cross(U, V);
		normal = unit_vector(n);
		D = dot(normal, Q);
		w = n / dot(n, n);
	}
	AABB bbox()const override { return bounding_box; }

	bool intersect(const ray& r, double r_tmin, double r_tmax, intersection& rec)const override
	{

		//����ƽ�����ı���  ע����Ϊ�Ǹ�������Ӧ������Ե��ڵ��ж�
		if (std::fabs(dot(r.direction(), normal)) < 0.0001)
			return false;

		double t = (D - dot(normal, r.origin())) / dot(normal, r.direction());
		if (t > r_tmax || t < r_tmin)
			return false;
		point3 P = r.origin() + r.direction() * t; //ƽ���Ͻ���P
		//��U,V��Ϊ��������P�ڶ�ά����ƽ���ϵ����꣬������㹫ʽ�ο��̲�

		double alpha = dot(w, cross(P - Q, V));
		double beta = dot(w, cross(U, P - Q));

		if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1)
		{
			rec.mat = mat;
			//��֤���������������䷽�����
			if (dot(normal, r.direction()) > 0)
				rec.normal = -normal;
			else
				rec.normal = normal;
			rec.p = P;
			rec.t = t;
			rec.u = alpha;
			rec.v = beta;
			return true;
		}
		else
			return false;
	}
private:
	point3 Q;
	vec3 U, V;
	vec3 w; //�����жϵ�P�Ƿ����ı����ڲ�
	shared_ptr<material>mat;
	AABB bounding_box;
	vec3 normal;
	double D;//Ax+By+Cz = D,����DĬ�Ϸ�����(A,B,C)Ϊ��λ����
};

inline shared_ptr<objectlist> box(const point3& a, const point3& b, shared_ptr<material> mat)
{
	// Returns the 3D box (six sides) that contains the two opposite vertices a & b.
	auto sides = make_shared<objectlist>();
	// Construct the two opposite vertices with the minimum and maximum coordinates.
	auto min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
	auto max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));
	auto dx = vec3(max.x() - min.x(), 0, 0);
	auto dy = vec3(0, max.y() - min.y(), 0);
	auto dz = vec3(0, 0, max.z() - min.z());
	sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat)); // front
	sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
	sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat)); // left
	sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat)); // bottom
	return sides;
}

#endif // !QUAD_H
