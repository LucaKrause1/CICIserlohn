import paho.mqtt.client as mqtt
import numpy as np
import cv2
from enum import Enum

MQTT_ADDRESS = '192.168.4.1'
MQTT_USER = 'cic'
MQTT_PASSWORD = 'cic'
MQTT_TOPIC = 'test'


#Which operation modes the projection
class Mode(Enum):
    NO_MODEL = 0
    ABUS = 1
    TREE = 2

modeToImage = {
    Mode.NO_MODEL : "img/colorAllStreets.png",
    Mode.ABUS : "img/colorBus.png",
    Mode.TREE : "img/buildings.png",
}

modeToInfo = {
    Mode.NO_MODEL : "img/info.png",
    Mode.ABUS : "img/infoAbus.png",
    Mode.TREE : "img/infoTree.png",
}
    
oldMode = Mode.NO_MODEL    
currentMode = Mode.NO_MODEL

def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    """The callback for when a PUBLISH message is received from the server."""
    print(msg.topic + ' ' + str(msg.payload))
    if msg.payload == "b'abus'":
        currentMode = Mode.ABUS
    elif msg.payload == "b'abus'":
        currentMode = Mode.TREE    
    elif msg.payload == "b'none'":
        currentMode = Mode.NO_MODEL  
    print(currentMode)



def main():
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_start()

    cv2.namedWindow("window", cv2.WND_PROP_FULLSCREEN)
    cv2.setWindowProperty("window",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)

    while 1:
        pro = np.zeros((800,800,3), np.uint8)

        #Read the right image
        map = cv2.imread(modeToImage[currentMode])
        map = cv2.resize(map, (800, 800), interpolation= cv2.INTER_LINEAR)

        info = cv2.imread(modeToImage[currentMode])
        map = cv2.resize(map, (800, 480), interpolation= cv2.INTER_LINEAR)
        oldMode = currentMode

        while oldMode == currentMode:
            pro = map.copy()
            img = np.concatenate((pro, info), axis=1)
            cv2.imshow("window", img)
            cv2.waitKey(50)

   

    


#Mapping real Coords to Image Coords
realCoordWindow = [7.68292,51.36682,7.70601,51.38097]
pixelCoordWindow = [0,799,799,0]

def map_range(x, in_min, in_max, out_min, out_max):
  return (x - in_min) * (out_max - out_min) // (in_max - in_min) + out_min

def realCoordsToPixelCoords(coords,realCoordWindow,pixelCoordWindow):
    return (int(map_range(coords[0], realCoordWindow[0], realCoordWindow[2],pixelCoordWindow[0],pixelCoordWindow[2])),int(map_range(coords[1], realCoordWindow[1], realCoordWindow[3],pixelCoordWindow[1],pixelCoordWindow[3])))



if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()
