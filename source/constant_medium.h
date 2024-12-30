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
		//-infinity, infinity��ԭ�����ڣ�ͨ��boundaryȷ����״����ķ�Χ�������ٸ���r_tmin,r_tmax��������ײ������
		if(!boundary->intersect(r,-infinity, infinity,rec1))
			return false;
		//���ǵ����������һ��ƽ������,ͬʱȷ���ڶ�������
		if (!boundary->intersect(r, rec1.t + 0.0001, infinity, rec2))
			return false;
		/*
		ͨ��r_tmin,r_tmax��������ײ�ķ�Χ����ʱ�������������һ��(r_tmin,r_tmax)��(rec1.t,rec2.t)�й������򣬷�Χ�õ�����
		�ڶ���û�й��������򲻷�����ײ��Ӧ�ų�����û�й�������ʱ��������ʹrec1.t>rec2.t
		*/
		if (rec1.t < r_tmin)
			rec1.t = r_tmin;
		if (rec2.t > r_tmax)
			rec2.t = r_tmax;
		//��һ�������ᵼ��rec1.t>rec2.t��������ĳ��֣���ζ��û�з�����ײ
		if (rec1.t > rec2.t)
			return false;
		//���ǵ����߷�����������ڲ�
		if (rec1.t < 0)
			rec1.t = 0;

		//Ĭ�����߷���Ϊ��λ����
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

