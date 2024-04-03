#define EMGPin1 A0
#define EMGPin2 A1

int numReadings = 100;
unsigned int readIndex = 0;
int readingsEMG1[100];
int readingsEMG2[100];

// variables que se emplean en la lectura de datos.
float suma1, suma2, promedio1, promedio2, dif, sumaCuadradosSTD1, sumaCuadradosSTD2, sumaCuadradosRMS1, sumaCuadradosRMS2, RMS1, RMS2, STD1, STD2, DAMV1, DAMV2 = 0;

// marcadores de posicion para DAMV.
int posicionAnterior;
int posicionPosterior1;
int posicionPosterior2;

// variables que se emplean con referencias (se aplica un cociente).
float AVG_mano, DIF_mano, DAMV_mano, RMS_mano, STD_mano;
float AVG_mun, DIF_mun, DAMV_mun, RMS_mun, STD_mun;
float score_mano, score_mun;

// set de datos.
int ParMano[8] = { 0, 1, 20, 40, 10, 10, 20, 30 };
int ParMun[8] = { 1, 0, 15, 40, 10, 10, 25, 25 };
int RefMano[10] = { 34, 242, -249, -249, 29, 211, 18, 166, 34, 242 };
int RefMun[10] = { 213, 21, 231, 231, 190, 67, 127, 48, 213, 21 };

int contador = 0;  // para calcular cantidad de muestras en intervalo de tiempo.

void setup() {
  Serial.begin(57600);
  pinMode(EMGPin1, INPUT);
  pinMode(EMGPin2, INPUT);
  for (int i = 0; i < 100; i++) {
    readingsEMG1[i] = 0;
    readingsEMG2[i] = 0;
  }
}

void loop() {
  to2(ParMano, ParMun, RefMano, RefMun);
  // print de variables para debuggear.
  Serial.print(0);
  Serial.print(" ");
  Serial.print(100);
  // Serial.print(" ");
  // Serial.print(readingsEMG1[readIndex]);
  // Serial.print(" ");
  // Serial.print(readingsEMG2[readIndex]);
  Serial.print(" ");
  Serial.print(score_mano);
  Serial.print(" ");
  Serial.println(score_mun);


  // print de tiempo para debuggear
  // contador += 1;
  // if (contador == 10000) {
  //   Serial.print("El tiempo es: ");
  //   Serial.println(millis());
  // }
}

