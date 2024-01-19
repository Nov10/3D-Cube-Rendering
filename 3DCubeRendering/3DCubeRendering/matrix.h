#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"
#include <iostream>

class matrix
{
public:
	int size_x;
	int size_y;
	vector* e;

	matrix(int sizex, int sizey)
	{
		size_x = sizex;
		size_y = sizey;
		e = new vector[size_y];

		for (int i = 0; i < size_y; i++)
		{
			(e[i]) = vector(size_x);
		}
	}
	vector operator[](int i) const { return e[i]; }
	vector& operator[](int i) { return e[i]; }

	matrix operator-()
	{
		matrix m(size_x, size_y);
		for (int i = 0; i < size_y; i++)
			m[i] = -e[i];
		return m;
	}
	matrix& operator+=(const matrix& v)
	{
		if (size_x != v.size_x || size_y != v.size_y)
		{
			std::cout << "Matrixes with Different Sizes Error\n";
			return *this;
		}

		for (int i = 0; i < size_y; i++)
			e[i] += v[i];
		return *this;
	}
	matrix& operator*=(double t)
	{
		for (int i = 0; i < size_y; i++)
			e[i] *= t;
		return *this;
	}
	matrix& operator/=(double t)
	{
		return *this *= 1 / t;
	}
	matrix clone()
	{
		matrix result(size_x, size_y);
		for (int i = 0; i < size_y; i++)
		{
			for (int j = 0; j < size_x; j++)
			{
					result[i][j] = e[i][j];
			}
		}
		return result;
	}
	matrix reshape(int sizex, int sizey, double add_value = 1)
	{
		if (size_x == sizex && size_y == sizey)
			return (*this).clone();

		int x = std::max(size_x, sizex);
		int y = std::max(size_y, sizey);

		matrix result(sizex, sizey);

		for (int i = 0; i < sizey; i++)
		{
			for (int j = 0; j < sizex; j++)
			{
				if(i < size_y && j < size_x)
					result[i][j] = e[i][j];
				else
					result[i][j] = add_value;
			}
		}
		return result;
	}

	matrix T()
	{
		matrix result(size_x, size_y);

		for (int i = 0; i < size_y; i++) {
			for (int j = 0; j < size_x; j++) {
				result[i][j] = e[j][i];
			}
		}

		return result;
	}

	void print()
	{
		for (int i = 0; i < size_y; i++)
		{
			(e[i]).print();
		}
	}
	std::string print_string()
	{
		std::string str = "";
		for (int i = 0; i < size_y; i++)
		{
			str += (e[i]).print_string() + "\n";
		}
		return str;
	}
};


inline matrix matrix4x4(
	double e11, double e12, double e13, double e14,
	double e21, double e22, double e23, double e24,
	double e31, double e32, double e33, double e34,
	double e41, double e42, double e43, double e44)
{
	matrix m(4, 4);
	m[0] = vector4(e11, e12, e13, e14);
	m[1] = vector4(e21, e22, e23, e24);
	m[2] = vector4(e31, e32, e33, e34);
	m[3] = vector4(e41, e42, e43, e44);
	return m;
}

