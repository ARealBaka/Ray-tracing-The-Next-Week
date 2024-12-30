#ifndef BVH_H
#define BVH_H
#include"AABB.h"
#include"object.h"
#include"objectlist.h"
#include"global.h"
#include<algorithm>
class bvh_node:public object
{
public:
	bvh_node(std::vector<shared_ptr<object>>& objs, unsigned start, unsigned end) //[start,end)
	{
		unsigned num = end - start;
		if (num == 1)
		{
			lchild = objs[start];
			rchild = objs[start];
		}
		else if (num == 2)
		{
			lchild = objs[start];
			rchild = objs[start+1];
		}
		else
		{
			unsigned axis_idx = random_int(0, 2);
			auto comparator = (axis_idx == 0) ? sort_x : (axis_idx == 1) ? sort_y : sort_z;
			std::sort(std::begin(objs) + start, std::begin(objs) + end, comparator);
			unsigned mid = start + num / 2;
			lchild = make_shared<bvh_node>(objs, start, mid);
			rchild = make_shared<bvh_node>(objs, mid, end);
		}
		bounding_box = AABB(lchild->bbox(), rchild->bbox());
	}
	bvh_node(objectlist objs): bvh_node(objs.objects, 0, objs.objects.size()){} //在参数初始化这里调用构造函数，很神奇
	bool intersect(const ray& r, double r_tmin, double r_tmax, intersection& rec)const override
	{
		//未命中则直接结束
		if (!bounding_box.is_hitted(r))
			return false;
		bool hit_left = lchild->intersect(r, r_tmin, r_tmax, rec);
		bool hit_right = rchild->intersect(r, r_tmin, hit_left ? rec.t : r_tmax, rec);
		return hit_left || hit_right;
	}
	AABB bbox()const override { return bounding_box; }

	static bool sort_x(const shared_ptr<object>a, const shared_ptr<object>b) { return a->bbox().min[0] < b->bbox().min[0]; }
	static bool sort_y(const shared_ptr<object>a, const shared_ptr<object>b) { return a->bbox().min[1] < b->bbox().min[1]; }
	static bool sort_z(const shared_ptr<object>a, const shared_ptr<object>b) { return a->bbox().min[2] < b->bbox().min[2]; }



private:
	AABB bounding_box;
	shared_ptr<object>lchild, rchild;
};

#endif

