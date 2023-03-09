#pragma once

#include <glm/glm.hpp>

template <int n1, int n2>
glm::vec<n1, float> embed(const glm::vec<n2, float>& v, float fill = 1)
{
    return glm::vec<n1, float>(v, fill);
}

template <int n1, int n2>
glm::vec<n1, float> proj(const glm::vec<n2, float>& v)
{
    glm::vec<n1, float> ret;
    for (int i = 0; i < n1; i++)
    {
        ret[i] = v[i];
    }
    return ret;
}

template <int n1, int n2, typename t = float>
auto invert_transpose(glm::mat<n1, n2, t>& mat)
{
    auto transposeMat    = glm::transpose(mat);
    auto invTransposeMat = glm::inverse(transposeMat);
    return invTransposeMat;
}


template <int nrows, int ncols>
void set_col(glm::mat<nrows, ncols, float>& mat, const int idx, const glm::vec<nrows, float>& vec)
{
    assert(idx >= 0 && idx < ncols);
    for (int i = 0; i < nrows; i++)
    {
        mat[i][idx] = vec[i];
    }
}

template<int nrows, int ncols>
glm::vec<nrows, float> col(const glm::mat<nrows, ncols, float>& mat, const int idx)
{
    assert(idx >= 0 && idx < ncols);
    glm::vec<nrows, float> ret;
    for (int i = 0; i < nrows; i++) {
        ret[i] = mat[i][idx];
    }
    return ret;
}


