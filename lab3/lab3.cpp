#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <numeric>
#include <fstream>
#include <omp.h>

int getRandomNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);
    return dis(gen);
}

void fillVector(std::vector<int>& vec, int size) {
    for (int i = 0; i < size; ++i) {
        vec.push_back(getRandomNumber());
    }
}

int calculateDotProduct(const std::vector<int>& vecA, const std::vector<int>& vecB, int start, int end) {
    return std::inner_product(vecA.begin() + start, vecA.begin() + end, vecB.begin() + start, 0);
}

int main() {
    std::vector<int> vectorSizes = { 10000, 100000, 1000000, 10000000 };
    std::vector<double> executionTimes;
    std::ofstream outputFile("execution_times.txt");

    for (int size : vectorSizes) {
        std::vector<int> vectorA, vectorB;
        fillVector(vectorA, size);
        fillVector(vectorB, size);

        int numThreads = 10;
        std::vector<int> results(numThreads);

        auto start = std::chrono::high_resolution_clock::now();

        #pragma omp parallel num_threads(numThreads)
        {
            int threadID = omp_get_thread_num();
            int chunkSize = size / numThreads;
            int startIdx = threadID * chunkSize;
            int endIdx = (threadID == numThreads - 1) ? size : (threadID + 1) * chunkSize;

            results[threadID] = calculateDotProduct(vectorA, vectorB, startIdx, endIdx);
        }

        int dotProduct = std::accumulate(results.begin(), results.end(), 0);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        executionTimes.push_back(duration.count());

        std::cout << "Vector size: " << size << " Time taken: " << duration.count() << " seconds\n";
        std::cout << "Dot product: " << dotProduct << std::endl;

        outputFile << size << " " << duration.count() << std::endl;
    }

    outputFile.close();

    return 0;
}
