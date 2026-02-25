#include "myLib.h"
#include <stdio.h>
#include <math.h>

void print_v4(vec4 v) {
    printf("[ %7.3f %7.3f %7.3f %7.3f ]\n\n", v.x, v.y, v.z, v.w);
}

vec4 scalar_vec_mult(float num, vec4 vec){
    vec4 res;
    res.x = num * vec.x;
    res.y = num * vec.y;
    res.z = num * vec.z;
    res.w = num * vec.w;
    return res;
}

vec4 vec_add(vec4 vec1, vec4 vec2){
    vec4 res = {vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec1.w + vec2.w};
    return res;
}

vec4 vec_sub(vec4 vec1, vec4 vec2){
    vec4 res = {vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z, vec1.w - vec2.w};
    return res;
}

float vec_mag(vec4 vec1){
    return sqrtf(vec1.x * vec1.x + vec1.y * vec1.y + vec1.z * vec1.z + vec1.w * vec1.w);
};

vec4 vec_norm(vec4 vec1){
    return scalar_vec_mult((1/vec_mag(vec1)), vec1);
}

float dot_product(vec4 vec1, vec4 vec2){
    return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z) + (vec1.w * vec2.w);
};

vec4 cross_product(vec4 vec1, vec4 vec2){
    vec4 res = {(vec1.y * vec2.z) - (vec1.z * vec2.y), (vec1.z * vec2.x) - (vec1.x * vec2.z), (vec1.x * vec2.y) - (vec1.y * vec2.x), 0};
    return res;
};

void print_m4(mat4 m){
    printf("[ %7.3f %7.3f %7.3f %7.3f ]\n", m.x.x, m.y.x, m.z.x, m.w.x);
    printf("[ %7.3f %7.3f %7.3f %7.3f ]\n", m.x.y, m.y.y, m.z.y, m.w.y);
    printf("[ %7.3f %7.3f %7.3f %7.3f ]\n", m.x.z, m.y.z, m.z.z, m.w.z);
    printf("[ %7.3f %7.3f %7.3f %7.3f ]\n\n", m.x.w, m.y.w, m.z.w, m.w.w);
}

mat4 scalar_mat_mult(float num, mat4 mat){
    mat4 res;
    res.x = scalar_vec_mult(num, mat.x);
    res.y = scalar_vec_mult(num, mat.y);
    res.z = scalar_vec_mult(num, mat.z);
    res.w = scalar_vec_mult(num, mat.w);
    return res;
}

mat4 mat_add(mat4 mat1, mat4 mat2){
    mat4 res;
    res.x = vec_add(mat1.x, mat2.x);
    res.y = vec_add(mat1.y, mat2.y);
    res.z = vec_add(mat1.z, mat2.z);
    res.w = vec_add(mat1.w, mat2.w);
    return res;
}

mat4 mat_sub(mat4 mat1, mat4 mat2){
    mat4 res;
    res.x = vec_sub(mat1.x, mat2.x);
    res.y = vec_sub(mat1.y, mat2.y);
    res.z = vec_sub(mat1.z, mat2.z);
    res.w = vec_sub(mat1.w, mat2.w);
    return res;
}

mat4 mat_mat_mult(mat4 mat1, mat4 mat2){
    mat4 res;
    res.x = mat_vec_mult(mat1, mat2.x);
    res.y = mat_vec_mult(mat1, mat2.y);
    res.z = mat_vec_mult(mat1, mat2.z);
    res.w = mat_vec_mult(mat1, mat2.w);
    return res;
}


mat4 mat_inverse(mat4 m){
    mat4 m_minor = mat_minor(m);
    mat4 m_cofactor = apply_mat_cofactor(m_minor);
    mat4 m_trans = mat_transpose(m_cofactor);

    float det = (m.x.x * m_minor.x.x) - (m.y.x * m_minor.y.x) + (m.z.x * m_minor.z.x) - (m.w.x * m_minor.w.x);

    mat4 res = scalar_mat_mult((1/det), m_trans);
    return res;
}


mat4 mat_transpose(mat4 m){
    mat4 res;

    res.x.x = m.x.x;
    res.x.y = m.y.x;
    res.x.z = m.z.x;
    res.x.w = m.w.x;

    res.y.x = m.x.y;
    res.y.y = m.y.y;
    res.y.z = m.z.y;
    res.y.w = m.w.y;

    res.z.x = m.x.z;
    res.z.y = m.y.z;
    res.z.z = m.z.z;
    res.z.w = m.w.z;

    res.w.x = m.x.w;
    res.w.y = m.y.w;
    res.w.z = m.z.w;
    res.w.w = m.w.w; 

    return res;
}


