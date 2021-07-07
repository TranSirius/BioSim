import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('--N', type=int, default=15)
parser.add_argument('--Nr', type=int, default=5)
parser.add_argument('--Np', type=int, default=5)
parser.add_argument('--Ns', type=int, default=5)
parser.add_argument('--s', type=float, default=1.)
parser.add_argument('--beta', type=float, default=0.01)
parser.add_argument('--exp_type', type=str)
parser.add_argument('--total_run', type=int, default=5000)
args = parser.parse_args()

pool_size = 40

os.system("./run {} {} {} {} {} {} {} {} {}".format(args.N, args.Nr, args.Np, args.Ns, args.s, args.beta, pool_size, args.exp_type, args.total_run))
