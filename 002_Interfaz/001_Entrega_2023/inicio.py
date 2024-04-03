from flask import Flask, render_template, jsonify, request
import json
import numpy as np
import time
import serial
from random import randint
import algo
app = Flask(__name__)

prendido = 0

# Codigo para cargar pantalla de pagina principal


@app.route('/')
def paginaPrincipal():
    return render_template('0 - PaginaPrincipal.html')

# Codigo para cargar pantalla de Nuevo Usuario


@app.route('/nuevoUsuario')
def nuevoUsuario():
    return render_template('nuevoUsuario.html')

# cuando se selecciona dos canales.


@app.route('/EntrenamientoMano')
def resumenEnrenamiento():
    global prendido
    global arduino
    if prendido == 0:
        arduino = serial.Serial(port='COM7', baudrate=57600, timeout=.1)
        prendido = 1
        time.sleep(3)
        arduino.write(bytes('0\\n', 'utf-8'))
    return render_template('1.1.a - EntrenamientoMano.html')
    


@app.route('/EntrenamientoMun')
def EntrenamientoMun():
    return render_template('1.1.b - EntrenamientoMun.html')

# cuando se selecciona un canal.


@app.route('/EntMunSimple')
def EntMunSimple():
    global prendido
    global arduino
    if prendido == 0:
        arduino = serial.Serial(port='COM7', baudrate=57600, timeout=.1)
        prendido = 1
        time.sleep(3)
        arduino.write(bytes('0\\n', 'utf-8'))
    return render_template('2.1 - EntrenamientoSimple.html')


@app.route('/testeo')
def testeo():
    return render_template('1.2 - Evaluacion.html')


@app.route('/testeoSimple')
def testeoSimple():
    return render_template('2.2 - EvaluacionSimple.html')


@app.route('/resumen')
def resumen():
    return render_template('1.3 - Resumen.html')


@app.route('/resumenSimple')
def resumenSimple():
    return render_template('2.3 - ResumenSimple.html')

# Para Adquirir info del arduino


@app.route('/_tomarDataTesteo')
def _tomarDataTesteo():
    scoreMano = randint(0, 100)/100
    scoreMuneca = randint(0, 100)/100
    return jsonify(scoreMano=scoreMano, scoreMuneca=scoreMuneca)


