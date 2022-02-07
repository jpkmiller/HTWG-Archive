# -*- coding: utf-8 -*-
"""
Created on Tue Nov 26 13:54:02 2019

@author: walte
"""

import numpy as np
import math
import matplotlib.pyplot as plt

#converts file to array of values:
def readSpannung(file):
    with open(file) as data:
        measures = np.genfromtxt((line.replace(',', '.') for line in data),
                                 skip_header=5000, skip_footer=3000,
                                 delimiter=";")
        return measures[:,1]

def readZeit(file):
    with open(file) as data:
        measures = np.genfromtxt((line.replace(',', '.') for line in data),
                                 skip_header=5000, skip_footer=3000,
                                 delimiter=";")
        return measures[:,0]

def readFile(file):
    with open(file) as data:
        measures = np.genfromtxt((line.replace(',','.') for line in data),
                                 delimiter= ";")
        return measures

#berechnet die zugehörigen Angaben in Dezibel:        
def dezi(array):
    bet = np.abs(array) #Betrag von array
    dezi= np.zeros(array.size) #Angaben in Dezibel
    i=0
    for n in bet:
        dezi[i]= 20 * np.log10(n)
        i=i+1
    return dezi

#berechnet den Phasenwinkel:
def phawi(frequenz, zeit):
    phawi= np.zeros(frequenz.size) #Phasenwinkel
    i=0
    for n in zeit:
        phawi[i]= (-n * 0.001) * frequenz[i] * 360
        i=i+1
    return phawi
    
    
    
    
spannung = readSpannung("./teil1/test_32.csv")
zeit = readZeit("./teil1/test_24.csv")

#Signal im Zeitbereich
fig1,ax1 = plt.subplots(figsize=(18,6), dpi=100)
plt.grid()
plt.ylabel("Spannung [mV]")
plt.xlabel("Zeit [ms]")
ax1.plot(zeit, spannung)

#Amplitudenspektrum
fig2,ax2 = plt.subplots(figsize=(18,6), dpi=100)
plt.grid()
a = np.abs(np.fft.fft(spannung))
ax2.plot(a[:200//2+2])
ax2.set_xlabel('Wellenlaenge')

#zugehoerige Frequenz f 
frequenz= np.zeros(100)
a2 = a[:100]
i=0
for n in a2:
    frequenz[i]=i/(10*0.005)
    i=i+1
    

#Amplitudenspektrum im Frequenzbereich
fig3,ax3 = plt.subplots(figsize=(18,6), dpi=100)
plt.grid()
plt.ylabel("")
plt.xlabel("Frequenz [Hz]")
ax3.plot(frequenz, a2)



#-------------------Ab hier Teil 2:


frequenz2 = readFile("./teil2/frequenzen.csv") #Unterschiedliche Frequenzen f
phaseA = readFile("./teil2/phaseA.csv") #Phasenverschiebung Lautsprecher A
phaseB = readFile("./teil2/phaseB.csv") #Phasenverschiebung Lautsprecher B
ampA = readFile("./teil2/ampA.csv") #Amplitude Lautsprecher A
ampB = readFile("./teil2/ampB.csv") #Amplitude Lautsprecher B

#Plot Ampitudengang
fig4,ax4 = plt.subplots(figsize=(18,6), dpi=100)
plt.grid()
plt.ylabel("Amplitude in mV")
plt.xlabel("Frequenz in Hz")
ax4.plot(frequenz2,ampA, color="red", label= "Lautsprecher A")
ax4.plot(frequenz2,ampB, color="blue", label= "Lautsprecher B")
plt.legend()

#Plot Phasenverschiebung
fig5,ax5 = plt.subplots(figsize=(18,6), dpi=100)
plt.grid()
plt.ylabel("Phasenverschiebung in ms")
plt.xlabel("Frequenz in Hz")
ax5.plot(frequenz2, phaseA, color="red", label= "Lautsprecher A")
ax5.plot(frequenz2, phaseB, color="blue", label= "Lautsprecher B")
plt.legend()

#Plot Ampitudengang halblogarithmisch
fig6,ax6 = plt.subplots(figsize=(18,6), dpi=100)
plt.grid()
plt.ylabel("Amplitude in dB")
plt.xlabel("Frequenz in Hz")
ax6.plot(frequenz2, dezi(ampA), color="red", label= "Lautsprecher A")
ax6.plot(frequenz2, dezi(ampB), color="blue", label= "Lautsprecher B")
plt.legend()
plt.semilogx()

#Plot Phasenverschiebung halblogarithmisch
fig7,ax7 = plt.subplots(figsize=(18,6), dpi=100)
plt.grid()
plt.ylabel("Phasenwinkel in Grad")
plt.xlabel("Frequenz in Hz")
ax7.plot(frequenz2, phawi(frequenz2, phaseA), color="red",
         label= "Lautsprecher A")
ax7.plot(frequenz2,phawi(frequenz2, phaseB), color="blue",
         label= "Lautsprecher B")
plt.legend()
plt.semilogx()
