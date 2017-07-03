#ifndef _CUBE_MATRIX4X4_HPP_
#define _CUBE_MATRIX4X4_HPP_

#include <cmath>
#include <cstring>

static const float Pi = 3.1415926536f;
static const float Pi180 = Pi / 180.0f;

class Matrix4x4 {
public:
    float v[16];

public:
    Matrix4x4() { }

    inline void load_identity()                                 { for (int i = 0; i < 16; ++i) v[i] = 0.0f; v[0] = v[5] = v[10] = v[15] = 1.0f; }
    inline Matrix4x4 operator*(const Matrix4x4& rhs) const      { Matrix4x4 m; for (int i = 0; i < 4; ++i) { for (int j = 0; j < 4; ++j) { m.v[j * 4 + i] = 0.0f; for (int k = 0; k < 4; ++k) { m.v[j * 4 + i] += v[k * 4 + i] * rhs.v[j * 4 + k]; } } } return m; }
    inline Matrix4x4& operator*=(const Matrix4x4& rhs)          { float res[16]; for (int i = 0; i < 4; ++i) { for (int j = 0; j < 4; ++j) { res[j * 4 + i] = 0.0f; for (int k = 0; k < 4; ++k) { res[j * 4 + i] += v[k * 4 + i] * rhs.v[j * 4 + k]; } } } std::memcpy(v, res, 16 * sizeof(float)); return *this; }
    inline void scale(float x, float y, float z)                { Matrix4x4 m; m.load_identity(); m.v[0] = x; m.v[5] = y; m.v[10] = z; *this *= m; }
    inline void translate(float x, float y, float z)            { Matrix4x4 m; m.load_identity(); m.v[12] = x; m.v[13] = y; m.v[14] = z; *this *= m; }
    inline void rotate(float angle, float x, float y, float z)  { Matrix4x4 m; m.load_identity(); float c = std::cos(angle * Pi180); float s = std::sin(angle * Pi180); m.v[ 0] = x * x * (1 - c) + c; m.v[ 1] = x * y * (1 - c) + z * s; m.v[ 2] = x * z * (1 - c) - y * s; m.v[ 4] = y * x * (1 - c) - z * s; m.v[ 5] = y * y * (1 - c) + c; m.v[ 6] = y * z * (1 - c) + x * s; m.v[ 8] = z * x * (1 - c) + y * s; m.v[ 9] = z * y * (1 - c) - x * s; m.v[10] = z * z * (1 - c) + c; *this *= m; }
    inline void set_projection(float fovy, float aspect, float znear, float zfar) { memset(v, 0, sizeof(v)); float tan_half_fovy = std::tan(fovy * (Pi / 360.0f)); v[0 * 4 + 0] = 1.0f / (aspect * tan_half_fovy); v[1 * 4 + 1] = 1.0f / tan_half_fovy; v[2 * 4 + 2] = - (zfar + znear) / (zfar - znear); v[2 * 4 + 3] = - 1.0f; v[3 * 4 + 2] = - (2.0f * zfar * znear)  / (zfar - znear); }
    inline void set_ortho(float left, float right, float bottom, float top, float znear, float zfar) { load_identity(); v[ 0] = 2.0f / (right - left); v[ 5] = 2.0f / (top - bottom); v[10] = -2.0f / (zfar - znear); v[12] = -((right + left) / (right - left)); v[13] = -((top + bottom) / (top - bottom)); v[14] = -((zfar + znear) / (zfar - znear)); }
};

#endif
