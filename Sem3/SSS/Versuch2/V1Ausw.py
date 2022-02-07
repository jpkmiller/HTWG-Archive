import cv2
import numpy as np


def grauwerte_teilen(path):
    grauwertkeil = cv2.imread(path)

    schwarz = grauwertkeil[::, 30:210]
    print("mean    schwarz: ", np.mean(schwarz))
    print("std     schwarz: ", np.std(schwarz))
    cv2.imwrite('./einzelbilder/schwarz.png', schwarz)

    dunkelgrau = grauwertkeil[::, 270:450]
    print("mean dunkelgrau: ", np.mean(dunkelgrau))
    print("std  dunkelgrau: ", np.std(dunkelgrau))
    cv2.imwrite('./einzelbilder/dunkelgrau.png', dunkelgrau)

    grau = grauwertkeil[::, 510:690]
    print("mean       grau: ", np.mean(grau))
    print("std        grau: ", np.std(grau))
    cv2.imwrite('./einzelbilder/grau.png', grau)

    hellgrau = grauwertkeil[::, 760:940]
    print("mean   hellgrau: ", np.mean(hellgrau))
    print("std    hellgrau: ", np.std(hellgrau))
    cv2.imwrite('./einzelbilder/hellgrau.png', hellgrau)

    weiss = grauwertkeil[::, 1100:]
    print("mean      weiss: ", np.mean(weiss))
    print("std       weiss: ", np.std(weiss))
    cv2.imwrite('./einzelbilder/weiss.png', weiss)


grauwerte_teilen('./finalKeil.png')
