#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <regex.h>
#include <ctime>
using namespace std;

template<typename T>
	std::string to_string(T value)
	{
	std::stringstream stream;
	stream << value;
	return stream.str();
	}

int main(int argc, char **argv) {
    int rank, size;
    int timecpu, cnfs, number;
    string cnf;
    ofstream out;
    const string MINISATMODPATH = "/home/vkondratev/data/int/minisatmod/minisatmod";
    const string MINISATPATH = "/home/vkondratev/data/int/minisat/minisat";
    const string HCSMPATH = "/home/vkondratev/data/int/NEFindingRep.py";
    const string HCMPATH = "/home/vkondratev/data/int/FindingRep.py";
    const string CLAUSEDEC = "/home/vkondratev/data/int/clause_decompose";
    timecpu = atoi(argv[1]);
    cnf = argv[2];
    //number = atoi(argv[3]);
    MPI_Init(&argc, &argv);
    ////Количество процессов в этом коммуникаторе5.80357e+08
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    //Ранг процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double runtime = MPI_Wtime();
    string nameM = "minisatout" + to_string(rank);
    string nameSH = "hashout" + to_string(rank);
    string nameHSM = "hashmodout" + to_string(rank);
    string nameHSM1 = "hashmodoutun" + to_string(rank);

srand(rank*time(NULL));
string rndfreq = "-rnd-freq=" + to_string(((float)rand()+1)/((float)RAND_MAX+2));
//string scmd = MINISATMODPATH + " " + rndfreq + " -cpu-lim=" + to_string(timecpu) + " sha_" + to_string(cnfs) + "_steps_inverse_test_" + to_string(number) + ".cnf > " + nameM;
string scmd = MINISATMODPATH + " " + rndfreq + " -cpu-lim=" + to_string(timecpu) + " " + cnf + " > " + nameM;
    	const char *cmd = scmd.c_str();
    	system(cmd); //запустили решатель в первый раз на каждом процессоре для
    			 	 //сбора базы ограничений
    	string scmd1 = HCSMPATH + " " + nameM + " > " + nameSH;
    	const char *cmd1 = scmd1.c_str();
		system(cmd1); //запустили для всех выводов решателя хэшклозес (через
					  //регулярку в питоне), отсеяли все уникальные дизъюнкты
		string scmd3 = HCMPATH + " " + nameM + " > " + nameHSM;
		const char *cmd3 = scmd3.c_str();
		system(cmd3); //запустили для всех выводов решателя хэшклозес (через
					  //регулярку в питоне), но теперь отсеяли повторяющиеся дизъюнкты
		string scmd5 = HCSMPATH + " " + nameHSM + " > " + nameHSM1;
		const char *cmd5 = scmd5.c_str();
		system(cmd5); 	//для этих отсеянных повторяющихся запустили хэшклозес,
						//отсеивающий уникальные, чтобы каждый повтор остался
						//по одному разу в базе (иначе проверка повторов между
						//база не сработает)
			MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0){ //главный процесс соединяет все базы с повторяющимися
						//дизъюнктами разных процессов в одну
    		string scmd4 = "cat";
    		for (int i = 0; i<size; i++){
    			string cathcm = " hashmodoutun" + to_string(i);
    			scmd4 = scmd4 + cathcm;
    		}
    		scmd4 = scmd4 + " > repeats";
    		const char *cmd4 = scmd4.c_str();
    		system(cmd4);
    }



			MPI_Barrier(MPI_COMM_WORLD);
	    if (rank == 0){
		//string scmd6 = CLAUSEDEC + " repeats sha_" + to_string(cnfs) + "_steps_inverse_test_" + to_string(number) + ".cnf";
	    string scmd6 = CLAUSEDEC + " repeats " + cnf + " " + to_string(size);	
		const char *cmd6 = scmd6.c_str();
		system(cmd6); 
		//string scmd17 = "cat " + nameHSM1 + " sha_" + to_string(cnfs) + "_steps_inverse_test_" + to_string(number) + ".cnf > sha_" + to_string(cnfs) + "_steps_inverse_test_" + to_string(number) + "_" + to_string(size) + ".cnf";
		//string scmd17 = "cat " + nameHSM1 + " MD4_implication_vars_" + to_string(cnfs) + ".cnf > MD4_implication_vars_" + to_string(cnfs) + "_" + to_string(size-1) + ".cnf";
		//const char *cmd17 = scmd17.c_str();
		//system(cmd17); 
		remove("freq_file");
			remove("stat");
			remove("split_stat");
			remove("str_len_file");
			remove("repeats");
			remove("nameunic");
    }
    
        	MPI_Barrier(MPI_COMM_WORLD);
    		string namehere1 = "hashout" + to_string(rank);
			const char *nh1 = namehere1.c_str();
			remove(nh1);
			string namehere2 = "hashmodout" + to_string(rank);
			const char *nh2 = namehere2.c_str();
			remove(nh2);
			string namehere7 = "hashmodoutun" + to_string(rank);
			const char *nh7 = namehere7.c_str();
			remove(nh7);
			string namehere3 = "minisatout" + to_string(rank);
			const char *nh3 = namehere3.c_str();
			remove(nh3);
    		MPI_Barrier(MPI_COMM_WORLD);

    	//string scmd20 = MINISATPATH + " sha_" + to_string(cnfs) + "_steps_inverse_test_" + to_string(number) + "_" + to_string(rank) + ".cnf > result_sha_" + to_string(cnfs) + "_steps_inverse_test_" + to_string(number) + "_" + to_string(rank);
		cnf.erase(cnf.end() - 4, cnf.end());
		string scmd20 = MINISATPATH + " " + cnf + "_" + to_string(rank) + ".cnf > result_"  + to_string(rank) + "_" + cnf;
		const char *cmd20 = scmd20.c_str();
		system(cmd20); 
		cout << "rank = " << rank << " runtime = " << MPI_Wtime() - runtime << endl;
	
	MPI_Abort(MPI_COMM_WORLD,MPI_ERR_OTHER );
    MPI_Finalize();
    return 0;
}