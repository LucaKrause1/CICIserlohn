import cv2
import numpy as np

def redraw():
    global posList
    global imgDis
    imgDis = img.copy()
    for i in posList:
        cv2.circle(imgDis,(i[0],i[1]),1,(0,255,0),-1)

posList = []
def onMouse(event, x, y, flags, param):
    global posList
    global imgDis
    if event == cv2.EVENT_LBUTTONDOWN:
        posList.append([x, y])
        redraw()
    if event == cv2.EVENT_RBUTTONDOWN:
        posList.pop()
        redraw()

cv2.namedWindow("window", cv2.WND_PROP_FULLSCREEN)
cv2.setWindowProperty("window",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)

map = cv2.imread("img/buildings.png")
map = cv2.resize(map, (800, 800), interpolation= cv2.INTER_LINEAR)
info = cv2.imread("img/info0.jpg")
img = np.concatenate((map, info), axis=1)

cv2.setMouseCallback('window', onMouse)

origPoints = [[743, 139], [84, 177], [67, 710], [722, 757]]

for i in origPoints:
    cv2.circle(img,(i[0],i[1]),1,(0,0,255),-1)
imgDis = img.copy()

while(1):
    cv2.imshow('window',imgDis)
    k = cv2.waitKey(20) & 0xFF
    if k == 27:
        break
    elif k == ord('a'):
        print(posList)
    elif k == ord('p'):  
        oP = np.float32(origPoints)
        pL = np.float32(posList)
        M = cv2.getPerspectiveTransform(oP,pL)
        map = cv2.warpPerspective(map,M,(800, 800),flags=cv2.INTER_LINEAR)
        imgDis = np.concatenate((map, info), axis=1)
    elif k == ord('i'): #Oben
        posList[len(posList)-1][1] = posList[len(posList)-1][1] - 1
        redraw()
    elif k == ord('l'): #Rechts
        posList[len(posList)-1][0] = posList[len(posList)-1][0] + 1
        redraw()
    elif k == ord('k'): #Unten
        posList[len(posList)-1][1] = posList[len(posList)-1][1] + 1
        redraw()
    elif k == ord('j'): #Links
        posList[len(posList)-1][0] = posList[len(posList)-1][0] - 1
        redraw()
