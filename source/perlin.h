#ifndef PERLIN_H
#define PERLIN_H

#include"texture.h"

class perlin
{
public:
	perlin()
	{
		for (int i = 0; i < point_count; i++)
			randomvec[i] = vec3(random_double(-1, 1), random_double(-1, 1), random_double(-1, 1));
		perlin_generate_perm(perm_x);
		perlin_generate_perm(perm_y);
		perlin_generate_perm(perm_z);
	}

	double noise(const point3& p)const
	{
		//注意float到int的强制转换是向0取整，即截掉小数部分，这里需要的是向下取整
		double u = p.x() - std::floor(p.x());
		double v = p.y() - std::floor(p.y());
		double w = p.z() - std::floor(p.z());

		u = u * u * (3 - 2 * u);
		v = v * v * (3 - 2 * v);
		w = w * w * (3 - 2 * w);

		//坐标整数部分
		int i = std::floor(p.x());
		int j = std::floor(p.y());
		int k = std::floor(p.z());

		//求出坐标所在正方形八个点的噪声值
		vec3 pointset[2][2][2];
		for(int l=0;l<2;l++)
			for(int m = 0;m<2;m++)
				for (int n=0;n<2;n++)
				{
					int tempx = ((int)((i+l) * 4)) & (point_count - 1);
					int tempy = ((int)((j + m) * 4)) & (point_count - 1);
					int tempz = ((int)((k + n) * 4)) & (point_count - 1);
					pointset[l][m][n] = randomvec[perm_x[tempx] ^ perm_y[tempy] ^ perm_z[tempz]];
				}
		return interpolate(pointset, u, v, w);
	}
	double turbulence(const point3& p, int depth)const
	{
		double sum = 0;
		point3 temp = p;
		double weight = 1;
		for (int i = 0; i < depth; i++)
		{
			sum += noise(temp) * weight;
			weight *= 0.5;
			temp *= 2;
		}
		return std::fabs(sum);
	}

private:
	static const int point_count = 256;
	vec3 randomvec[point_count];
	int perm_x[point_count];
	int perm_y[point_count];
	int perm_z[point_count];
	
	static void perlin_generate_perm(int *array)
	{
		for (int i = 0; i < point_count; i++)
			array[i] = i;
		permute(array, point_count-1);
	}
	static void permute(int* array, int n)
	{
		for (int i = n; i > 0; i--)
		{
			int temp = random_int(0, n-1);
			std::swap(array[temp], array[i]);
		}
	}
	double interpolate(vec3 array[2][2][2], double u, double v, double w)const
	{
		double sum = 0;
		for(int i=0;i<2;i++)
			for(int j=0;j<2;j++)
				for (int k = 0; k < 2; k++)
				{
					vec3 weight_v(u - i, v - j, w - k);
					sum += (i * u + (1 - i) * (1 - u))
						* (j * v + (1 - j) * (1 - v))
						* (k * w + (1 - k) * (1 - w))
						* dot(array[i][j][k], weight_v);

				}
		return sum;
	}
};



#endif // !PERLIN_H
