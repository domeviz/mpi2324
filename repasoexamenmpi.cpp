#include <iostream>
#include <mpi.h>
#include <vector>
#include <chrono>
#include <functional>
#include <fstream>
#include <math.h>

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

int max(int *vector, const int n)
{
    int max = 0;
    for (int i = 0; i < n; i++)
    {
        if (vector[i] > max)
            max = vector[i];
    }
    return max;
}

int min(int *vector, const int n)
{
    int min = 0;
    for (int i = 0; i < n; i++)
    {
        if (vector[i] < min)
            min = vector[i];
    }
    return min;
}


int main( int argc, char *argv[]){

    int rank, nprocs;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //Tamaño del bloque
    int block_size;
    //Tamaño del vector (datos)
    int real_size;

    //Para los datos que procesa el ultimo 
    int padding=0;

    //Crear variables para los datos y variables temporales para cada rank
    //Crear un vector donde van a estar todos los datos que va a recibir
    std::vector<int> datitos;

    //Para todos los ranks
    std::vector<int> datitos_local;

    //Se leen los datos en el rank  0
    if(rank==0){
       datitos=read_file();
       int datitos_tam=datitos.size();
       //primero asignamos
       real_size=datitos_tam;
       block_size=real_size/nprocs;

       if(datitos_tam%nprocs!=0){
            real_size=std::ceil((double)datitos_tam/nprocs)*nprocs;
            block_size=real_size/nprocs;
            padding=real_size-datitos_tam;
       }
       std::printf("RANK_%d, Dimension: %d, real_size: %d, block_size: %d, padding: %d\n",
                    rank, datitos_tam, real_size, block_size, padding);
    }

    //enviamos tam del bloque, 1 dato, entero, del rank 0, todos
    MPI_Bcast( &block_size , 1 , MPI_INT , 0 , MPI_COMM_WORLD);

//Punto a punto
    if(rank==0){
        //enviamos puntero del padding, 1 dato, entero, para el rank ultimo, desde el rank 0, todos
        MPI_Send( &padding , 1 , MPI_INT , nprocs-1 , 0 , MPI_COMM_WORLD);
    } else if(rank==nprocs-1){
        MPI_Recv( &padding , 1 , MPI_INT , 0 , MPI_ANY_TAG , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
    }

    //Resize con el tamaño que va a recibir
    datitos_local.resize(block_size);

    //de donde envio, cuantos datos, tipo, donde recibo, cuantos, tipo, de que rank, todos
    MPI_Scatter( datitos.data() , block_size , MPI_INT ,
     datitos_local.data() , block_size , MPI_INT , 0 , MPI_COMM_WORLD);

    int new_size=block_size;

    //si el rank es el ultimo
    if(rank==nprocs-1){
        //tamaño del ultimo bloque
        new_size=block_size-padding;
    }

    int min_local=min(datitos_local.data(),new_size);
    int max_local=max(datitos_local.data(),new_size);

    int min_general;
    int max_general;

    //puntero del que envia, puntero del que recibe, cantidad de datos que envia el rank,
    //tipo de dato, OPERACION, va al rank 0, comm
    MPI_Reduce( &min_local, &min_general , 1 , MPI_INT , MPI_MIN , 0 , MPI_COMM_WORLD);
    
    MPI_Reduce( &max_local, &max_general , 1 , MPI_INT , MPI_MAX , 0 , MPI_COMM_WORLD);

    if(rank == 0){
        std::printf("Minimo %d, Maximo %d\n", min_general, max_general);
    }
    MPI_Finalize();
    
    return 0;
//BASE
    // int rank, nprocs;
    // MPI_Init(&argc, &argv);

    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    // //Tamaño del bloque
    // int block_size;
    // //Tamaño del vector (datos)
    // int real_size;

    // //Para los datos que procesa el ultimo 
    // int padding=0;

    // //Crear variables para los datos y variables temporales para cada rank
    // //Crear un vector donde van a estar todos los datos que va a recibir
    // std::vector<int> datitos;

    // //Para todos los ranks
    // std::vector<int> datitos_local;

    // //Se leen los datos en el rank  0
    // if(rank==0){
       
    // }

    // MPI_Finalize();
    
    // return 0;
}