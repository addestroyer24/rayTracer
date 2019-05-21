#ifndef _MATRIX_H
#define _MATRIX_H

#include <math.h>
#include <string>
#include <sstream>

namespace Mat
{
	template<unsigned int height, typename T = float>
	class Vector
	{
	private:
		T vals[height];

	public:
		template<typename T2 = T, typename std::enable_if<std::is_convertible<T2, T>::value, int>::type = 1>
		explicit Vector(T2 initVal = 0);
		template<typename T2 = T, typename std::enable_if<std::is_convertible<T2, T>::value, int>::type = 1>
		explicit Vector(T2 values[]);
		Vector(const Vector &vec);

		inline T& operator[](int index);
		inline const T operator[](int index) const;

		std::string toString() const;

		template<unsigned int newHeight, typename T2, typename std::enable_if<newHeight <= height, int>::type = 1>
		explicit operator Vector<newHeight, T2>();
	};



	template<unsigned int height, typename T>
	template<typename T2, typename std::enable_if<std::is_convertible<T2, T>::value, int>::type>
	Vector<height, T>::Vector(T2 initVal)
	{
		for (unsigned int i = 0; i < height; i++)
		{
			vals[i] = static_cast<T>(initVal);
		}
	}
	
	template<unsigned int height, typename T>
	template<typename T2, typename std::enable_if<std::is_convertible<T2, T>::value, int>::type>
	Vector<height, T>::Vector(T2 values[])
	{
		for (unsigned int i = 0; i < height; i++)
		{
			vals[i] = static_cast<T>(values[i]);
		}
	}

	template<unsigned int height, typename T>
	Vector<height, T>::Vector(const Vector &vec)
	{
		for (unsigned int i = 0; i < height; i++)
		{
			this->vals[i] = vec.vals[i];
		}
	}



	template<unsigned int height, typename T>
	inline T& Vector<height, T>::operator[](int index)
	{
		return this->vals[index];
	}

	template<unsigned int height, typename T>
	inline const T Vector<height, T>::operator[](int index) const
	{
		return this->vals[index];
	}



	template<unsigned int height, typename T>
	bool operator==(const Vector<height, T> &left, const Vector<height, T> &right)
	{
		for (unsigned int i = 0; i < height; i++)
		{
			if (left[i] != right[i])
				return false;
		}
		return true;
	}



