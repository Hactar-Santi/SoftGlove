#define EMGPin1 A0
#define EMGPin2 A1

int numReadings = 100;
unsigned int readIndex = 0;
int readingsEMG1[100];
float promedios1[100];

// variables que se emplean en la lectura de datos.
float suma1, promedio1, sumaCuadradosSTD1, sumaCuadradosRMS1, RMS1, STD1, DAMV1 = 0;

// marcadores de posicion para DAMV.
int posicionAnterior;
int posicionPosterior1;
int posicionPosterior2;

// variables que se emplean con referencias (se aplica un cociente).
float AVG_mano, DIF_mano, DAMV_mano, RMS_mano, STD_mano;
float AVG_mun, DIF_mun, DAMV_mun, RMS_mun, STD_mun;
float score_mun;

// set de datos.

int ParMunSimple[5] = { 25, 25, 25, 25, 25 };

int RefMunSimple[4] = { 256, 78, 95, 256 };


void setup() {
  Serial.begin(57600);
  pinMode(EMGPin1, INPUT);
  pinMode(EMGPin2, INPUT);
  for (int i = 0; i < 100; i++) {
    readingsEMG1[i] = 0;
    promedios1[i] = 0;
  }
}

void loop() {
  to1(ParMunSimple, RefMunSimple);
  // print de variables para debuggear.
  Serial.print(0);
  Serial.print(" ");
  Serial.print(100);
  Serial.print(" ");
  Serial.print(readingsEMG1[readIndex] / 10);
  Serial.print(" ");
  Serial.println(score_mun);
}

void to1(int ParMunSimple[5], int RefMunSimple[4]) {  // readIndex dicta todas las variables.
  readingsEMG1[readIndex] = analogRead(EMGPin1);
  if (readIndex == 99) {  // caso donde readIndex llega a la ultima posicion.
    // Suma.
    suma1 += readingsEMG1[readIndex] - readingsEMG1[0];
    // calculo de promedios, paso intermedio.
    promedio1 = suma1 / 100.0;
    promedios1[readIndex] = promedio1;
    // STD.
    sumaCuadradosSTD1 += pow(readingsEMG1[readIndex] - promedio1, 2) - pow(readingsEMG1[0] - promedios1[0], 2);
    // RMS.
    sumaCuadradosRMS1 += pow(readingsEMG1[readIndex], 2) - pow(readingsEMG1[0], 2);
  } else {
    // Suma.
    suma1 += readingsEMG1[readIndex] - readingsEMG1[readIndex + 1];
    // calculo de promedios, paso intermedio.
    promedio1 = suma1 / 100.0;
    promedios1[readIndex] = promedio1;
    // STD.
    sumaCuadradosSTD1 += pow(readingsEMG1[readIndex] - promedio1, 2) - pow(readingsEMG1[readIndex + 1] - promedios1[readIndex + 1], 2);
    // RMS.
    sumaCuadradosRMS1 += pow(readingsEMG1[readIndex], 2) - pow(readingsEMG1[readIndex + 1], 2);
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

  // paso intermedio para calculo de STD.
  STD1 = sqrt(sumaCuadradosSTD1 / 100.0);
  // paso intermedio para calculo de RMS.
  RMS1 = sqrt(sumaCuadradosRMS1 / 100.0);

  AVG_mun = (promedio1 / (float)RefMunSimple[0]) * 0.25;  // promedio.
  DAMV_mun = (DAMV1 / (float)RefMunSimple[1]) * 20.0;     // DAMV.
  STD_mun = (STD1 / (float)RefMunSimple[2]) * 0.8;        // STD
  RMS_mun = (RMS1 / (float)RefMunSimple[3]) * 0.25;       // RMS


  // condicion de borde promedio.
  if (AVG_mun > 1) {
    AVG_mun = 1;
  } else if (AVG_mun < 0) {
    AVG_mun = 0;
  }
  // condicion de borde DAMV.
  if (DAMV_mun > 1) {
    DAMV_mun = 1;
  } else if (DAMV_mun < 0) {
    DAMV_mun = 0;
  }
  // condicion de borde STD.
  if (STD_mun > 1) {
    STD_mun = 1;
  } else if (STD_mun < 0) {
    STD_mun = 0;
  }
  // condicion de borde RMS.
  if (RMS_mun > 1) {
    RMS_mun = 1;
  } else if (RMS_mun < 0) {
    RMS_mun = 0;
  }

  // calculo de score.
  score_mun = ParMunSimple[0] * AVG_mun + ParMunSimple[1] * DAMV_mun + ParMunSimple[2] * STD_mun + ParMunSimple[3] * RMS_mun;
  // condicion para aumentar readIndex.
  if (readIndex == 99) {
    readIndex = 0;
  } else {
    readIndex++;
  }
}