@app.route('/_calculoScoreTesteo')
def _calculoScoreTesteo():
    try:
        # para la mano
        canal1Mano = request.args.get('canal1Mano', 0, type=float)
        canal2Mano = request.args.get('canal2Mano', 0, type=float)

        aMano = request.args.get('aMano', 0, type=float)
        bMano = request.args.get('bMano', 0, type=float)
        cMano = request.args.get('cMano', 0, type=float)
        dMano = request.args.get('dMano', 0, type=float)
        eMano = request.args.get('eMano', 0, type=float)

        MAVref1Mano = request.args.get('MAVref1Mano', 0, type=float)
        DIFref1Mano = request.args.get('DIFref1Mano', 0, type=float)
        DAMVref1Mano = request.args.get('DAMVref1Mano', 0, type=float)
        STDref1Mano = request.args.get('STDref1Mano', 0, type=float)
        RMSref1Mano = request.args.get('RMSref1Mano', 0, type=float)

        MAVref2Mano = request.args.get('MAVref2Mano', 0, type=float)
        DIFref2Mano = request.args.get('DIFref2Mano', 0, type=float)
        DAMVref2Mano = request.args.get('DAMVref2Mano', 0, type=float)
        STDref2Mano = request.args.get('STDref2Mano', 0, type=float)
        RMSref2Mano = request.args.get('RMSref2Mano', 0, type=float)

        UmbralMano = request.args.get('UmbralMano', 0, type=float)

        coeficientes_mano = [canal1Mano, canal2Mano,
                             aMano, bMano, cMano, dMano, eMano]
        referencias_mano = [MAVref1Mano, MAVref2Mano, DIFref1Mano, DIFref2Mano,
                            DAMVref1Mano, DAMVref2Mano, STDref1Mano, STDref2Mano, RMSref1Mano, RMSref2Mano]

        # ahora todo de nuevo para la muneca

        canal1 = request.args.get('canal1', 0, type=float)
        canal2 = request.args.get('canal2', 0, type=float)

        a = request.args.get('a', 0, type=float)
        b = request.args.get('b', 0, type=float)
        c = request.args.get('c', 0, type=float)
        d = request.args.get('d', 0, type=float)
        e = request.args.get('e', 0, type=float)

        MAVref1 = request.args.get('MAVref1', 0, type=float)
        DIFref1 = request.args.get('DIFref1', 0, type=float)
        DAMVref1 = request.args.get('DAMVref1', 0, type=float)
        STDref1 = request.args.get('STDref1', 0, type=float)
        RMSref1 = request.args.get('RMSref1', 0, type=float)

        MAVref2 = request.args.get('MAVref2', 0, type=float)
        DIFref2 = request.args.get('DIFref2', 0, type=float)
        DAMVref2 = request.args.get('DAMVref2', 0, type=float)
        STDref2 = request.args.get('STDref2', 0, type=float)
        RMSref2 = request.args.get('RMSref2', 0, type=float)

        Umbral = request.args.get('UmbralMun', 0, type=float)

        coeficientes_mun = [canal1, canal2, a, b, c, d, e]
        referencias_mun = [MAVref1, MAVref2, DIFref1, DIFref2,
                           DAMVref1, DAMVref2, STDref1, STDref2, RMSref1, RMSref2]

        mav, DIF, DAMV, STD, RMS, t, EMG1, EMG2 = algo.Ventana(arduino)
        parametros = [mav, DIF, DAMV, STD, RMS]
        ScoreIndMano, ScoreMaxMano, desvioMano = algo.Score(
            coeficientes_mano, referencias_mano, parametros, len(EMG1))

        ScoreIndMun, ScoreMaxMun, desvioMun = algo.Score(
            coeficientes_mun, referencias_mun, parametros, len(EMG1))

        return jsonify(ScoreIndMano=ScoreIndMano, ScoreMaxMano=ScoreMaxMano, desvioMano=desvioMano, ScoreIndMun=ScoreIndMun, ScoreMaxMun=ScoreMaxMun, desvioMun=desvioMun, EMG1=EMG1, EMG2=EMG2, tiempo=t)

    except Exception as e:
        return str(e)


@app.route('/_dataCierre')
def dataCierre():
    mav, DIF, DAMV, STD, RMS, tiempo, EMG1, EMG2 = algo.Ventana(arduino)
    return jsonify(mav=mav, DIF=DIF, DAMV=DAMV, STD=STD, RMS=RMS, tiempo=tiempo, EMG1=EMG1, EMG2=EMG2)


@app.route('/_dataCierreSimple')
def dataCierreSimple():
    print('hola')
    mav, DAMV, STD, RMS, tiempo, EMG1 = algo.VentanaSimple(arduino)
    print('chau')
    return jsonify(mav=mav, DAMV=DAMV, STD=STD, RMS=RMS, tiempo=tiempo, EMG1=EMG1)


@app.route('/_dataApertura')
def dataApertura():

    mav, DIF, DAMV, STD, RMS, tiempo, EMG1, EMG2 = algo.Ventana(arduino)

    return jsonify(mav=mav, DIF=DIF, DAMV=DAMV, STD=STD, RMS=RMS, tiempo=tiempo, EMG1=EMG1, EMG2=EMG2)


@app.route('/_dataAperturaSimple')
def dataAperturaSimple():
    mav, DAMV, STD, RMS, tiempo, EMG1 = algo.VentanaSimple(arduino)

    return jsonify(mav=mav, DAMV=DAMV, STD=STD, RMS=RMS, tiempo=tiempo, EMG1=EMG1)


