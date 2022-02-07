import numpy as np
import cv2


def dunkelbild_auswerten():
    dunkelbild_mittelwert = cv2.imread("dunkel1.png").astype(float)

    for i in range(2, 11):
        dunkelbild_mittelwert += cv2.imread("dunkel" + str(i) + ".png").astype(float)

    dunkelbild_mittelwert /= 10.0
    cv2.imwrite('./mittelDunkelBild.png', dunkelbild_mittelwert)

    nullpunkt = dunkelbild_mittelwert - np.min(dunkelbild_mittelwert)
    contrastiert = nullpunkt
    if np.max(dunkelbild_mittelwert) - np.min(dunkelbild_mittelwert) != 0:
        contrastiert = nullpunkt * (255 / (np.max(dunkelbild_mittelwert) - np.min(dunkelbild_mittelwert)))

    cv2.imwrite('./contrastDunkelBild.png', contrastiert)


dunkelbild_auswerten()


def dunkelbild_korrektur(path):
    org = cv2.imread("./" + path)
    cor = cv2.imread('./mittelDunkelBild.png')
    cv2.imwrite("./cor_" + path, org - cor)


dunkelbild_korrektur("keil.png")
