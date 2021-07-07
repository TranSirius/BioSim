#pragma once

#include <iostream>
#include <random>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
// #include <Python.h>
#include <cmath>
#include <mutex>

typedef double real;
typedef long long integer;

const int tr = 0;
const int tp = 1;
const int ts = 2;

typedef struct tState {
    integer Nr, Np, Ns;
    integer T;
} State;


class Network {
public:
    State fix_state;
    State ext_state;

protected:
    char str2char[3];                                       // strategy to character 0 -> R, 1 -> P, 2 -> S
    integer No;
    std::string model_name;
    
    integer N;
    integer Nr, Np, Ns;
    integer Ndis[3];

    int num_of_type = 3;

    long long T = 0;
    real s;

    real pm[3][3] = {0};                                    // The payoff matrix

    bool ** network = nullptr;                              // The adjcency matrix
    int * strategy = nullptr;

    std::vector<integer> * adj = nullptr;                   // The incident matrix (邻接表)
    std::mt19937 mt;

    std::mutex & fmutex;

public:
    Network(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex);
    ~Network();
    virtual bool update() = 0;

    void report_ext();
    void report_fix();

    void output(std::string output_file_name, integer exp_num = 0);
    void repr();
    void gen_adj();
    real payoff(integer who);
    bool transfer(integer who, int to_what);
    void simulation();
};

// Graph Structure

class WellMixed: virtual public Network {
public:
    WellMixed(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex);
};

class RegularGraph: virtual public Network {
public:
    RegularGraph(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, integer degree);
};

class Lattice: virtual public Network {
public:
    Lattice(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex);
};

// Dynamics, or update processes

class Fermi: virtual public Network {
protected:
    std::uniform_int_distribution<> fo_sam;
    std::uniform_real_distribution<> transfer_sam;
    real beta;
public:
    Fermi(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta);
    virtual bool update();
};


class LocalUpdate: virtual public Network {
protected:
    std::uniform_int_distribution<> fo_sam;
    std::uniform_real_distribution<> transfer_sam;
    real beta;
    real max_pi;
public:
    LocalUpdate(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta);
    virtual bool update();
};


class ArctanImitation: virtual public Network {
protected:
    std::uniform_int_distribution<> fo_sam;
    std::uniform_real_distribution<> transfer_sam;
    real beta;
public:
    ArctanImitation(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta);
    virtual bool update();
};


class TanhImitation: virtual public Network {
protected:
    std::uniform_int_distribution<> fo_sam;
    std::uniform_real_distribution<> transfer_sam;
    real beta;
public:
    TanhImitation(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta);
    virtual bool update();
};


class Moran: virtual public Network {
public:
    virtual bool update();
};

// Experiments

class WellMixedFermi: public WellMixed, public Fermi {
public:
    WellMixedFermi(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta, integer ExpNum = 0);
};

class WellMixedLocalUpdate: public WellMixed, public LocalUpdate {
public:
    WellMixedLocalUpdate(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta, integer ExpNum = 0);
};

class WellMixedArctanImitation: public WellMixed, public ArctanImitation {
public:
    WellMixedArctanImitation(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta, integer ExpNum = 0);
};

class WellMixedTanhImitation: public WellMixed, public TanhImitation {
public:
    WellMixedTanhImitation(integer N, integer Nr, integer Np, integer Ns, real s, integer srand, std::mutex & file_mutex, real beta, integer ExpNum = 0);
};