@app.route('/_calculoScore')
def _calculoScore():
    print("---------------------------------------------------")
    print('Funcion de calculo score en calibracion')
    print("---------------------------------------------------")
    try:
        canal1 = request.args.get('canal1', 0, type=float)
        canal2 = request.args.get('canal2', 0, type=float)

        a = request.args.get('a', 0, type=float)
        b = request.args.get('b', 0, type=float)
        c = request.args.get('c', 0, type=float)
        d = request.args.get('d', 0, type=float)
        e = request.args.get('e', 0, type=float)

        MAVref1 = request.args.get('MAVref1', 0, type=float)
        DIFref1 = request.args.get('DIFref1', 0, type=float)
        DAMVref1 = request.args.get('DAMVref1', 0, type=float)
        STDref1 = request.args.get('STDref1', 0, type=float)
        RMSref1 = request.args.get('RMSref1', 0, type=float)

        MAVref2 = request.args.get('MAVref2', 0, type=float)
        DIFref2 = request.args.get('DIFref2', 0, type=float)
        DAMVref2 = request.args.get('DAMVref2', 0, type=float)
        STDref2 = request.args.get('STDref2', 0, type=float)
        RMSref2 = request.args.get('RMSref2', 0, type=float)

        Umbral = request.args.get('Umbral', 0, type=float)

        coeficientes_mano = [canal1, canal2, a, b, c, d, e]
        referencias_mano = [MAVref1, MAVref2, DIFref1, DIFref2,
                            DAMVref1, DAMVref2, STDref1, STDref2, RMSref1, RMSref2]

        mav, DIF, DAMV, STD, RMS, tiempo, EMG1, EMG2 = algo.Ventana(arduino)

        variables = [mav, DIF, DAMV, STD, RMS]

        ecuacion_mano, prom_ecuaciones, desvio = algo.Score(
            coeficientes_mano, referencias_mano, variables, len(EMG1))

        return jsonify(ecuacion_mano=ecuacion_mano, prom_ecuaciones=prom_ecuaciones, desvio=desvio, EMG1=EMG1, EMG2=EMG2, tiempo=tiempo)
    except Exception as e:
        return str(e)


@app.route('/_calculoScoreSimple')
def _calculoScoreSimple():

    a = request.args.get('a', 0, type=float)
    b = request.args.get('b', 0, type=float)
    c = request.args.get('c', 0, type=float)
    d = request.args.get('d', 0, type=float)

    MAVref1 = request.args.get('MAVref1', 0, type=float)
    DAMVref1 = request.args.get('DAMVref1', 0, type=float)
    STDref1 = request.args.get('STDref1', 0, type=float)
    RMSref1 = request.args.get('RMSref1', 0, type=float)

    Umbral = request.args.get('Umbral', 0, type=float)

    coeficientes_mano = [a, b, c, d]
    referencias_mano = [MAVref1, DAMVref1, STDref1, RMSref1]

    mav, DAMV, STD, RMS, tiempo, EMG1 = algo.VentanaSimple(arduino)

    variables = [mav, DAMV, STD, RMS]

    print('prueba 1')
    ecuacion_mano, prom_ecuaciones, desvio = algo.ScoreSimple(
        coeficientes_mano, referencias_mano, variables, len(EMG1))

    print('prueba 2')

    return jsonify(ecuacion_mano=ecuacion_mano, prom_ecuaciones=prom_ecuaciones, desvio=desvio, EMG1=EMG1, tiempo=tiempo)


