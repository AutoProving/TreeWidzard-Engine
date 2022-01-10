import fileinput
import re
import matplotlib.pyplot as plt
import networkx as nx
import pydot
import pathlib
from networkx.drawing.nx_pydot import graphviz_layout
import os
import sys


s = sys.argv[1]

G = nx.read_gml(s, label='id')

m = nx.adj_matrix(G)
print(m.todense())
for r in m.todense():
    for i in range(r.size):
        if i != r.size-1:
            print(str(r[0,i])+" & ",end='')
        else:
            print(str(r[0,i]),end='')


    print(" \\\ ")
v = 1    
commaflag = False
for r in m.todense():
    commaflag = False
    print(str(v)+": &[",end='')     
    for i in range(r.size):
        if r[0,i]==1 and commaflag == True:
            print(","+str(i+1),end='')
        elif r[0,i] == 1 and commaflag == False:
            print(str(i+1),end='')
            commaflag = True
    print(']',end='')
    v+=1

    print(" \\\[0.05cm]")

