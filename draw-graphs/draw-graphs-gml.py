import fileinput
import re
import matplotlib.pyplot as plt
import networkx as nx
import pydot
import pathlib
from networkx.drawing.nx_pydot import graphviz_layout
import os
import sys
import nxviz as nv 
from nxviz import plots

print(sys.argv[1])


s = sys.argv[1]

G = nx.read_gml(s, label='id')
subax1 = plt.subplot()
#labels = nx.get_node_attributes(G, 'l')
options = {
    "font_size": 12,
    "node_size": 400,
    "node_color": "white",
    "edgecolors": "black",
    "font_color": "green",
    "font_weight": "bold",
    "linewidths": .9,
    "width": .8,
}
#pos = graphviz_layout(G, prog="dot")
#nv.circos(G)

#nx.draw_kamada_kawai(G, with_labels = True, **options)
# nx.draw(G, pos, with_labels = True, **options)


plt.show()
