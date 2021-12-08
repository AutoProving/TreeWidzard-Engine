import fileinput
import re
import matplotlib.pyplot as plt
import networkx as nx
import pydot
import pathlib
from networkx.drawing.nx_pydot import graphviz_layout
import os
import sys

print(sys.argv[1])


s = sys.argv[1]

G = nx.read_gml(s, label='id')
subax1 = plt.subplot()
#labels = nx.get_node_attributes(G, 'l')
options = {
    "font_size": 7,
    "node_size": 120,
    "node_color": "white",
    "edgecolors": "black",
    "font_color": "green",
    "font_weight": "bold",
    "linewidths": .8,
    "width": .8,
}
pos = graphviz_layout(G, prog="dot")
nx.draw(G, pos, with_labels = True, **options)

plt.show()
