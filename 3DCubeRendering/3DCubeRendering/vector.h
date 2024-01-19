#ifndef VECTOR_H
#define VECTOR_H

#include <cstdarg>
#include <iostream>
#include <cmath>
#include <string>

const double PI = 3.14159265358979;
const double PI2RAD = (PI / 180);

class vector
{
public:
	int count;
	double* e;

	double x() { return e[0]; }
	double y() { return e[1]; }
	double z() { return e[2]; }

	vector()
	{
		count = 1;
		e = new double[count];
		e[0] = 0;
	}

	vector(int _count)
	{
		count = _count;
		e = new double[count];
		for (int i = 0; i < count; i++)
		{
			e[i] = 0;
		}
	}
	vector set(double elements, ...)
	{
		va_list args;
		va_start(args, elements);

		e[0] = elements;
		for (int i = 1; i < count; ++i) {
			double value = static_cast<double>(va_arg(args, double));
			e[i] = value;
		}
		va_end(args);
		return *this;
	}
	vector set(float elements, ...)
	{
		va_list args;
		va_start(args, elements);

		e[0] = elements;
		for (int i = 1; i < count; ++i) {
			double value = static_cast<double>(va_arg(args, float));
			e[i] = value;
		}
		va_end(args);
		return *this;
	}
	vector set(int elements, ...)
	{
		va_list args;
		va_start(args, elements);
		e[0] = elements;
		for (int i = 1; i < count; ++i) {
			double value = static_cast<double>(va_arg(args, int));
			e[i] = value;
		}
		va_end(args);
		return *this;
	}

	vector clone()
	{
		vector v(count);
		for (int i = 0; i < count; i++)
		{
			v[i] = e[i];
		}
		return v;
	}

	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vector operator-()
	{
		vector v(count);
		for (int i = 0; i < count; i++)
			v[i] = -e[i];
		return v;
	}
	vector& operator+=(const vector& v)
	{
		int counter = v.count;
		//¾ÈÇØµµ OK
		//if (v.length < this->length)
		//	counter = v.length;
		//else 
		if (v.count > this->count)
			counter = this->count;

		for (int i = 0; i < counter; i++)
			e[i] += v[i];
		return *this;
	}
	vector& operator*=(double t)
	{
		for (int i = 0; i < count; i++)
			e[i] *= t;
		return *this;
	}
	vector& operator/=(double t)
	{
		return *this *= 1 / t;
	}

	vector reshape(int length, double add_value = 1)
	{
		if (count == length)
			return (*this).clone();

		//cut
		if (count > length)
		{
			vector v(length);
			for (int i = 0; i < length; i++)
				v[i] = e[i];
			return v;
		}
		//add
		else
		{
			vector v(length);
			for (int i = 0; i < length; i++)
			{
				if (i < count)
					v[i] = e[i];
				else
					v[i] = add_value;
			}
			return v;
		}
	}
	vector cross(const vector& v) const {
		if (count != 3 || v.count != 3) {
			std::cout << "Cross Product Error: Vectors must be 3-dimensional\n";
			return vector(0);
		}

		double x = (*this)[1] * v[2] - (*this)[2] * v[1];
		double y = (*this)[2] * v[0] - (*this)[0] * v[2];
		double z = (*this)[0] * v[1] - (*this)[1] * v[0];

		vector result(3);
		return result.set(x, y, z);
	}

	double length() const
	{
		return std::sqrt(length_squared());
	}
	double length_squared() const
	{
		double l = 0;
		for (int i = 0; i < count; i++)
			l += e[i] * e[i];
		return l;
	}

	bool near_zero() const {
		// Return true if the vector is close to zero in all dimensions.
		auto s = 1e-8;
		for (int i = 0; i < count; i++)
		{
			if (fabs(e[i]) > s)
				return false;
		}
		return true;
	}

	void print() const
	{
		std::cout << "(";
		for (int i = 0; i < count; i++)
		{
			if (fabs(e[i]) > 1e-8)
				std::cout << e[i];
			else
				std::cout << 0;
			if (i + 1 != count)
				std::cout << ", ";
		}
		std::cout << ")\n";
	}

	std::string print_string()
	{
		std::string str = "";
		str += "(";
		for (int i = 0; i < count; i++)
		{
			if (fabs(e[i]) > 1e-8)
			{
				str += (std::to_string(e[i])).substr(0, 4);
			}
			else
				str += "0";
			if (i + 1 != count)
				str += ", ";
		}
		str += ")";
		return str;
	}
};

inline vector vector2(double e1, double e2)
{
	vector v(2);
	return v.set(e1, e2);
}
inline vector vector3(double e1, double e2, double e3)
{
	vector v(3);	
	return v.set(e1, e2, e3);
}
inline vector vector3(int e1, int e2, int e3)
{
	vector v(3);
	return v.set(e1, e2, e3);
}
inline vector vector4(double e1, double e2, double e3, double e4)
{
	vector v(4);
	return v.set(e1, e2, e3, e4);
}
inline vector vector4(int e1, int e2, int e3, int e4)
{
	vector v(4);
	return v.set(e1, e2, e3, e4);
}
inline vector operator+(const vector& u, const vector& v) {
	int counter = std::min(u.count, v.count);

	vector result(counter);

	for (int i = 0; i < counter; i++)
		result[i] = u[i] + v[i];
	return result;
}
inline vector operator-(const vector& u, const vector& v) {
	int counter = std::min(u.count, v.count);

	vector result(counter);

	for (int i = 0; i < counter; i++)
		result[i] = u[i] - v[i];
	return result;
}
inline vector operator*(const vector& u, const vector& v) {
	int counter = std::min(u.count, v.count);

	vector result(counter);

	for (int i = 0; i < counter; i++)
		result[i] = u[i] * v[i];
	return result;
}
inline vector operator/(const vector& u, const vector& v) {
	int counter = std::min(u.count, v.count);

	vector result(counter);

	for (int i = 0; i < counter; i++)
		result[i] = u[i] / v[i];
	return result;
}
inline vector operator*(double t, const vector& v) {
	vector result(v.count);

	for (int i = 0; i < v.count; i++)
		result[i] = t * v[i];
	return result;
}
inline vector operator*(const vector& v, double t) {
	return t * v;
}
inline vector operator/(double t, const vector& v) {
	return (1 / t) * v;
}
inline vector operator/(const vector& v, double t) {
	return (1 / t) * v;
}
inline vector euler(double e1, double e2, double e3)
{
	return vector3(e1, e2, e3) * PI2RAD;
}
inline vector euler(int e1, int e2, int e3)
{
	return vector3(e1, e2, e3) * PI2RAD;
}
inline vector convert_homogeneous2normal(vector v)
{
	double last = v[v.count - 1];
	return v.reshape(v.count - 1) / last;
}
inline vector convert_normal2homogeneous(vector v)
{
	return v.reshape(v.count + 1, 1);
}
#endif