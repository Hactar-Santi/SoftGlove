#include <Servo.h>

const unsigned int numReadings = 100;
float analogVals1[numReadings];
float score1_mano;
float score2_mano;
float aread1;
float analogVals2[numReadings];
float score1_muneca;
float score2_muneca;
float aread2;
float ecu_mano = 0;
float ecu_muneca = 0;
float EMG1;
float EMG2;
int SERVO_PIN1 = 7;
int SERVO_PIN2 = 6;
int s = 0;
long x;
int i = 0;
float AVG1, DIF1, DAMV1, RMS1, STD1, DASDV1;
float AVG2, DIF2, DAMV2, RMS2, STD2, DASDV2;
float AVG1_mano, DIF1_mano, DAMV1_mano, RMS1_mano, STD1_mano, DASDV1_mano;
float AVG2_mano, DIF2_mano, DAMV2_mano, RMS2_mano, STD2_mano, DASDV2_mano;
float AVG1_muneca, DIF1_muneca, DAMV1_muneca, RMS1_muneca, STD1_muneca, DASDV1_muneca;
float AVG2_muneca, DIF2_muneca, DAMV2_muneca, RMS2_muneca, STD2_muneca, DASDV2_muneca;
int lectura_EMG1 = A8;
int lectura_EMG2 = A0;
bool score_listo = false;  // Este flag es el que luego me va a indicar si el buffer1 se cargó por completo
int send_counter = 0;
int ParMano[8] = { 1, 0, 25, 25, 25, 0, 25, 50 };
int ParMun[8] = { 0, 1, 20, 20, 20, 0, 40, 45 };
int RefMun[10] = { 87, 32, 1, 92, 82, 28, 96, 42, 84, 28 };
int RefMano[10] = { 41, 164, 0, -173, 20, 157, 36, 144, 35, 156 };



Servo SERVO_1;
Servo SERVO_2;
Servo SERVO_3;

float promedio(float vector[]) {
  float sum = 0;
  float avg = 0;
  for (int j = 0; j < numReadings; j++) {
    sum += vector[j];
  }
  avg = (float)sum * 0.3 / (float)numReadings;
  return avg;
}

float DIF(float vector1[], float vector2[]) {
  /*
     Σ|xi-x(i-1)|
  */
  float sumaDIF = 0;
  for (int j = 0; j < numReadings; j++) {
    sumaDIF += vector1[j] - vector2[j];
  }
  sumaDIF = sumaDIF / numReadings;
  return sumaDIF * 0.4;
}

float damv(float vector[]) {
  /*
     1/N* N-1Σi=1 |xi+1-xi|
  */
  float damveee = 0;
  float sumadamv = 0;
  for (int j = 0; j < numReadings - 1; j++) {
    sumadamv += abs(vector[j + 1] - vector[j]);
  }
  damveee = sumadamv / float(numReadings);
  return damveee * 35;
}

float RMS(float vector[]) {
  /*
     log10(sqrt(N-1Σi=1 |xi+1-xi|^2))
  */
  float sumaRMS = 0;

  for (int j = 0; j < numReadings - 1; j++) {
    sumaRMS += pow(2, vector[j]);
  }
  sumaRMS = sumaRMS / numReadings;
  sumaRMS = sqrt(sumaRMS);
  return sumaRMS * 2.5;
}
float STD(float vector[], int promedio) {
  /*
     NΣi=1 |xi|
  */

  float sumaSTD = 0;

  for (int j = 0; j < numReadings; j++) {
    sumaSTD += pow(2, (vector[j] - promedio));
  }

  sumaSTD = sqrt(sumaSTD / numReadings);
  return sumaSTD * 1.7;
}



