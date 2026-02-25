#ifndef _MYLIB_H_

#define _MYLIB_H_

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct {
    vec4 x;
    vec4 y;
    vec4 z;
    vec4 w;
} mat4;

typedef struct{
    GLfloat x;
    GLfloat y;
} vec2;

// Insert function signatures after this line

void print_v4(vec4);

vec4 scalar_vec_mult(float num, vec4 vec);

vec4 vec_add(vec4 vec1, vec4 vec2);

vec4 vec_sub(vec4 vec1, vec4 vec2);

float vec_mag(vec4 vec1);

vec4 vec_norm(vec4 vec1);

float dot_product(vec4 vec1, vec4 vec2);

vec4 cross_product(vec4 vec1, vec4 vec2);

void print_m4(mat4);

mat4 scalar_mat_mult(float num, mat4 mat);

mat4 mat_add(mat4 mat1, mat4 mat2);

mat4 mat_sub(mat4 mat1, mat4 mat2);

mat4 mat_mat_mult(mat4 mat1, mat4 mat2);

mat4 mat_inverse(mat4 m);

mat4 mat_transpose(mat4 m);

vec4 mat_vec_mult(mat4 mat, vec4 vec1);

float mat_3x3_det(float a, float b, float c, float d, float e, float f, float g, float h, float i);

mat4 mat_minor(mat4 mat);

mat4 apply_mat_cofactor(mat4 mat);

mat4 identity();

mat4 translate(GLfloat x, GLfloat y, GLfloat z);

mat4 scale(GLfloat x, GLfloat y, GLfloat z);

mat4 rotateZ(GLfloat theta);

mat4 rotateX(GLfloat theta);

mat4 rotateY(GLfloat theta);

float calcZ(GLfloat x, GLfloat y);

mat4 rotateArbitrary(mat4 rotateZ, mat4 rotateY, mat4 rotateX);

mat4 look_at(vec4 eyePoint, vec4 atPoint, vec4 upVector);

mat4 ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);

mat4 frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);

vec4 product(vec4 u, vec4 v);


// Do not put anything after this line

#endif