@app.route('/_Guardar1')
def _Guardar1():

    canal1Mano = request.args.get('canal1Mano', 0, type=int)
    canal2Mano = request.args.get('canal2Mano', 0, type=int)

    aMano = request.args.get('aMano', 0, type=int)
    bMano = request.args.get('bMano', 0, type=int)
    cMano = request.args.get('cMano', 0, type=int)
    dMano = request.args.get('dMano', 0, type=int)
    eMano = request.args.get('eMano', 0, type=int)

    MAVref1Mano = request.args.get('MAVref1Mano', 0, type=int)
    DIFref1Mano = request.args.get('DIFref1Mano', 0, type=int)
    DAMVref1Mano = request.args.get('DAMVref1Mano', 0, type=int)
    STDref1Mano = request.args.get('STDref1Mano', 0, type=int)
    RMSref1Mano = request.args.get('RMSref1Mano', 0, type=int)

    MAVref2Mano = request.args.get('MAVref2Mano', 0, type=int)
    DIFref2Mano = request.args.get('DIFref2Mano', 0, type=int)
    DAMVref2Mano = request.args.get('DAMVref2Mano', 0, type=int)
    STDref2Mano = request.args.get('STDref2Mano', 0, type=int)
    RMSref2Mano = request.args.get('RMSref2Mano', 0, type=int)

    UmbralMano = request.args.get('UmbralMano', 0, type=int)

    canal1Mun = request.args.get('canal1Mun', 0, type=int)
    canal2Mun = request.args.get('canal2Mun', 0, type=int)

    aMun = request.args.get('aMun', 0, type=int)
    bMun = request.args.get('bMun', 0, type=int)
    cMun = request.args.get('cMun', 0, type=int)
    dMun = request.args.get('dMun', 0, type=int)
    eMun = request.args.get('eMun', 0, type=int)

    MAVref1Mun = request.args.get('MAVref1Mun', 0, type=int)
    DIFref1Mun = request.args.get('DIFref1Mun', 0, type=int)
    DAMVref1Mun = request.args.get('DAMVref1Mun', 0, type=int)
    STDref1Mun = request.args.get('STDref1Mun', 0, type=int)
    RMSref1Mun = request.args.get('RMSref1Mun', 0, type=int)

    MAVref2Mun = request.args.get('MAVref2Mun', 0, type=int)
    DIFref2Mun = request.args.get('DIFref2Mun', 0, type=int)
    DAMVref2Mun = request.args.get('DAMVref2Mun', 0, type=int)
    STDref2Mun = request.args.get('STDref2Mun', 0, type=int)
    RMSref2Mun = request.args.get('RMSref2Mun', 0, type=int)

    UmbralMun = request.args.get('UmbralMun', 0, type=int)

    ParMano = [canal1Mano, canal2Mano, aMano,
               bMano, cMano, dMano, eMano, UmbralMano]
    ParMun = [canal1Mun, canal2Mun, aMun, bMun, cMun, dMun, eMun, UmbralMun]

    RefMano = [MAVref1Mano, MAVref2Mano, DIFref1Mano, DIFref2Mano, DAMVref1Mano,
               DAMVref2Mano, STDref1Mano, STDref2Mano, RMSref1Mano, RMSref2Mano]
    RefMun = [MAVref1Mun, MAVref2Mun, DIFref1Mun, DIFref2Mun, DAMVref1Mun,
              DAMVref2Mun, STDref1Mun, STDref2Mun, RMSref1Mun, RMSref2Mun]

    algo.Enviar_parametros(ParMano, ParMun, RefMano, RefMun, arduino)
    arduino.close()
    return jsonify(a='')


@app.route('/_Guardar1Simple')
def _Guardar1Simple():

    aMun = request.args.get('aMun', 0, type=int)
    bMun = request.args.get('bMun', 0, type=int)
    cMun = request.args.get('cMun', 0, type=int)
    dMun = request.args.get('dMun', 0, type=int)

    MAVref1Mun = request.args.get('MAVref1Mun', 0, type=int)
    DAMVref1Mun = request.args.get('DAMVref1Mun', 0, type=int)
    STDref1Mun = request.args.get('STDref1Mun', 0, type=int)
    RMSref1Mun = request.args.get('RMSref1Mun', 0, type=int)

    UmbralMun = request.args.get('UmbralMun', 0, type=int)

    ParMun = [aMun, bMun, cMun, dMun, UmbralMun]

    RefMun = [MAVref1Mun, DAMVref1Mun, STDref1Mun, RMSref1Mun]
    print(RefMun)

    algo.Enviar_parametrosSimple(ParMun, RefMun, arduino)
    return jsonify(a='')


if __name__ == '__main__':
    app.run(debug=True)
