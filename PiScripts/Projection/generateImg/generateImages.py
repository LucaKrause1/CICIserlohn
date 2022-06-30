import os
import sys
os.chdir(os.path.dirname(sys.argv[0]))
import osmnx as ox
import networkx as nx
import numpy as np
import cv2
import matplotlib.pyplot as plt
import pandas as pd

CONFIG_FILE = 'configgrey.json'
CONFIF_BOTH = True

bbox=(51.38097,51.36682,7.70601,7.68292)
gdf = ox.geometries_from_xml(filepath="./finalMap.osm")

configDf = None
with open(CONFIG_FILE, 'r') as f:
    configDf = pd.read_json(f.read(), orient='index')

nodeIds = [
5071232969
,1976845083
,6942461856
,1741362949
,599402192
,5069564551
,520269533
,520269532
,520269531
,520269530
,520269529
,520269528
,406672100
,146610774
,39253953
,146610775
,553981882
,553981879
,24821471
,552233294
,552233293
,552233289
,552233284
,405738880
,750487113
,751581926
,725921847
,146771260
,146770682
,532002862
,46974872
,737227190
,737227189
,533879749
,46974873
]

newgdf = gdf[gdf["highway"].isin(list(configDf.index.values))]

def calc_node_id(row):
    return row.name[1]

def calc_new_width(row):
   return configDf["street_width"][row["highway"]]
   
def calc_new_width2(row):
   return 5 #configDf["street_width"][row["highway"]]*2
   
def calc_new_color2(row):
    return "#FFFFFF"   

def calc_new_color(row):
    return configDf["street_colors"][row["highway"]]

newgdf["node_id"] = newgdf.apply(calc_node_id, axis=1)

newgdfbus = newgdf[newgdf["node_id"].isin(nodeIds)]

newgdf["street_width"] = newgdf.apply(calc_new_width, axis=1)
newgdf["street_color"] = newgdf.apply(calc_new_color, axis=1)

newgdfbus["street_width"] = newgdfbus.apply(calc_new_width2, axis=1)
newgdfbus["street_color"] = newgdfbus.apply(calc_new_color2, axis=1)

newgdf = newgdf.sort_values("street_width", ascending=True)

t = set()
for i in gdf["landuse"]:
    t.add(i)
for i in t:
    print(i)

#buildinggdf =gdf[gdf["geometry"].type.isin({"Polygon", "MultiPolygon"})]
# buildinggdf =gdf[gdf["building"] == "yes"]
# buildinggdf =gdf[gdf["landuse"] == "forest"]
buildinggdf =gdf[gdf["landuse"].isin(["forest", "grass", "village_green", "farmland", "greenfield", "meadow"])]

ax = newgdf.plot(color=newgdf["street_color"], linewidth= newgdf["street_width"])
# newgdfbus.plot(ax=ax, color=newgdfbus["street_color"], linewidth= newgdfbus["street_width"])
fig = ax.get_figure()
fig.set_size_inches(20,20)
ax.set_xlim([7.68292, 7.70601])
ax.set_ylim([51.36682, 51.38097])
plt.axis('off')
plt.savefig('image.png', facecolor="black", dpi=100, bbox_inches='tight',pad_inches = 0)


if(CONFIF_BOTH):
    buildinggdf.plot(ax=ax, color="green")
else:
    ax = buildinggdf.plot(color="green")
fig = ax.get_figure()
fig.set_size_inches(20,20)
ax.set_xlim([7.68292, 7.70601])
ax.set_ylim([51.36682, 51.38097])
plt.axis('off')
plt.savefig('image2.png', facecolor="black", dpi=100, bbox_inches='tight',pad_inches = 0)
