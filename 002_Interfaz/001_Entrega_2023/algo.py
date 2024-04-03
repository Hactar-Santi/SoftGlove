from numpy.core.defchararray import title
import serial
import time
import matplotlib.pyplot as plt
import numpy as np
import json
import sys
import binascii
import struct


def avg(x):
    avg = np.mean(x)
    # ajusto para poder enviar y redondeo
    return avg*0.25


def STAND(x):
    STD = np.std(x)
    # redondeo, mas facil para enviar a arduino
    return STD*0.8


def FunDIF(x1, x2):
    DIF = 0
    for i in range(len(x1)):
        DIF += x1[i] - x2[i]
    # redondeo
    return DIF/len(x1)*0.3


def damv(x):
    damv = 0

    for i in range(len(x)-1):
        damv += abs(x[i+1] - x[i])

    # Da un valor muy bajo, lo subo y lo redondeo
    return damv*20/len(x)


def dasdv(x):
    dasdv = 0
    for i in range(len(x)-1):
        dasdv += (x[i+1] - x[i])**2
    return np.sqrt(dasdv/(len(x)-1))


def RMSS(x):
    RMS = 0
    for i in range(len(x)):
        RMS += x[i]**2
    RMS = RMS / len(x)
    RMS = np.sqrt(RMS)
    return RMS*0.25

# recibir datos para dos canales.

def Ventana(arduino):

    senales = []
    ven1 = ven2 = AVG1 = AVG2 = DIF1 = DIF2 = DAMV1 = DAMV2 = RMS1 = RMS2 = STD1 = STD2 = np.array([
    ])
    medAVG1 = medAVG2 = medDIF1 = medDIF2 = medDAMV1 = medDAMV2 = medRMS1 = medRMS2 = medSTD1 = medSTD2 = np.array([
    ])

    arduino.write(bytes('2\\n', 'utf-8'))
    time.sleep(1)

    # Aca es donde tomo los datos y donde lo debo modificar

    start = time.time()
    EMG1 = []
    EMG2 = []
    end = time.time()

    while end - start <= 3.5:
        if end - start > 0.5:
            lectura1 = arduino.read(size=1)
            lectura2 = arduino.read(size=1)
            lectura3 = arduino.read(size=1)
            lectura4 = arduino.read(size=1)

            data1 = str(binascii.b2a_hex(lectura1))[2:-1]  # Hex a cadena
            data2 = str(binascii.b2a_hex(lectura2))[2:-1]  # Hex a cadena
            data3 = str(binascii.b2a_hex(lectura3))[2:-1]  # Hex a cadena
            data4 = str(binascii.b2a_hex(lectura4))[2:-1]  # Hex a cadena

            data1 = int(data1, 16)
            data2 = int(data2, 16)
            data3 = int(data3, 16)
            data4 = int(data4, 16)

            dataCanal1 = (data1*256 + data2)
            EMG1.append(dataCanal1)
            dataCanal2 = (data3*256 + data4)
            EMG2.append(dataCanal2)

        end = time.time()        
    arduino.write(bytes('0\\n', 'utf-8'))

    t = np.linspace(0, 3, len(EMG1))
    t = [round(i, 3) for i in t]



    # Tomamos ventanas de 100 muestras
    ventana = 100

    Muestras1 = EMG1[0:ventana]
    Muestras2 = EMG2[0:ventana]

    for i in range(ventana, len(EMG1)):

        AVG1 = np.append(AVG1, round(avg(Muestras1), 2))
        AVG2 = np.append(AVG2, round(avg(Muestras2), 2))

        DIF1 = np.append(DIF1, round(FunDIF(Muestras1, Muestras2), 2))
        DIF2 = DIF1

        DAMV1 = np.append(DAMV1, round(damv(Muestras1), 2))
        DAMV2 = np.append(DAMV2, round(damv(Muestras2), 2))

        STD1 = np.append(STD1, round(STAND(Muestras1), 2))
        STD2 = np.append(STD2, round(STAND(Muestras2), 2))

        RMS1 = np.append(RMS1, round(RMSS(Muestras1), 2))
        RMS2 = np.append(RMS2, round(RMSS(Muestras2), 2))

        Muestras1.pop(0)
        Muestras1.append(EMG1[i])
        Muestras2.pop(0)
        Muestras2.append(EMG2[i])

    mav = [AVG1.tolist(), round(np.amax(AVG1)), round(np.std(AVG1)),
           AVG2.tolist(), round(np.amax(AVG2)), round(np.std(AVG2))]

    if round(np.amax(DIF1)) >= abs(round(np.amin(DIF1))):
        DIF = [DIF1.tolist(), round(np.amax(DIF1)), round(np.std(DIF1)),
               DIF2.tolist(), round(np.amax(DIF2)), round(np.std(DIF2))]
    else:
        DIF = [DIF1.tolist(), round(np.amin(DIF1)), round(np.std(DIF1)),
               DIF2.tolist(), round(np.amin(DIF2)), round(np.std(DIF2))]

    DAMV = [DAMV1.tolist(), round(np.amax(DAMV1)), round(np.std(DAMV1)),
            DAMV2.tolist(), round(np.amax(DAMV2)), round(np.std(DAMV2))]
    STD = [STD1.tolist(), round(np.amax(STD1)), round(np.std(STD1)),
           STD2.tolist(), round(np.amax(STD2)), round(np.std(STD2))]
    RMS = [RMS1.tolist(), round(np.amax(RMS1)), round(np.std(RMS1)),
           RMS2.tolist(), round(np.amax(RMS2)), round(np.std(RMS2))]

    

    return mav, DIF, DAMV, STD, RMS, t, EMG1, EMG2

