// #pragma once

// #include <glm/glm.hpp>

// template <int n1, int n2>
// glm::vec<n1, float> embed(const glm::vec<n2, float>& v, float fill = 1)
// {
//     return glm::vec<n1, float>(v, fill);
// }

// template <int n1, int n2>
// glm::vec<n1, float> proj(const glm::vec<n2, float>& v)
// {
//     glm::vec<n1, float> ret;
//     for (int i = 0; i < n1; i++)
//     {
//         ret[i] = v[i];
//     }
//     return ret;
// }

// template <int n1, int n2, typename t = float>
// auto invert_transpose(glm::mat<n1, n2, t>& mat)
// {
//     auto transposeMat    = glm::transpose(mat);
//     auto invTransposeMat = glm::inverse(transposeMat);
//     return invTransposeMat;
// }


// template <int nrows, int ncols>
// void set_col(glm::mat<nrows, ncols, float>& mat, const int idx, const glm::vec<nrows, float>& vec)
// {
//     assert(idx >= 0 && idx < ncols);
//     for (int i = 0; i < nrows; i++)
//     {
//         mat[i][idx] = vec[i];
//     }
// }

// template<int nrows, int ncols>
// glm::vec<nrows, float> col(const glm::mat<nrows, ncols, float>& mat, const int idx)
// {
//     assert(idx >= 0 && idx < ncols);
//     glm::vec<nrows, float> ret;
//     for (int i = 0; i < nrows; i++) {
//         ret[i] = mat[i][idx];
//     }
//     return ret;
// }


#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <vector>
#include<iostream>

class Matrix;

template <class t> struct Vec2 {
    t x, y;
    Vec2<t>() : x(t()), y(t()) {}
    Vec2<t>(t _x, t _y) : x(_x), y(_y) {}
    Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(x+V.x, y+V.y); }
    Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(x-V.x, y-V.y); }
    Vec2<t> operator *(float f)          const { return Vec2<t>(x*f, y*f); }
    t& operator[](const int i) { return i<=0 ? x : y; }
    template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
    t x, y, z;
    Vec3<t>() : x(t()), y(t()), z(t()) { }
    Vec3<t>(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
    Vec3<t>(Matrix m);
    template <class u> Vec3<t>(const Vec3<u> &v);
    Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
    Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
    Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
    Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
    t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
    float norm () const { return std::sqrt(x*x+y*y+z*z); }
    Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
    t& operator[](const int i) { return i<=0 ? x : (1==i ? y : z); }
    template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <> template <> Vec3<int>::Vec3(const Vec3<float> &v);
template <> template <> Vec3<float>::Vec3(const Vec3<int> &v);


template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

//////////////////////////////////////////////////////////////////////////////////////////////

class Matrix {
    std::vector<std::vector<float> > m;
    int rows, cols;
public:
    Matrix(int r=4, int c=4);
    Matrix(Vec3f v);
    int nrows();
    int ncols();
    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int i);
    Matrix operator*(const Matrix& a);
    Matrix transpose();
    Matrix inverse();
    friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};


#endif //__GEOMETRY_H__