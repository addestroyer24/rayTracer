#ifndef _MATRIX_H
#define _MATRIX_H

#include <math.h>
#include <initializer_list>
#include <string>
#include <sstream>

namespace Mat
{

	template<unsigned int height, unsigned int width, typename T = float>
	class Matrix
	{
		static_assert(std::is_fundamental<T>::value, "Matrix and Vector can only use primitive types!");

		template<unsigned int h, unsigned int w, typename T2> friend class Matrix;

	protected:

		T vals[height*width];

	public:
		//TODO: passing 0 for diagVal looks the same as passing nullptr for values, resulting in ambiguous calls..
		template<typename T2 = T, typename std::enable_if<std::is_fundamental<T2>::value, int>::type = 1>
		explicit Matrix(T2 diagVal = 1);
		template<typename T2 = T>
		explicit Matrix(T2 values[]);
		explicit Matrix(std::initializer_list<T> values);
		Matrix(const Matrix &mat);
		~Matrix();

		//can get nontrivial speed up by bypassing this in operators to access vals directly..
		T* operator[](int row);
		T const* operator[](int row) const;

		template<typename T2>
		explicit operator Matrix<height, width, T2>();

		std::string toString() const;

		//Returns a T[] representing the matrix, in row-major order
		const T* getGLFormat() const;
	};


	//Totally not sure if this class is worth it, the only changed functionality is the 
	//subscript operator, but I haven't been able to find another way around it yet
	//Tried std::enable_if / SFINAE, but it errors before that since the functions differ
	//only by return type
	template<unsigned int length, typename T = float>
	class Vector : public Matrix<length, 1, T>
	{
	public:
		template<typename T2 = T>
		explicit Vector(T2 initVal = 0) : Matrix<length, 1, T>(initVal){}
		template<typename T2 = T>
		explicit Vector(T2 values[]) : Matrix<length, 1, T>(values){}
		explicit Vector(std::initializer_list<T> values) : Matrix<length, 1, T>(values){}
		Vector(const Matrix<length, 1, T> &mat) : Matrix<length, 1, T>(mat){}

		T& operator[](int index)
		{ 
			return this->vals[index]; 
		}

		const T operator[](int index) const
		{
			return this->vals[index];
		}

		template<typename T2>
		explicit operator Vector<length, T2>();
	};



