#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <numeric>
#include <fstream>

// Функция для генерации случайных чисел в диапазоне от 0 до 100
int getRandomNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);
    return dis(gen);
}

// Функция для заполнения вектора случайными числами
void fillVector(std::vector<int>& vec, int size) {
    for (int i = 0; i < size; ++i) {
        vec.push_back(getRandomNumber());
    }
}

// Функция для вычисления скалярного произведения векторов в заданном диапазоне
int calculateDotProduct(const std::vector<int>& vecA, const std::vector<int>& vecB, int start, int end) {
    return std::inner_product(vecA.begin() + start, vecA.begin() + end, vecB.begin() + start, 0);
}

int main() {
    // Размеры векторов для тестирования
    std::vector<int> vectorSizes = {10000, 100000, 1000000, 10000000 };
    // Вектор для хранения времени выполнения
    std::vector<double> executionTimes;
    // Открываем файл для записи времени выполнения
    std::ofstream outputFile("execution_times.txt");

    // Проход по различным размерам векторов
    for (int size : vectorSizes) {
        std::vector<int> vectorA, vectorB;
        fillVector(vectorA, size);
        fillVector(vectorB, size);

        int numThreads = 10;
        std::vector<std::thread> threads;
        std::vector<int> results(numThreads);

        auto start = std::chrono::high_resolution_clock::now();
        results[0] = calculateDotProduct(vectorA, vectorB, 0, size / numThreads);

        // Разделение вычислений между потоками
        int chunkSize = size / numThreads;
        for (int i = 0; i < numThreads; ++i) {
            int startIdx = i * chunkSize;
            int endIdx = (i == numThreads - 1) ? size : (i + 1) * chunkSize;
            // Создание потоков для вычисления скалярного произведения в своем диапазоне
            threads.emplace_back([&, startIdx, endIdx, i]() {
                results[i] = calculateDotProduct(vectorA, vectorB, startIdx, endIdx);
                //std::thread::id threadId = std::this_thread::get_id();
                //std::cout << "Thread " << i << " ID: " << threadId << std::endl;
                });
        }

        // Ожидание завершения всех потоков
        for (auto& thread : threads) {
            thread.join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        // Вычисление общего скалярного произведения из частей, вычисленных в потоках
        int dotProduct = std::accumulate(results.begin(), results.end(), 0);

        // Сохранение времени выполнения для данного размера вектора
        executionTimes.push_back(duration.count());

        // Вывод времени выполнения и скалярного произведения
        std::cout << "Vector size: " << size << " Time taken: " << duration.count() << " seconds\n";
        std::cout << "Dot product: " << dotProduct << std::endl;
        // Вывод времени выполнения в файл
        outputFile << size << " " << duration.count() << std::endl;
    }

    // Закрываем файл после записи
    outputFile.close();

    return 0;
}
