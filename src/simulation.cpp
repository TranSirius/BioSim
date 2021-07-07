#include "simulation.hpp"
#include <fstream>


Network::Network(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex): 
    N(N), T(0), s(s), mt(srand), num_of_type(3), fmutex(file_mutex) {
        // printf("Init Network!\n");
        if (Nr + Np + Ns != N) {
            throw "Initialization Error!";
        }

        str2char[0] = 'R';
        str2char[1] = 'P';
        str2char[2] = 'S';

        Ndis[tr] = Nr; 
        Ndis[tp] = Np; 
        Ndis[ts] = Ns;

        pm[tr][tp] = pm[tp][ts] = pm[ts][tr] = -s;
        pm[tp][tr] = pm[ts][tp] = pm[tr][ts] = 1.;

        
        network = new bool*[N];
        for (int i = 0; i < N; i++) network[i] = new bool[N];
        for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) {
            network[i][j] = false;
        }

        strategy = new int[N];
        for (int i = 0; i < N; i++) {
            if (i < Nr) {
                strategy[i] = tr;
            }
            else if (i >= Nr && i < (Nr + Np)) {
                strategy[i] = tp;
            }
            else if (i >= (Nr + Np) && i < (Nr + Np + Ns)) {
                strategy[i] = ts;
            }
        }
    };

Network::~Network() {
    delete[] strategy;
    delete[] adj;
    for (integer i = 0; i < N; i++) {
        delete[] network[i];
    }
    delete[] network;
}

void Network::output(std::string output_file_name, integer exp_num) {
    std::lock_guard<std::mutex> lock(fmutex);
    char state_str[100];
    sprintf(state_str, "No.%9lld Step: %8lld, Num_R: %5lld, Num_P: %5lld, Num_S: %5lld\n", exp_num, T, Ndis[tr], Ndis[tp], Ndis[ts]);
    // printf("%s", state_str);
    std::ofstream file(output_file_name, std::ios::app);
    file << state_str;
    file.close();
}

void Network::repr() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%3d", network[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    for (int i = 0; i < N; i++) {
        printf("%c: ", str2char[strategy[i]]);
        for (auto j : adj[i]) {
            printf("%6lld%c", j, str2char[strategy[j]]);
        }
        printf("\n");
    }

}

void Network::gen_adj() {
    adj = new std::vector<integer>[N];
    for (int i = 0; i < N; i++) for (int j = i; j < N; j++) {
        if (network[i][j]) {
            adj[i].push_back(j);
            adj[j].push_back(i);
        }
    }
}

real Network::payoff(integer who) {
    int who_strg = strategy[who];
    real payoff = 0.0;
    for (auto nei : adj[who]) {
        int nei_strg = strategy[nei];
        payoff += pm[who_strg][nei_strg];
    }
    return payoff;
}

bool Network::transfer(integer who, int to_what) {
    int from_what = strategy[who];
    Ndis[from_what] -= 1;
    Ndis[to_what] += 1;
    strategy[who] = to_what;

    if (Ndis[from_what] == 0) return true;
    else return false;
}

void Network::report_ext() {
    // output(std::string("ext-") + model_name);
    ext_state.T = T;
    ext_state.Nr = Ndis[tr];
    ext_state.Np = Ndis[tp];
    ext_state.Ns = Ndis[ts];
}
void Network::report_fix() {
    // output(std::string("fix-") + model_name);
    fix_state.T = T;
    fix_state.Nr = Ndis[tr];
    fix_state.Np = Ndis[tp];
    fix_state.Ns = Ndis[ts];
}

void Network::simulation() {
    while(num_of_type > 1) {
        if (this -> update()) {
            if (num_of_type == 2) {
                report_ext();
            }
            else if (num_of_type == 1) {
                report_fix();
            }
        }
    }
}



// Graph Structure



WellMixed::WellMixed(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex) : Network(N, Nr, Np, Ns, s, srand, file_mutex) {
    // printf("Init WellMixed!\n");
    for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) {
        if (i != j) {
            network[i][j] = true;
        }
    }
    gen_adj();
}

// RegularGraph::RegularGraph(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, integer degree) : Network(N, Nr, Np, Ns, s, srand, file_mutex) {
// 	Py_Initialize();

// 	PyRun_SimpleString("import sys");
// 	PyRun_SimpleString("sys.path.append('./')");
// 	PyObject * pModule = PyImport_ImportModule("gen");
// 	if (!pModule)
// 	{
// 		std::cout << "Python get module failed." << std::endl;
//         throw "Error!";
// 	}

