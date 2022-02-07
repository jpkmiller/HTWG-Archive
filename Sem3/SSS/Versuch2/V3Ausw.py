import numpy as np
import cv2


def dunkelbild_korrektur(path):
    org = cv2.imread("./" + path)
    cor = cv2.imread('./mittelDunkelBild.png')
    cv2.imwrite("./cor_" + path, org - cor)


def weissbild_auswerten():
    weissbild_mittelwert = cv2.imread("weiss1.png").astype(float)

    for i in range(2, 11):
        weissbild_mittelwert += cv2.imread("weiss" + str(i) + ".png").astype(float)
    weissbild_mittelwert /= 10.0
    cv2.imwrite('./mittelWeissBild.png', weissbild_mittelwert)

    dunkelbild_korrektur("mittelWeissBild.png")

    corWeiss = cv2.imread("./cor_mittelWeissBild.png")

    nullpunkt = corWeiss - np.min(corWeiss)
    contrastiert = nullpunkt * (255 / (np.max(corWeiss) - np.min(corWeiss)))
    cv2.imwrite('./contrastWeissBild.png', contrastiert)


weissbild_auswerten()


def norm():
    weiss = cv2.imread("./cor_mittelWeissBild.png").astype(float)
    norm = weiss / np.mean(weiss)
    finalKeil = cv2.imread("./cor_keil.png").astype(float) / norm

    cv2.imwrite("./finalKeil.png", finalKeil)


norm()
