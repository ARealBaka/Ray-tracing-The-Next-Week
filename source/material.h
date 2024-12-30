#ifndef MAT_H
#define MAT_H

#include"object.h"
#include"texture.h"
//material做两件事：根据不同的材质给出出射角，根据不同的材质给出吸收率。
class material
{
public:
	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const intersection& rec, color& attenuation, ray& scattered)
		const { return false; }
	//只有光源需要重写emitted函数，所以不应该设置为纯虚函数，并且默认应返回（0，0，0）
	virtual color emitted(double u, double v, point3 P)const { return color(0, 0, 0); }
};
//不反射光线，只发射光线
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


//这里用的漫反射是lambertian，也就是说光线均匀的漫反射到各个方向，并且考虑入射角角度对能量吸收的影响
//注意法线方向永远与入射方向相对
//这里入射角角度对能量影响的方法与GAMES101中不同，是通过一个随机单位向量与法向量相加来实现不同角度能量的不同
//总的来讲，法线方向漫反射的能量更强，与法线方向水平的方向能量更弱。
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

//金属材质可以分为绝对光滑的材质和fuzzy材质，取决于反射光是打在固定方向还是一个区域内
//类似于lambertian diffuse实现的方法，通过一个随机向量实现fuzzy材质
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
		//根据物体是射入还是射出设置相应反射系数比例
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
	//Fresnel Equations求出的值是反射率
	static double reflectance(double cosine, double refraction_index) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};

//向任意方向反射
class isotropic :public material
{
public:
	isotropic(shared_ptr<Texture>tex):texture(tex){}
	isotropic(const color& c) :texture(make_shared<solid_color>(c)) {}
	bool scatter(const ray& r_in, const intersection& rec, color& attenuation, ray& scattered)const override
	{
		scattered = ray(rec.p, random_unit_vector(), r_in.time());
		//雾状物体碰撞点是没有求纹理坐标的，因此这里uv随便填的0
		attenuation = texture->get_color(0, 0, rec.p);
		return true;
	}
private:
	shared_ptr<Texture>texture;

};
#endif // !MAT_H
