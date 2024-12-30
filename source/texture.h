#ifndef TEXTURE_H
#define TEXTURE_H
#include"color.h"
#include"rtw_stb_image.h"
#include"perlin.h"
class Texture
{
public:
	Texture(){}
	virtual ~Texture() = default;
	virtual color get_color(float u, float v,const point3& P) const = 0;
};

class solid_color :public Texture
{
public:
	solid_color(){}
	solid_color(const color& c) :albedo(c) {}
	solid_color(double r,double g,double b):albedo(color(r,g,b)){}
	color get_color(float u, float v, const point3& P)const override
	{
		return albedo;
	}
private:
	color albedo;
};

class Texture_color :public Texture
{
public:
	Texture_color(const char* image_filename):image(image_filename){}
	color get_color(float u, float v, const point3& P) const override
	{
		// If we have no texture data, then return solid cyan as a debugging aid.
		if (image.height() <= 0)
		{
			return color(0, 1, 1);
		}
		v = 1 - v;
		int x = (int)(u * image.width());
		int y = (int)(v * image.height());
		auto colors = image.pixel_data(x, y);
		return color(colors[0] / 255.0, colors[1] / 255.0, colors[2] / 255.0);
	}
private:
	rtw_image image;
};


class noise_texture :public Texture
{
public:
	noise_texture(double scale):scale(scale){}
	color get_color(float u, float v, const point3& P) const override
	{
		return color(.5, .5, .5) * (1 + std::sin(scale * P.z() + 10 * noise.turbulence(P, 7)));
	}
private:
	double scale;
	perlin noise;
};

#endif // !TEXTURE_H

