#include <iostream>
#include "simulation.hpp"

#include <random>
// #include <Python.h>
#include <ctime>
#include <chrono>
#include <thread>
#include <fstream>

std::mutex fmutex;
std::mutex tmutex;

integer total_run = 5000;
std::mt19937 mt(time(0));

integer pool_size = 2;

integer N  		= 12;
integer Nr 		= 4;
integer Np 		= 4;
integer Ns 		= 4;
real 	s  		= 1.01;
real 	beta	= 0.01;			//0.01,0.02,0.04,0.06,0.10,0.15,0.20,0.25,0.30, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.25, 1.5, 1.75, 2.0, 2.5, 3.0


void exec_exp(int No, std::string exp_type) {
	std::vector<State> ext_state;
	std::vector<State> fix_state;

	for ( ; ; ) {

		tmutex.lock();
		if (total_run) 	{total_run -= 1; 	tmutex.unlock();}
		else 			{tmutex.unlock();  	break;}
		
		if (exp_type == std::string("WMF")) {
			WellMixedFermi n (N, Nr, Np, Ns, s, mt(), fmutex, beta);
			n.simulation();
			ext_state.emplace_back(n.ext_state);
			fix_state.emplace_back(n.fix_state);
		}
		else if (exp_type == std::string("WML")) {
			WellMixedLocalUpdate n (N, Nr, Np, Ns, s, mt(), fmutex, beta);
			n.simulation();
			ext_state.emplace_back(n.ext_state);
			fix_state.emplace_back(n.fix_state);
		}
		else if (exp_type == std::string("WMAI")) {
			WellMixedArctanImitation n (N, Nr, Np, Ns, s, mt(), fmutex, beta);
			n.simulation();
			ext_state.emplace_back(n.ext_state);
			fix_state.emplace_back(n.fix_state);
		}
		else if (exp_type == std::string("WMTH")) {
			WellMixedTanhImitation n (N, Nr, Np, Ns, s, mt(), fmutex, beta);
			n.simulation();
			ext_state.emplace_back(n.ext_state);
			fix_state.emplace_back(n.fix_state);
		}

		// WellMixedFermi n (N, Nr, Np, Ns, s, mt(), fmutex, beta);
		// n.simulation();

		// ext_state.emplace_back(n.ext_state);
		// fix_state.emplace_back(n.fix_state);
	}


	char mn[200];
	sprintf(mn, "-%lld-%lld-%lld-%lld-%.8f-%.8f", N, Nr, Np, Ns, s, beta);
	std::string fix_file_name = std::string("logs/fix-") + exp_type + std::string(mn);
	std::string ext_file_name = std::string("logs/ext-") + exp_type + std::string(mn);

	std::lock_guard<std::mutex> lock(fmutex);
	std::ofstream fix_file(fix_file_name, std::ios::app);
	std::ofstream ext_file(ext_file_name, std::ios::app);

	char state_str[100];

	for (integer i = 0; i < ext_state.size(); i++) {
    	sprintf(state_str, "Step: %8lld, Num_R: %5lld, Num_P: %5lld, Num_S: %5lld\n", ext_state[i].T, ext_state[i].Nr, ext_state[i].Np, ext_state[i].Ns);
		ext_file << state_str;

		sprintf(state_str, "Step: %8lld, Num_R: %5lld, Num_P: %5lld, Num_S: %5lld\n", fix_state[i].T, fix_state[i].Nr, fix_state[i].Np, fix_state[i].Ns);
		fix_file << state_str;		
	}
}







int main(int argc, char *argv[]) {
	N = atoi(argv[1]);
	Nr = atoi(argv[2]);
	Np = atoi(argv[3]);
	Ns = atoi(argv[4]);
	s = atof(argv[5]);
	beta = atof(argv[6]);
	pool_size = atoi(argv[7]);
	std::string exp_type = std::string(argv[8]);
	total_run = atoi(argv[9]);
	
	std::cout << "N = " << N << ", Nr, Np, Ns = " << Nr << " " << Np << " " << Ns << ", s = " << s << ", beta = " << beta << ", pool_size = " << pool_size << ", exp_type = " << exp_type << std::endl;

	std::cout << mt() << std::endl;

	std::vector<std::thread> tpool;
	tpool.reserve(pool_size);

	for (std::size_t i = 0; i < pool_size; i++) {
		tpool.emplace_back(exec_exp, i, exp_type);
	}

	std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();

	while (true) {
		{
			std::lock_guard<std::mutex> lg1(tmutex);
			std::lock_guard<std::mutex> lg2(fmutex);
			if (total_run) {
				printf("Run Left is : %8lld\n", total_run);
			}
			else {
				break;
			}	
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	for (std::size_t i = 0; i < pool_size; i++) tpool[i].join();

	std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_used = std::chrono::duration_cast<std::chrono::duration<double>>(time_end-time_start);
	std::cout << "time use:" << time_used.count() << "s" << std::endl;
}





















// 	Py_Initialize();

// 	PyRun_SimpleString("import sys");
// 	PyRun_SimpleString("sys.path.append('./')");

// 	PyObject* pModule = PyImport_ImportModule("gen");

// 	if (!pModule)
// 	{
// 		std::cout << "Python get module failed." << std::endl;
// 		return 0;
// 	}

// 	PyObject* pv = PyObject_GetAttrString(pModule, "generate_random_regulax_graph");

// 	PyObject* args = PyTuple_New(2);   
//     PyObject* arg1 = PyLong_FromLong(10);
//     PyObject* arg2 = PyLong_FromLong(2);
//     PyTuple_SetItem(args, 0, arg1);
//     PyTuple_SetItem(args, 1, arg2);

// 	PyObject* pRet = PyObject_CallObject(pv, args);
// 	long data[10][10] = { 0 };

// 	if (PyList_Check(pRet)) {
// 		int SizeOfList = PyList_Size(pRet);

// 		for (int Index_i = 0; Index_i < SizeOfList; Index_i++) {
// 			PyObject * ListItem = PyList_GetItem(pRet, Index_i);		
// 			int NumOfItems = PyList_Size(ListItem);
// 			for (int Index_k = 0; Index_k < NumOfItems; Index_k++) {

// 				PyObject *Item = PyList_GetItem(ListItem, Index_k);

// 				data[Index_i][Index_k] = PyLong_AsLong(Item);
// 				Py_DECREF(Item);
// 			}
// 			Py_DECREF(ListItem);
// 		}
// 		std::cout << std::endl;
// 	}
// 	else 
// 	{
// 		std::cout << "Not a List" << std::endl;
// 	}

// 	for (int i = 0; i < 10; i++) {
// 		for (int j = 0; j < 10; j++) {
// 			printf("%3d", data[i][j]);
// 		}
// 		printf("\n");
// 	}

// 	Py_Finalize();