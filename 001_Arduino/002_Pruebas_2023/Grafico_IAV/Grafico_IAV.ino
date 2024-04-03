#define LED 13
#define LED2 10

unsigned long myTime;
int a;
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
int s = 0;
long x;
int i = 0;
float AVG1, STD1, DIF1, DAMV1, MFL1, IAV1, DASDV1;
float AVG2, STD2, DIF2, DAMV2, MFL2, IAV2, DASDV2;
float AVG1_mano, STD1_mano, DIF1_mano, DAMV1_mano, MFL1_mano, IAV1_mano, DASDV1_mano;
float AVG2_mano, STD2_mano, DIF2_mano, DAMV2_mano, MFL2_mano, IAV2_mano, DASDV2_mano;
float AVG1_muneca, STD1_muneca, DIF1_muneca, DAMV1_muneca, MFL1_muneca, IAV1_muneca, DASDV1_muneca;
float AVG2_muneca, STD2_muneca, DIF2_muneca, DAMV2_muneca, MFL2_muneca, IAV2_muneca, DASDV2_muneca;
int lectura_EMG1 = A0;
int lectura_EMG2 = A8;
bool score_listo = false; // Este flag es el que luego me va a indicar si el buffer1 se cargó por completo
int send_counter = 0;

//VER DE DEFINIR ESTO EN EL LOOP
int ParMano [8] = {0, 1, 100, 0, 0, 0, 0, 60};
int ParMun [8] = {1, 0, 100, 0, 0, 0, 0, 60};
int RefMano [10] = {68, 172, -239, -239, 54, 119, 57, 143, 59, 59};
int RefMun [10] = {177, 51, 240, 240, 136, 22, 147, 43, 60, 60};

int ParMunSimple [5];
int RefMunSimple [4];

int resultado;
int incoming [9];




float promedio(float vector[]) {
  float sum = 0;
  float avg = 0;
  for (int j = 0; j < numReadings; j++) {
    sum += vector[j];
  }
  avg = (float)sum * 0.4 / (float)numReadings;
  return avg ;
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
  return damveee * 40;
}

float mfl(float vector[]) {
  /*
     log10(sqrt(N-1Σi=1 |xi+1-xi|^2))
  */
  float sumamfl = 0;
  float raiz = 0;
  float Mfl = 0;
  for (int j = 0; j < numReadings - 1; j++) {
    sumamfl += pow(vector[j + 1] - vector[j], 2);
  }
  raiz = sqrt(sumamfl);
  Mfl = log10(raiz);
  return Mfl * 50;
}

float iav(float vector[]) {
  /*
     NΣi=1 |xi|
  */

  float sumaiav = 0;

  for (int j = 0; j < numReadings; j++) {
    sumaiav += vector[j];
  }
  return sumaiav / 120;
}



void calculo_scoresSimple(int a[5], int b[4]) {
  static unsigned long counter = 0;  // El "static" hace que la variable no vuelva a 0 cada vez que se la llama.
  static unsigned int i = 0;
  counter++;
  if (counter % 1 == 0) { // cada 1 ms
    aread1 = analogRead(lectura_EMG1); // Tomo el valor en la entrada analogica A0 (puede ser cualquier puerto analógico A1,A2,etc)
    analogVals1[i] = aread1;
    i ++;
  }
  if (i % numReadings == 0) {
    i = 0;

    AVG1 = promedio(analogVals1);


    if (AVG1 > b[0]) {
      AVG1_muneca = 1;
    } else {
      AVG1_muneca = 1 - (abs(b[0] - AVG1) / (b[0]));
    }



    DAMV1 = damv(analogVals1);
    DAMV1_muneca = 1 - (abs(b[1] - DAMV1) / (b[1]));

    IAV1 = iav(analogVals1);
    IAV1_muneca = 1 - (abs(b[2] - IAV1) / (b[2]));

    MFL1 = mfl(analogVals1);
    MFL1_muneca = 1 - (abs(b[3] - MFL1) / (b[3]));


    score1_muneca = float(a[0] * AVG1_muneca + a[1] * DAMV1_muneca + a[2] * IAV1_muneca + a[3] * MFL1_muneca);


    if (isnan(score1_muneca)) {
      score1_muneca = 0;
    }

    ecu_muneca = score1_muneca;

    score_listo = true;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(20);
  pinMode(LED_BUILTIN, OUTPUT);



}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t tStart = millis(); // ms; start time
  const uint32_t DESIRED_PERIOD = 1; // ms
  uint32_t tNow = millis(); // ms; time now
  unsigned int j = 0;

  int incoming [9] = {50, 50, 0, 0, 50, 249, 224, 182, 147  };

  for (int i = 0; i < 9; i++) {
    if (i < 5) {
      ParMunSimple[i] = incoming[i];
    } else if (i < 9) {
      RefMunSimple[i - 5] = incoming[i];
    }
  }



  if (tNow - tStart >= DESIRED_PERIOD) {
    //digitalWrite(LED, HIGH);  // Enciende el LED


    tStart += DESIRED_PERIOD;

    //VER DE REHACER ESTO, INGRESANDO PARAMETROS Y QUE ME DEVUELVA ALGO
    calculo_scoresSimple(ParMunSimple,RefMunSimple);
    if (score_listo) {

      // Add "fake" plots to stabilize Y axis
      Serial.print(0); // To freeze the lower limit
      Serial.print(" ");
      Serial.print(100); // To freeze the upper limit
      Serial.print(" ");

      // Print value to Serial Monitor
      Serial.print(MFL1_muneca*100);
      Serial.print(" ");
      Serial.print(50);
      Serial.print(" ");
      Serial.println(ecu_muneca);
      score_listo = false;

      if (ecu_muneca >= (ParMunSimple[4] )) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);

      }
    }



  }
}