void to2(int ParMano[8], int ParMun[8], int RefMano[10], int RefMun[10]) {  // readIndex dicta todas las variables.
  readingsEMG1[readIndex] = analogRead(EMGPin1);
  readingsEMG2[readIndex] = analogRead(EMGPin2);
  if (readIndex == 99) {  // caso donde readIndex llega a la ultima posicion.
    // Suma.
    suma1 += readingsEMG1[readIndex] - readingsEMG1[0];
    suma2 += readingsEMG2[readIndex] - readingsEMG2[0];
    // Diff.
    dif += (readingsEMG1[readIndex] - readingsEMG2[readIndex]) - (readingsEMG1[0] - readingsEMG2[0]);
    // STD.
    sumaCuadradosSTD1 += pow(readingsEMG1[readIndex] - promedio1, 2) - pow(readingsEMG1[0] - promedio1, 2);
    sumaCuadradosSTD2 += pow(readingsEMG2[readIndex] - promedio2, 2) - pow(readingsEMG2[0] - promedio2, 2);
    // RMS.
    sumaCuadradosRMS1 += pow(readingsEMG1[readIndex], 2) - pow(readingsEMG1[0], 2);
    sumaCuadradosRMS2 += pow(readingsEMG2[readIndex], 2) - pow(readingsEMG2[0], 2);
  } else {
    // Suma.
    suma1 += readingsEMG1[readIndex] - readingsEMG1[readIndex + 1];
    suma2 += readingsEMG2[readIndex] - readingsEMG2[readIndex + 1];
    // Diff.
    dif += (readingsEMG1[readIndex] - readingsEMG2[readIndex]) - (readingsEMG1[readIndex + 1] - readingsEMG2[readIndex + 1]);
    // STD.
    sumaCuadradosSTD1 += pow(readingsEMG1[readIndex] - promedio1, 2) - pow(readingsEMG1[readIndex + 1] - promedio1, 2);
    sumaCuadradosSTD2 += pow(readingsEMG2[readIndex] - promedio2, 2) - pow(readingsEMG2[readIndex + 1] - promedio2, 2);
    // RMS.
    sumaCuadradosRMS1 += pow(readingsEMG1[readIndex], 2) - pow(readingsEMG1[readIndex + 1], 2);
    sumaCuadradosRMS2 += pow(readingsEMG2[readIndex], 2) - pow(readingsEMG2[readIndex + 1], 2);
  }
  // calculo de DAMV.
  posicionAnterior = readIndex - 1;
  posicionPosterior1 = readIndex + 1;
  posicionPosterior2 = readIndex + 2;
  if (posicionAnterior == -1) {
    posicionAnterior = 99;
  }
  if (posicionPosterior2 == 100) {
    posicionPosterior2 = 0;
  } else if (posicionPosterior2 == 101) {
    posicionPosterior2 = 1;
    posicionPosterior1 = 0;
  }
  DAMV1 += (abs((readingsEMG1[readIndex] - readingsEMG1[posicionAnterior])) - abs((readingsEMG1[posicionPosterior2] - readingsEMG1[posicionPosterior1]))) / 100.0;
  DAMV2 += (abs((readingsEMG2[readIndex] - readingsEMG2[posicionAnterior])) - abs((readingsEMG2[posicionPosterior2] - readingsEMG2[posicionPosterior1]))) / 100.0;
  // calculo de promedios, paso intermedio.
  promedio1 = suma1 / 100.0;
  promedio2 = suma2 / 100.0;
  // paso intermedio para calculo de STD.
  STD1 = sqrt(sumaCuadradosSTD1 / 100.0);
  STD2 = sqrt(sumaCuadradosSTD2 / 100.0);
  // paso intermedio para calculo de RMS.
  RMS1 = sqrt(sumaCuadradosRMS1 / 100.0);
  RMS2 = sqrt(sumaCuadradosRMS2 / 100.0);
  // Dif. Es el unico estadistico que se puede calcular sin discriminar canal.
  DIF_mun = (dif / (float)RefMun[3]) * 0.3 / 100.0;
  DIF_mano = (dif / (float)RefMano[3]) * 0.3 / 100.0;
  if (DIF_mun > 1) {
    DIF_mun = 1;
  } else if (DIF_mun < 0) {
    DIF_mun = 0;
  }
  if (DIF_mano > 1) {
    DIF_mano = 1;
  } else if (DIF_mano < 0) {
    DIF_mano = 0;
  }
  // Discriminar canal para calcular estadisticos.
  // para la mano -------------------------------------------------
  if (ParMano[0] == 1) {                                // canal 1.
    AVG_mano = (promedio1 / (float)RefMano[0]) * 0.25;  // promedio.
    DAMV_mano = (DAMV1 / (float)RefMano[4]) * 20.0;     // DAMV.
    STD_mano = (STD1 / (float)RefMano[6]) * 0.8;        // STD
    RMS_mano = (RMS1 / (float)RefMano[8]) * 0.25;       // RMS
  } else {                                              // canal 2.
    AVG_mano = (promedio2 / (float)RefMano[1]) * 0.25;  // promedio.
    DAMV_mano = (DAMV2 / (float)RefMano[5]) * 20.0;     // DAMV.
    STD_mano = (STD2 / (float)RefMano[7]) * 0.8;        // STD
    RMS_mano = (RMS2 / (float)RefMano[9]) * 0.25;       // RMS
  }
  // para la muneca -------------------------------------------------
  if (ParMun[0] == 1) {                               // canal 1.
    AVG_mun = (promedio1 / (float)RefMun[0]) * 0.25;  // promedio.
    DAMV_mun = (DAMV1 / (float)RefMun[4]) * 20.0;     // DAMV.
    STD_mun = (STD1 / (float)RefMun[6]) * 0.8;        // STD
    RMS_mun = (RMS1 / (float)RefMun[8]) * 0.25;       // RMS
  } else {                                            // canal 2.
    AVG_mun = (promedio2 / (float)RefMun[1]) * 0.25;  // promedio.
    DAMV_mun = (DAMV2 / (float)RefMun[5]) * 20.0;     // DAMV.
    STD_mun = (STD2 / (float)RefMun[7]) * 0.8;        // STD
    RMS_mun = (RMS2 / (float)RefMun[9]) * 0.25;       // RMS
  }
  // condicion de borde promedio.
  if (AVG_mun > 1) {
    AVG_mun = 1;
  } else if (AVG_mun < 0) {
    AVG_mun = 0;
  }
  if (AVG_mano > 1) {
    AVG_mano = 1;
  } else if (AVG_mano < 0) {
    AVG_mano = 0;
  }
  // condicion de borde DAMV.
  if (DAMV_mun > 1) {
    DAMV_mun = 1;
  } else if (DAMV_mun < 0) {
    DAMV_mun = 0;
  }
  if (DAMV_mano > 1) {
    DAMV_mano = 1;
  } else if (DAMV_mano < 0) {
    DAMV_mano = 0;
  }
  // condicion de borde STD.
  if (STD_mun > 1) {
    STD_mun = 1;
  } else if (STD_mun < 0) {
    STD_mun = 0;
  }
  if (STD_mano > 1) {
    STD_mano = 1;
  } else if (STD_mano < 0) {
    STD_mano = 0;
  }
  // condicion de borde RMS.
  if (RMS_mun > 1) {
    RMS_mun = 1;
  } else if (RMS_mun < 0) {
    RMS_mun = 0;
  }
  if (RMS_mano > 1) {
    RMS_mano = 1;
  } else if (RMS_mano < 0) {
    RMS_mano = 0;
  }
  // calculo de score para mano y muneca.
  score_mano = ParMano[2] * AVG_mano + ParMano[3] * DIF_mano + ParMano[4] * DAMV_mano + ParMano[5] * STD_mano + ParMano[6] * RMS_mano;
  score_mun = ParMun[2] * AVG_mun + ParMun[3] * DIF_mun + ParMun[4] * DAMV_mun + ParMun[5] * STD_mun + ParMun[6] * RMS_mun;
  // condicion para aumentar readIndex.
  if (readIndex == 99) {
    readIndex = 0;
  } else {
    readIndex++;
  }
}