vec4 mat_vec_mult(mat4 mat, vec4 vec1){
    vec4 res;
    res.x = (mat.x.x * vec1.x) + (mat.y.x * vec1.y) + (mat.z.x * vec1.z) + (mat.w.x * vec1.w);
    res.y = (mat.x.y * vec1.x) + (mat.y.y * vec1.y) + (mat.z.y * vec1.z) + (mat.w.y * vec1.w);
    res.z = (mat.x.z * vec1.x) + (mat.y.z * vec1.y) + (mat.z.z * vec1.z) + (mat.w.z * vec1.w);
    res.w = (mat.x.w * vec1.x) + (mat.y.w * vec1.y) + (mat.z.w * vec1.z) + (mat.w.w * vec1.w);
    return res;
}


float mat_3x3_det(float a, float b, float c, float d, float e, float f, float g, float h, float i){
    return (a*e*i) + (b*f*g) + (c*d*h) - (g*e*c) - (h*f*a) - (i*d*b);
}

mat4 mat_minor(mat4 mat){
    mat4 res;
    res.x.x = mat_3x3_det(mat.y.y, mat.z.y, mat.w.y, mat.y.z, mat.z.z, mat.w.z, mat.y.w, mat.z.w, mat.w.w);
    res.x.y = mat_3x3_det(mat.y.x, mat.z.x, mat.w.x, mat.y.z, mat.z.z, mat.w.z, mat.y.w, mat.z.w, mat.w.w);
    res.x.z = mat_3x3_det(mat.y.x, mat.z.x, mat.w.x, mat.y.y, mat.z.y, mat.w.y, mat.y.w, mat.z.w, mat.w.w);
    res.x.w = mat_3x3_det(mat.y.x, mat.z.x, mat.w.x, mat.y.y, mat.z.y, mat.w.y, mat.y.z, mat.z.z, mat.w.z);

    res.y.x = mat_3x3_det(mat.x.y, mat.z.y, mat.w.y, mat.x.z, mat.z.z, mat.w.z, mat.x.w, mat.z.w, mat.w.w);
    res.y.y = mat_3x3_det(mat.x.x, mat.z.x, mat.w.x, mat.x.z, mat.z.z, mat.w.z, mat.x.w, mat.z.w, mat.w.w);
    res.y.z = mat_3x3_det(mat.x.x, mat.z.x, mat.w.x, mat.x.y, mat.z.y, mat.w.y, mat.x.w, mat.z.w, mat.w.w);
    res.y.w = mat_3x3_det(mat.x.x, mat.z.x, mat.w.x, mat.x.y, mat.z.y, mat.w.y, mat.x.z, mat.z.z, mat.w.z);

    res.z.x = mat_3x3_det(mat.x.y, mat.y.y, mat.w.y, mat.x.z, mat.y.z, mat.w.z, mat.x.w, mat.y.w, mat.w.w);
    res.z.y = mat_3x3_det(mat.x.x, mat.y.x, mat.w.x, mat.x.z, mat.y.z, mat.w.z, mat.x.w, mat.y.w, mat.w.w);
    res.z.z = mat_3x3_det(mat.x.x, mat.y.x, mat.w.x, mat.x.y, mat.y.y, mat.w.y, mat.x.w, mat.y.w, mat.w.w);
    res.z.w = mat_3x3_det(mat.x.x, mat.y.x, mat.w.x, mat.x.y, mat.y.y, mat.w.y, mat.x.z, mat.y.z, mat.w.z);

    res.w.x = mat_3x3_det(mat.x.y, mat.y.y, mat.z.y, mat.x.z, mat.y.z, mat.z.z, mat.x.w, mat.y.w, mat.z.w);
    res.w.y = mat_3x3_det(mat.x.x, mat.y.x, mat.z.x, mat.x.z, mat.y.z, mat.z.z, mat.x.w, mat.y.w, mat.z.w);
    res.w.z = mat_3x3_det(mat.x.x, mat.y.x, mat.z.x, mat.x.y, mat.y.y, mat.z.y, mat.x.w, mat.y.w, mat.z.w);
    res.w.w = mat_3x3_det(mat.x.x, mat.y.x, mat.z.x, mat.x.y, mat.y.y, mat.z.y, mat.x.z, mat.y.z, mat.z.z);

    return res;
}

