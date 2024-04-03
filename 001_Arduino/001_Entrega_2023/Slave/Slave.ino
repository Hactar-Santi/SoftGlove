#include <Wire.h>
#include <stdlib.h>

int x = 0;
int statusLectura = 0;
#define EMGPin1 A0
#define EMGPin2 A1

int EMG1 = 0;
int EMG2 = 0;

void setup() {
  Serial.begin(57600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(EMGPin1, INPUT);
  pinMode(EMGPin2, INPUT);
  // Inicializa el bus I2C como slave en dispositivo 9.
  Wire.begin(9);
  // configurar funciones para los eventos de envio y recepecion.
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void requestEvent() {  // accion a realizar cuando se pide un evento.
  // EMG1 = analogRead(EMGPin1);
  // EMG2 = analogRead(EMGPin2);
  byte valArray[4];
  valArray[0] = (EMG1 >> 8) & 0xFF;
  valArray[1] = EMG1 & 0xFF;
  valArray[2] = (EMG2 >> 8) & 0xFF;
  valArray[3] = EMG2 & 0xFF;
  Wire.write(valArray, 4);
}

void receiveEvent() {  // accion a realizar cuando se recibe un dato.
  x = Wire.read();
}

void loop() {
  EMG1 = analogRead(EMGPin1);
  EMG2 = analogRead(EMGPin2);
  if (x == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else if (x == 1) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  // Serial.print(0);
  // Serial.print(" ");
  // Serial.print(1000);
  // Serial.print(" ");
  // Serial.print(EMG1);
  // Serial.print(" ");
  // Serial.println(EMG2);
}