//     PyObject * pGenerator = PyObject_GetAttrString(pModule, "generate_random_regulax_graph");
    
//     PyObject * args = PyTuple_New(2);   
//     PyObject * arg_N = PyLong_FromLong(N);
//     PyObject * arg_k = PyLong_FromLong(degree);
//     PyTuple_SetItem(args, 0, arg_N);
//     PyTuple_SetItem(args, 1, arg_k);

//     PyObject * pRet = PyObject_CallObject(pGenerator, args);

//     for (int i = 0; i < N; i++) {
//         PyObject * pRow = PyList_GetItem(pRet, i);
//         for (int j = 0; j < N; j++) {
//             PyObject * pItem = PyList_GetItem(pRow, j);
//             long item = PyLong_AsLong(pItem);
//             if (item) {
//                 network[i][j] = true;
//             }
//             Py_DECREF(pItem);
//         }
//         Py_DECREF(pRow);
//     }
// 	Py_Finalize();

//     gen_adj();
// }

Lattice::Lattice(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex) : Network(N, Nr, Np, Ns, s, srand, file_mutex) {

}


// Dynamics, or update processes

Fermi::Fermi(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta) : 
    Network(N, Nr, Np, Ns, s, srand, file_mutex), fo_sam(0, N - 1), transfer_sam(0, 1), beta(beta) {}

bool Fermi::update() {
    T++;

    // integer threshold = 500;

    // if (T > threshold) {
    //     std::cout << "T = " << T << ", Nr = " << Ndis[tr] << ", Np = " << Ndis[tp] << ", Ns = " << Ndis[ts] << " ";
    // }
    
    integer fo = fo_sam(mt);
    integer mo;
    std::vector<integer> & fo_nei = adj[fo];
    
    ////////////////////////////////////////////////////////////
    // std::uniform_int_distribution<> mo_sam(0, fo_nei.size());
    // mo = mo_sam(mt);        // 看起来好像有点 BUG, 不过在 well-mix 情况下不影响，以后改
    // while(mo == fo) mo = mo_sam(mt);
    ////////////////////////////////////////////////////////////

    std::uniform_int_distribution<> mo_sam(0, fo_nei.size() - 1);
    mo = fo_nei[mo_sam(mt)];
    while(mo == fo) mo = fo_nei[mo_sam(mt)];

    real fo_pof = payoff(fo);
    real mo_pof = payoff(mo);
    real dif = fo_pof - mo_pof;
    real exp_dif = exp(beta * dif);

    real transfer_prob = 1 / (1 + exp_dif);
    real toll = transfer_sam(mt);


    // if (T > threshold) {
    //     // std::cout << "focus = " << fo << "," << strategy[fo] << ", model = " << mo << "," << strategy[mo] << std::endl;
    //     printf("focus = %3d, %c, %.3f, model = %3d, %c, %.3f | ", fo, str2char[strategy[fo]], fo_pof, mo, str2char[strategy[mo]], mo_pof);
    //     for (auto i : fo_nei) {
    //         printf("%c ", str2char[strategy[i]]);
    //     }
    //     std::cout << std::endl;
    // }

    if (toll < transfer_prob) {
        if (transfer(fo, strategy[mo])) {
            num_of_type -= 1;
            if (num_of_type == 2) {
                // printf("Extinction!\n");
                return true;
            }
            else if (num_of_type == 1) {
                // printf("Fixation!\n");
                return true;
            }
        }
    }
    return false;
}


LocalUpdate::LocalUpdate(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta) : 
    Network(N, Nr, Np, Ns, s, srand, file_mutex), fo_sam(0, N - 1), transfer_sam(0, 1), beta(beta) {
        if (s >= 1) max_pi = N * s; else max_pi = N * 1.;
}


bool LocalUpdate::update() {
    T++;
    
    integer fo = fo_sam(mt);
    integer mo;
    std::vector<integer> & fo_nei = adj[fo];

    std::uniform_int_distribution<> mo_sam(0, fo_nei.size() - 1);
    mo = fo_nei[mo_sam(mt)];
    while(mo == fo) mo = fo_nei[mo_sam(mt)];

    real fo_pof = payoff(fo);
    real mo_pof = payoff(mo);
    real dif = mo_pof - fo_pof;
    real transfer_prob = 0.5 + (beta * dif) / (2 * max_pi);
    real toll = transfer_sam(mt);

    if (toll < transfer_prob) {
        if (transfer(fo, strategy[mo])) {
            num_of_type -= 1;
            if (num_of_type == 2) return true;
            else if (num_of_type == 1) return true;
        }
    }
    return false;
}


