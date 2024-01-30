#include <iostream>
#include <fstream>
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
 
std::vector<int> read_file() {
    std::fstream fs("./datos.txt", std::ios::in);
    std::string line;
    std::vector<int> ret;
 
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
 
    fs.close();
    return ret;
}
 
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
 
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
 
    std::vector<int> datos;
    int tam;
 
    if (rank == 0) {
        datos = read_file();
        tam = datos.size();
    }

    MPI_Bcast(&tam, 1, MPI_INT, 0, MPI_COMM_WORLD);
 
    std::vector<int> datosp(tam / nprocs);
    MPI_Scatter(datos.data(), tam / nprocs, MPI_INT,
                datosp.data(), tam / nprocs, MPI_INT, 0, MPI_COMM_WORLD);
 
    std::vector<int> frecuenciap(100, 0);
    for (int i = 0; i < datosp.size(); ++i) {
        frecuenciap[datosp[i]]++;
    }
 
    std::vector<int> frecuencia(100, 0);
    MPI_Reduce(frecuenciap.data(), frecuencia.data(), 101, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
 
    int minimo, maximo;

    int minimop = *std::min_element(datosp.begin(), datosp.end());
    int maximop = *std::max_element(datosp.begin(), datosp.end());
 
    MPI_Reduce(&minimop, &minimo, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&maximop, &maximo, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
 
    int sumaparcial = std::accumulate(datosp.begin(), datosp.end(), 0);
 
    int suma;
    MPI_Reduce(&sumaparcial, &suma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
 
    double promedio = static_cast<double>(suma) / tam;
 
    if (rank == 0) {
        std::printf("+---------+-----------+\n");
        std::printf("|  Valor  |   Conteo  |\n");
        std::printf("+---------+-----------+\n");
        for (int i = 0; i <= 100; ++i) {
            std::printf("|  %-6d |   %-6d  |\n", i, frecuencia[i]);
        }
        std::printf("+---------+-----------+\n");
 
    std::printf("Promedio: %.2f\n", promedio);
        std::printf("Valor Mínimo: %d\n", minimo);
        std::printf("Valor Máximo: %d\n", maximo);
        
    }
 
    MPI_Finalize();
    return 0;
}