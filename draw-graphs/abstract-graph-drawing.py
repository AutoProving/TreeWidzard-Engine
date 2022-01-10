import fileinput
import re
import matplotlib.pyplot as plt
import networkx as nx
import pydot
from networkx.drawing.nx_pydot import graphviz_layout

G = nx.DiGraph()

for line in fileinput.input():
    node = re.match('(\d+)\s([^()\n]*)(\([\d\,]+\))?', line).groups()
    G.add_node(node[0], l=node[1])
    if node[2] is not None:
        neighbours = node[2][1:-1].split(',')
        for neighbour in neighbours:
            G.add_edge(node[0], neighbour)

subax1 = plt.subplot()
labels = nx.get_node_attributes(G, 'l')
options = {
    "font_size": 7,
    "node_size": 120,
    "node_color": "white",
    "edgecolors": "white",
    "font_color": "green",
    "font_weight": "bold",
    "linewidths": .8,
    "width": .8,
}
pos = graphviz_layout(G, prog="dot")
nx.draw(G, pos, labels=labels, **options)

plt.show()