	template<unsigned int height, typename T>
	Vector<height, T> operator+(const Vector<height, T> &left, const Vector<height, T> &right)
	{
		Vector<height, T> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			ret[i] += right[i];
		}
		return ret;
	}

	template<unsigned int height, typename TMat, typename TNum>
	Vector<height, TMat> operator+(const Vector<height, TMat> &left, TNum num)
	{
		Vector<height, TMat> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			ret[i] += num;
		}
		return ret;
	}



	template<unsigned int height, typename T>
	Vector<height, T>& operator+=(Vector<height, T> &left, const Vector<height, T> &right)
	{
		return (left = left + right);
	}



	template<unsigned int height, typename T>
	Vector<height, T> operator-(const Vector<height, T> &right)
	{
		float vals[height];
		for (unsigned int i = 0; i < height; i++)
		{
			vals[i] = -right[i];

		}
		return Vector<height, T>(vals);
	}

	template<unsigned int height, typename TMat, typename TNum>
	Vector<height, TMat> operator-(const Vector<height, TMat> &left, TNum num)
	{
		Vector<height, TMat> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			ret[i] -= num;
		}
		return ret;
	}



	template<unsigned int height, typename T>
	Vector<height, T>& operator-=(Vector<height, T> &left, const Vector<height, T> &right)
	{
		return (left = left - right);
	}



	template<unsigned int height, typename T>
	Vector<height, T> operator-(const Vector<height, T> &left, const Vector<height, T> &right)
	{
		Vector<height, T> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			ret[i] -= right[i];
		}
		return ret;
	}



	template<unsigned int height, typename T>
	Vector<height, T> operator*(const Vector<height, T> &left, const Vector<height, T> &right)
	{
		Vector<height, T> ret(left);

		for (unsigned int row = 0; row < height; row++)
		{
			ret[row] *= right[row];
		}
		return ret;
	}

	template<unsigned int height, typename TMat, typename TNum>
	Vector<height, TMat> operator*(const Vector<height, TMat> &left, TNum num)
	{
		Vector<height, TMat> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			ret[i] *= num;
		}
		return ret;
	}

	template<unsigned int height, typename T>
	Vector<height, T> operator*(T num, const Vector<height, T> &vec)
	{
		return vec * num;
	}



	template<unsigned int height, typename TMat, typename TNum>
	Vector<height, TMat> operator/(const Vector<height, TMat> &left, TNum num)
	{
		Vector<height, TMat> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			ret[i] /= num;
		}
		return ret;
	}



	template<unsigned int height, typename T1>
	template<unsigned int newHeight, typename T2, typename std::enable_if<newHeight <= height, int>::type>
	Vector<height, T1>::operator Vector<newHeight, T2>()
	{
		Vector<newHeight, T2> ret;
		for (unsigned int i = 0; i < newHeight; i++)
		{
			ret[i] = static_cast<T2>(this->vals[i]);
		}
		return ret;
	}



	template<unsigned int height, typename T>
	std::string Vector<height, T>::toString() const
	{
		std::stringstream ret = std::stringstream();

		for (unsigned int i = 0; i < height; i++)
		{
			ret << vals[i] << (",");
			// ret << (vals[(i + 1)*width - 1]) << ("\n");
		}
		return ret.str();
	}




    //Non-member Matrix functions
    template<unsigned int length>
	float magnitude(const Vector<length> &vec)
	{
		float mag = 0;

		for (unsigned int i = 0; i < length; i++)
		{
			mag += vec[i] * vec[i];
		}

		return sqrtf(mag);
	}

	template<unsigned int length>
	Vector<length> normalize(const Vector<length> &vec)
	{
		float mag = magnitude(vec);

		if (mag != 0)
			return vec / mag;
		else
			return vec;
	}

	template<typename T>
	Vector<3, T> cross(const Vector<3, T> left, const Vector<3, T> right)
	{
		T vals[3];

		vals[0] = left[1] * right[2] - left[2] * right[1];
		vals[1] = left[2] * right[0] - left[0] * right[2];
		vals[2] = left[0] * right[1] - left[1] * right[0];

		return Vector<3, T>(vals);
	}

	template<unsigned int length, typename T>
	T dot(const Vector<length, T> &left, const Vector<length, T> &right) 
	{
		T ret = 0;
		
		for (int i = 0; i < length; i++)
		{
			ret += left[i] * right[i];
		}

		return ret;
	}

	template<typename T>
	Vector<3, T> reflectOut(const Vector<3, T> outgoing, const Vector<3, T> axis)
	{
		return normalize(2 * dot(outgoing, axis) * axis - outgoing);
	}

	template<typename T>
	Vector<3, T> reflectIn(const Vector<3, T> incoming, const Vector<3, T> axis)
	{
		return normalize(incoming - 2 * Mat::dot(incoming, axis) * axis);
	}

	float toRads(float in)
	{
		return in * 3.14159265f / 180;
	}
}


typedef Mat::Vector<2, float> Vec2;
typedef Mat::Vector<3, float> Vec3;
typedef Mat::Vector<4, float> Vec4;

typedef Mat::Vector<3, unsigned char> Color;

struct Vec3Bundle
{
	Vec3 vecs[4];

	Vec3& operator[](int index)
	{
		return vecs[index];
	}
};

struct ColorBundle
{
	Color vecs[4];

	Color& operator[](int index)
	{
		return vecs[index];
	}
};

#endif
