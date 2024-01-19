#include <iostream>
#include <mpi.h>

template<typename T>
T sumar(T *tmp, int n) {
    T suma = 0;
    for (int i = 0; i < n; i++) {
        suma = suma + tmp[i];
    }
    return suma;
}

template<typename T>
void enviarDatos(T *data, int start, int size, int dest) {
    MPI_Send(&data[start], size, MPI::Type<T>(), dest, 0, MPI_COMM_WORLD);
}

template<typename T>
void recibirDatos(T *data, int size, int source) {
    MPI_Recv(data, size, MPI::Type<T>(), source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    std::printf("Rank %d of %d procs\n", rank, nprocs);

    const int dataSize = 100;
    int data[dataSize];

    if (rank == 0) {
        std::printf("total de ranks:%d\n", nprocs);
        for (int i = 0; i < dataSize; i++) {
            data[i] = i;
        }

        for (int rank_id = 1; rank_id < nprocs; rank_id++) {
            std::printf("RANK_0 enviando datos a RANK_%d \n", rank_id);
            int start = rank_id * 25;
            enviarDatos<int>(data, start, 25, rank_id);
        }

        int suma_ranks[4];
        suma_ranks[0] = sumar<int>(data, 25);

        for (int rank_id = 1; rank_id < nprocs; rank_id++) {
            recibirDatos<int>(&suma_ranks[rank_id], 1, rank_id);
        }

        std::printf("sumas parciales: %d, %d, %d, %d\n", suma_ranks[0], suma_ranks[1], suma_ranks[2], suma_ranks[3]);
        int suma_total = sumar<int>(suma_ranks, 4);
        std::printf("SUMA TOTAL: %d\n", suma_total);
    } else {
        std::printf("RANK_%d recibiendo datos\n", rank);
        recibirDatos<int>(data, 25, 0);

        int suma_parcial = sumar<int>(data, 25);
        std::printf("RANK_%d enviando suma parcial %d\n", rank, suma_parcial);
        enviarDatos<int>(&suma_parcial, 1, 1, 0);
    }

    MPI_Finalize();
    return 0;
}