mat4 apply_mat_cofactor(mat4 mat){
    mat4 res;
    res.x.x = mat.x.x;
    res.x.y = -mat.x.y;
    res.x.z = mat.x.z;
    res.x.w = -mat.x.w;

    res.y.x = -mat.y.x;
    res.y.y = mat.y.y;
    res.y.z = -mat.y.z;
    res.y.w = mat.y.w;

    res.z.x = mat.z.x;
    res.z.y = -mat.z.y;
    res.z.z = mat.z.z;
    res.z.w = -mat.z.w;

    res.w.x = -mat.w.x;
    res.w.y = mat.w.y;
    res.w.z = -mat.w.z;
    res.w.w = mat.w.w;

    return res;
}

mat4 identity(){
    mat4 result = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
    return result;
};

mat4 translate(GLfloat x, GLfloat y, GLfloat z){
    mat4 result = identity();

    result.w.x = x;
    result.w.y = y;
    result.w.z = z;

    return result;
}

mat4 scale(GLfloat x, GLfloat y, GLfloat z){
    mat4 result = identity();

    result.x.x = x;
    result.y.y = y;
    result.z.z = z;

    return result;
}

mat4 rotateZ(GLfloat theta){
    mat4 result = identity();

    float rad = (theta / 180.0) * M_PI;

    result.x.x = cos(rad);
    result.x.y = sin(rad);
    result.y.x = -sin(rad);
    result.y.y = cos(rad);

    return result;
}

mat4 rotateX(GLfloat theta){
    mat4 result = identity();

    float rad = (theta / 180.0) * M_PI;

    result.y.y = cos(rad);
    result.y.z = sin(rad);
    result.z.y = -sin(rad);
    result.z.z = cos(rad);

    return result;
}

mat4 rotateY(GLfloat theta){
    mat4 result = identity();

    float rad = (theta / 180.0) * M_PI;

    result.x.x = cos(rad);
    result.x.z = -sin(rad);
    result.z.x = sin(rad);
    result.z.z = cos(rad);

    return result;
}

float calcZ(GLfloat x, GLfloat y){
    
    float z = sqrtf(1 - (x*x) - (y*y));
    return z;
}

mat4 rotateArbitrary(mat4 rotateZ, mat4 rotateY, mat4 rotateX){
    mat4 forwardRotate = mat_mat_mult(rotateZ,(mat_mat_mult(rotateY, rotateX)));
    mat4 backwardRotate = mat_mat_mult((mat_transpose(rotateX)),(mat_transpose(rotateY)));

    return mat_mat_mult(backwardRotate,forwardRotate);

}

mat4 look_at(vec4 eyePoint, vec4 atPoint, vec4 upVector){
    vec4 n = vec_norm(vec_sub(eyePoint, atPoint));
    
    vec4 u = vec_norm(cross_product(upVector, n));

    vec4 v = vec_norm(cross_product(n, u));

    mat4 r = identity();
    r.x.x = u.x;
    r.y.x = u.y;
    r.z.x = u.z;

    r.x.y = v.x;
    r.y.y = v.y;
    r.z.y = v.z;

    r.x.z = n.x;
    r.y.z = n.y;
    r.z.z = n.z;

    mat4 t = identity();
    t.w.x = -(eyePoint.x);
    t.w.y = -(eyePoint.y);
    t.w.z = -(eyePoint.z);

    mat4 m = mat_mat_mult(r, t);
    return m;
}

mat4 ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far){
    float xCenter = (right + left) / 2.0;
    float yCenter = (top + bottom) / 2.0;
    float zCenter = (near + far) / 2.0;

    mat4 t = translate(-xCenter, -yCenter, -zCenter);

    float scaleX = 2.0 / (right - left);
    float scaleY = 2.0 / (top - bottom);
    float scaleZ = 2.0 / (near - far);

    mat4 s = scale(scaleX, scaleY, scaleZ);

    mat4 result = mat_mat_mult(s, t);

    return result;
}

mat4 frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far){
    mat4 result = { {(-2.0 * near)/(right - left),0, 0, 0}, {0, (-2.0 * near)/(top - bottom), 0, 0}, {(left + right)/(right - left), (bottom + top)/(top - bottom), (near + far)/(far - near), -1.0}, {0, 0, (-2.0 * near * far)/(far - near), 0}};
    return result;
}

vec4 product(vec4 u, vec4 v){
    vec4 result;
    result.x = u.x * v.x;
    result.y = u.y * v.y;
    result.z = u.z * v.z;
    result.w = u.w * v.w;
    return result;
}




