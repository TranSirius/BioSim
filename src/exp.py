import argparse
import os
import time
import math



exp_type = "WMF"
total_run = 100000

# population_conf = [(6, 2, 2, 2), (15, 5, 5, 5), (30, 10, 10, 10)]
population_conf = [(30, 10, 10, 10), ]

for N, Nr, Np, Ns in population_conf:
    # for s in [1, 2, 0.5]:
    # for s in [0.001, 0.01, 0.1, 1., 10, 100]:
    for s in [0.0001, 0.001, 0.01, 0.1, 1., 10, 100]:
        print("s = ", s)
        betas = list(range(-3, 4))
        for beta in betas:
            # beta /= 10
            beta = 10 ** beta
            beta = round(beta, 8)
            print(N, Nr, Np, Ns, s, beta)
            os.system("python3 sub_exp.py --N {} --Nr {} --Np {} --Ns {} --s {} --beta {} --exp_type {} --total_run {}".format(N, Nr, Np, Ns, s, beta, exp_type, total_run))
            time.sleep(0.1)
            # input("Here")
