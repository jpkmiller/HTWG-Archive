import numpy as np
import cv2

cap = cv2.VideoCapture(0)

# eingestellte Belichtungsparameter
cap.set(3, 1280.0)
cap.set(4, 720.0)
cap.set(10, 100.0)
cap.set(11, 42.0)
cap.set(12, 0.0)
cap.set(14, 10.0)
cap.set(15, 50)

while (True):
    ret, frame = cap.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    cv2.imshow('frame', gray)
    print(np.mean(gray))
    if cv2.waitKey(1) & 0xFF == ord('q'):
        for i in range(1, 11):
            cv2.imwrite("weiss" + str(i) + ".png", gray)
            ret, frame = cap.read()
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        break

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
