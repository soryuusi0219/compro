// https://github.com/soryuusi0219/compro/blob/main/library/math/matrix.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<typename T>
struct Matrix : vector<vector<T>>{
  using vector<vector<T>>::vector;
  Matrix(int h,int w){
    (*this).resize(h);
    for(int i = 0; i < h; i++) (*this)[i].resize(w);
  }
  int height() const {return (*this).size();}
  int width() const {if((*this).empty()) return 0; return (*this)[0].size();}
  pair<int,int> hwsize() const {return {height(),width()};}
  void hwresize(int h,int w) {
    (*this).resize(h);
    for(int i = 0; i < h; i++) (*this)[i].resize(w);
  }
  static Matrix I(int n){
    Matrix ret(n,n);
    for(int i = 0; i < n; i++) ret[i][i] = 1;
    return ret;
  } 
  
  Matrix &operator+=(const Matrix &other) {
    assert(hwsize() == other.hwsize());
    for(int i = 0; i < height(); i++) for(int j = 0; j < width(); j++) (*this)[i][j] += other[i][j];
    return (*this);
  }
  Matrix &operator-=(const Matrix &other) {
    assert(hwsize() == other.hwsize());
    for(int i = 0; i < height(); i++) for(int j = 0; j < width(); j++) (*this)[i][j] -= other[i][j];
    return (*this);
  }
  Matrix &operator*=(const T &k) {
    for(int i = 0; i < height(); i++) for(int j = 0; j < width(); j++) (*this)[i][j] *= k;
    return (*this);
  }
  Matrix &operator/=(const T &k) {
    for(int i = 0; i < height(); i++) for(int j = 0; j < width(); j++) (*this)[i][j] /= k;
    return (*this);
  }
  Matrix &operator%=(const T &k) {
    for(int i = 0; i < height(); i++) for(int j = 0; j < width(); j++) (*this)[i][j] %= k;
    return (*this);
  }
  Matrix operator+(const Matrix &other) const { return (Matrix(*this) += other); }
  Matrix operator-(const Matrix &other) const { return (Matrix(*this) -= other); }
  Matrix operator*(const T &k) const { return (Matrix(*this) *= k); }
  Matrix operator/(const T &k) const { return (Matrix(*this) /= k); }
  Matrix operator%(const T &k) const { return (Matrix(*this) %= k); }

  Matrix &operator-() const {
    Matrix ret(*this);
    for(int i = 0; i < height(); i++) for(int j = 0; j < width(); j++) ret[i][j] = -ret[i][j];
    return ret;
  }

  Matrix &operator==(const Matrix &other) const {
    assert(hwsize() == other.hwsize());
    for(int i = 0; i < height(); i++) for(int j = 0; j < width(); j++) if((*this)[i][j] != other[i][j]) return false;
    return true;
  }
  Matrix &operator!=(const Matrix &other) const {
    assert(hwsize() == other.hwsize());
    for(int i = 0; i < height(); i++) for(int j = 0; j < width(); j++) if((*this)[i][j] != other[i][j]) return true;
    return false;
  }

  Matrix operator*(const Matrix &other) const {
    assert(width() == other.height());
    Matrix ret(height(), other.width());
    for(int i = 0; i < height(); i++)for(int j = 0; j < width(); j++)for(int k = 0; k < other.width(); k++){
      ret[i][k] += (*this)[i][j]*other[j][k];
    }
    return ret;
  }
  Matrix operator*=(const Matrix &other){
    assert(width() == other.height());
    assert(other.width() == other.height());
    return ((*this) = (*this) * other);
  }
  Matrix pow(long long k) const {
    assert(height() == width());
    Matrix ret = I(height()), B = (*this);
    while(k){
      if(k&1) ret *= B;
      B *= B;
      k >>= 1;
    }
    return ret;
  }
  T Gaussian_elimination(bool f = true){
    int c = 0;
    T x(1);
    for(int i = 0; i < width(); i++){
      if(c == height()) break;
      int k = -1;
      for(int j = c; j < height(); j++){
        if((*this)[j][i] != 0) {
          k = j;
          break;
        }
      }
      if(k == -1) {
        x = 0;
        continue;
      }
      if(k != c) swap((*this)[k],(*this)[c]);
      x *= (*this)[c][i];
      T ai = T(1)/(*this)[c][i];
      for(int ii = i; ii < width(); ii++) (*this)[c][ii] *= ai;
      for(int j = k+1; j < height(); j++){
        if((*this)[j][i] != 0) {
          T b = (*this)[j][i];
          for(int ii = i; ii < width(); ii++) (*this)[j][ii] -=  (*this)[c][ii] * b;
        }
      }
      if(f) {
        for(int j = 0; j < c; j++){
          if((*this)[j][i] != 0) {
            T b = (*this)[j][i];
            for(int ii = i; ii < width(); ii++) (*this)[j][ii] -=  (*this)[c][ii] * b;
          }
        }
      }
      c++;
    }
    if(c < height()) return 0;
    else return x;
  }
  T det(int n = -1) const {
    assert(n <= min(height(),width()));
    if(n == -1) n = min(height(),width());
    Matrix B(n,n);
    for(int i = 0; i < n; i++)for(int j = 0; j < n; j++) B[i][j] = (*this)[i][j];
    return B.Gaussian_elimination(false);
  }
  Matrix inv() const {
    assert(height() == width());
    int n = height();
    Matrix B(n,2*n);
    for(int i = 0; i < n; i++)for(int j = 0; j < n; j++) B[i][j] = (*this)[i][j];
    for(int i = 0; i < n; i++) B[i][i+n] = 1;
    assert(B.Gaussian_elimination() != 0);
    Matrix ret(n,n);
    for(int i = 0; i < n; i++)for(int j = 0; j < n; j++) ret[i][j] = B[i][j+n];
    return ret;
  }
};

}  // namespace soryuusi
