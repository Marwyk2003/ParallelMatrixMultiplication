#!/usr/bin/python3

from random import randint
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument('size', type=int)
parser.add_argument('-MIN', type=int, default=-100)
parser.add_argument('-MAX', type=int, default=100)
args = parser.parse_args()

n = args.size
MIN = args.MIN
MAX = args.MAX
print(n)
for t in range(2):
    for y in range(n):
        row = [randint(MIN, MAX) for x in range(n)]
        print(*row, sep=' ')
    