	template<unsigned int height, unsigned int width, typename T>
	template<typename T2, typename std::enable_if<std::is_fundamental<T2>::value, int>::type>
	Matrix<height, width, T>::Matrix(T2 diagVal)
	{
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				if (i == j || width == 1)
				{
					vals[i*width + j] = static_cast<T>(diagVal);
				}
				else
				{
					vals[i*width + j] = 0;
				}
			}
		}
	}
	
	template<unsigned int height, unsigned int width, typename T>
	template<typename T2>
	Matrix<height, width, T>::Matrix(T2 values[])
	{
		for (unsigned int i = 0; i < height*width; i++)
		{
			vals[i] = static_cast<T>(values[i]);
		}
	}

	template<unsigned int height, unsigned int width, typename T>
	Matrix<height, width, T>::Matrix(std::initializer_list<T> values)
	{
		const T *begin = values.begin();

		for (unsigned int i = 0; i < height*width; i++)
		{
			vals[i] = begin[i];
		}
	}

	template<unsigned int height, unsigned int width, typename T>
	Matrix<height, width, T>::Matrix(const Matrix &mat)
	{
		for (unsigned int i = 0; i < height*width; i++)
		{
			this->vals[i] = mat.vals[i];
		}
	}

	template<unsigned int height, unsigned int width, typename T>
	Matrix<height, width, T>::~Matrix()
	{
	}



	template<unsigned int height, unsigned int width, typename T>
	bool operator==(const Matrix<height, width, T> &left, const Matrix<height, width, T> &right)
	{
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				if (left[i][j] != right[i][j])
					return false;
			}
		}
		return true;
	}

	//The assignment operator shouldn't need to be overloaded since vals is stored in the class



	template<unsigned int height, unsigned int width, typename T>
	Matrix<height, width, T> operator+(const Matrix<height, width, T> &left, const Matrix<height, width, T> &right)
	{
		Matrix<height, width, T> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				ret[i][j] += right[i][j];
			}
		}
		return ret;
	}

	//Not a fan of all this code duplication, but it's better than vec<x> + vec<x> = mat<x,1> followed by an implicit
	//conversion back to vec<x>
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

	template<unsigned int height, unsigned int width, typename TMat, typename TNum>
	Matrix<height, width, TMat> operator+(const Matrix<height, width, TMat> &left, TNum num)
	{
		Matrix<height, width, TMat> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				ret[i][j] += num;
			}
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



	template<unsigned int height, unsigned int width, typename T>
	Matrix<height, width, T>& operator+=(Matrix<height, width, T> &left, const Matrix<height, width, T> &right)
	{
		return (left = left + right);
	}

	template<unsigned int height, typename T>
	Vector<height, T>& operator+=(Vector<height, T> &left, const Vector<height, T> &right)
	{
		return (left = left + right);
	}



	template<unsigned int height, unsigned int width, typename T>
	Matrix<height, width, T> operator-(const Matrix<height, width, T> &right)
	{
		Matrix<height, width, T> ret;
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				ret[i][j] = -right[i][j];
			}
		}
		return ret;
	}

	template<unsigned int height, typename T>
	Vector<height, T> operator-(const Vector<height, T> &right)
	{
		Vector<height, T> ret;
		for (unsigned int i = 0; i < height; i++)
		{
			ret[i] = -right[i];

		}
		return ret;
	}

	template<unsigned int height, unsigned int width, typename TMat, typename TNum>
	Matrix<height, width, TMat> operator-(const Matrix<height, width, TMat> &left, TNum num)
	{
		Matrix<height, width, TMat> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				ret[i][j] -= num;
			}
		}
		return ret;
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



	template<unsigned int height, unsigned int width, typename T>
	Matrix<height, width, T>& operator-=(Matrix<height, width, T> &left, const Matrix<height, width, T> &right)
	{
		return (left = left - right);
	}

	template<unsigned int height, typename T>
	Vector<height, T>& operator-=(Vector<height, T> &left, const Vector<height, T> &right)
	{
		return (left = left - right);
	}



	template<unsigned int height, unsigned int width, typename T>
	Matrix<height, width, T> operator-(const Matrix<height, width, T> &left, const Matrix<height, width, T> &right)
	{
		Matrix<height, width, T> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				ret[i][j] -= right[i][j];
			}
		}
		return ret;
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



	template<unsigned int height, unsigned int common, unsigned int width, typename T>
	Matrix<height, width, T> operator*(const Matrix<height, common, T> &left, const Matrix<common, width, T> &right)
	{
		Matrix<height, width, T> ret;

		for (unsigned int row = 0; row < height; row++)
		{
			for (unsigned int col = 0; col < width; col++)
			{
				ret[row][col] = 0;
				for (unsigned int i = 0; i < common; i++)
				{
					ret[row][col] += left[row][i] * right[i][col];
				}
			}
		}
		return ret;
	}

	template<unsigned int height, unsigned int common, typename T>
	Vector<height, T> operator*(const Matrix<height, common, T> &left, const Vector<common, T> &right)
	{
		Vector<height, T> ret;

		for (unsigned int row = 0; row < height; row++)
		{
			ret[row] = 0;
			for (unsigned int i = 0; i < common; i++)
			{
				ret[row] += left[row][i] * right[i];
			}
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

	template<unsigned int height, unsigned int width, typename TMat, typename TNum>
	Matrix<height, width, TMat> operator*(const Matrix<height, width, TMat> &left, TNum num)
	{
		Matrix<height, width, TMat> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				ret[i][j] *= num;
			}
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

	template<unsigned int height, unsigned int width, typename TMat, typename TNum>
	Matrix<height, width, TMat> operator*(TNum num, const Matrix<height, width, TMat> &mat)
	{
		return mat * num;
	}

	template<unsigned int height, typename T>
	Vector<height, T> operator*(T num, const Vector<height, T> &vec)
	{
		return vec * num;
	}



	template<unsigned int height, unsigned int width, typename TMat, typename TNum>
	Matrix<height, width, TMat> operator/(const Matrix<height, width, TMat> &left, TNum num)
	{
		Matrix<height, width, TMat> ret(left);
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				ret[i][j] /= num;
			}
		}
		return ret;
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



	template<unsigned int height, unsigned int width, typename T>
	T* Matrix<height, width, T>::operator[](int row)
	{
		return this->vals + row * width;
	}

	template<unsigned int height, unsigned int width, typename T>
	T const* Matrix<height, width, T>::operator[](int row) const
	{
		return this->vals + row * width;
	}



	template<unsigned int height, unsigned int width, typename T1>
	template<typename T2>
	Matrix<height, width, T1>::operator Matrix<height, width, T2>()
	{
		Matrix<height, width, T2> ret;
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				ret[i][j] = static_cast<T2>(this->operator[](i)[j]);
			}
		}
		return ret;
	}

	template<unsigned int height, typename T1>
	template<typename T2>
	Vector<height, T1>::operator Vector<height, T2>()
	{
		Vector<height, T2> ret;
		for (unsigned int i = 0; i < height; i++)
		{
			ret[i] = static_cast<T2>(this->vals[i]);
		}
		return ret;
	}



	template<unsigned int height, unsigned int width, typename T>
	std::string Matrix<height, width, T>::toString() const
	{
		std::stringstream ret = std::stringstream();

		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width - 1; j++)
			{
				ret << (vals[i*width + j]) << (",");
			}
			ret << (vals[(i + 1)*width - 1]) << ("\n");
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


typedef Mat::Matrix<2, 2, float> Mat2;
typedef Mat::Matrix<3, 3, float> Mat3;
typedef Mat::Matrix<4, 4, float> Mat4;
typedef Mat::Vector<2, float> Vec2;
typedef Mat::Vector<3, float> Vec3;
typedef Mat::Vector<4, float> Vec4;

typedef Mat::Vector<3, unsigned char> Color;

#endif