inline matrix rotation_from_euler(vector euler)
{
	double cx = cos(euler[0]);
	double sx = sin(euler[0]);
	double cy = cos(euler[1]);
	double sy = sin(euler[1]);
	double cz = cos(euler[2]);
	double sz = sin(euler[2]);

	return matrix4x4(
		cy * cz, -cy * sz, sy, 0,
		cx * sz + sx * sy * cz, cx * cz - sx * sy * sz, -sx * cy, 0,
		sx * sz - cx * sy * cz, cx * sy * sz + sx * cz, cx * cy, 0,
		0, 0, 0, 1
	);
}
inline matrix operator+(const matrix& u, const matrix& v) {
	if (u.size_x != v.size_x || u.size_y != v.size_y)
	{
		std::cout << "Matrixes with Different Sizes Error\n";
		return matrix(0, 0);
	}

	matrix result(u.size_x, u.size_y);

	for (int i = 0; i < u.size_y; i++)
		result[i] = u[i] + v[i];
	return result;
}
inline matrix operator-(const matrix& u, const matrix& v) {
	if (u.size_x != v.size_x || u.size_y != v.size_y)
	{
		std::cout << "Matrixes with Different Sizes Error\n";
		return matrix(0, 0);
	}

	matrix result(u.size_x, u.size_y);

	for (int i = 0; i < u.size_y; i++)
		result[i] = u[i] - v[i];
	return result;
}
inline matrix operator*(const matrix& u, const matrix& v) {
	if (u.size_x != v.size_y)
	{
		std::cout << "Matrixes with Different Sizes Error\n";
		return matrix(0, 0);
	}

	matrix result(u.size_y, v.size_x);

	for (int i = 0; i < u.size_y; i++) {
		for (int j = 0; j < v.size_x; j++) {
			for (int k = 0; k < u.size_x; k++) {
				result[i][j] += u[i][k] * v[k][j];
			}
		}
	}

	return result;
}
inline vector operator*(const matrix& m, const vector& v) {
	if (m.size_x != v.count) {
		std::cout << "Matrix-Vector Multiplication Error: Incompatible Sizes\n";
		return vector(0);
	}

	vector result(m.size_y);

	for (int i = 0; i < m.size_y; i++) {
		double sum = 0.0;
		for (int j = 0; j < m.size_x; j++) {
			sum += m[i][j] * v[j];
		}
		result[i] = sum;
	}

	return result;
}


inline matrix operator*(double t, const matrix& v) {
	matrix result(v.size_x, v.size_y);

	for (int i = 0; i < v.size_y; i++)
		result[i] = t * v[i];
	return result;
}
inline matrix operator*(const matrix& v, double t) {
	return t * v;
}
inline matrix operator/(double t, const matrix& v) {
	return (1 / t) * v;
}
inline matrix operator/(const matrix& v, double t) {
	return (1 / t) * v;
}

inline matrix transpose(const matrix& m) {
	matrix result(m.size_y, m.size_x);

	for (int i = 0; i < m.size_y; i++) {
		for (int j = 0; j < m.size_x; j++) {
			result[i][j] = m[j][i];
		}
	}

	return result;
}
// 행렬의 역행렬을 계산하는 함수
inline matrix inverse(const matrix& m) {
	if (m.size_x != m.size_y) {
		std::cout << "Non-Square Matrix Error\n";
		return matrix(0, 0);
	}

	int n = m.size_x;
	matrix augmentedMatrix(n, 2 * n);

	// m 행렬과 단위 행렬을 합친 augmentedMatrix 생성
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			augmentedMatrix[i][j] = m[i][j];
			augmentedMatrix[i][j + n] = (i == j) ? 1.0 : 0.0;
		}
	}

	// 가우스 소거법을 사용하여 augmentedMatrix를 역행렬로 변환
	for (int i = 0; i < n; i++) {
		// 대각선 원소를 1로 만들기
		double pivot = augmentedMatrix[i][i];
		if (pivot == 0.0) {
			std::cout << "Matrix is Singular (No Inverse)\n";
			return matrix(0, 0);
		}

		for (int j = 0; j < 2 * n; j++) {
			augmentedMatrix[i][j] /= pivot;
		}

		// 다른 행들의 현재 열 원소를 0으로 만들기
		for (int k = 0; k < n; k++) {
			if (k != i) {
				double factor = augmentedMatrix[k][i];
				for (int j = 0; j < 2 * n; j++) {
					augmentedMatrix[k][j] -= factor * augmentedMatrix[i][j];
				}
			}
		}
	}

	// 역행렬 부분만 추출하여 반환
	matrix result(n, n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			result[i][j] = augmentedMatrix[i][j + n];
		}
	}

	return result;
}

#endif