#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <regex.h>
using namespace std;

template<typename T>
	std::string to_string(T value)
	{
		std::stringstream stream;
		stream << value;
		return stream.str();
	}
//чтобы to_string работал

int main(int argc, char **argv) {
	int rank, size;
    //const string MINISATMODPATH = "/home/vkondratev/data/int/minisatmod/minisatmod";
    //const string MINISATPATH = "/home/vkondratev/data/int/minisat/minisat";
    const string MINISATPATH = "/home/vkondratev/data/int/maple/maple";
    const string MINISATMODPATH = "/home/vkondratev/data/int/maplemod/maplemod";
    const string HCSMPATH = "/home/vkondratev/data/int/NEFindingRep.py";
    const string HCMPATH = "/home/vkondratev/data/int/FindingRep.py";
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //собственно наши тестовые кнф
    std::string cnfs[4] = {
      		"10.cnf",
      		"11.cnf",
      		"12.cnf",
      		"13.cnf"
      		//"12.cnf"
      };


    			string scmd15 = MINISATPATH + " -cpu-lim=100000 " + cnfs[rank] + " > Before/before_" + cnfs[rank];
    			const char *cmd15 = scmd15.c_str();
    			system(cmd15); //запустили оригинальный тест для проверки

//printf("1");
MPI_Barrier(MPI_COMM_WORLD);

    		    string scmd1 = MINISATMODPATH + " -cpu-lim=120 " + cnfs[rank] + " > dizjunkts_for_" + cnfs[rank];
    		   	const char *cmd1 = scmd1.c_str();
    		    system(cmd1); //собрали дизъюнкты

MPI_Barrier(MPI_COMM_WORLD);

            string scmd3 = HCMPATH + " dizjunkts_for_" + cnfs[rank] + " > repeats_" + cnfs[rank];
            const char *cmd3 = scmd3.c_str();
            system(cmd3); //запустили для всех выводов решателя хэшклозес (через
                    //регулярку в питоне), но теперь отсеяли повторяющиеся дизъюнкты

MPI_Barrier(MPI_COMM_WORLD);

    		    string removedizj = "dizjunkts_for_" + cnfs[rank];
    			const char *remdiz = removedizj.c_str();
    			remove(remdiz);//удалили файлы с дизъюнктами

MPI_Barrier(MPI_COMM_WORLD);

            string scmd4 = HCSMPATH + " repeats_" + cnfs[rank] + " > repeats_unic_" + cnfs[rank];
            const char *cmd4 = scmd4.c_str();
            system(cmd4); //запустили для всех выводов решателя хэшклозес (через
                    //регулярку в питоне), отсеяли все уникальные дизъюнкты

MPI_Barrier(MPI_COMM_WORLD);

    		    string removehash = "repeats_" + cnfs[rank];
    			const char *remhas = removehash.c_str();
    			remove(remhas);//удалили файлы после сортировки уникальных

MPI_Barrier(MPI_COMM_WORLD);

    			string cat = "cat "+ cnfs[rank] + " repeats_unic_" + cnfs[rank] + " > new_" + cnfs[rank];
    			const char *catcat = cat.c_str();
    			system(catcat);

MPI_Barrier(MPI_COMM_WORLD);

    		    string removerep = "repeats_unic_" + cnfs[rank];
    			const char *remrep = removerep.c_str();
    			remove(remrep);//удалили файлы после сортировки поторов


    MPI_Barrier(MPI_COMM_WORLD);


    		    string scmd2 = MINISATPATH + " -cpu-lim=100000 new_" + cnfs[rank] + " > After/after_" + cnfs[rank];
    		    const char *cmd2 = scmd2.c_str();
    		    system(cmd2); //запустили с дизъюнктами тест для проверки

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