ArctanImitation::ArctanImitation(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta) : 
    Network(N, Nr, Np, Ns, s, srand, file_mutex), fo_sam(0, N - 1), transfer_sam(0, 1), beta(beta) {}


bool ArctanImitation::update() {
    T++;
    
    integer fo = fo_sam(mt);
    integer mo;
    std::vector<integer> & fo_nei = adj[fo];

    std::uniform_int_distribution<> mo_sam(0, fo_nei.size() - 1);
    mo = fo_nei[mo_sam(mt)];
    while(mo == fo) mo = fo_nei[mo_sam(mt)];

    real fo_pof = payoff(fo);
    real mo_pof = payoff(mo);
    real dif = mo_pof - fo_pof;
    dif = dif * beta;
    real transfer_prob = (atan(dif) + M_PI / 2) / M_PI;
    
    real toll = transfer_sam(mt);

    if (toll < transfer_prob) {
        if (transfer(fo, strategy[mo])) {
            num_of_type -= 1;
            if (num_of_type == 2) {
                return true;
            }
            else if (num_of_type == 1) {
                return true;
            }
        }
    }
    return false;
}



TanhImitation::TanhImitation(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta) : 
    Network(N, Nr, Np, Ns, s, srand, file_mutex), fo_sam(0, N - 1), transfer_sam(0, 1), beta(beta) {}


bool TanhImitation::update() {
    T++;
    
    integer fo = fo_sam(mt);
    integer mo;
    std::vector<integer> & fo_nei = adj[fo];

    std::uniform_int_distribution<> mo_sam(0, fo_nei.size() - 1);
    mo = fo_nei[mo_sam(mt)];
    while(mo == fo) mo = fo_nei[mo_sam(mt)];

    real fo_pof = payoff(fo);
    real mo_pof = payoff(mo);
    real dif = mo_pof - fo_pof;
    dif = dif * beta;
    real transfer_prob = (1 + tanh(dif)) / 2;
    
    real toll = transfer_sam(mt);

    if (toll < transfer_prob) {
        if (transfer(fo, strategy[mo])) {
            num_of_type -= 1;
            if (num_of_type == 2) {
                return true;
            }
            else if (num_of_type == 1) {
                return true;
            }
        }
    }
    return false;
}



bool Moran::update() {
    T++;
}



// Experiments

WellMixedFermi::WellMixedFermi(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta, integer ExpNum):
    Network(N, Nr, Np, Ns, s, srand, file_mutex), WellMixed(N, Nr, Np, Ns, s, srand, file_mutex), Fermi(N, Nr, Np, Ns, s, srand, file_mutex, beta){
        No = ExpNum;
        char wn[100];
        sprintf(wn, "WMF-%lld-%lld-%lld-%lld-%.5f-%.5f", N, Nr, Np, Ns, s, beta);
        model_name = std::string(wn);
    }

WellMixedLocalUpdate::WellMixedLocalUpdate(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta, integer ExpNum):
    Network(N, Nr, Np, Ns, s, srand, file_mutex), WellMixed(N, Nr, Np, Ns, s, srand, file_mutex), LocalUpdate(N, Nr, Np, Ns, s, srand, file_mutex, beta){
        No = ExpNum;
        char wn[100];
        sprintf(wn, "WML-%lld-%lld-%lld-%lld-%.5f-%.5f", N, Nr, Np, Ns, s, beta);
        model_name = std::string(wn);
    }

WellMixedArctanImitation::WellMixedArctanImitation(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta, integer ExpNum):
    Network(N, Nr, Np, Ns, s, srand, file_mutex), WellMixed(N, Nr, Np, Ns, s, srand, file_mutex), ArctanImitation(N, Nr, Np, Ns, s, srand, file_mutex, beta){
        No = ExpNum;
        char wn[100];
        sprintf(wn, "WMAI-%lld-%lld-%lld-%lld-%.5f-%.5f", N, Nr, Np, Ns, s, beta);
        model_name = std::string(wn);
    }

WellMixedTanhImitation::WellMixedTanhImitation(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta, integer ExpNum):
    Network(N, Nr, Np, Ns, s, srand, file_mutex), WellMixed(N, Nr, Np, Ns, s, srand, file_mutex), TanhImitation(N, Nr, Np, Ns, s, srand, file_mutex, beta){
        No = ExpNum;
        char wn[100];
        sprintf(wn, "WMTH-%lld-%lld-%lld-%lld-%.5f-%.5f", N, Nr, Np, Ns, s, beta);
        model_name = std::string(wn);
    }