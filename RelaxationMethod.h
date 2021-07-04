#pragma once
#ifndef RELAXATION_METHOD_H
#define RELAXATION_METHOD_H

#include <iostream>
#include <vector>
#include <omp.h>


template<typename T>
std::vector<T> operator-(const std::vector<T>& vec1, const std::vector<T>& vec2)
{
  const int size = vec1.size();
  std::vector<T> result(size);

#pragma omp parallel for
  for (int i = 0;i < size;++i) {
    result[i] = vec1[i] - vec2[i];
  }

  return result;
}

template<typename T>
std::vector<T> operator*(const std::vector<std::vector<T>>& matr, const std::vector<T>& vec)
{
  const int n = matr.size();
  const int m = matr[0].size();
  std::vector<T> result(n);

#pragma omp parallel for
  for (int i = 0;i < n;++i) {
    const auto& row = matr[i];
    T sum = 0;
    for (int j = 0; j < m; ++j) {
      sum += row[j] * vec[j];
    }
    result[i] = sum;
  }

  return result;
}

template<typename T>
T euclideanNorm(const std::vector<T>& vec)
{
  const int size = vec.size();
  T norm = 0;

#pragma omp parallel for reduction(+: norm)
  for (int i = 0;i < size;++i) {
    norm += vec[i] * vec[i];
  }

  return sqrt(norm); 
}

template<typename T>
void relaxationMethod(const std::vector<std::vector<T>>& A, std::vector<T>& x, const std::vector<T>& b,
  const T omega, const T residualConvergence)
{
  const int size = A.size();

  // запоминаем повторяющиеся вычисления
  T oneMinusOmega = 1 - omega;
  std::vector<T> omegaDividedByA(size);
#pragma omp parallel for
  for (int i = 0;i < size;++i) {
    omegaDividedByA[i] = omega / A[i][i];
  }
  //

  //int k = 0;
#if 1
  while (euclideanNorm(A * x - b) > residualConvergence) { 
#pragma omp parallel for
    for (int i = 0;i < size;++i) {
      const auto& row = A[i];
      T sigma = 0;
      for (int j = 0;j < size;++j) {
        if (i != j) {
          sigma += row[j] * x[j];
        }
      }
      x[i] = oneMinusOmega * x[i] + omegaDividedByA[i] * (b[i] - sigma);
    }
    //++k;
  }
  //std::cout << "k: " << k << '\n';
#endif
#if 0
  while (euclideanNorm(A * x - b) > residualConvergence) { 
    for (int i = 0;i < size;++i) {
      const auto& row = A[i];
      T sigma = 0;
#pragma omp parallel for reduction(+: sigma)
      for (int j = 0;j < size;++j) {
        if (i != j) {
          sigma += row[j] * x[j];
        }
      }
      x[i] = oneMinusOmega * x[i] + omegaDividedByA[i] * (b[i] - sigma);
    }
  }
#endif
}

#endif // !RELAXATION_METHOD_H