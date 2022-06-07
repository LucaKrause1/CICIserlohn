import os
import sys
os.chdir(os.path.dirname(sys.argv[0]))
import osmnx as ox
import networkx as nx
import numpy as np
import cv2
import matplotlib.pyplot as plt

# bbox=(51.38097,51.36682,7.70601,7.68292)
# gdf = ox.geometries_from_xml(filepath="./finalMap.osm")

exampleCoords = [7.6912702,51.3735214]

realCoordWindow = [7.68292,51.36682,7.70601,51.38097]
pixelCoordWindow = [0,799,799,0]

def map_range(x, in_min, in_max, out_min, out_max):
  return (x - in_min) * (out_max - out_min) // (in_max - in_min) + out_min

def realCoordsToPixelCoords(coords,realCoordWindow,pixelCoordWindow):
    return (int(map_range(coords[0], realCoordWindow[0], realCoordWindow[2],pixelCoordWindow[0],pixelCoordWindow[2])),int(map_range(coords[1], realCoordWindow[1], realCoordWindow[3],pixelCoordWindow[1],pixelCoordWindow[3])))

cv2.namedWindow("window", cv2.WND_PROP_FULLSCREEN)
cv2.setWindowProperty("window",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)

pro = np.zeros((800,800,3), np.uint8)
map = cv2.imread("img/image2.png")
map = cv2.resize(map, (800, 800), interpolation= cv2.INTER_LINEAR)


f = open("coords.txt", "r")
l = f.read().split("\n")

l.sort()

print(l)

info = cv2.imread("img/info.png")

while 1:
  for i in l:
      x,y = i.split(";")
      x = float(x)
      y = float(y)
      print(type(x))
      print(x)
      coords = realCoordsToPixelCoords((x,y),realCoordWindow,pixelCoordWindow)
      print(coords)
      pro = map.copy()
      pro = cv2.circle(pro, coords, radius=4, color=(0, 0, 255), thickness=-1)
      img = np.concatenate((pro, info), axis=1)
      cv2.imshow("window", img)
      cv2.waitKey(50)