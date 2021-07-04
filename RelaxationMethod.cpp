// RelaxationMethod.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include <ctime>

#include "LinalGenerator.h"
#include "RelaxationMethod.h"

#include <exception>


template<typename T>
void printVector(const std::vector<T>& vec, std::ostream& out = std::cout)
{
  for (const auto& elem : vec) {
    out << elem << ' ';
  }
}

template<typename T>
void printMatrix(const std::vector<std::vector<T>>& matr, std::ostream& out = std::cout)
{
  for (const auto& vec : matr) {
    std::for_each(vec.begin(), vec.end(), [&out](const auto& elem) {out << elem << ' ';});
    out << '\n';
  }
}

int getPositiveInt(const std::string message)
{
  while (true) {
    int number = 0;
    std::cout << message;
    std::cin >> number;
    if (std::cin.fail() || number < 0) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "invalid value, positive number was expected\n";
    }
    else {
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      return number;
    }
  }
}

void openFiles(std::map<int, std::ofstream>& runTimeInThreads_f, const std::vector<int>& arrayOfNumThreads)
{
  for (const auto& numThreads : arrayOfNumThreads) {
    std::string name;
    name.reserve(25);
    name += "runtimeWith";
    name += std::to_string(numThreads);
    name += "Threads.txt";
    runTimeInThreads_f[numThreads] = std::ofstream(name);
  }
}

void uploadDataOfTests(const int nEquations, std::ostream& nEquations_f, 
  std::map<int, double>& runTimeInThreads, std::map<int, std::ofstream>& runTimeInThreads_f)
{
  nEquations_f << nEquations << '\n';

  for (auto& elem : runTimeInThreads_f) {
    elem.second << runTimeInThreads[elem.first] << '\n';
  }
}

using elem_t = double; // тип элемента системы, должен быть как минимум вещественным

int main()
{
  try {
    // параметры для исполнения тестов
    const int initialN = getPositiveInt("enter initial amount of equations in a system: ");
    const int stepOfIncreasingN = getPositiveInt("enter step of increasing amount of equations in a system: ");
    const int maxN = getPositiveInt("enter max amount of equations in a system: ");
    const int numSystemsInEachN = getPositiveInt("enter amount of systems in each dimension: ");
    std::cout << '\n';
    const std::vector<int> arrayOfNumThreads = { 1,2,4,8,12,16 }; // на каком кол - ве потоков тестируем


    // после исполнения берем данные для графиков из этих файлов
    std::ofstream nEquations_f("nEquations.txt");
    std::map<int, std::ofstream> runTimeInThreads_f; // ассоциация между кол-вом потоков и файлом со временем исполнения в них
    openFiles(runTimeInThreads_f, arrayOfNumThreads);


    clock_t startTime = clock();
    for (int nEquations = initialN;nEquations <= maxN;nEquations += stepOfIncreasingN) {

      // Ax = b
      std::vector<std::vector<elem_t>> A(nEquations, std::vector<elem_t>(nEquations));
      std::vector<elem_t> x(nEquations); // вектор приближенного решения СЛАУ
      std::vector<elem_t> b(nEquations);

      std::map<int, double> runTimeInThreads; // ассоциация между кол-вом потоков и временем исполнения в них
      for (int i = 0;i < numSystemsInEachN;++i) {
        // генерация СЛАУ
        omp_set_num_threads(omp_get_num_procs());
        generateGoodConditionedMatrix<elem_t>(A);
        std::vector<elem_t> accurateX(nEquations); // точное решение СЛАУ
        generateVector<elem_t>(accurateX);
        b = A * accurateX;

        elem_t omega = 0.5;                  // фактор релаксации [0, 2]
        std::cout << "enter omega: ";
        std::cin >> omega;
        const elem_t residualConvergence = 1e-5;   // точность вычислений, чем выше точность тем больше итераций

        for (const auto& numThreads : arrayOfNumThreads) { // прогоняем метод на разном количествe потоков
          omp_set_num_threads(numThreads);
          clock_t startTime = clock();
          relaxationMethod(A, x, b, omega, residualConvergence);
          runTimeInThreads[numThreads] += static_cast<double>(clock()) - startTime;

          // напечатать СЛАУ для проверки решения
#if 0
          std::cout << '\n';
          std::cout << "A:\n";
          printMatrix(A);
          std::cout << '\n';
#endif
#if 0
          std::cout << "accurateX: ";
          printVector(accurateX);
          std::cout << '\n';
          std::cout << '\n';
          std::cout << "x: ";
          printVector(x);
          std::cout << '\n';
#endif
#if 0
          std::cout << '\n';
          std::cout << "b: ";
          printVector(b);
          std::cout << '\n';
          std::cout << "------------------------------\n";
#endif

          x.assign(nEquations, elem_t());  // сбрасываем решение до начального приближения для решения на другом количестве потоков
        }
      }
      std::for_each(runTimeInThreads.begin(), runTimeInThreads.end(), 
        [numSystemsInEachN](auto& elem) {elem.second /= numSystemsInEachN;});
      uploadDataOfTests(nEquations, nEquations_f, runTimeInThreads, runTimeInThreads_f);
      std::cout << nEquations << " / " << maxN << " is ready\n";
    }
    std::cout << "general runtime: " << (static_cast<double>(clock()) - startTime) / CLOCKS_PER_SEC << "secs\n\n";
    std::cout << "txt files are ready!\n";
  }
  catch (std::exception& e) {
    std::cout << e.what();
  }
  catch (...) {
    std::cout << "unknown error...";
  }

  // скрипт для записи данных в excel
  system("python moveDataToExcelScript.py");

  return 0;
}