# recibir datos para un canal.

def VentanaSimple(arduino):
    
    senales = []
    ven1 = AVG1 = DAMV1 = RMS1 = STD1 = np.array([])

    arduino.write(bytes('4\\n', 'utf-8'))
    time.sleep(1)

    # Aca es donde tomo los datos y donde lo debo modificar

    start = time.time()
    EMG1 = []
    end = time.time()

    while end - start <= 3.5:
        if end - start > 0.5:
            lectura1 = arduino.read(size=1)
            lectura2 = arduino.read(size=1)

            data1 = str(binascii.b2a_hex(lectura1))[2:-1]  # Hex a cadena
            data2 = str(binascii.b2a_hex(lectura2))[2:-1]  # Hex a cadena

            data1 = int(data1, 16)
            data2 = int(data2, 16)

            dataCanal1 = (data1*256 + data2)
            EMG1.append(dataCanal1)

        end = time.time()
    arduino.write(bytes('0\\n', 'utf-8'))

    t = np.linspace(0, 3, len(EMG1))
    t = [round(i, 3) for i in t]


    ventana = 100

    Muestras1 = EMG1[0:ventana]

    for i in range(ventana, len(EMG1)):

        AVG1 = np.append(AVG1, round(avg(Muestras1), 2))

        DAMV1 = np.append(DAMV1, round(damv(Muestras1), 2))

        STD1 = np.append(STD1, round(STAND(Muestras1), 2))

        RMS1 = np.append(RMS1, round(RMSS(Muestras1), 2))

        Muestras1.pop(0)
        Muestras1.append(EMG1[i])

    mav = [AVG1.tolist(), round(np.amax(AVG1)), round(np.std(AVG1))]

    DAMV = [DAMV1.tolist(), round(np.amax(DAMV1)), round(np.std(DAMV1))]
    STD = [STD1.tolist(), round(np.amax(STD1)), round(np.std(STD1))]
    RMS = [RMS1.tolist(), round(np.amax(RMS1)), round(np.std(RMS1))]

    

    return mav, DAMV, STD, RMS, t, EMG1


