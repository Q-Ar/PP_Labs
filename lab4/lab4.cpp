#include <iostream>
#include <cstdlib>
#include <ctime>
#include <../../../../../../Program Files (x86)/Microsoft SDKs/MPI/Include/mpi.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        int studentsCount = 9;
        int totalScore = 0;

        for (int i = 1; i < size; ++i) {
            int studentNumber;
            MPI_Recv(&studentNumber, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int score = std::rand() % 4 + 2;
            std::cout << "Student " << studentNumber << " received score: " << score << std::endl;

            MPI_Send(&score, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            totalScore += score;
        }

        double averageScore = static_cast<double>(totalScore) / studentsCount;
        std::cout << "Average score: " << averageScore << std::endl;
    }
    else {
        int studentNumber = rank;
        MPI_Send(&studentNumber, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        int receivedScore;
        MPI_Recv(&receivedScore, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Finalize();
    return 0;
}