void calculo_scores() {
  static unsigned long counter = 0;  // El "static" hace que la variable no vuelva a 0 cada vez que se la llama.
  static unsigned int i = 0;
  counter++;
  if (counter % 1 == 0) {               // cada 1 ms
    aread1 = analogRead(lectura_EMG1);  // Tomo el valor en la entrada analogica A0 (puede ser cualquier puerto analógico A1,A2,etc)
    aread2 = analogRead(lectura_EMG2);  // Tomo el valor en la entrada analogica A0 (puede ser cualquier puerto analógico A1,A2,etc)
    analogVals1[i] = aread1;
    analogVals2[i] = aread2;
    i++;
  }
  if (i % numReadings == 0) {
    i = 0;

    if (RefMano[2] == 0) {  //Cambio el valor a negativo del diff de ser necesario
      RefMano[3] = RefMano[3] * (-1);
      RefMano[2] = -1;
    }

    if (RefMun[2] == 0) {
      RefMun[3] = RefMun[3] * (-1);
      RefMun[2] = -1;
    }

    AVG1 = promedio(analogVals1);


    if (AVG1 > RefMano[0]) {
      AVG1_mano = 1;
    } else {
      AVG1_mano = 1 - (abs(RefMano[0] - AVG1) / (RefMano[0]));
    }

    if (AVG1 > RefMun[0]) {
      AVG1_muneca = 1;
    } else {
      AVG1_muneca = 1 - (abs(RefMun[0] - AVG1) / (RefMun[0]));
    }

    AVG2 = promedio(analogVals2);


    if (AVG2 > RefMano[1]) {
      AVG2_mano = 1;
    } else {
      AVG2_mano = 1 - (abs(RefMano[1] - AVG2) / (RefMano[1]));
    }

    if (AVG2 > RefMun[1]) {
      AVG2_muneca = 1;
    } else {
      AVG2_muneca = 1 - (abs(RefMun[1] - AVG2) / (RefMun[1]));
    }


    DIF1 = DIF(analogVals1, analogVals2);

    DIF1_mano = 1 - abs((RefMano[2] - DIF1) / (RefMano[2]));

    DIF1_muneca = 1 - abs((RefMun[2] - DIF1) / (RefMun[2]));
    if (DIF1_mano > 1) {
      DIF1_mano = 1;
    } else if (DIF1_mano < 0) {
      (DIF1_mano = 0);
    }

    if (DIF1_muneca > 1) {
      DIF1_muneca = 1;
    } else if (DIF1_muneca < 0) {
      (DIF1_muneca = 0);
    }

    DIF2_mano = DIF1_mano;

    DIF2_muneca = DIF2_muneca;


    DAMV1 = damv(analogVals1);
    DAMV2 = damv(analogVals2);


    if (DAMV1 > RefMano[4]) {
      DAMV1_mano = 1;
    } else {
      DAMV1_mano = 1 - (abs(RefMano[4] - DAMV1) / (RefMano[4]));
    }

    if (DAMV1 > RefMun[4]) {
      DAMV1_muneca = 1;
    } else {
      DAMV1_muneca = 1 - (abs(RefMun[4] - DAMV1) / (RefMun[4]));
    }


    if (DAMV2 > RefMano[5]) {
      DAMV2_mano = 1;
    } else {
      DAMV2_mano = 1 - (abs(RefMano[5] - DAMV2) / (RefMano[5]));
    }

    if (DAMV2 > RefMun[5]) {
      DAMV2_muneca = 1;
    } else {
      DAMV2_muneca = 1 - (abs(RefMun[5] - DAMV2) / (RefMun[5]));
    }






    STD1 = STD(analogVals1, AVG1);
    STD2 = STD(analogVals2, AVG2);

    if (STD1 > RefMano[6]) {
      STD1_mano = 1;
    } else {
      STD1_mano = 1 - (abs(RefMano[6] - STD1) / (RefMano[6]));
    }

    if (STD1 > RefMun[6]) {
      STD1_muneca = 1;
    } else {
      STD1_muneca = 1 - (abs(RefMun[6] - STD1) / (RefMun[6]));
    }


    if (STD2 > RefMano[7]) {
      STD2_mano = 1;
    } else {
      STD2_mano = 1 - (abs(RefMano[7] - STD2) / (RefMano[7]));
    }

    if (STD2 > RefMun[7]) {
      STD2_muneca = 1;
    } else {
      STD2_muneca = 1 - (abs(RefMun[7] - STD2) / (RefMun[7]));
    }

    RMS1 = RMS(analogVals1);
    RMS2 = RMS(analogVals2);

    if (RMS1 > RefMano[8]) {
      RMS1_mano = 1;
    } else {
      RMS1_mano = 1 - (abs(RefMano[8] - RMS1) / (RefMano[8]));
    }

    if (RMS1 > RefMun[8]) {
      RMS1_muneca = 1;
    } else {
      RMS1_muneca = 1 - (abs(RefMun[8] - RMS1) / (RefMun[8]));
    }


    if (RMS2 > RefMano[9]) {
      RMS2_mano = 1;
    } else {
      RMS2_mano = 1 - (abs(RefMano[9] - RMS2) / (RefMano[9]));
    }

    if (RMS2 > RefMun[9]) {
      RMS2_muneca = 1;
    } else {
      RMS2_muneca = 1 - (abs(RefMun[9] - RMS2) / (RefMun[9]));
    }

    score1_mano = float(ParMano[2] * AVG1_mano + ParMano[3] * DIF1_mano + ParMano[4] * DAMV1_mano + ParMano[5] * STD1_mano + ParMano[6] * RMS1_mano);
    score2_mano = float(ParMano[2] * AVG2_mano + ParMano[3] * DIF2_mano + ParMano[4] * DAMV2_mano + ParMano[5] * STD2_mano + ParMano[6] * RMS2_mano);

    score1_muneca = float(ParMun[2] * AVG1_muneca + ParMun[3] * DIF1_muneca + ParMun[4] * DAMV1_muneca + ParMun[5] * STD1_muneca + ParMun[6] * RMS1_muneca);
    score2_muneca = float(ParMun[2] * AVG2_muneca + ParMun[3] * DIF2_muneca + ParMun[4] * DAMV2_muneca + ParMun[5] * STD2_muneca + ParMun[6] * RMS2_muneca);

    if (isnan(score1_mano)) {
      score1_mano = 0;
    }
    if (isnan(score2_mano)) {
      score2_mano = 0;
    }
    if (isnan(score1_muneca)) {
      score1_muneca = 0;
    }
    if (isnan(score2_muneca)) {
      score2_muneca = 0;
    }

    ecu_mano = float(ParMano[0] * score1_mano + ParMano[1] * score2_mano);
    ecu_muneca = float(ParMun[0] * score1_muneca + ParMun[1] * score2_muneca);

    score_listo = true;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SERVO_1.attach(SERVO_PIN1);
  SERVO_2.attach(SERVO_PIN2);
  Serial.setTimeout(20);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t tStart = millis();  // ms; start time
  const uint32_t DESIRED_PERIOD = 1;  // ms
  uint32_t tNow = millis();           // ms; time now
  unsigned int j = 0;

  if (tNow - tStart >= DESIRED_PERIOD) {

    tStart += DESIRED_PERIOD;
    calculo_scores();
    if (score_listo) {

      // Add "fake" plots to stabilize Y axis
      Serial.print(0);  // To freeze the lower limit
      Serial.print(" ");
      Serial.print(100);  // To freeze the upper limit
      Serial.print(" ");

      // Print value to Serial Monitor
      Serial.print(STD1);
      Serial.print(" ");
      Serial.print(50);
      Serial.print(" ");
      Serial.println(STD2);
      score_listo = false;
    }
  }
}
