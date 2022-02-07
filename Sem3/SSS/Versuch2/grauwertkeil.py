# -*- coding: utf-8 -*-
"""
Created on Wed Dec  4 14:00:49 2019

@author: wa391voe
"""

import numpy as np
import cv2

#-----Versuch1:
cap = cv2.VideoCapture(0)

#eingestellte Belichtungsparameter
cap.set(3, 640.0)
cap.set(4, 480.0)
cap.set(10, 128.0)
cap.set(11, 40.0)
cap.set(12, 32.0)
cap.set(14, 20.0)
cap.set(15, -3)
cap.set(17, 6000.0)


while(True): 
    ret, frame = cap.read() 
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    cv2.imshow('frame', gray) 
    if cv2.waitKey(1) & 0xFF == ord('q'): 
        break;



print("frame width: " + str(cap.get(3)))
print("frame height: " + str(cap.get(4)))
print("--------------------------------") 
print("brightness: " + str(cap.get(10)))
print("contrast: " + str(cap.get(11)))
print("saturation: " + str(cap.get(12))) 
print("--------------------------------") 
print("gain: " + str(cap.get(14))) 
print("exposure: " + str(cap.get(15))) 
print("--------------------------------") 
print("white balance: " + str(cap.get(17)))

cap.release()
cv2.destroyAllWindows()

cv2.imwrite("keil.png", frame)