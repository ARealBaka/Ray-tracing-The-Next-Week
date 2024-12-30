#ifndef MAT_H
#define MAT_H

#include"object.h"
#include"texture.h"
//material�������£����ݲ�ͬ�Ĳ��ʸ�������ǣ����ݲ�ͬ�Ĳ��ʸ��������ʡ�
class material
{
public:
	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const intersection& rec, color& attenuation, ray& scattered)
		const { return false; }
	//ֻ�й�Դ��Ҫ��дemitted���������Բ�Ӧ������Ϊ���麯��������Ĭ��Ӧ���أ�0��0��0��
	virtual color emitted(double u, double v, point3 P)const { return color(0, 0, 0); }
};
//��������ߣ�ֻ�������
class diffuse_light :public material
{
public:
	diffuse_light(){}
	diffuse_light(shared_ptr<Texture>tex):texture(tex){}
	diffuse_light(const color&c):texture(make_shared<solid_color>(c)){}
	bool scatter(const ray& r_in, const intersection& rec, color& attenuation, ray& scattered) const override { return false; }
	color emitted(double u, double v, point3 P)const override { return texture->get_color(u, v, P); }

private:
	shared_ptr<Texture>texture;
};


//�����õ���������lambertian��Ҳ����˵���߾��ȵ������䵽�������򣬲��ҿ�������ǽǶȶ��������յ�Ӱ��
//ע�ⷨ�߷�����Զ�����䷽�����
//��������ǽǶȶ�����Ӱ��ķ�����GAMES101�в�ͬ����ͨ��һ�������λ�����뷨���������ʵ�ֲ�ͬ�Ƕ������Ĳ�ͬ
//�ܵ����������߷����������������ǿ���뷨�߷���ˮƽ�ķ�������������
class diffuse :public material
{
public:
	diffuse(){}
	diffuse(shared_ptr<Texture> tx):texture(tx){}
	diffuse(const color&c):texture(make_shared<solid_color>(c)){}
	bool scatter(const ray& r_in, const intersection& rec, color& attenuation, ray& scattered)const override
	{
		auto scatter_direction = rec.normal + random_unit_vector();
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;
		scattered = ray(rec.p, scatter_direction, r_in.time());
		attenuation = texture->get_color(rec.u, rec.v, rec.p);
		return true;
	}
private:
	shared_ptr<Texture>texture;
};

//�������ʿ��Է�Ϊ���Թ⻬�Ĳ��ʺ�fuzzy���ʣ�ȡ���ڷ�����Ǵ��ڹ̶�������һ��������
//������lambertian diffuseʵ�ֵķ�����ͨ��һ���������ʵ��fuzzy����
class metal :public material
{
public:
	metal(){}
	metal(const color& al,const double& fz) :albedo(al),fuzz(fz) 
	{	
		fuzz = (fuzz < 1) ? fuzz : 1;
	}

	bool scatter(const ray& r_in, const intersection& rec, color& attenuation, ray& scattered)const override
	{
		auto reflected_direction = reflect(r_in.direction(),rec.normal);
		reflected_direction = unit_vector(reflected_direction) + fuzz * random_unit_vector();
		reflected_direction = unit_vector(reflected_direction);
		scattered = ray(rec.p, reflected_direction, r_in.time());
		attenuation = albedo;
		if (dot(reflected_direction, rec.normal) > 0)
			return true;
		else 
			return false;
	}
private:
	color albedo;
	double fuzz;
};
class dielectric :public material
{
public:
	dielectric(double refraction_idx) :refraction_index(refraction_idx) {}

	bool scatter(const ray& r_in, const intersection& rec, color& attenuation, ray& scattered)const override
	{
		attenuation = color(1.0, 1.0, 1.0);
		point3 ori = rec.p;
		//�������������뻹�����������Ӧ����ϵ������
		double cos = dot(-r_in.direction(), rec.normal);
		double sin = std::sqrt(1 - cos * cos);
		double idx = rec.normal_face_outward ? (1 / refraction_index) : refraction_index;
		vec3 dir;
		double random_probility = random_double();
		if (idx * sin > 1|| reflectance(cos, idx)> random_probility)
			dir = reflect(r_in.direction(), rec.normal);
		else
			dir = refract(r_in.direction(), rec.normal, idx);
		scattered = ray(ori, dir, r_in.time());
		return true;
	}

private:
	double refraction_index;
	//Fresnel Equations�����ֵ�Ƿ�����
	static double reflectance(double cosine, double refraction_index) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};

//�����ⷽ����
class isotropic :public material
{
public:
	isotropic(shared_ptr<Texture>tex):texture(tex){}
	isotropic(const color& c) :texture(make_shared<solid_color>(c)) {}
	bool scatter(const ray& r_in, const intersection& rec, color& attenuation, ray& scattered)const override
	{
		scattered = ray(rec.p, random_unit_vector(), r_in.time());
		//��״������ײ����û������������ģ��������uv������0
		attenuation = texture->get_color(0, 0, rec.p);
		return true;
	}
private:
	shared_ptr<Texture>texture;

};
#endif // !MAT_H
