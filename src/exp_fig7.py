import argparse
import os
import time
import math



exp_type = "WMF"
total_run = 100000

population_conf = [(30, 10, 10, 10), ]
 
for N, Nr, Np, Ns in population_conf:
    # 
    # for beta in [10000, 100, 1., 0.01, 0.0001, ]:
    for beta in [0.1]:
        for s in [10 ** (-2 + x * 4 / 100) for x in range(101)]:
            beta = round(beta, 8)
            print(N, Nr, Np, Ns, s, beta)
            os.system("python3 sub_exp.py --N {} --Nr {} --Np {} --Ns {} --s {} --beta {} --exp_type {} --total_run {}".format(N, Nr, Np, Ns, s, beta, exp_type, total_run))
            time.sleep(0.1)
            # input("Here")