def Score(coeficientes, referencias, variables, largo):



    MAV = variables[0]
    MAV1 = MAV[0]
    MAV2 = MAV[3]

    print(MAV2)
    
    DIF = variables[1]
    DIF1 = DIF[0]
    DIF2 = DIF[3]

    DAMV = variables[2]
    DAMV1 = DAMV[0]
    DAMV2 = DAMV[3]

    STD = variables[3]
    STD1 = STD[0]
    STD2 = STD[3]

    RMS = variables[4]
    RMS1 = RMS[0]
    RMS2 = RMS[3]

    ecuacion = []


    # for i in range(len(referencias)):
    #     if referencias[i] == 0:
    #         referencias[i] = 1
    

    for i in range(len(MAV1)):
 
        ScoreMAV1 = MAV1[i]/referencias[0]
        if ScoreMAV1 > 1:
            ScoreMAV1 = 1
        elif ScoreMAV1 < 0:
            ScoreMAV1 = 0

        ScoreMAV2 = MAV2[i]/referencias[1]
        if ScoreMAV2 > 1:
            ScoreMAV2 = 1
        elif ScoreMAV2 < 0:
            ScoreMAV2 = 0
        ScoreDIF1 = DIF1[i]/referencias[2]

        if ScoreDIF1 > 1:
            ScoreDIF1 = 1
        elif ScoreDIF1 < 0:
            ScoreDIF1 = 0
        ScoreDIF2 = DIF2[i]/referencias[3]

        if ScoreDIF2 > 1:
            ScoreDIF2 = 1
        elif ScoreDIF2 < 0:
            ScoreDIF2 = 0
        ScoreDAMV1 = DAMV1[i]/referencias[4]

        if ScoreDAMV1 > 1:
            ScoreDAMV1 = 1
        elif ScoreDAMV1 < 0:
            ScoreDAMV1 = 0

        ScoreDAMV2 = DAMV2[i]/referencias[5]

        if ScoreDAMV2 > 1:
            ScoreDAMV2 = 1
        elif ScoreDAMV2 < 0:
            ScoreDAMV2 = 0

        ScoreSTD1 = STD1[i]/referencias[6]

        if ScoreSTD1 > 1:
            ScoreSTD1 = 1
        elif ScoreSTD1 < 0:
            ScoreSTD1 = 0

        ScoreSTD2 = STD2[i]/referencias[7]

        if ScoreSTD2 > 1:
            ScoreSTD2 = 1
        elif ScoreSTD2 < 0:
            ScoreSTD2 = 0

        ScoreRMS1 = RMS1[i]/referencias[8]

        if ScoreRMS1 > 1:
            ScoreRMS1 = 1
        elif ScoreRMS1 < 0:
            ScoreRMS1 = 0

        ScoreRMS2 = RMS2[i]/referencias[9]

        if ScoreRMS2 > 1:
            ScoreRMS2 = 1
        elif ScoreRMS2 < 0:
            ScoreRMS2 = 0


        if coeficientes[0] == 1:
            ScoreTotal = coeficientes[2] * ScoreMAV1 + coeficientes[3] * ScoreDIF1 + coeficientes[4] * ScoreDAMV1 + coeficientes[5] * ScoreSTD1 + coeficientes[6] * ScoreRMS1
        elif coeficientes[1] == 1:
            ScoreTotal = coeficientes[2] * ScoreMAV2 + coeficientes[3] * ScoreDIF2 + coeficientes[4] * ScoreDAMV2 + coeficientes[5] * ScoreSTD2 + coeficientes[6] * ScoreRMS2

        ecuacion.append(ScoreTotal)

        # score1_muneca = coeficientes_muneca[0] * AVG1 + coeficientes_muneca[2] * DIF1 + coeficientes_muneca[4] * DAMV1 + coeficientes_muneca[6] * RMS1 + coeficientes_muneca[8] * STD1
        # score2_muneca = coeficientes_muneca[1] * AVG1 + coeficientes_muneca[3] * DIF1 + coeficientes_muneca[5] * DAMV1 + coeficientes_muneca[7] * RMS1 + coeficientes_muneca[9] * STD1

    ScoreInd = np.array([])
        
    ventana = largo - len(MAV1)

    for i in range(largo):
        if i < ventana:
            ScoreInd = np.append(ScoreInd, ecuacion[0])
        else:
            ScoreInd = np.append(ScoreInd, ecuacion[i-ventana])

    prom_ecuaciones = round(np.amax(ScoreInd), 1)
    desvio = round(np.std(ScoreInd), 1)

    ScoreInd = list(ScoreInd)

    return ScoreInd, prom_ecuaciones, desvio


