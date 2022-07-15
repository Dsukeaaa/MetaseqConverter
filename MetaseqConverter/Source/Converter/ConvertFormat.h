#ifndef _CONVERT_FORMAT_
#define _CONVERT_FORMAT_

#ifndef _OUTPUTRES_
#include "Res/OutputRes.h"
#endif

namespace MetaseqConverter
{
	struct Quaternion;
}

namespace MetaseqConverter
{

	void ConvertFormat( OutputRes& src );



	struct Mat33
	{
		Mat33()
		{
			m[0][0] = 0; m[0][1] = 0; m[0][2] = 0;
			m[1][0] = 0; m[1][1] = 0; m[1][2] = 0;
			m[2][0] = 0; m[2][1] = 0; m[2][2] = 0;
		}

		Mat33(const Quaternion& q);

		float m[3][3];


		Mat33 operator* (const Mat33& src) const
		{
			Mat33 dst;
			for (int row = 0; row < 3; ++row)
			{
				for (int col = 0; col < 3; ++col)
				{
					for (int k = 0; k < 3; ++k)
					{
						dst.m[row][col] += m[row][k] * src.m[k][col];
					}
				}
			}
			return dst;
		}


		static Mat33 RotX(float rad)
		{
			Mat33 dst;
			dst.m[0][0] = 1; dst.m[0][1] = 0; dst.m[0][2] = 0;
			dst.m[1][0] = 0; dst.m[1][1] = cosf(rad); dst.m[1][2] = -sinf(rad);
			dst.m[2][0] = 0; dst.m[2][1] = sinf(rad); dst.m[2][2] = cosf(rad);
			return dst;
		}

		static Mat33 RotY(float rad)
		{
			Mat33 dst;
			dst.m[0][0] = cosf(rad);  dst.m[0][1] = 0; dst.m[0][2] = sinf(rad);
			dst.m[1][0] = 0;		  dst.m[1][1] = 1; dst.m[1][2] = 0;
			dst.m[2][0] = -sinf(rad); dst.m[2][1] = 0; dst.m[2][2] = cosf(rad);
			return dst;
		}

		static Mat33 RotZ(float rad)
		{
			Mat33 dst;
			dst.m[0][0] = cosf(rad); dst.m[0][1] = -sinf(rad);  dst.m[0][2] = 0;
			dst.m[1][0] = sinf(rad); dst.m[1][1] = cosf(rad);   dst.m[1][2] = 0;
			dst.m[2][0] = 0;		 dst.m[2][1] = 0;			dst.m[2][2] = 1;
			return dst;
		}
	};

	//http://edom18.hateblo.jp/entry/2018/06/25/084023
	//http://www.mss.co.jp/technology/report/pdf/18-07.pdf
	//https://t-pot.com/program/35_quaternion/old/quaternion.html
	struct Quaternion
	{	
		float qx, qy, qz, qw;	

		Quaternion(){}
		Quaternion(const DS_VERTEX& v)
			: qx(v.x)
			, qy(v.y)
			, qz(v.z)
			, qw(v.w)
		{}
		Quaternion(const Mat33& mat);

		Quaternion operator*(const Quaternion& r) const
		{
			Quaternion ret;
			ret.qx = (qx*r.qx - qy*r.qy - qz*r.qz - qw*r.qw);
			ret.qy = (qx*r.qy + qy*r.qx + qz*r.qw - qw*r.qz);
			ret.qz = (qx*r.qz - qy*r.qw + qz*r.qx + qw*r.qy);
			ret.qw = (qx*r.qw + qy*r.qz - qz*r.qy + qw*r.qx);
			return ret;
		}

		static Quaternion Inverse(const Quaternion& in)
		{
			Quaternion ret;
			const float n = in.qx*in.qx + in.qy*in.qy + in.qz*in.qz + in.qw*in.qw;
			ret.qx = in.qx / n;
			ret.qy = -in.qy / n;
			ret.qz = -in.qz / n;
			ret.qw = -in.qw / n;
			return ret;
		}

	private:
		void _Conv1(const Mat33& mat);
		void _Conv2(const Mat33& mat);

		inline float SIGN(float x) { return (x >= 0.0f) ? +1.0f : -1.0f; }
		inline float NORM(float a, float b, float c, float d) { return sqrt(a * a + b * b + c * c + d * d); }
	};


}

#endif