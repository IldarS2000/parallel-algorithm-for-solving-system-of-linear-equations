#pragma once
#ifndef LINAL_GENERATOR_H
#define LINAL_GENERATOR_H

#include <vector>
#include <random>
#include <omp.h>


std::random_device rd;
std::mt19937 generator(rd()); // состояние генератора изменяется из всех потоков :(, числа разные, но все равно костыль
// отрезок распределения -- опционален
const int A = -10;
const int B = 10;
const std::uniform_real_distribution<double> uniformDistribution(A, B);

template<typename T>
void generateVector(std::vector<T>& vec)
{
  const int size = vec.size();

#pragma omp parallel for
  for (int i = 0;i < size;++i) {
    vec[i] = uniformDistribution(generator);
  }
}

template<typename T>
void generateGoodConditionedMatrix(std::vector<std::vector<T>>& matrix)
{
  const int n = matrix.size();
  const int m = matrix[0].size();

#pragma omp parallel for
  for (int i = 0;i < n;++i) {
    auto& row = matrix[i];
    T sum = 0;
    for (int j = 0;j < m;++j) {
      if (i != j) {
        row[j] = uniformDistribution(generator);
        sum += std::fabs(row[j]);
      }
    }
    row[i] = sum + 1;
  }
}

#endif // LINAL_GENERATOR_H