#ifndef  CONSTANT_MEDIUM
#define CONSTANT_MEDIUM

#include"object.h"

class constant_medium :public object
{
public:
	constant_medium(shared_ptr<object>obj,double dense_index,shared_ptr<Texture>tex):boundary(obj), neg_inv_dense(-1/ dense_index),mat(make_shared<isotropic>(tex)){}
	constant_medium(shared_ptr<object>obj, double dense_index, const color&c) :boundary(obj), neg_inv_dense(-1 / dense_index), mat(make_shared<isotropic>(c)) {}

	bool intersect(const ray& r, double r_tmin, double r_tmax, intersection& rec)const override
	{
		intersection rec1, rec2;
		//-infinity, infinity的原因在于：通过boundary确定雾状区域的范围，后面再根据r_tmin,r_tmax收缩可碰撞的区域
		if(!boundary->intersect(r,-infinity, infinity,rec1))
			return false;
		//考虑到物体可能是一个平面的情况,同时确定第二个交点
		if (!boundary->intersect(r, rec1.t + 0.0001, infinity, rec2))
			return false;
		/*
		通过r_tmin,r_tmax收缩可碰撞的范围，此时有两种情况，第一种(r_tmin,r_tmax)和(rec1.t,rec2.t)有公共区域，范围得到收缩
		第二种没有公共区域，则不发生碰撞，应排除，当没有公共区域时本操作会使rec1.t>rec2.t
		*/
		if (rec1.t < r_tmin)
			rec1.t = r_tmin;
		if (rec2.t > r_tmax)
			rec2.t = r_tmax;
		//上一步操作会导致rec1.t>rec2.t这种情况的出现，意味着没有发生碰撞
		if (rec1.t > rec2.t)
			return false;
		//考虑到射线发射点在物体内部
		if (rec1.t < 0)
			rec1.t = 0;

		//默认射线方向为单位向量
		auto boundary_distance = (rec2.t - rec1.t);
		auto hit_distance = neg_inv_dense * std::log(random_double());
		if (hit_distance > boundary_distance)
			return false;
		
		rec.t = rec1.t + hit_distance;
		rec.p = r.at(rec.t);

		rec.mat = mat;

		return true;
	}

	AABB bbox()const override { return boundary->bbox(); }

private:
	shared_ptr<object>boundary;
	double neg_inv_dense;
	shared_ptr<material>mat;
};


#endif // ! CONSTANT_MEDIUM