# faltaría coeficientes_muneca y referencias_mano
def ScoreSimple(coeficientes_mano, referencias_mano, variables, largo):
    # referencias_mano = [ref_AVG1,ref_AVG2,ref_DIF1,ref_DIF2,ref_DAMV1,ref_DAMV2,ref_RMS1,ref_RMS2,ref_STD1,ref_STD2]
    # coeficientes_mano = [score1, score2, a * AVG, b * DIF, c * DAMV, d * RMS , e * STD]

    MAV = variables[0]
    MAV1 = MAV[0]

    DAMV = variables[1]
    DAMV1 = DAMV[0]

    STD = variables[2]
    STD1 = STD[0]

    RMS = variables[3]
    RMS1 = RMS[0]

    ecuacion_mano = []
    # ecuacion_muneca = []

    for i in range(len(MAV1)):

        ScoreMAV1 = MAV1[i]/referencias_mano[0]

        if ScoreMAV1 > 1:
            ScoreMAV1 = 1
        elif ScoreMAV1 < 0:
            ScoreMAV1 = 0

        ScoreDAMV1 = DAMV1[i]/referencias_mano[1]

        if ScoreDAMV1 > 1:
            ScoreDAMV1 = 1
        elif ScoreDAMV1 < 0:
            ScoreDAMV1 = 0

        ScoreSTD1 = STD1[i]/referencias_mano[2]

        if ScoreSTD1 > 1:
            ScoreSTD1 = 1
        elif ScoreSTD1 < 0:
            ScoreSTD1 = 0

        ScoreRMS1 = RMS1[i]/referencias_mano[3]

        if ScoreRMS1 > 1:
            ScoreRMS1 = 1
        elif ScoreRMS1 < 0:
            ScoreRMS1 = 0

        ecuacion_mano.append(coeficientes_mano[0] * ScoreMAV1 + coeficientes_mano[1] *
                             ScoreDAMV1 + coeficientes_mano[2] * ScoreSTD1 + coeficientes_mano[3] * ScoreRMS1)

    ventana = largo - len(MAV1)

    ScoreInd = np.array([])

    for i in range(largo):
        if i < ventana:
            ScoreInd = np.append(ScoreInd, ecuacion_mano[0])
        else:
            ScoreInd = np.append(ScoreInd, ecuacion_mano[i-ventana])

    prom_ecuaciones = round(np.amax(ScoreInd), 1)
    desvio = round(np.std(ScoreInd), 1)

    ScoreInd = list(ScoreInd)

    return ScoreInd, prom_ecuaciones, desvio


# enviar parametros para un solo canal.
def Enviar_parametrosSimple(ParMun, RefMun, arduino):

    arduino.write(bytes('3\\n', 'utf-8'))
    time.sleep(1)

    # Para que no de error
    for i in range(4):
        if RefMun[i] > 255:
            RefMun[i] = 255
        if RefMun[i] < 0:
            RefMun[i] = 0

    b = struct.pack('>BBBBBBBBB',
                    ParMun[0], ParMun[1], ParMun[2], ParMun[3], ParMun[4],
                    RefMun[0], RefMun[1], RefMun[2], RefMun[3])

    arduino.write(b)


# enviar parametros para dos canales.
def Enviar_parametros(ParMano, ParMun, RefMano, RefMun, arduino):

    
    arduino.write(bytes('1\\n', 'utf-8'))
    time.sleep(1)

    if RefMano[2] < 0:
        RefMano[2] = 0
        RefMano[3] = RefMano[3]*-1
    else:
        RefMano[2] = 1

    if RefMun[2] < 0:
        RefMun[2] = 0
        RefMun[3] = RefMun[3]*-1
    else:
        RefMun[2] = 1

    # Para que no de error
    for i in range(10):
        if RefMano[i] > 255:
            RefMano[i] = 255
        if RefMun[i] > 255:
            RefMun[i] = 255
        if RefMano[i] < 0:
            RefMano[i] = 0
        if RefMun[i] < 0:
            RefMun[i] = 0

    print("----------------------------------------------")
    print("----------------------------------------------")
    print("----------------------------------------------")
    print(RefMano)
    print(RefMun)
    

    b = struct.pack('>BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB', ParMano[0], ParMano[1], ParMano[2], ParMano[3],
                    ParMano[4], ParMano[5], ParMano[6], ParMano[7], ParMun[0], ParMun[1],
                    ParMun[2], ParMun[3], ParMun[4], ParMun[5], ParMun[6], ParMun[7],
                    RefMano[0], RefMano[1], RefMano[2], RefMano[3], RefMano[4],
                    RefMano[5], RefMano[6], RefMano[7], RefMano[8], RefMano[9],
                    RefMun[0], RefMun[1], RefMun[2], RefMun[3], RefMun[4],
                    RefMun[5], RefMun[6], RefMun[7], RefMun[8], RefMun[9])

    arduino.write(b)
