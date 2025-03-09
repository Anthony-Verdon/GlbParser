#pragma once

#include <iostream>
#include <memory>
#include <vector>

#define ERROR_MESSAGE "error: " + std::string(__FILE__)  + " : " + std::string(__FUNCTION__)  + " : line " + std::to_string(__LINE__)

namespace AlgOps
{
  template <size_t R, size_t C>
  class Matrix
  {
    private:
      std::unique_ptr<float[]> data;
      size_t size;
        
    public:
      Matrix();
      Matrix(const std::vector<float> &values);
      Matrix(const Matrix &copy);
      Matrix &operator=(const Matrix &copy);
      Matrix &operator=(const std::vector<float> &values);
      ~Matrix();
      
      bool operator==(const Matrix &copy);
      bool operator!=(const Matrix &copy);
      Matrix operator+(const Matrix &instance) const;
      Matrix operator-(const Matrix &instance) const;
      Matrix operator*(float number) const;
      
      float *getData() const;
      float getData(unsigned int rowIndex, unsigned int columnIndex) const;
      float getX() const;
      float getY() const;
      float getZ() const;
      float getW() const;
      
      void setData(unsigned int rowIndex, unsigned int columnIndex, float value);
      void setData(const float *values, unsigned int size);
      void uniform(float value);
      void identity();
    };
    
    typedef Matrix<2, 1> vec2;
    typedef Matrix<3, 1> vec3;
    typedef Matrix<4, 1> vec4;
    typedef Matrix<2, 2> mat2;
    typedef Matrix<3, 3> mat3;
    typedef Matrix<4, 4> mat4;
    
    template <size_t R, size_t C>
    Matrix<R, C> Zero(const Matrix<R, C> &instance);
    
    template <size_t R, size_t C>
    std::ostream &operator<<(std::ostream &os, const Matrix<R, C> &instance);
    
    template <size_t R, size_t C>
    Matrix<R, C> operator*(float number, const Matrix<R, C> &instance);

    template <size_t R1, size_t C1, size_t R2, size_t C2>
    Matrix<R1, C2> operator*(const Matrix<R1, C1> &matA, const Matrix<R2, C2> &matB);

    // matrix operations
    mat4 rotate(const mat4 &instance, float angle, const vec3 &vector);
    mat4 perspective(float fov, float aspect, float near, float far);
    vec3 normalize(const vec3 &vector);
    vec3 crossProduct(const vec3 &vectorA, const vec3 &vectorB);
    float dotProduct(const vec3 &vectorA, const vec3 &vectorB);
    mat4 lookAt(const vec3 &position, const vec3 &target, const vec3 &initialUpVector);
    mat4 translate(const mat4 &instance, const vec3 &vector);
    mat4 scale(const mat4 &instance, const vec3 &vector);
    
}
  
#include "Matrix.tpp"