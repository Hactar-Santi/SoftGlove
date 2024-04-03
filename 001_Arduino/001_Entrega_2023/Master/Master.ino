#include <LiquidCrystal_I2C.h>
#include <stdlib.h>

// botones y su respectiva funcion.
#define BUTTON_PIN1 2
#define BUTTON_PIN2 4
#define BUTTON_PIN3 5
#define BUTTON_PIN4 3
#define BUTTON_PIN5 6
#define JOYSTICK_AXIS_X A0
#define JOYSTICK_AXIS_Y A1
#define POT_PIN1 A2

// canal 1: bomba de 12V.
// canal 2: bomba de 6v y 3V.

// bomba 12V con solenoides -------> bomba1, solenoide1, presion1.
// el solenoide de la bomba de 12 V necesita estar en HIGH para dejar el paso de aire a los actuadores. El resto de bombas y solenoides no tienen este comportamiento.
#define sensorPresion1 A3
#define bomba1 22
#define solenoide1 25
int presionMax1 = 150;  // presion maxima para el actuador.
int presionMin1 = 90;  // marca el momento en el que se deebe prender la bomba.
float sensorValuePresion1;
float lecturaPresion1;
float presion1;
int presionMax1_escalada;
int presionMin1_escalada;
// bomba 6V -------> bomba2, solenoide2, presion2.
#define sensorPresion2 A4
#define bomba2 23
#define solenoide2 26
int presionMax2 = 35;  // presion maxima para el actuador.
int presionMin2 = 25;  // marca el momento en el que se deebe prender la bomba.
float sensorValuePresion2;
float lecturaPresion2;
float presion2;
int presionMax2_escalada;
int presionMin2_escalada;
// bomba 3V -------> bomba3, solenoid3, presion3.
#define sensorPresion3 A5
#define bomba3 24
#define solenoide3 27
int presionMax3 = 30;  // presion maxima para el actuador.
int presionMin3 = 25;  // marca el momento en el que se deebe prender la bomba.
float sensorValuePresion3;
float lecturaPresion3;
float presion3;
int presionMax3_escalada;
int presionMin3_escalada;


// estado de los botones.
int buttonState1;
int buttonState2;
int buttonState3;
int buttonState4;
int buttonState5;
int yDown;
int yUp;
int xLeft;
int xRight;

// variables para cursor personalizado.
unsigned long blinkTime = 0;
int blinkState = 0;
int buttonPress = 0;
unsigned long buttonTime;


// bytes para la barra de progreso.
byte b1[] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B11111 };
byte b2[] = { B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111 };
byte b3[] = { B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111 };
byte b4[] = { B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111 };
byte b5[] = { B00000, B00000, B00000, B11111, B11111, B11111, B11111, B11111 };
byte b6[] = { B00000, B00000, B11111, B11111, B11111, B11111, B11111, B11111 };
byte b7[] = { B00000, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };
byte b8[] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };

// bytes para flecha superior e inferior.
byte b9[] = { B00000, B00100, B01110, B11111, B00100, B00100, B00100, B00000 };
byte b10[] = { B00000, B00100, B00100, B00100, B11111, B01110, B00100, B00000 };

// bytes para el cursor personalizado.
byte b11[] = { B01000, B01100, B01110, B01111, B01111, B01110, B01100, B01000 };

// bytes para progreso en entrenamiento.
byte b12[] = { B00001, B00001, B00001, B00001, B00001, B00001, B00001, B00001 };
byte b13[] = { B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000 };
byte b14[] = { B00000, B11111, B11111, B00000, B00000, B11111, B11111, B00000 };

// bytes para palabras si y no.
// Si
byte b15[] = { B10000, B01111, B01111, B10001, B11110, B11110, B00001, B11111 };
byte b16[] = { B11011, B11111, B10011, B11011, B11011, B11011, B10001, B11111 };
// No
// createChar a partir de 16 deja de tomar valores, cuando se crea el caracter hace falta sobreescribir otro.
byte b17[] = { B01110, B01110, B00110, B01010, B01100, B01110, B01110, B11111 };
byte b18[] = { B11111, B11111, B10001, B01110, B01110, B01110, B10001, B11111 };

// byte para las flechas de cambio de texto.
byte b19[] = { B00000, B00100, B00110, B11111, B00110, B00100, B00000, B00000 };
byte b20[] = { B00000, B00100, B01100, B11111, B01100, B00100, B00000, B00000 };

// variables para el texto de si y no con blink y otros flags de posicion.
int yesNo = 0;
int leftRight = 0;

// variables para limpiar el duty cycle del display cuando se tenga velocidad en 100.
int velocidad100 = 0;

// variables para usar el potenciometro.
int valuePot = 0;
int level = 0;
int levelPorcentual = 0;
int porcentaje = 0;
int oldLevelPorcentual = 101;
int oldLevel = 17;

// variables que se guardan en el modo manual.
int intensidadManual = 0;
int tiempoInflado = 1;
int oldTiempoInflado = 0;
int tiempoDesinflado = 1;
int oldTiempoDesinflado = 0;
int velocidadInfladoManual = 100;
int velocidadDesinfladoManual = 100;
int DCInflado = 50;
int DCDesinflado = 50;


// variables para ventana de tiempo de movimiento.
int contPwm = 0;
unsigned long currentTime;  // estas variables se cuentan en milisegundos.
unsigned long elapsedTime;
unsigned long oldTime;
// variables para el tren de pulsos.
unsigned long elapsedTimePWM;
unsigned long elapsedTimePWM_canal1;  // para modos asistidos, se requieren marcadores adicionales de tiempo para discriminar entre canales.
unsigned long elapsedTimePWM_canal2;
unsigned long oldTimePWM;
unsigned long oldTimePWM_canal1;
unsigned long oldTimePWM_canal2;
int enCiclo = 0;
unsigned long miliInflado;     // tiempo en movimiento en milisegundos.
unsigned long miliDesinflado;  // tiempo en relax4 en milisegundo;
int frecuenciaInfladoPWM;      // toma un valor entre 0 y 100 Hz.
int frecuenciaDesinfladoPWM;   // toma un valor entre 0 y 15 Hz.
unsigned long miliPeriodoInfladoPWM;
unsigned long miliPeriodoDesinfladoPWM;
unsigned long miliTiempoOnInflado;
unsigned long miliTiempoOnDesinflado;
unsigned long miliTiempoOffInflado;
unsigned long miliTiempoOffDesinflado;
int enPWM = 0;         // flag para indicar el estado del tren de pulsos.
int enPWM_canal1 = 0;  // para modos asistidos, se requiere de un marcador de PWM por canal
int enPWM_canal2 = 0;
int estadoEntrenamiento = 0;     // flag para conocer si se está en entrenamiento o en pausa.
int escrituraEntrenamiento = 0;  // flag para sobreescribir tecto en la pantalla de entrenamiento.

// variables para poner en pausa el entrenamiento y pdoer retomar de donde se dejo.
unsigned long tiempoEnPausa;
unsigned long inicioPausa;
unsigned long finPausa;

// para la barra de progreso durante el entrenamiento.
int contProgreso = 17;  // arranca en 2 porque la barra de progreso comienza en esa posicion.
unsigned long blinkTimeAnimacion = 0;
int contAnimacion = 2;
int blinkStateAnimacion = 0;
int levelAnimacion = 0;
int oldLevelAnimacion = 17;

// parametros para la calibracion.
int enCalibracion;
int escrituraCalibracion;
int tiempoRestante;
int calibradoRapido = 0;              // para dar la pauta si se calibra por primera vez o si se quiere recuperar una calibración (modo rapido).
int calibradoCompleto = 0;            // para dar la pauta si se calibra por primera vez o si se quiere recuperar una calibración (modo completo).
unsigned long tiempoAnimacion;        // tiempo para la animacion de calibracion.
unsigned long oldTiempoAnimacion;     // tiempo de referencia para la animacion..
unsigned long comienzoCalibracion;    // marca el inicio de la calibracion.
unsigned long tiempoCalibracion;      // marca el tiempo transcurrido en calibracion.
unsigned long instanteCalibracion;    // marca un instante de tiempo para calcular tiempoCalibracion.
unsigned long referenciaCalibracion;  // para saber en que instante se encuentra la calibracion.
int16_t bigNum1;                      // parametros para la comunicacion.
int16_t bigNum2;
byte a, b, c, d;
int maxAverageEMG1 = 0;  // parametros para las medias moviles.
int maxAverageEMG2 = 0;
long averageEMG1;
long averageEMG2;
long totalEMG1 = 0;
long totalEMG2 = 0;
int numReadings1 = 100;
int readingsEMG1[100];
int readingsEMG2[100];
int modoAsistido = 0;
int sensibilidad = 0;
int sensibilidadInv = 0;
int umbralEMG1 = 0;
int umbralEMG2 = 0;
int canal1 = 0;
int canal2 = 0;

// sirve como flag para conocer el estado de la lectura.
int fila = 0;
// flag para ubicar en que menu esta el usuario.
int menu = 0;
// flag para escribir pantalla en la primera iteración.
int escritura = 0;

// variables para la calibracion completa.
int flagCom = 0;
int flagPyAr = 6;
int oldFlagPyAr = 7;
int escrituraDisplay = 0;
int cantCanales = 0;


// variables que llegan desde la calibracion de python.
int ParMano[8];
int ParMun[8];
int RefMano[10];
int RefMun[10];
int ParMunSimple[5];
int RefMunSimple[4];
int incoming[36];
int umbralMano = 0;
int umbralMun = 0;
int umbralMunSimple = 0;
int canalMano = 0;
int canalMun = 0;

// variables para funcion to2 de score.
int numReadings = 100;
int readIndex = 0;

// variables que se emplean en la lectura de datos.
float suma1, suma2, promedio1, promedio2, dif, sumaCuadradosSTD1, sumaCuadradosSTD2, 
sumaCuadradosRMS1, sumaCuadradosRMS2, RMS1, RMS2, STD1, STD2, DAMV1, DAMV2 = 0;
float promedios1[100];
float promedios2[100];

// marcadores de posicion para DAMV.
int posicionAnterior;
int posicionPosterior1;
int posicionPosterior2;

// variables que se emplean con referencias (se aplica un cociente).
float AVG_mano, DIF_mano, DAMV_mano, RMS_mano, STD_mano;
float AVG_mun, DIF_mun, DAMV_mun, RMS_mun, STD_mun;
float score_mano, score_mun;

LiquidCrystal_I2C lcd(0x27, 20, 4);  // configura la direccion de la pantalla y su dimension de 20x4.

void setup() {
  Serial.begin(57600);
  lcd.init();
  lcd.backlight();
  Wire.begin();                             // Inicializa el bus I2C como master.
  for (int i = 0; i < numReadings1; i++) {  // los arrays se completan con ceros.
    readingsEMG1[i] = 0;
    readingsEMG2[i] = 0;
    promedios1[i] = 0;
    promedios2[i] = 0;
  }
  Serial.setTimeout(20);  // tiempo de espera maximo para la llegada de datos.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(bomba1, OUTPUT);
  pinMode(bomba2, OUTPUT);
  pinMode(bomba3, OUTPUT);
  pinMode(solenoide1, OUTPUT);
  pinMode(solenoide2, OUTPUT);
  pinMode(solenoide3, OUTPUT);
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  pinMode(BUTTON_PIN3, INPUT_PULLUP);
  pinMode(BUTTON_PIN4, INPUT_PULLUP);
  pinMode(BUTTON_PIN5, INPUT_PULLUP);
}

void loop() {
  // lectura del dato flagPyAr de python.
  if (Serial.available() >= 1) {
    flagPyAr = Serial.readStringUntil('\n').toInt();
  }
  if (flagPyAr == 0 && menu != 222) {  // quiero que entre la primera vez cuando python envia 0.
    menu = 222;
    escritura = 0;
  }
  // lectura de los botones en todas las iteraciones.
  buttonState1 = digitalRead(BUTTON_PIN1);
  buttonState2 = digitalRead(BUTTON_PIN2);
  buttonState3 = digitalRead(BUTTON_PIN3);
  buttonState4 = digitalRead(BUTTON_PIN4);
  buttonState5 = digitalRead(BUTTON_PIN5);
  yDown = analogRead(JOYSTICK_AXIS_Y) < 100;
  yUp = analogRead(JOYSTICK_AXIS_Y) > 900;
  xLeft = analogRead(JOYSTICK_AXIS_X) < 150;
  xRight = analogRead(JOYSTICK_AXIS_X) > 900;
  // ----------------------------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------------------------
  if (menu == 0) {  // configuracion del menu 0 o menu principal.
    if (escritura == 0) {
      lcd.clear();
      lcd.createChar(10, b11);  // se crea b11.
      lcd.setCursor(2, 0);
      lcd.print("Modo Asistido");
      lcd.setCursor(2, 1);
      lcd.print("Modo manual");
      lcd.setCursor(2, 2);
      lcd.print("Rutina 1");
      lcd.setCursor(2, 3);
      lcd.print("Rutina 2");
      lcd.setCursor(0, 0);
      escritura = 1;
      fila = 0;
      buttonPress = 0;
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(bomba1, LOW);  // se apagan todas las bombas.
      digitalWrite(bomba2, LOW);
      digitalWrite(bomba3, LOW);
      digitalWrite(solenoide1, LOW);  // se apagan todos los solenoides.
      digitalWrite(solenoide2, LOW);
      digitalWrite(solenoide3, LOW);
    }
    // cursor personalizado.
    if (millis() - blinkTime > 200 && blinkState == 0) {
      lcd.setCursor(0, 0);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      lcd.setCursor(0, fila);
      lcd.write(10);
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      lcd.setCursor(0, 0);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      blinkTime = millis();
      lcd.setCursor(0, fila);
      blinkState = 0;
    }
    if ((buttonState1 == LOW || yUp) && buttonPress == 0) {  // para subir de fila en el menu 0.
      digitalWrite(LED_BUILTIN, HIGH);                       // Enciende el LED
      buttonPress = 1;
      buttonTime = millis();
      fila = fila - 1;
      if (fila == -1) {
        fila = 3;
      }
      lcd.setCursor(0, fila);
    } else if ((buttonState2 == LOW || yDown) && buttonPress == 0) {  // para bajar de fila en el menu 0.
      digitalWrite(LED_BUILTIN, HIGH);                                // Enciende el LED.
      buttonPress = 1;
      buttonTime = millis();
      fila = fila + 1;
      if (fila == 4) {
        fila = 0;
      }
      lcd.setCursor(0, fila);
    } else if (buttonState3 == LOW && fila == 1) {  // para ir al menu 1 o modo manual.
      digitalWrite(LED_BUILTIN, HIGH);              // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 11;
      escritura = 0;
      modoAsistido = 0;  // se deja constancia que no es el modo asistido y de que se entra al modo manual.
      lcd.clear();
    } else if (buttonState3 == LOW && fila == 0) {  // para ir al menu 2 o modo asistido.
      digitalWrite(LED_BUILTIN, HIGH);              // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 21;
      escritura = 0;
      lcd.clear();
    } else if ((millis() - buttonTime > 200) && buttonPress == 1) {  // para no usar delays que traban al cursor.
      buttonPress = 0;
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 11) {  // menu de seleccion de intensidad.
    if (escritura == 0) {
      lcd.noBlink();
      lcd.clear();
      // hay un máximo de 8 símbolos personalizados, si se quieren usar mas hay que inicializarlos despues.
      lcd.createChar(0, b1);
      lcd.createChar(1, b2);
      lcd.createChar(2, b3);
      lcd.createChar(3, b4);
      lcd.createChar(4, b5);
      lcd.createChar(5, b6);
      lcd.createChar(6, b7);
      lcd.createChar(7, b8);
      oldLevel = 17;
      oldLevelPorcentual = 101;
      escritura = 1;
      lcd.setCursor(2, 3);  // se escribe la palabra intensidad para evitar que se borre y escriba constantemente.
      lcd.print("Intensidad: ");
    }
    valuePot = analogRead(POT_PIN1);
    level = map(valuePot, 40, 940, 0, 16);
    levelPorcentual = map(valuePot, 40, 940, 0, 100);
    if (level > 16 || levelPorcentual > 100) {
      porcentaje = 100;
      level = 16;
    } else if (level < 0 || levelPorcentual < 1) {
      porcentaje = 1;
      level = 0;
    } else {
      porcentaje = levelPorcentual;
    }
    if (level != oldLevel) {
      lcd.setCursor(2, 1);
      lcd.print("                ");
      lcd.setCursor(2, 2);
      lcd.print("                ");
      if (level < 9) {
        for (int i = 0; i < level; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
      } else {
        for (int i = 0; i < 8; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
        for (int i = 0; i < level - 8; i++) {
          lcd.setCursor(i + 10, 1);
          lcd.write(i);
          lcd.setCursor(i + 10, 2);
          lcd.print(char(255));
        }
      }
      lcd.setCursor(13, 3);
      lcd.print("    ");
      lcd.setCursor(13, 3);
      lcd.print(porcentaje);
      if (porcentaje == 100) {
        lcd.setCursor(16, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(14, 3);
      } else {
        lcd.setCursor(15, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;  // detectar cambios en la entrada del potenciometro.
      oldLevel = level;
    } else if (levelPorcentual != oldLevelPorcentual) {
      lcd.setCursor(13, 3);
      lcd.print("    ");
      lcd.setCursor(13, 3);
      lcd.print(porcentaje);
      if (porcentaje == 100) {
        lcd.setCursor(16, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(14, 3);
      } else {
        lcd.setCursor(15, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;
    }
    if (buttonState4 == LOW) {          // para volver al menu principal.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      if (modoAsistido == 0) {
        menu = 0;
      } else if (modoAsistido == 1) {  // si se viene del modo asistido se vuelve a seleccion de sensibilidad.
        menu = 234;
      } else if (modoAsistido == 2) {  // si se viene del modo asistido se vuelve a seleccion de sensibilidad.
        menu = 21;
      }
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW) {   // para salir de seleccion de intensidad y avanzar a seleccion de tiempo.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      if (modoAsistido == 0) {  // si se viene del modo asistido se avanza directamente a seleccion de velocidad.
        menu = 12;
      } else if (modoAsistido == 1 || modoAsistido == 2) {
        menu = 14;
      }
      escritura = 0;
      intensidadManual = porcentaje;  // se guarda el porcentaje de intensidad.
      presionMax1_escalada = ((float)intensidadManual / 100.0) * (float)presionMax1;
      presionMax2_escalada = ((float)intensidadManual / 100.0) * (float)presionMax2;
      presionMax3_escalada = ((float)intensidadManual / 100.0) * (float)presionMax3;
      presionMin1_escalada = ((float)intensidadManual / 100.0) * (float)presionMin1;
      presionMin2_escalada = ((float)intensidadManual / 100.0) * (float)presionMin2;
      presionMin3_escalada = ((float)intensidadManual / 100.0) * (float)presionMin3;
      lcd.clear();



      // -----------------------------------------------------------------------
      // -------------------------- Test n Stuff -------------------------------

      Serial.print("La intensidad manual es:");
      Serial.print(" ");
      Serial.println(intensidadManual);

      Serial.print("Presiones para sensor 1:");
      Serial.print(" ");
      Serial.print(presionMin1_escalada);
      Serial.print(" ");
      Serial.println(presionMax1_escalada);

      Serial.print("Presiones para sensor 2:");
      Serial.print(" ");
      Serial.print(presionMin2_escalada);
      Serial.print(" ");
      Serial.println(presionMax2_escalada);

      Serial.print("Presiones para sensor 3:");
      Serial.print(" ");
      Serial.print(presionMin3_escalada);
      Serial.print(" ");
      Serial.println(presionMax3_escalada);

      // -------------------------- Test n Stuff -------------------------------
      // -----------------------------------------------------------------------
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 12) {  // menu de seleccion de tiempo de movimiento.
    if (escritura == 0) {
      lcd.createChar(8, b9);  // se se escriben b9 y b10.
      lcd.createChar(9, b10);
      oldTiempoInflado = 0;
      lcd.setCursor(2, 1);
      lcd.print("[s] Tiempo de");
      lcd.setCursor(2, 2);
      lcd.print("inflado:");
      escritura = 1;
    }
    if (tiempoInflado != oldTiempoInflado) {
      lcd.setCursor(10, 2);  // limpieza mas prolija que lcd.clear(). Limpia solo el número sin refrescar la pantalla.
      lcd.print("  ");
      lcd.setCursor(10, 2);
      lcd.print(tiempoInflado);
      oldTiempoInflado = tiempoInflado;  // para que la pantalla no refresque si no cambia el valor.
    }
    if ((buttonState1 == LOW || yUp) && tiempoInflado < 60) {  // cambiar el valor de la frecuencia con los botones 1 y 2.
      if (tiempoInflado >= 10) {
        lcd.setCursor(12, 2);
      } else {
        lcd.setCursor(11, 2);
      }
      lcd.write(8);
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      lcd.setCursor(12, 2);
      lcd.print(" ");
      tiempoInflado = tiempoInflado + 1;
    } else if ((buttonState2 == LOW || yDown) && tiempoInflado > 1) {
      if (tiempoInflado >= 10) {
        lcd.setCursor(12, 2);
      } else {
        lcd.setCursor(11, 2);
      }
      lcd.write(9);
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      lcd.setCursor(12, 2);
      lcd.print(" ");
      tiempoInflado = tiempoInflado - 1;
    }
    if (buttonState4 == LOW) {          // para volver al menu de intensidad.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 11;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW) {   // para salir de seleccion de frecuencia y avanzar a velocidad.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 13;
      escritura = 0;
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 13) {  // menu de seleccion de tiempo de relajacion.
    if (escritura == 0) {
      lcd.createChar(8, b9);  // se escriben b9 y b10.
      lcd.createChar(9, b10);
      oldTiempoDesinflado = 0;
      lcd.setCursor(2, 1);
      lcd.print("[s] Tiempo de");
      lcd.setCursor(2, 2);
      lcd.print("desinflado:");
      escritura = 1;
    }
    if (tiempoDesinflado != oldTiempoDesinflado) {
      lcd.setCursor(13, 2);  // limpieza mas prolija que lcd.clear().
      lcd.print("  ");
      lcd.setCursor(13, 2);
      lcd.print(tiempoDesinflado);
      oldTiempoDesinflado = tiempoDesinflado;  // para que la pantalla no refresque si no cambia el valor.
    }
    if ((buttonState1 == LOW || yUp) && tiempoDesinflado < 60) {  // cambiar el valor de la frecuencia con los botones 1 y 2.
      if (tiempoDesinflado >= 10) {
        lcd.setCursor(15, 2);
      } else {
        lcd.setCursor(14, 2);
      }
      lcd.write(8);
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      lcd.setCursor(15, 2);
      lcd.print(" ");
      tiempoDesinflado = tiempoDesinflado + 1;
    } else if ((buttonState2 == LOW || yDown) && tiempoDesinflado > 1) {
      if (tiempoDesinflado >= 10) {
        lcd.setCursor(15, 2);
      } else {
        lcd.setCursor(14, 2);
      }
      lcd.write(9);
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      lcd.setCursor(15, 2);
      lcd.print(" ");
      tiempoDesinflado = tiempoDesinflado - 1;
    }
    if (buttonState4 == LOW) {          // para volver al menu de intensidad.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 12;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW) {   // para salir de seleccion de frecuencia y avanzar a velocidad.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 14;
      escritura = 0;
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 14) {  // preguntar si se quiere cambiar la velocidad de inflado
    if (escritura == 0) {
      lcd.createChar(14, b15);  // se escribe de b15 a b18.
      lcd.createChar(15, b16);
      lcd.createChar(12, b17);  // se sobreescriben 12 y 13.
      lcd.createChar(13, b18);
      escritura = 1;
      lcd.setCursor(2, 0);
      lcd.print("Desea modificar");
      lcd.setCursor(2, 1);
      lcd.print("la velocidad de");
      lcd.setCursor(2, 2);
      lcd.print("inflado?");
      lcd.setCursor(6, 3);
      lcd.print("No");
      lcd.setCursor(12, 3);
      lcd.print("Si");
      yesNo = 0;
    }
    // blink de texto.
    if (yesNo == 0 && xRight) {
      yesNo = 1;
      lcd.setCursor(6, 3);
      lcd.print("No");
      lcd.setCursor(12, 3);
      lcd.print("Si");
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    } else if (yesNo == 1 && xLeft) {
      yesNo = 0;
      lcd.setCursor(6, 3);
      lcd.print("No");
      lcd.setCursor(12, 3);
      lcd.print("Si");
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    }
    if (millis() - blinkTime > 300 && blinkState == 0) {
      if (yesNo == 0) {
        lcd.setCursor(6, 3);
        lcd.print("  ");
        lcd.setCursor(6, 3);
        lcd.write(12);
        lcd.setCursor(7, 3);
        lcd.write(13);
      } else if (yesNo == 1) {
        lcd.setCursor(12, 3);
        lcd.print("  ");
        lcd.setCursor(12, 3);
        lcd.write(14);
        lcd.setCursor(13, 3);
        lcd.write(15);
      }
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      if (yesNo == 0) {
        lcd.setCursor(6, 3);
        lcd.print("  ");
        lcd.setCursor(6, 3);
        lcd.print("No");
      } else if (yesNo == 1) {
        lcd.setCursor(12, 3);
        lcd.print("  ");
        lcd.setCursor(12, 3);
        lcd.print("Si");
      }
      blinkTime = millis();
      blinkState = 0;
    }
    if (buttonState4 == LOW) {          // para volver a seleccion de tiempo de desinflado.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      if (modoAsistido == 0) {  // si se viene del modo asistido se vuelve a seleccion de intensidad.
        menu = 13;
      } else if (modoAsistido == 1 || modoAsistido == 2) {
        menu = 11;
      }
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW && yesNo == 0) {  // cuando se elige no se avanza a seleccion de velocidad de desinflado.
      digitalWrite(LED_BUILTIN, HIGH);               // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 15;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW && yesNo == 1) {  // cuando se elige si entra al menu de seleccion de velocidad de inflado.
      digitalWrite(LED_BUILTIN, HIGH);               // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 141;
      escritura = 0;
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 141) {  // menu de seleccion de velocidad de inflado.
    if (escritura == 0) {
      lcd.noBlink();
      // hay un máximo de 8 símbolos personalizados, si se quieren usar mas hay que inicializarlos despues.
      lcd.createChar(0, b1);
      lcd.createChar(1, b2);
      lcd.createChar(2, b3);
      lcd.createChar(3, b4);
      lcd.createChar(4, b5);
      lcd.createChar(5, b6);
      lcd.createChar(6, b7);
      lcd.createChar(7, b8);
      oldLevel = 17;
      oldLevelPorcentual = 101;
      escritura = 1;
      lcd.setCursor(2, 3);  // se escribe la palabra intensidad para evitar que se borre y escriba constantemente.
      lcd.print("Velocidad: ");
      lcd.setCursor(0, 0);
      lcd.print("Inflado");
      // para imprimir el duty cycle.
      lcd.setCursor(13, 0);
      lcd.print("DC:");
      lcd.setCursor(16, 0);
      lcd.print(DCInflado);
      if (DCInflado < 10) {
        lcd.setCursor(17, 0);
      } else if (DCInflado == 100) {
        lcd.setCursor(19, 0);
      } else {
        lcd.setCursor(18, 0);
      }
      lcd.print("%");
      velocidad100 = 0;
    }
    valuePot = analogRead(POT_PIN1);
    level = map(valuePot, 40, 940, 0, 16);
    levelPorcentual = map(valuePot, 40, 940, 0, 100);
    if (level > 16 || levelPorcentual > 100) {
      porcentaje = 100;
      level = 16;
    } else if (level < 0 || levelPorcentual < 1) {
      porcentaje = 1;
      level = 0;
    } else {
      porcentaje = levelPorcentual;
    }
    if (level != oldLevel) {
      lcd.setCursor(2, 1);
      lcd.print("                ");
      lcd.setCursor(2, 2);
      lcd.print("                ");
      if (level < 9) {
        for (int i = 0; i < level; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
      } else {
        for (int i = 0; i < 8; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
        for (int i = 0; i < level - 8; i++) {
          lcd.setCursor(i + 10, 1);
          lcd.write(i);
          lcd.setCursor(i + 10, 2);
          lcd.print(char(255));
        }
      }
      lcd.setCursor(13, 3);
      lcd.print("    ");
      lcd.setCursor(12, 3);
      lcd.print(porcentaje);
      if (level == 16) {
        lcd.setCursor(15, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(13, 3);
      } else {
        lcd.setCursor(14, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;  // detectar cambios en la entrada del potenciometro.
      oldLevel = level;
    } else if (levelPorcentual != oldLevelPorcentual) {
      lcd.setCursor(13, 3);
      lcd.print("    ");
      lcd.setCursor(12, 3);
      lcd.print(porcentaje);
      if (level == 16) {
        lcd.setCursor(15, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(13, 3);
      } else {
        lcd.setCursor(14, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;
    }
    if (porcentaje == 100 && velocidad100 == 0) {
      lcd.setCursor(16, 0);
      lcd.print("    ");
      lcd.setCursor(16, 0);
      lcd.print("--");
      velocidad100 = 1;
    } else if (porcentaje != 100 && velocidad100 == 1) {
      lcd.setCursor(16, 0);
      lcd.print(DCDesinflado);
      if (DCDesinflado < 10) {
        lcd.setCursor(17, 0);
      } else if (DCDesinflado == 100) {
        lcd.setCursor(19, 0);
      } else {
        lcd.setCursor(18, 0);
      }
      lcd.print("%");
      velocidad100 = 0;
    }
    if (buttonState4 == LOW) {  // para volver seleccion de cambio de velocidad de inflado.
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 14;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW) {  // para avanzar a la pantalla de seleccion de velocidad de desinflado.
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 15;
      escritura = 0;
      velocidadInfladoManual = porcentaje;  // se guarda el porcentaje.
      lcd.clear();
    } else if (buttonState5 == LOW) {   // para entrar a configuracion avanzada y modificar el DC.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 142;
      escritura = 0;
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 142) {  // menu de seleccion de duty cycle en inflado.
    if (escritura == 0) {
      lcd.noBlink();
      // hay un máximo de 8 símbolos personalizados, si se quieren usar mas hay que inicializarlos despues.
      lcd.createChar(0, b1);
      lcd.createChar(1, b2);
      lcd.createChar(2, b3);
      lcd.createChar(3, b4);
      lcd.createChar(4, b5);
      lcd.createChar(5, b6);
      lcd.createChar(6, b7);
      lcd.createChar(7, b8);
      oldLevel = 17;
      oldLevelPorcentual = 101;
      escritura = 1;
      lcd.setCursor(2, 0);
      lcd.print("Modo avanzado:");
      lcd.setCursor(2, 3);
      lcd.print("Duty Cycle:");
    }
    valuePot = analogRead(POT_PIN1);
    level = map(valuePot, 40, 940, 0, 16);
    levelPorcentual = map(valuePot, 40, 940, 0, 100);
    if (level > 16 || levelPorcentual > 100) {
      porcentaje = 100;
      level = 16;
    } else if (level < 0 || levelPorcentual < 1) {
      porcentaje = 1;
      level = 0;
    } else {
      porcentaje = levelPorcentual;
    }
    if (level != oldLevel) {
      lcd.setCursor(2, 1);
      lcd.print("                ");
      lcd.setCursor(2, 2);
      lcd.print("                ");
      if (level < 9) {
        for (int i = 0; i < level; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
      } else {
        for (int i = 0; i < 8; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
        for (int i = 0; i < level - 8; i++) {
          lcd.setCursor(i + 10, 1);
          lcd.write(i);
          lcd.setCursor(i + 10, 2);
          lcd.print(char(255));
        }
      }
      lcd.setCursor(14, 3);
      lcd.print("    ");
      lcd.setCursor(13, 3);
      lcd.print(porcentaje);
      if (level == 16) {
        lcd.setCursor(16, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(14, 3);
      } else {
        lcd.setCursor(15, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;  // detectar cambios en la entrada del potenciometro.
      oldLevel = level;
    } else if (levelPorcentual != oldLevelPorcentual) {
      lcd.setCursor(14, 3);
      lcd.print("    ");
      lcd.setCursor(13, 3);
      lcd.print(porcentaje);
      if (level == 16) {
        lcd.setCursor(16, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(14, 3);
      } else {
        lcd.setCursor(15, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;
    }
    if (buttonState4 == LOW) {  // para volver a seleccion de velocidad de inflado.
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 141;  // no se guarda el valor de duty cycle modificado cuando se toca el boton volver.
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW) {  // para volver al menu de seleccion de velocidad de inflado con el duty cycle modificado.
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 141;
      escritura = 0;
      DCInflado = porcentaje;  // guarda el duty cycle con el que se va a trabajar.
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 15) {  // preguntar si se quiere cambiar la velocidad de desinflado
    if (escritura == 0) {
      lcd.createChar(14, b15);  // se escribe de b15 a b18.
      lcd.createChar(15, b16);
      lcd.createChar(12, b17);  // se sobreescriben 12 y 13.
      lcd.createChar(13, b18);
      escritura = 1;
      lcd.setCursor(2, 0);
      lcd.print("Desea modificar");
      lcd.setCursor(2, 1);
      lcd.print("la velocidad de");
      lcd.setCursor(2, 2);
      lcd.print("desinflado?");
      lcd.setCursor(6, 3);
      lcd.print("No");
      lcd.setCursor(12, 3);
      lcd.print("Si");
      yesNo = 0;
    }
    // blink de texto.
    if (yesNo == 0 && xRight) {
      yesNo = 1;
      lcd.setCursor(6, 3);
      lcd.print("No");
      lcd.setCursor(12, 3);
      lcd.print("Si");
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    } else if (yesNo == 1 && xLeft) {
      yesNo = 0;
      lcd.setCursor(6, 3);
      lcd.print("No");
      lcd.setCursor(12, 3);
      lcd.print("Si");
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    }
    if (millis() - blinkTime > 300 && blinkState == 0) {
      if (yesNo == 0) {
        lcd.setCursor(6, 3);
        lcd.print("  ");
        lcd.setCursor(6, 3);
        lcd.write(12);
        lcd.setCursor(7, 3);
        lcd.write(13);
      } else if (yesNo == 1) {
        lcd.setCursor(12, 3);
        lcd.print("  ");
        lcd.setCursor(12, 3);
        lcd.write(14);
        lcd.setCursor(13, 3);
        lcd.write(15);
      }
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      if (yesNo == 0) {
        lcd.setCursor(6, 3);
        lcd.print("  ");
        lcd.setCursor(6, 3);
        lcd.print("No");
      } else if (yesNo == 1) {
        lcd.setCursor(12, 3);
        lcd.print("  ");
        lcd.setCursor(12, 3);
        lcd.print("Si");
      }
      blinkTime = millis();
      blinkState = 0;
    }
    if (buttonState4 == LOW) {          // para volver a seleccion de tiempo de inflado.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 14;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW && yesNo == 0) {  // cuando se elige no se avanza a la pantalla de funcionamiento del modo manual.
      digitalWrite(LED_BUILTIN, HIGH);               // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      if (modoAsistido == 0) {  // si se viene del modo asistido, se va a la pantalla de funcionamiento propia del modo asistido.
        menu = 16;
      } else if (modoAsistido == 1) {
        menu = 235;
      } else if (modoAsistido == 2) {
        menu = 224;
      }
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW && yesNo == 1) {  // cuando se elige si entra al menu de seleccion de velocidad de desinflado.
      digitalWrite(LED_BUILTIN, HIGH);               // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 151;
      escritura = 0;
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 151) {  // menu de seleccion de velocidad de desinflado.
    if (escritura == 0) {
      lcd.noBlink();
      // hay un máximo de 8 símbolos personalizados, si se quieren usar mas hay que inicializarlos despues.
      lcd.createChar(0, b1);
      lcd.createChar(1, b2);
      lcd.createChar(2, b3);
      lcd.createChar(3, b4);
      lcd.createChar(4, b5);
      lcd.createChar(5, b6);
      lcd.createChar(6, b7);
      lcd.createChar(7, b8);
      oldLevel = 17;
      oldLevelPorcentual = 101;
      escritura = 1;
      lcd.setCursor(2, 3);  // se escribe la palabra intensidad para evitar que se borre y escriba constantemente.
      lcd.print("Velocidad: ");
      lcd.setCursor(0, 0);
      lcd.print("Desinflado");
      // para imprimir el duty cycle.
      lcd.setCursor(13, 0);
      lcd.print("DC:");
      lcd.setCursor(16, 0);
      lcd.print(DCDesinflado);
      if (DCDesinflado < 10) {
        lcd.setCursor(17, 0);
      } else if (DCDesinflado == 100) {
        lcd.setCursor(19, 0);
      } else {
        lcd.setCursor(18, 0);
      }
      lcd.print("%");
      velocidad100 = 0;
    }
    valuePot = analogRead(POT_PIN1);
    level = map(valuePot, 40, 940, 0, 16);
    levelPorcentual = map(valuePot, 40, 940, 0, 100);
    if (level > 16 || levelPorcentual > 100) {
      porcentaje = 100;
      level = 16;
    } else if (level < 0 || levelPorcentual < 1) {
      porcentaje = 1;
      level = 0;
    } else {
      porcentaje = levelPorcentual;
    }
    if (level != oldLevel) {
      lcd.setCursor(2, 1);
      lcd.print("                ");
      lcd.setCursor(2, 2);
      lcd.print("                ");
      if (level < 9) {
        for (int i = 0; i < level; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
      } else {
        for (int i = 0; i < 8; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
        for (int i = 0; i < level - 8; i++) {
          lcd.setCursor(i + 10, 1);
          lcd.write(i);
          lcd.setCursor(i + 10, 2);
          lcd.print(char(255));
        }
      }
      lcd.setCursor(13, 3);
      lcd.print("    ");
      lcd.setCursor(12, 3);
      lcd.print(porcentaje);
      if (level == 16) {
        lcd.setCursor(15, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(13, 3);
      } else {
        lcd.setCursor(14, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;  // detectar cambios en la entrada del potenciometro.
      oldLevel = level;
    } else if (levelPorcentual != oldLevelPorcentual) {
      lcd.setCursor(13, 3);
      lcd.print("    ");
      lcd.setCursor(12, 3);
      lcd.print(porcentaje);
      if (level == 16) {
        lcd.setCursor(15, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(13, 3);
      } else {
        lcd.setCursor(14, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;
    }
    if (porcentaje == 100 && velocidad100 == 0) {
      lcd.setCursor(16, 0);
      lcd.print("    ");
      lcd.setCursor(16, 0);
      lcd.print("--");
      velocidad100 = 1;
    } else if (porcentaje != 100 && velocidad100 == 1) {
      lcd.setCursor(16, 0);
      lcd.print(DCDesinflado);
      if (DCDesinflado < 10) {
        lcd.setCursor(17, 0);
      } else if (DCDesinflado == 100) {
        lcd.setCursor(19, 0);
      } else {
        lcd.setCursor(18, 0);
      }
      lcd.print("%");
      velocidad100 = 0;
    }
    if (buttonState4 == LOW) {  // para volver a pantalla con pregunta para cambiar velocidad de desinflado.
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 15;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW) {  // para avanzar a la pantalla de entrenamiento.
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      if (modoAsistido == 0) {  // si se viene del modo asistido, se va a la pantalla de funcionamiento propia del modo asistido (una pantalla para rapido y otra para completo).
        menu = 16;
      } else if (modoAsistido == 1) {
        menu = 235;
      } else if (modoAsistido == 2) {
        menu = 224;
      }
      escritura = 0;
      velocidadDesinfladoManual = porcentaje;  // se guarda el porcentaje.
      lcd.clear();
    } else if (buttonState5 == LOW) {   // para entrar a configuracion avanzada y modificar el DC.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 152;
      escritura = 0;
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 152) {  // menu de seleccion de duty cycle en desinflado.
    if (escritura == 0) {
      lcd.clear();
      // hay un máximo de 8 símbolos personalizados, si se quieren usar mas hay que inicializarlos despues.
      lcd.createChar(0, b1);
      lcd.createChar(1, b2);
      lcd.createChar(2, b3);
      lcd.createChar(3, b4);
      lcd.createChar(4, b5);
      lcd.createChar(5, b6);
      lcd.createChar(6, b7);
      lcd.createChar(7, b8);
      oldLevel = 17;
      oldLevelPorcentual = 101;
      escritura = 1;
      lcd.setCursor(2, 0);
      lcd.print("Modo avanzado:");
      lcd.setCursor(2, 3);
      lcd.print("Duty Cycle:");
    }
    valuePot = analogRead(POT_PIN1);
    level = map(valuePot, 40, 940, 0, 16);
    levelPorcentual = map(valuePot, 40, 940, 0, 100);
    if (level > 16 || levelPorcentual > 100) {
      porcentaje = 100;
      level = 16;
    } else if (level < 0 || levelPorcentual < 1) {
      porcentaje = 1;
      level = 0;
    } else {
      porcentaje = levelPorcentual;
    }
    if (level != oldLevel) {
      lcd.setCursor(2, 1);
      lcd.print("                ");
      lcd.setCursor(2, 2);
      lcd.print("                ");
      if (level < 9) {
        for (int i = 0; i < level; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
      } else {
        for (int i = 0; i < 8; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
        for (int i = 0; i < level - 8; i++) {
          lcd.setCursor(i + 10, 1);
          lcd.write(i);
          lcd.setCursor(i + 10, 2);
          lcd.print(char(255));
        }
      }
      lcd.setCursor(14, 3);
      lcd.print("    ");
      lcd.setCursor(13, 3);
      lcd.print(porcentaje);
      if (level == 16) {
        lcd.setCursor(16, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(14, 3);
      } else {
        lcd.setCursor(15, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;  // detectar cambios en la entrada del potenciometro.
      oldLevel = level;
    } else if (levelPorcentual != oldLevelPorcentual) {
      lcd.setCursor(14, 3);
      lcd.print("    ");
      lcd.setCursor(13, 3);
      lcd.print(porcentaje);
      if (level == 16) {
        lcd.setCursor(16, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(14, 3);
      } else {
        lcd.setCursor(15, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;
    }
    if (buttonState4 == LOW) {  // para volver a seleccion de velocidad de desinflado.
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 151;  // no se guarda el valor de duty cycle modificado cuando se toca el boton volver.
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW) {  // para volver al menu de seleccion de velocidad de inflado con el duty cycle modificado.
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 151;
      escritura = 0;
      DCDesinflado = porcentaje;  // guarda el duty cycle con el que se va a trabajar.
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 16) {  // menu de funcionamiento del modo manual.
    // el modo manual esta configurado para un solo movimiento, el de cierre de mano. Se puede mejorar con una pantalla adicional de seleccion de movimiento.
    if (escritura == 0) {  // calculo de parametros necesarios, presion, velocidad y frecuencia.
      lcd.clear();
      escritura = 1;
      lcd.createChar(7, b8);    // se crea b8.
      lcd.createChar(10, b11);  // se crea b11.
      lcd.createChar(11, b12);  // se crea b12.
      lcd.createChar(12, b13);  // se crea b13.
      lcd.createChar(13, b14);  // se crea b14.
      miliInflado = 1000.0 * tiempoInflado;
      miliDesinflado = 1000.0 * tiempoDesinflado;
      frecuenciaInfladoPWM = velocidadInfladoManual;
      frecuenciaDesinfladoPWM = velocidadDesinfladoManual;
      miliPeriodoInfladoPWM = round((1000.0 / frecuenciaInfladoPWM));
      miliPeriodoDesinfladoPWM = round((1000.0 / frecuenciaDesinfladoPWM));
      miliTiempoOnInflado = round((miliPeriodoInfladoPWM) * (DCInflado / 100.0));
      miliTiempoOnDesinflado = round((miliPeriodoDesinfladoPWM) * (DCDesinflado / 100.0));
      miliTiempoOffInflado = miliPeriodoInfladoPWM - miliTiempoOnInflado;
      miliTiempoOffDesinflado = miliPeriodoDesinfladoPWM - miliTiempoOnDesinflado;

      estadoEntrenamiento = 0;  // arranca con el menu sin opcion seleccionada.
      buttonPress = 0;
      fila = 1;
      escrituraEntrenamiento = 1;  // para escribir el display cuando se entra al menu de entrenamiento.
      enCiclo = 0;
      contProgreso = 17;
      oldTime = currentTime;
      oldLevelAnimacion = 17;
      levelAnimacion = 0;
    }
    // cursor personalizado.
    if (millis() - blinkTime > 300 && blinkState == 0) {
      lcd.setCursor(0, 1);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      lcd.setCursor(0, fila);
      lcd.write(10);
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      lcd.setCursor(0, 1);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      lcd.setCursor(0, fila);
      blinkTime = millis();
      blinkState = 0;
    }
    // subir y bajar de fila y seleccionar una opcion.
    if ((buttonState1 == LOW || yUp) && buttonPress == 0) {  // para subir de fila en el menu 15.
      digitalWrite(LED_BUILTIN, HIGH);                       // Enciende el LED.
      fila = fila - 1;
      if (fila == 0) {  // casos particulares dado las dimensiones del menu.
        fila = 2;
      }
      lcd.setCursor(0, fila);
      buttonPress = 1;
      buttonTime = millis();
    } else if ((buttonState2 == LOW || yDown) && buttonPress == 0) {  // para bajar de fila en el menu 15.
      digitalWrite(LED_BUILTIN, HIGH);                                // Enciende el LED.
      fila = fila + 1;
      if (fila == 3) {  // casos particulares dado las dimensiones del menu.
        fila = 1;
      }
      lcd.setCursor(0, fila);
      buttonPress = 1;
      buttonTime = millis();
    } else if (buttonState3 == LOW && buttonPress == 0) {
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED.
      buttonPress = 1;
      buttonTime = millis();
      if (estadoEntrenamiento == 0 && fila == 1) {         // para pasar de una opcion a otra en el menu de entrenamiento.
        estadoEntrenamiento = 1;                           // el flag indica que el entrenamiento comienza.
      } else if (estadoEntrenamiento == 0 && fila == 2) {  // voler al menu principal.
        menu = 0;
        escritura = 0;
      } else if (estadoEntrenamiento == 1 && fila == 2) {  // volver al menu principal.
        menu = 0;
        escritura = 0;
      } else if (estadoEntrenamiento == 1 && fila == 1) {  // pausar el entrenamiento.
        estadoEntrenamiento = 2;
        digitalWrite(bomba1, LOW);  // se apagan todas las bombas.
        digitalWrite(bomba2, LOW);
        digitalWrite(bomba3, LOW);
        inicioPausa = millis();  // se guarda el instante en el que comienza la pausa.
        contAnimacion = 2;       // se reinician las variables de la animacion de la barra de progreso.
        blinkStateAnimacion = 0;
      } else if (estadoEntrenamiento == 2 && fila == 2) {
        menu = 0;
        escritura = 0;
      } else if (estadoEntrenamiento == 2 && fila == 1) {
        estadoEntrenamiento = 1;  // se recupera el entrenamiento en el estado en el que se lo dejo.
        finPausa = millis();      // se guarda el isntante en el que termina la pausa;
        tiempoEnPausa = finPausa - inicioPausa;
        oldTime = oldTime + tiempoEnPausa;
      }
      escrituraEntrenamiento = 1;                // el flag indica que se sobreescriba la pantalla.
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
    } else if (millis() - buttonTime > 200 && buttonPress == 1) {  // para no usar delays que puedan interferir con el entrenamiento.
      buttonPress = 0;
      digitalWrite(LED_BUILTIN, LOW);
    }
    // escritura de la pantalla en base a estadoEntrenamiento y escrituraEntrenamiento.
    if (escrituraEntrenamiento == 1) {
      lcd.clear();
      if (estadoEntrenamiento == 0) {
        lcd.setCursor(2, 0);
        lcd.print("Entrenamiento:");
        lcd.setCursor(2, 1);
        lcd.print("Comenzar");
        lcd.setCursor(2, 2);
        lcd.print("Volver a pantalla");
        lcd.setCursor(2, 3);
        lcd.print("principal");
      } else if (estadoEntrenamiento == 1) {
        lcd.setCursor(2, 0);
        lcd.print("Entrenamiento:");
        lcd.setCursor(2, 1);
        lcd.print("Pausar");
        lcd.setCursor(2, 2);
        lcd.print("Terminar");
        lcd.setCursor(1, 3);
        lcd.write(11);
        lcd.setCursor(18, 3);
        lcd.write(12);
        for (int i = 2; i < contProgreso; i++) {  // para escribir la barra de progreso cuando se vuelve de pausa o se inicia el entrenamiento.
          lcd.setCursor(i, 3);
          lcd.write(13);
        }
      } else if (estadoEntrenamiento == 2) {
        lcd.setCursor(2, 0);
        lcd.print("Entrenamiento:");
        lcd.setCursor(2, 1);
        lcd.print("Retomar");
        lcd.setCursor(2, 2);
        lcd.print("Terminar");
        lcd.setCursor(1, 3);
        lcd.write(11);
        lcd.setCursor(18, 3);
        lcd.write(12);
        for (int j = 2; j < contProgreso; j++) {  // para escribir la barra de progreso en el menu de pausa.
          lcd.setCursor(j, 3);
          lcd.write(13);
        }
      }
      fila = 1;
      escrituraEntrenamiento = 0;
    }
    // se toman los valores de presion.
    sensorValuePresion1 = analogRead(sensorPresion1);
    sensorValuePresion2 = analogRead(sensorPresion2);
    sensorValuePresion3 = analogRead(sensorPresion3);
    // se escalan los valores de presion a voltaje.
    lecturaPresion1 = 5 * (sensorValuePresion1 / 1023);
    lecturaPresion2 = 5 * (sensorValuePresion2 / 1023);
    lecturaPresion3 = 3.3 * (sensorValuePresion3 / 1023);
    // se transforma el valor de voltaje a un valor de presion real.
    presion1 = (((lecturaPresion1 / 5) + 0.00842) / 0.002421) - 100;
    presion2 = (((lecturaPresion2 / 5) + 0.00842) / 0.002421) - 100;
    presion3 = (((lecturaPresion3 / 3.3) - 0.04) / 0.018);
    // PWM.
    // Calculo de tiempos.
    currentTime = millis();               // se toma el tiempo en la iteración.
    elapsedTime = currentTime - oldTime;  // intervalo de tiempo.
    elapsedTimePWM = currentTime - oldTimePWM;
    if (estadoEntrenamiento == 1) {
      // inflado.
      if (elapsedTime > miliDesinflado && enCiclo == 0) {
        digitalWrite(solenoide1, HIGH);  // se cierran los solenoides.
        digitalWrite(solenoide2, LOW);
        digitalWrite(solenoide3, LOW);
        oldTime = currentTime;
        enCiclo = 1;
      } else if (elapsedTime <= miliInflado && enCiclo == 1 && (frecuenciaInfladoPWM == 100 || DCInflado == 100)) {
        // se encieden las bombas y se limitan por presion.
        if (presion1 < presionMin1_escalada) {
          digitalWrite(bomba1, HIGH);
          digitalWrite(solenoide1, HIGH);
        } else if (presion1 > presionMax1_escalada) {
          digitalWrite(bomba1, LOW);
        }
        if (presion2 < presionMin2_escalada) {
          digitalWrite(bomba2, HIGH);
        } else if (presion2 > presionMax2_escalada) {
          digitalWrite(bomba2, LOW);
        }
        if (presion3 < presionMin3_escalada) {
          digitalWrite(bomba3, HIGH);
        } else if (presion3 > presionMax3_escalada) {
          digitalWrite(bomba3, LOW);
        }
      } else if (elapsedTime <= miliInflado && enCiclo == 1 && frecuenciaInfladoPWM != 100) {
        if (elapsedTimePWM > miliTiempoOffInflado && enPWM == 0) {
          // senal de encendido a las bombas.
          oldTimePWM = currentTime;
          enPWM = 1;
        } else if (elapsedTimePWM <= miliTiempoOnInflado && enPWM == 1) {
          // se encieden las bombas y se limitan por presion.
          if (presion1 < presionMin1_escalada) {
            digitalWrite(bomba1, HIGH);
            digitalWrite(solenoide1, HIGH);
          } else if (presion1 > presionMax1_escalada) {
            digitalWrite(bomba1, LOW);
          }
          if (presion2 < presionMin2_escalada) {
            digitalWrite(bomba2, HIGH);
          } else if (presion2 > presionMax2_escalada) {
            digitalWrite(bomba2, LOW);
          }
          if (presion3 < presionMin3_escalada) {
            digitalWrite(bomba3, HIGH);
          } else if (presion3 > presionMax3_escalada) {
            digitalWrite(bomba3, LOW);
          }
        } else if (elapsedTimePWM > miliTiempoOnInflado && enPWM == 1) {
          digitalWrite(bomba1, LOW);  // se apagan las bombas.
          digitalWrite(bomba2, LOW);
          digitalWrite(bomba3, LOW);
          oldTimePWM = currentTime;
          enPWM = 0;
        }
        // desinflado.
      } else if (elapsedTime > miliInflado && enCiclo == 1) {
        digitalWrite(bomba1, LOW);  // se apagan las bombas.
        digitalWrite(bomba2, LOW);
        digitalWrite(bomba3, LOW);
        oldTime = currentTime;
        enCiclo = 0;
      } else if (elapsedTime <= miliDesinflado && enCiclo == 0 && (velocidadDesinfladoManual == 100 || DCDesinflado == 100)) {
        digitalWrite(solenoide1, LOW);  // se abren los solenoides.
        digitalWrite(solenoide2, HIGH);
        digitalWrite(solenoide3, HIGH);
      } else if (elapsedTime <= miliDesinflado && enCiclo == 0 && frecuenciaDesinfladoPWM != 100) {
        if (elapsedTimePWM > miliTiempoOffDesinflado && enPWM == 0) {
          // señal de encendido a los solenoides
          oldTimePWM = currentTime;
          enPWM = 1;
        } else if (elapsedTimePWM <= miliTiempoOnDesinflado && enPWM == 1) {
          digitalWrite(solenoide1, LOW);  // se abren los solenoides.
          digitalWrite(solenoide2, HIGH);
          digitalWrite(solenoide3, HIGH);
        } else if (elapsedTimePWM > miliTiempoOnDesinflado && enPWM == 1) {
          digitalWrite(solenoide1, HIGH);  // se cierran los solenoides.
          digitalWrite(solenoide2, LOW);
          digitalWrite(solenoide3, LOW);
          oldTimePWM = currentTime;
          enPWM = 0;
        }
      }
      // barra de progreso.
      if (enCiclo == 1) {
        levelAnimacion = map(elapsedTime, 0, miliInflado, 0, 15);
        if (levelAnimacion != oldLevelAnimacion && contProgreso <= 17) {
          lcd.setCursor(contProgreso, 3);
          lcd.write(13);
          contProgreso = contProgreso + 1;
        }
      } else if (enCiclo == 0) {
        levelAnimacion = map(miliDesinflado - elapsedTime, 0, miliDesinflado, 0, 15);
        if (levelAnimacion != oldLevelAnimacion && contProgreso >= 3) {
          lcd.setCursor(contProgreso - 1, 3);
          lcd.print(" ");
          contProgreso = contProgreso - 1;
        }
      }
      oldLevelAnimacion = levelAnimacion;  // detectar cambios en el tiempo de progreso.
    } else if (estadoEntrenamiento == 0) {
      oldTime = currentTime;
    } else if (estadoEntrenamiento == 2) {
      // animacion de la barra de progreso en pausa.
      while (2 <= contAnimacion <= contProgreso && millis() - blinkTimeAnimacion > 50 && contProgreso > 5) {
        lcd.setCursor(contAnimacion, 3);
        lcd.print(" ");
        if (contAnimacion == 2) {
          blinkStateAnimacion = 0;
          contAnimacion = 3;
        } else if (contAnimacion == contProgreso - 1) {
          blinkStateAnimacion = 1;
          contAnimacion = contAnimacion - 1;
        } else if (blinkStateAnimacion == 0 && contAnimacion > 2) {
          lcd.setCursor(contAnimacion - 1, 3);
          lcd.write(13);
          contAnimacion = contAnimacion + 1;
        } else if (blinkStateAnimacion == 1 && contAnimacion < contProgreso) {
          lcd.setCursor(contAnimacion + 1, 3);
          lcd.write(13);
          contAnimacion = contAnimacion - 1;
        }
        blinkTimeAnimacion = millis();
      }
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 21) {  // configuracion del menu 21 o seleccion de modo asistido.
    if (escritura == 0) {
      lcd.clear();
      lcd.createChar(10, b11);  // se crea b11.
      lcd.setCursor(2, 0);
      lcd.print("Completo");
      lcd.setCursor(3, 1);
      lcd.print("(Requiere de PC)");
      lcd.setCursor(2, 2);
      lcd.print("Rapido");
      lcd.setCursor(3, 3);
      lcd.print("(No requiere PC)");
      lcd.setCursor(0, 0);
      escritura = 1;
      fila = 0;
    }
    // cursor personalizado.
    if (millis() - blinkTime > 300 && blinkState == 0) {
      lcd.setCursor(0, 0);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      lcd.setCursor(0, fila);
      lcd.write(10);
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      lcd.setCursor(0, 0);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      blinkTime = millis();
      lcd.setCursor(0, fila);
      blinkState = 0;
    }
    if (buttonState4 == LOW) {          // para volver al menu principal.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 0;
      escritura = 0;
      lcd.clear();
    } else if ((buttonState1 == LOW || yUp) && fila == 2) {  // para subir de fila en el menu 21.
      digitalWrite(LED_BUILTIN, HIGH);                       // Enciende el LED
      fila = 0;
      lcd.setCursor(0, fila);
    } else if ((buttonState2 == LOW || yDown) && fila == 0) {  // para bajar de fila en el menu 21.
      digitalWrite(LED_BUILTIN, HIGH);                         // Enciende el LED.
      fila = 2;
      lcd.setCursor(0, fila);
    } else if (buttonState3 == LOW && fila == 0) {  // entra al modo asistido completo
      digitalWrite(LED_BUILTIN, HIGH);              // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      modoAsistido = 2;                                        // se deja constancia de que es el modo asistido completo.
      if (calibradoCompleto == 1 || calibradoCompleto == 2) {  // se entra a calibracion o se recupera la calibracion previa (completo).
        menu = 223;
      } else {
        menu = 221;
      }
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW && fila == 2) {  // entra al modo asistido rapido.
      digitalWrite(LED_BUILTIN, HIGH);              // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      modoAsistido = 1;            // se deja constancia de que es el modo asistido rapido.
      if (calibradoRapido == 1) {  // se entra a calibracion o se recupera la calibracion previa (rapido).
        menu = 233;
      } else {
        menu = 231;
      }
      escritura = 0;
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 221) {  // se pide que la computadora este conectada.
    if (escritura == 0) {
      lcd.clear();
      lcd.createChar(10, b11);  // se crea b11.
      lcd.setCursor(2, 0);
      lcd.print("Asegurar conexion");
      lcd.setCursor(2, 1);
      lcd.print("con computadora");
      lcd.setCursor(2, 2);
      lcd.print("para comenzar");
      escritura = 1;
    }
    if (buttonState4 == LOW) {          // para volver a seleccion de modo asistido.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 21;
      escritura = 0;
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 222) {  // comunicacion con computadora y webapp.
    if (escritura == 0) {
      escritura = 1;
      oldTiempoAnimacion = millis();
      referenciaCalibracion = 0;
      tiempoCalibracion = 0;
      lcd.clear();
      lcd.createChar(10, b11);  // se crea b11.
      oldFlagPyAr = 7;
      flagCom = 0;
    }
    // escritura de la pantalla.
    if ((flagPyAr != oldFlagPyAr || flagCom != 0) && escrituraDisplay == 0) {
      escrituraDisplay = 1;
      oldFlagPyAr = flagPyAr;
    }
    if (flagPyAr == 0 && escrituraDisplay == 1 && flagCom == 0) {
      escrituraDisplay = 0;
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("En espera de");
      lcd.setCursor(2, 2);
      lcd.print("datos");
      oldTiempoAnimacion = millis();
    } else if ((flagPyAr == 2 || flagPyAr == 4) && escrituraDisplay == 1 && flagCom == 0) {
      escrituraDisplay = 0;
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("Enviando datos");
      lcd.setCursor(2, 2);
      lcd.print("espere");
    } else if ((flagPyAr == 1 || flagPyAr == 3) && escrituraDisplay == 1 && flagCom == 0) {
      escrituraDisplay = 0;
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("Recibiendo datos");
      lcd.setCursor(2, 2);
      lcd.print("espere");
    } else if (escrituraDisplay == 1 && flagCom == 1) {  // se pasa al menu de sensibilidad.
      escrituraDisplay = 0;
      flagCom = 0;
      calibradoCompleto = 1;  // se deja constancia de que la calibracion ha sido completada.
      modoAsistido = 2;
      cantCanales = 1;
      menu = 11;
      escritura = 0;
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("Terminado");
      lcd.setCursor(2, 2);
      lcd.print("Modo 1 canal");
      delay(4000);
      flagPyAr = 7;
    } else if (escrituraDisplay == 1 && flagCom == 2) {  // se pasa al menu de sensibilidad.
      escrituraDisplay = 0;
      flagCom = 0;
      calibradoCompleto = 2;  // se deja constancia de que la calibracion ha sido completada.
      modoAsistido = 2;
      cantCanales = 2;
      menu = 11;
      escritura = 0;
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("Terminado");
      lcd.setCursor(2, 2);
      lcd.print("Modo 2 canales");
      delay(4000);
      flagPyAr = 7;
    }
    // animacion en espera de datos.
    tiempoAnimacion = millis();
    if ((tiempoAnimacion - oldTiempoAnimacion) > referenciaCalibracion && flagPyAr == 0) {
      lcd.setCursor(7, 2);
      if (referenciaCalibracion == 0) {
        lcd.print("    ");
      } else if (referenciaCalibracion == 1000) {
        lcd.print(".");
      } else if (referenciaCalibracion == 2000) {
        lcd.print("..");
      } else if (referenciaCalibracion == 3000) {
        lcd.print("...");
      } else if (referenciaCalibracion == 4000) {
        lcd.print("....");
      }
      if (referenciaCalibracion == 5000) {
        referenciaCalibracion = 0;
        oldTiempoAnimacion = millis();
      } else {
        referenciaCalibracion = referenciaCalibracion + 1000;
      }
    }
    // comunicacion con python.
    while (flagPyAr == 4 && flagCom == 0) {  // enviar informacion un solo canal.
      digitalWrite(LED_BUILTIN, HIGH);
      Wire.requestFrom(9, 4);
      while (Wire.available()) {
        a = Wire.read();
        b = Wire.read();
        c = Wire.read();
        d = Wire.read();
        bigNum1 = a;
        bigNum1 = bigNum1 << 8 | b;
        bigNum2 = c;
        bigNum2 = bigNum2 << 8 | d;
      }
      Serial.write(bigNum1 / 256);
      Serial.write(bigNum1 % 256);
      Serial.flush();
      if (Serial.available() >= 1) {
        int fin = Serial.readStringUntil('\n').toInt();
        if (fin == 0) {
          flagPyAr = 0;
        }
      }
    }
    while (flagPyAr == 2 && flagCom == 0) {  // enviar informacion dos canales.
      Wire.requestFrom(9, 4);
      while (Wire.available()) {
        a = Wire.read();
        b = Wire.read();
        c = Wire.read();
        d = Wire.read();
        bigNum1 = a;
        bigNum1 = bigNum1 << 8 | b;
        bigNum2 = c;
        bigNum2 = bigNum2 << 8 | d;
      }
      Serial.write(bigNum1 / 256);
      Serial.write(bigNum1 % 256);
      Serial.write(bigNum2 / 256);
      Serial.write(bigNum2 % 256);
      Serial.flush();
      if (Serial.available() >= 1) {
        int fin = Serial.readStringUntil('\n').toInt();
        if (fin == 0) {
          flagPyAr = 0;
        }
      }
    }
    while (flagPyAr == 3 && flagCom == 0) {  // recibir informacion 1 canal.
      if (Serial.available() >= 9) {
        for (int i = 0; i < 9; i++) {
          incoming[i] = Serial.read();
        }
        for (int i = 0; i < 9; i++) {
          if (i < 5) {
            ParMunSimple[i] = incoming[i];
          } else if (i < 9) {
            RefMunSimple[i - 5] = incoming[i];
          }
        }
        flagCom = 1;
        umbralMunSimple = ParMunSimple[4];  // se guarda el umbral para un solo canal.
      }
    }
    while (flagPyAr == 1 && flagCom == 0) {  // recibir informacion dos canales.
      if (Serial.available() >= 36) {
        for (int i = 0; i < 36; i++) {
          incoming[i] = Serial.read();
        }
        for (int i = 0; i < 36; i++) {
          if (i < 8) {
            ParMano[i] = incoming[i];
          } else if (i < 16) {
            ParMun[i - 8] = incoming[i];
          } else if (i < 26) {
            RefMano[i - 16] = incoming[i];
          } else if (i < 36) {
            RefMun[i - 26] = incoming[i];
          }
        }
        if (RefMano[2] == 0) {
          RefMano[3] = RefMano[3] * -1;
        }
        if (RefMun[2] == 0) {
          RefMun[3] = RefMun[3] * -1;
        }
        RefMano[2] = RefMano[3];
        RefMun[2] = RefMun[3];
        flagCom = 2;
        umbralMano = ParMano[7];  // se guardan los umbrales para dos canales.
        umbralMun = ParMun[7];
        if (ParMano[1] == 1) {  // se asigna el canal para la mano y la muñeca.
          canalMano = 2;
        } else if (ParMano[0] == 1) {
          canalMano = 1;
        }
        if (ParMun[1] == 1) {
          canalMun = 2;
        } else if (ParMun[0] == 1) {
          canalMun = 1;
        }
      }
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 223) {  // preguntar si se quiere recalibrar (completo).
    if (escritura == 0) {
      lcd.createChar(14, b15);  // se escribe de b15 a b18.
      lcd.createChar(15, b16);
      lcd.createChar(12, b17);  // se sobreescriben 12 y 13.
      lcd.createChar(13, b18);
      lcd.clear();
      yesNo = 0;
      lcd.setCursor(2, 1);
      lcd.print("Desea recuperar");
      lcd.setCursor(2, 2);
      lcd.print("la calibracion?");
      lcd.setCursor(6, 3);
      lcd.print("Si");
      lcd.setCursor(12, 3);
      lcd.print("No");
      escritura = 1;
    }
    // blink de texto.
    if (yesNo == 0 && xRight) {
      yesNo = 1;
      lcd.setCursor(6, 3);
      lcd.print("Si");
      lcd.setCursor(12, 3);
      lcd.print("No");
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    } else if (yesNo == 1 && xLeft) {
      yesNo = 0;
      lcd.setCursor(6, 3);
      lcd.print("Si");
      lcd.setCursor(12, 3);
      lcd.print("No");
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    }
    if (millis() - blinkTime > 300 && blinkState == 0) {
      if (yesNo == 0) {
        lcd.setCursor(6, 3);
        lcd.print("  ");
        lcd.setCursor(6, 3);
        lcd.write(14);
        lcd.setCursor(7, 3);
        lcd.write(15);
      } else if (yesNo == 1) {
        lcd.setCursor(12, 3);
        lcd.print("  ");
        lcd.setCursor(12, 3);
        lcd.write(12);
        lcd.setCursor(13, 3);
        lcd.write(13);
      }
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      if (yesNo == 0) {
        lcd.setCursor(6, 3);
        lcd.print("  ");
        lcd.setCursor(6, 3);
        lcd.print("Si");
      } else if (yesNo == 1) {
        lcd.setCursor(12, 3);
        lcd.print("  ");
        lcd.setCursor(12, 3);
        lcd.print("No");
      }
      blinkTime = millis();
      blinkState = 0;
    }
    if (buttonState4 == LOW) {          // para volver a seleccion modo asistido.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 21;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW && yesNo == 1) {  // cuando se elige no se vuelve a realizar la calibracion.
      digitalWrite(LED_BUILTIN, HIGH);               // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      flagPyAr = 0;  // vuelve a entrar a espera de datos
      escritura = 0;
      calibradoCompleto = 0;  // se deja constancia de que se va a sobreescribir la calibracion.
      menu = 221;
      lcd.clear();
    } else if (buttonState3 == LOW && yesNo == 0) {  // cuando se elige si se avanza con la calibracion guardada.
      digitalWrite(LED_BUILTIN, HIGH);               // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      escritura = 0;
      modoAsistido = 2;  // se deja constancia de que es el modo completo.
      menu = 11;         // se pasa a la seleccion de intensidad.
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 224) {  // pantalla de funcionamiento del modo asistido completo de dos canales o un canal.
    // es el codigo del modo asistido rapido con leves modificaciones.
    if (escritura == 0) {
      lcd.clear();
      lcd.createChar(10, b11);  // se crea b11.
      lcd.createChar(9, b19);   // se crea b19.
      frecuenciaInfladoPWM = velocidadInfladoManual;
      frecuenciaDesinfladoPWM = velocidadDesinfladoManual;
      miliPeriodoInfladoPWM = round((1000.0 / frecuenciaInfladoPWM));
      miliPeriodoDesinfladoPWM = round((1000.0 / frecuenciaDesinfladoPWM));
      miliTiempoOnInflado = round((miliPeriodoInfladoPWM) * (DCInflado / 100.0));
      miliTiempoOnDesinflado = round((miliPeriodoDesinfladoPWM) * (DCDesinflado / 100.0));
      miliTiempoOffInflado = miliPeriodoInfladoPWM - miliTiempoOnInflado;
      miliTiempoOffDesinflado = miliPeriodoDesinfladoPWM - miliTiempoOnDesinflado;
      estadoEntrenamiento = 0;
      escrituraEntrenamiento = 1;
      canal1 = 0;
      canal2 = 0;
      enPWM = 0;
      enPWM_canal1 = 0;
      enPWM_canal2 = 0;
      buttonPress = 0;
      readIndex = 0;
      fila = 1;
      escritura = 1;
    }
    // cursor personalizado.
    if (millis() - blinkTime > 300 && blinkState == 0) {
      lcd.setCursor(0, 1);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      lcd.setCursor(0, fila);
      lcd.write(10);
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      lcd.setCursor(0, 1);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      lcd.setCursor(0, fila);
      blinkTime = millis();
      blinkState = 0;
    }
    // subir y bajar de fila y seleccionar una opcion.
    if ((buttonState1 == LOW || yUp) && buttonPress == 0) {  // para subir de fila en el menu 15.
      digitalWrite(LED_BUILTIN, HIGH);                       // Enciende el LED.
      fila = fila - 1;
      if (fila == 0) {  // casos particulares dado las dimensiones del menu.
        fila = 2;
      }
      lcd.setCursor(0, fila);
      buttonPress = 1;
      buttonTime = millis();
    } else if ((buttonState2 == LOW || yDown) && buttonPress == 0) {  // para bajar de fila en el menu 15.
      digitalWrite(LED_BUILTIN, HIGH);                                // Enciende el LED.
      fila = fila + 1;
      if (fila == 3) {  // casos particulares dado las dimensiones del menu.
        fila = 1;
      }
      lcd.setCursor(0, fila);
      buttonPress = 1;
      buttonTime = millis();
    } else if (buttonState3 == LOW && buttonPress == 0) {
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED.
      buttonPress = 1;
      buttonTime = millis();
      if (estadoEntrenamiento == 0 && fila == 1) {  // prender la deteccion con EMG.
        estadoEntrenamiento = 1;
      } else if (estadoEntrenamiento == 0 && fila == 2) {  // voler al menu principal.
        menu = 0;
        escritura = 0;
      } else if (estadoEntrenamiento == 1 && fila == 2) {  // volver al menu principal.
        menu = 0;
        escritura = 0;
      } else if (estadoEntrenamiento == 1 && fila == 1) {  // pausar la deteccion con EMG.
        estadoEntrenamiento = 2;
        digitalWrite(bomba1, LOW);  // se apagan todas las bombas para congelar el estado del guante.
        digitalWrite(bomba2, LOW);
        digitalWrite(bomba3, LOW);
      } else if (estadoEntrenamiento == 2 && fila == 2) {
        menu = 0;
        escritura = 0;
      } else if (estadoEntrenamiento == 2 && fila == 1) {
        estadoEntrenamiento = 1;  // se recupera el entrenamiento en el estado en el que se lo dejo.
      }
      escrituraEntrenamiento = 1;                // el flag indica que se sobreescriba la pantalla.
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
    } else if (millis() - buttonTime > 200 && buttonPress == 1) {  // para no usar delays que puedan interferir con el entrenamiento.
      buttonPress = 0;
      digitalWrite(LED_BUILTIN, LOW);
    }
    // escritura de la pantalla en base a estadoEntrenamiento y escrituraEntrenamiento.
    if (escrituraEntrenamiento == 1) {
      lcd.clear();
      if (estadoEntrenamiento == 0) {
        lcd.setCursor(2, 0);
        lcd.print("Modo asistido:");
        lcd.setCursor(2, 1);
        lcd.print("Comenzar deteccion");
        lcd.setCursor(2, 2);
        lcd.print("Volver a pantalla");
        lcd.setCursor(2, 3);
        lcd.print("principal");
      } else if (estadoEntrenamiento == 1) {
        lcd.setCursor(2, 0);
        lcd.print("Modo asistido:");
        lcd.setCursor(2, 1);
        lcd.print("Pausar deteccion");
        lcd.setCursor(2, 2);
        lcd.print("Terminar");
      } else if (estadoEntrenamiento == 2) {
        lcd.setCursor(2, 0);
        lcd.print("Modo asistido:");
        lcd.setCursor(2, 1);
        lcd.print("Retomar deteccion");
        lcd.setCursor(2, 2);
        lcd.print("Terminar");
      }
      fila = 1;
      escrituraEntrenamiento = 0;
    }
    // se toman los valores de presion.
    sensorValuePresion1 = analogRead(sensorPresion1);
    sensorValuePresion2 = analogRead(sensorPresion2);
    sensorValuePresion3 = analogRead(sensorPresion3);
    // se escalan los valores de presion a voltaje.
    lecturaPresion1 = 5 * (sensorValuePresion1 / 1023);
    lecturaPresion2 = 5 * (sensorValuePresion2 / 1023);
    lecturaPresion3 = 3.3 * (sensorValuePresion3 / 1023);
    // se transforma el valor de voltaje a un valor de presion real.
    presion1 = (((lecturaPresion1 / 5) + 0.00842) / 0.002421) - 100;
    presion2 = (((lecturaPresion2 / 5) + 0.00842) / 0.002421) - 100;
    presion3 = (((lecturaPresion3 / 3.3) - 0.04) / 0.018);
    // PWM.
    currentTime = millis();
    elapsedTimePWM = currentTime - oldTimePWM;
    elapsedTimePWM_canal1 = currentTime - oldTimePWM_canal1;
    elapsedTimePWM_canal2 = currentTime - oldTimePWM_canal2;
    if (cantCanales == 2) {  // deteccion para dos canales.
      to2(ParMano, ParMun, RefMano, RefMun);
      if (estadoEntrenamiento == 1) {
        if (score_mano >= umbralMano && canal1 == 0) {  // mano.
          digitalWrite(solenoide2, LOW);
          digitalWrite(solenoide3, LOW);
          canal1 = 1;
          lcd.setCursor(2, 3);
          lcd.print("                  ");
          lcd.setCursor(4, 3);
          lcd.write(9);
          lcd.setCursor(6, 3);
          if (canal2 == 1) {
            lcd.print("Conjunto");
          } else {
            lcd.print("Mano");
          }
        } else if (score_mun >= umbralMun && canal2 == 0) {  // muneca.
          digitalWrite(solenoide1, HIGH);
          canal2 = 1;
          lcd.setCursor(2, 3);
          lcd.print("                  ");
          lcd.setCursor(4, 3);
          lcd.write(9);
          lcd.setCursor(6, 3);
          if (canal1 == 1) {
            lcd.print("Conjunto");
          } else {
            lcd.print("Muneca");
          }
        } else if (canal1 == 1 && score_mano < umbralMano) {  // corte mano.
          digitalWrite(bomba2, LOW);
          digitalWrite(bomba3, LOW);
          canal1 = 0;
          lcd.setCursor(2, 3);
          lcd.print("                  ");
          lcd.setCursor(4, 3);
          lcd.write(9);
          lcd.setCursor(6, 3);
          if (canal2 == 1) {  // corte conjunto.
            lcd.print("Muneca");
          } else {
            lcd.print("Sin esfuerzo");
          }
        } else if (canal2 == 1 && score_mun < umbralMun) {  // corte muneca.
          digitalWrite(bomba1, LOW);
          canal2 = 0;
          lcd.setCursor(2, 3);
          lcd.print("                  ");
          lcd.setCursor(4, 3);
          lcd.write(9);
          lcd.setCursor(6, 3);
          if (canal1 == 1) {  // corte conjunto
            lcd.print("Mano");
          } else {
            lcd.print("Sin esfuerzo");
          }
        }
        // logica para canal 1.
        if (canal1 == 1 && (frecuenciaInfladoPWM == 100 || DCInflado == 100)) {  // sin PMW.
          if (presion2 < presionMin2_escalada) {
            digitalWrite(bomba2, HIGH);
          } else if (presion2 > presionMax2_escalada) {
            digitalWrite(bomba2, LOW);
          }
          if (presion3 < presionMin3_escalada) {
            digitalWrite(bomba3, HIGH);
          } else if (presion3 > presionMax3_escalada) {
            digitalWrite(bomba3, LOW);
          }
        } else if (canal1 == 1 && frecuenciaInfladoPWM != 100) {  // PWM inflado.
          if (elapsedTimePWM_canal1 > miliTiempoOffInflado && enPWM_canal1 == 0) {
            if (presion2 < presionMin2_escalada) {
              digitalWrite(bomba2, HIGH);
            } else if (presion2 > presionMax2_escalada) {
              digitalWrite(bomba2, LOW);
            }
            if (presion3 < presionMin3_escalada) {
              digitalWrite(bomba3, HIGH);
            } else if (presion3 > presionMax3_escalada) {
              digitalWrite(bomba3, LOW);
            }
            oldTimePWM_canal1 = currentTime;
            enPWM_canal1 = 1;
          } else if (elapsedTimePWM_canal1 > miliTiempoOnInflado && enPWM_canal1 == 1) {
            digitalWrite(bomba2, LOW);
            digitalWrite(bomba3, LOW);
            oldTimePWM_canal1 = currentTime;
            enPWM_canal1 = 0;
          }
        } else if (canal1 == 0 && (velocidadDesinfladoManual == 100 || DCDesinflado == 100)) {  // sin PMW.
          digitalWrite(solenoide2, HIGH);
          digitalWrite(solenoide3, HIGH);
        } else if (canal1 == 0 && frecuenciaDesinfladoPWM != 100) {  // entra al PMW de desinflado.
          if (elapsedTimePWM_canal1 > miliTiempoOffDesinflado && enPWM_canal1 == 0) {
            // señal de encendido a los solenoides
            oldTimePWM_canal1 = currentTime;
            enPWM_canal1 = 1;
          } else if (elapsedTimePWM_canal1 <= miliTiempoOnDesinflado && enPWM_canal1 == 1) {
            digitalWrite(solenoide2, HIGH);  // se abren los solenoides.
            digitalWrite(solenoide3, HIGH);
          } else if (elapsedTimePWM_canal1 > miliTiempoOnDesinflado && enPWM_canal1 == 1) {
            digitalWrite(solenoide2, LOW);
            digitalWrite(solenoide3, LOW);
            oldTimePWM_canal1 = currentTime;
            enPWM_canal1 = 0;
          }
        }
        // logica para canal 2.
        if (canal2 == 1 && (frecuenciaInfladoPWM == 100 || DCInflado == 100)) {  // sin PMW.
          if (presion1 < presionMin1_escalada) {
            digitalWrite(bomba1, HIGH);
            digitalWrite(solenoide1, HIGH);
          } else if (presion1 > presionMax1_escalada) {
            digitalWrite(bomba1, LOW);
          }
        } else if (canal2 == 1 && frecuenciaInfladoPWM != 100) {  // PWM inflado.
          if (elapsedTimePWM_canal2 > miliTiempoOffInflado && enPWM_canal2 == 0) {
            if (presion1 < presionMin1_escalada) {
              digitalWrite(bomba1, HIGH);
              digitalWrite(solenoide1, HIGH);
            } else if (presion1 > presionMax1_escalada) {
              digitalWrite(bomba1, LOW);
            }
            oldTimePWM_canal2 = currentTime;
            enPWM_canal2 = 1;
          } else if (elapsedTimePWM_canal2 > miliTiempoOnInflado && enPWM_canal2 == 1) {
            digitalWrite(bomba1, LOW);
            oldTimePWM_canal2 = currentTime;
            enPWM_canal2 = 0;
          }
        } else if (canal2 == 0 && (velocidadDesinfladoManual == 100 || DCDesinflado == 100)) {  // sin PMW.
          digitalWrite(solenoide1, LOW);
        } else if (canal2 == 0 && frecuenciaDesinfladoPWM != 100) {  // entra al PMW de desinflado.
          if (elapsedTimePWM_canal2 > miliTiempoOffDesinflado && enPWM_canal2 == 0) {
            digitalWrite(solenoide1, LOW);
            oldTimePWM_canal2 = currentTime;
            enPWM_canal2 = 1;
          } else if (elapsedTimePWM_canal1 <= miliTiempoOnDesinflado && enPWM_canal1 == 1) {
            digitalWrite(solenoide1, LOW);  // se abre el solenoide.
          } else if (elapsedTimePWM_canal2 > miliTiempoOnDesinflado && enPWM_canal2 == 1) {
            digitalWrite(solenoide1, HIGH);
            oldTimePWM_canal2 = currentTime;
            enPWM_canal2 = 0;
          }
        }
      }
    } else if (cantCanales == 1) {  // deteccion de un solo canal. Se asume que ese canal es muneca.
      to1(ParMunSimple, RefMunSimple);
      if (estadoEntrenamiento == 1) {
        if (score_mun >= umbralMunSimple && enCiclo == 0) {
          digitalWrite(solenoide1, HIGH);
          digitalWrite(solenoide2, LOW);
          digitalWrite(solenoide3, LOW);
          enCiclo = 1;
          canal2 = 1;
          lcd.setCursor(2, 3);
          lcd.print("                  ");
          lcd.setCursor(4, 3);
          lcd.write(9);
          lcd.setCursor(6, 3);
          lcd.print("Muneca");
        } else if (enCiclo == 1 && canal2 == 1 && score_mun < umbralMunSimple) {
          digitalWrite(bomba1, LOW);
          digitalWrite(bomba2, LOW);
          digitalWrite(bomba3, LOW);
          enCiclo = 0;
          canal2 = 0;
          lcd.setCursor(2, 3);
          lcd.print("                  ");
          lcd.setCursor(4, 3);
          lcd.write(9);
          lcd.setCursor(6, 3);
          lcd.setCursor(4, 3);
          lcd.print("Sin esfuerzo");
        }
        if (enCiclo == 1 && (frecuenciaInfladoPWM == 100 || DCInflado == 100)) {
          if (presion1 < presionMin1_escalada) {
            digitalWrite(bomba1, HIGH);
            digitalWrite(solenoide1, HIGH);
          } else if (presion1 > presionMax1_escalada) {
            digitalWrite(bomba1, LOW);
          }
          if (presion2 < presionMin2_escalada) {
            digitalWrite(bomba2, HIGH);
          } else if (presion2 > presionMax2_escalada) {
            digitalWrite(bomba2, LOW);
          }
          if (presion3 < presionMin3_escalada) {
            digitalWrite(bomba3, HIGH);
          } else if (presion3 > presionMax3_escalada) {
            digitalWrite(bomba3, LOW);
          }
        } else if (enCiclo == 1 && frecuenciaInfladoPWM != 100) {
          if (elapsedTimePWM > miliTiempoOffInflado && enPWM == 0) {
            if (presion1 < presionMin1_escalada) {
              digitalWrite(bomba1, HIGH);
              digitalWrite(solenoide1, HIGH);
            } else if (presion1 > presionMax1_escalada) {
              digitalWrite(bomba1, LOW);
            }
            if (presion2 < presionMin2_escalada) {
              digitalWrite(bomba2, HIGH);
            } else if (presion2 > presionMax2_escalada) {
              digitalWrite(bomba2, LOW);
            }
            if (presion3 < presionMin3_escalada) {
              digitalWrite(bomba3, HIGH);
            } else if (presion3 > presionMax3_escalada) {
              digitalWrite(bomba3, LOW);
            }
            oldTimePWM = currentTime;
            enPWM = 1;
          } else if (elapsedTimePWM > miliTiempoOnInflado && enPWM == 1) {
            digitalWrite(bomba1, LOW);
            digitalWrite(bomba2, LOW);
            digitalWrite(bomba3, LOW);
            oldTimePWM = currentTime;
            enPWM = 0;
          }
        } else if (enCiclo == 0 && (velocidadDesinfladoManual == 100 || DCDesinflado == 100)) {
          digitalWrite(solenoide1, LOW);
          digitalWrite(solenoide2, HIGH);
          digitalWrite(solenoide3, HIGH);
        } else if (enCiclo == 0 && frecuenciaDesinfladoPWM != 100) {
          if (elapsedTimePWM > miliTiempoOffDesinflado && enPWM == 0) {
            digitalWrite(solenoide1, LOW);
            digitalWrite(solenoide2, HIGH);
            digitalWrite(solenoide3, HIGH);
            oldTimePWM = currentTime;
            enPWM = 1;
          } else if (elapsedTimePWM <= miliTiempoOnDesinflado && enPWM == 1) {
            digitalWrite(solenoide1, LOW);  // se abren los solenoides.
            digitalWrite(solenoide2, HIGH);
            digitalWrite(solenoide3, HIGH);
          } else if (elapsedTimePWM > miliTiempoOnDesinflado && enPWM == 1) {
            digitalWrite(solenoide1, HIGH);
            digitalWrite(solenoide2, LOW);
            digitalWrite(solenoide3, LOW);
            oldTimePWM = currentTime;
            enPWM = 0;
          }
        }
      }
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 231) {  // modo asistido rapido, pantalla con informacion de calibracion.
    if (escritura == 0) {
      lcd.createChar(8, b20);   // se crea b20.
      lcd.createChar(9, b19);   // se crea b19.
      lcd.createChar(10, b11);  // se crea b11.
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("La calibracion");
      lcd.setCursor(2, 1);
      lcd.print("dura 20 segundos");
      lcd.setCursor(19, 1);
      lcd.write(9);
      lcd.setCursor(3, 3);
      lcd.print("Comenzar");
      escritura = 1;
      leftRight = 0;
    }
    // cambio de texto.
    if (leftRight == 0 && xRight) {
      leftRight = 1;
      lcd.setCursor(2, 0);
      lcd.print("              ");
      lcd.setCursor(2, 1);
      lcd.print("                ");
      lcd.setCursor(19, 1);
      lcd.print(" ");
      lcd.setCursor(2, 0);
      lcd.print("Es importante");
      lcd.setCursor(2, 1);
      lcd.print("realizar el");
      lcd.setCursor(2, 2);
      lcd.print("movimiento deseado");
      lcd.setCursor(0, 1);
      lcd.write(8);
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    } else if (leftRight == 1 && xLeft) {
      leftRight = 0;
      lcd.setCursor(2, 0);
      lcd.print("             ");
      lcd.setCursor(2, 1);
      lcd.print("           ");
      lcd.setCursor(2, 2);
      lcd.print("                  ");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(2, 0);
      lcd.print("La calibracion");
      lcd.setCursor(2, 1);
      lcd.print("dura 20 segundos");
      lcd.setCursor(19, 1);
      lcd.write(9);
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    }
    // cursor personalizado.
    if (millis() - blinkTime > 300 && blinkState == 0) {
      lcd.setCursor(1, 3);
      lcd.print(" ");
      lcd.setCursor(1, 3);
      lcd.write(10);
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      lcd.setCursor(1, 3);
      lcd.print(" ");
      blinkTime = millis();
      blinkState = 0;
    }
    if (buttonState4 == LOW) {          // para volver a seleccion de modo asistido.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 21;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW) {   // para avanzar a la calibracion.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 232;
      escritura = 0;
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 232) {  // en calibracion y toma de señal del EMG.
    if (escritura == 0) {
      lcd.clear();
      lcd.createChar(10, b11);  // se crea b11.
      lcd.setCursor(2, 1);
      lcd.print("La calibracion");
      lcd.setCursor(2, 2);
      lcd.print("comienza en ");
      referenciaCalibracion = 0;  // reinicio variables para no guardar un estado anterior.
      tiempoCalibracion = 0;
      enCalibracion = 0;
      calibradoRapido = 0;
      escrituraCalibracion = 0;
      comienzoCalibracion = millis();
      instanteCalibracion = comienzoCalibracion;
      maxAverageEMG1 = 0;
      maxAverageEMG2 = 0;
      averageEMG1 = 0;
      averageEMG2 = 0;
      totalEMG1 = 0;
      totalEMG2 = 0;
      readIndex = 0;
      escritura = 1;
    }
    Wire.requestFrom(9, 4);
    while (Wire.available()) {
      a = Wire.read();
      b = Wire.read();
      c = Wire.read();
      d = Wire.read();
      bigNum1 = a;
      bigNum1 = bigNum1 << 8 | b;
      bigNum2 = c;
      bigNum2 = bigNum2 << 8 | d;
    }
    totalEMG1 = totalEMG1 - readingsEMG1[readIndex];
    totalEMG2 = totalEMG2 - readingsEMG2[readIndex];
    readingsEMG1[readIndex] = bigNum1;
    readingsEMG2[readIndex] = bigNum2;
    totalEMG1 = totalEMG1 + readingsEMG1[readIndex];
    totalEMG2 = totalEMG2 + readingsEMG2[readIndex];
    if (readIndex == numReadings1 - 1) {
      readIndex = 0;
    } else {
      readIndex = readIndex + 1;
    }
    averageEMG1 = totalEMG1 / numReadings1;
    averageEMG2 = totalEMG2 / numReadings1;
    if (averageEMG1 > maxAverageEMG1) {
      maxAverageEMG1 = averageEMG1;
    }
    if (averageEMG2 > maxAverageEMG2) {
      maxAverageEMG2 = averageEMG2;
    }
    instanteCalibracion = millis();
    tiempoCalibracion = instanteCalibracion - comienzoCalibracion;
    if (tiempoCalibracion >= 10000 && enCalibracion == 0) {  // se da comienzo a la calibracion.
      enCalibracion = 1;
      oldTiempoAnimacion = millis();
      referenciaCalibracion = 0;
    } else if (enCalibracion == 0) {                     // toma de datos de EMG para preparar la media movil.
      if (tiempoCalibracion >= referenciaCalibracion) {  // se acondiciona la media ovil en la pantalla de espera.
        tiempoRestante = (10000 - referenciaCalibracion) / 1000;
        referenciaCalibracion = referenciaCalibracion + 1000;
        lcd.setCursor(14, 2);
        lcd.print("  ");
        lcd.setCursor(14, 2);
        lcd.print(tiempoRestante);
      }
    } else if (enCalibracion == 1 && escrituraCalibracion == 0) {  // se escribe la pantalla.
      lcd.clear();
      lcd.setCursor(4, 1);
      lcd.print("Calibrando");
      lcd.setCursor(4, 2);
      lcd.print("espere");
      escrituraCalibracion = 2;
    } else if (escrituraCalibracion == 2 && tiempoCalibracion < 30000) {  // se toman los datos de EMG.
      tiempoAnimacion = millis();
      if ((tiempoAnimacion - oldTiempoAnimacion) > referenciaCalibracion) {
        lcd.setCursor(10, 2);
        if (referenciaCalibracion == 0) {
          lcd.print("    ");
        } else if (referenciaCalibracion == 1000) {
          lcd.print(".");
        } else if (referenciaCalibracion == 2000) {
          lcd.print("..");
        } else if (referenciaCalibracion == 3000) {
          lcd.print("...");
        } else if (referenciaCalibracion == 4000) {
          lcd.print("....");
        }
        if (referenciaCalibracion == 5000) {
          referenciaCalibracion = 0;
          oldTiempoAnimacion = millis();
        } else {
          referenciaCalibracion = referenciaCalibracion + 1000;
        }
      }
    } else if (tiempoCalibracion >= 20000 && escrituraCalibracion == 2) {
      enCalibracion = 0;
      escrituraCalibracion = 3;
      lcd.clear();
      lcd.setCursor(4, 1);
      lcd.print("Calibracion");
      lcd.setCursor(4, 2);
      lcd.print("completada!");
      delay(4000);
      calibradoRapido = 1;  // se toma constancia de la calibracion.
      menu = 234;           // se pasa a la seleccion de intensidad.
      lcd.clear();
      escritura = 0;
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 233) {  // preguntar si se quiere recalibrar (rapido).
    if (escritura == 0) {
      lcd.createChar(14, b15);  // se escribe de b15 a b18.
      lcd.createChar(15, b16);
      lcd.createChar(12, b17);  // se sobreescriben 12 y 13.
      lcd.createChar(13, b18);
      lcd.clear();
      yesNo = 0;
      lcd.setCursor(2, 1);
      lcd.print("Desea recuperar");
      lcd.setCursor(2, 2);
      lcd.print("la calibracion?");
      lcd.setCursor(6, 3);
      lcd.print("Si");
      lcd.setCursor(12, 3);
      lcd.print("No");
      escritura = 1;
    }
    // blink de texto.
    if (yesNo == 0 && xRight) {
      yesNo = 1;
      lcd.setCursor(6, 3);
      lcd.print("Si");
      lcd.setCursor(12, 3);
      lcd.print("No");
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    } else if (yesNo == 1 && xLeft) {
      yesNo = 0;
      lcd.setCursor(6, 3);
      lcd.print("Si");
      lcd.setCursor(12, 3);
      lcd.print("No");
      delay(200);
      while (analogRead(JOYSTICK_AXIS_X) > 1000 || analogRead(JOYSTICK_AXIS_X) < 20) {  // para no moverse de forma brusca.
        delay(100);
      }
    }
    if (millis() - blinkTime > 300 && blinkState == 0) {
      if (yesNo == 0) {
        lcd.setCursor(6, 3);
        lcd.print("  ");
        lcd.setCursor(6, 3);
        lcd.write(14);
        lcd.setCursor(7, 3);
        lcd.write(15);
      } else if (yesNo == 1) {
        lcd.setCursor(12, 3);
        lcd.print("  ");
        lcd.setCursor(12, 3);
        lcd.write(12);
        lcd.setCursor(13, 3);
        lcd.write(13);
      }
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      if (yesNo == 0) {
        lcd.setCursor(6, 3);
        lcd.print("  ");
        lcd.setCursor(6, 3);
        lcd.print("Si");
      } else if (yesNo == 1) {
        lcd.setCursor(12, 3);
        lcd.print("  ");
        lcd.setCursor(12, 3);
        lcd.print("No");
      }
      blinkTime = millis();
      blinkState = 0;
    }
    if (buttonState4 == LOW) {          // para volver a seleccion modo asistido.
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 21;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW && yesNo == 1) {  // cuando se elige no se vuelve a realizar la calibracion.
      digitalWrite(LED_BUILTIN, HIGH);               // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 231;
      escritura = 0;
      calibradoRapido = 0;  // se deja constancia de que se va a sobreescribir la calibracion.
      lcd.clear();
    } else if (buttonState3 == LOW && yesNo == 0) {  // cuando se elige si se avanza con la calibracion guardada.
      digitalWrite(LED_BUILTIN, HIGH);               // Enciende el LED
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      escritura = 0;
      menu = 234;  // se pasa a la seleccion de sensibilidad.
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 234) {  // seleccion de sensibilidad.
    if (escritura == 0) {
      lcd.noBlink();
      lcd.createChar(0, b1);
      lcd.createChar(1, b2);
      lcd.createChar(2, b3);
      lcd.createChar(3, b4);
      lcd.createChar(4, b5);
      lcd.createChar(5, b6);
      lcd.createChar(6, b7);
      lcd.createChar(7, b8);
      oldLevel = 17;
      oldLevelPorcentual = 101;
      escritura = 1;
      lcd.setCursor(2, 3);
      lcd.print("Sensibilidad: ");
    }
    valuePot = analogRead(POT_PIN1);
    level = map(valuePot, 40, 940, 0, 16);
    levelPorcentual = map(valuePot, 40, 940, 0, 100);
    if (level > 16 || levelPorcentual > 100) {
      porcentaje = 100;
      level = 16;
    } else if (level < 0 || levelPorcentual < 1) {
      porcentaje = 1;
      level = 0;
    } else {
      porcentaje = levelPorcentual;
    }
    if (level != oldLevel) {
      lcd.setCursor(2, 1);
      lcd.print("                ");
      lcd.setCursor(2, 2);
      lcd.print("                ");
      if (level < 9) {
        for (int i = 0; i < level; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
      } else {
        for (int i = 0; i < 8; i++) {
          lcd.setCursor(i + 2, 2);
          lcd.write(i);
        }
        for (int i = 0; i < level - 8; i++) {
          lcd.setCursor(i + 10, 1);
          lcd.write(i);
          lcd.setCursor(i + 10, 2);
          lcd.print(char(255));
        }
      }
      lcd.setCursor(15, 3);
      lcd.print("    ");
      lcd.setCursor(15, 3);
      lcd.print(porcentaje);
      if (porcentaje == 100) {
        lcd.setCursor(18, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(16, 3);
      } else {
        lcd.setCursor(17, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;
      oldLevel = level;
    } else if (levelPorcentual != oldLevelPorcentual) {
      lcd.setCursor(15, 3);
      lcd.print("    ");
      lcd.setCursor(15, 3);
      lcd.print(porcentaje);
      if (porcentaje == 100) {
        lcd.setCursor(18, 3);
      } else if (porcentaje < 10) {
        lcd.setCursor(16, 3);
      } else {
        lcd.setCursor(17, 3);
      }
      lcd.print("%");
      oldLevelPorcentual = levelPorcentual;
    }
    if (buttonState4 == LOW) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN4) == LOW) {
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 21;
      escritura = 0;
      lcd.clear();
    } else if (buttonState3 == LOW) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      while (digitalRead(BUTTON_PIN3) == LOW) {
        delay(100);
      }
      digitalWrite(LED_BUILTIN, LOW);
      menu = 11;
      escritura = 0;
      sensibilidad = porcentaje;  // se guarda el valor de sensibilidad.
      lcd.clear();
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------
  } else if (menu == 235) {  // pantalla de funcionamiento del modo asistido rapido.
    if (escritura == 0) {
      lcd.clear();
      lcd.createChar(10, b11);  // se crea b11.
      lcd.createChar(9, b19);   // se crea b19.
      frecuenciaInfladoPWM = velocidadInfladoManual;
      frecuenciaDesinfladoPWM = velocidadDesinfladoManual;
      miliPeriodoInfladoPWM = round((1000.0 / frecuenciaInfladoPWM));
      miliPeriodoDesinfladoPWM = round((1000.0 / frecuenciaDesinfladoPWM));
      miliTiempoOnInflado = round((miliPeriodoInfladoPWM) * (DCInflado / 100.0));
      miliTiempoOnDesinflado = round((miliPeriodoDesinfladoPWM) * (DCDesinflado / 100.0));
      miliTiempoOffInflado = miliPeriodoInfladoPWM - miliTiempoOnInflado;
      miliTiempoOffDesinflado = miliPeriodoDesinfladoPWM - miliTiempoOnDesinflado;
      // calculo del umbral de EMG.
      sensibilidadInv = 100 - sensibilidad;  // al elegir una sensibilidad mayor el umbral de deteccion se tiene que hacer mas chico, se toma el valor opuesto a la sensibilidad.
      umbralEMG1 = maxAverageEMG1 * (sensibilidadInv / 100.0);
      umbralEMG2 = maxAverageEMG2 * (sensibilidadInv / 100.0);
      // acoto el umbral para tener un minimo umbral a sensibilidad muy alta.
      if (umbralEMG1 < 50) {
        umbralEMG1 = 50;
      }
      if (umbralEMG2 < 50) {
        umbralEMG2 = 50;
      }
      estadoEntrenamiento = 0;
      escrituraEntrenamiento = 1;
      canal1 = 0;
      canal2 = 0;
      enPWM = 0;
      enPWM_canal1 = 0;
      enPWM_canal2 = 0;
      buttonPress = 0;
      averageEMG1 = 0;
      averageEMG2 = 0;
      totalEMG1 = 0;
      totalEMG2 = 0;
      readIndex = 0;
      fila = 1;
      escritura = 1;
    }
    // cursor personalizado.
    if (millis() - blinkTime > 300 && blinkState == 0) {
      lcd.setCursor(0, 1);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      lcd.setCursor(0, fila);
      lcd.write(10);
      blinkState = 1;
    } else if (millis() - blinkTime > 600 && blinkState == 1) {
      lcd.setCursor(0, 1);  // limpiar el cursor de otras filas.
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      lcd.setCursor(0, fila);
      blinkTime = millis();
      blinkState = 0;
    }
    // subir y bajar de fila y seleccionar una opcion.
    if ((buttonState1 == LOW || yUp) && buttonPress == 0) {  // para subir de fila en el menu 15.
      digitalWrite(LED_BUILTIN, HIGH);                       // Enciende el LED.
      fila = fila - 1;
      if (fila == 0) {  // casos particulares dado las dimensiones del menu.
        fila = 2;
      }
      lcd.setCursor(0, fila);
      buttonPress = 1;
      buttonTime = millis();
    } else if ((buttonState2 == LOW || yDown) && buttonPress == 0) {  // para bajar de fila en el menu 15.
      digitalWrite(LED_BUILTIN, HIGH);                                // Enciende el LED.
      fila = fila + 1;
      if (fila == 3) {  // casos particulares dado las dimensiones del menu.
        fila = 1;
      }
      lcd.setCursor(0, fila);
      buttonPress = 1;
      buttonTime = millis();
    } else if (buttonState3 == LOW && buttonPress == 0) {
      digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED.
      buttonPress = 1;
      buttonTime = millis();
      if (estadoEntrenamiento == 0 && fila == 1) {  // prender la deteccion con EMG.
        estadoEntrenamiento = 1;
      } else if (estadoEntrenamiento == 0 && fila == 2) {  // voler al menu principal.
        menu = 0;
        escritura = 0;
      } else if (estadoEntrenamiento == 1 && fila == 2) {  // volver al menu principal.
        menu = 0;
        escritura = 0;
      } else if (estadoEntrenamiento == 1 && fila == 1) {  // pausar la deteccion con EMG.
        estadoEntrenamiento = 2;
        digitalWrite(bomba1, LOW);  // se apagan todas las bombas para congelar el estado del guante.
        digitalWrite(bomba2, LOW);
        digitalWrite(bomba3, LOW);
      } else if (estadoEntrenamiento == 2 && fila == 2) {
        menu = 0;
        escritura = 0;
      } else if (estadoEntrenamiento == 2 && fila == 1) {
        estadoEntrenamiento = 1;  // se recupera el entrenamiento en el estado en el que se lo dejo.
      }
      escrituraEntrenamiento = 1;                // el flag indica que se sobreescriba la pantalla.
      while (digitalRead(BUTTON_PIN3) == LOW) {  // para no moverse entre menus de forma brusca.
        delay(100);
      }
    } else if (millis() - buttonTime > 200 && buttonPress == 1) {  // para no usar delays que puedan interferir con el entrenamiento.
      buttonPress = 0;
      digitalWrite(LED_BUILTIN, LOW);
    }
    // escritura de la pantalla en base a estadoEntrenamiento y escrituraEntrenamiento.
    if (escrituraEntrenamiento == 1) {
      lcd.clear();
      if (estadoEntrenamiento == 0) {
        lcd.setCursor(2, 0);
        lcd.print("Modo asistido:");
        lcd.setCursor(2, 1);
        lcd.print("Comenzar deteccion");
        lcd.setCursor(2, 2);
        lcd.print("Volver a pantalla");
        lcd.setCursor(2, 3);
        lcd.print("principal");
      } else if (estadoEntrenamiento == 1) {
        lcd.setCursor(2, 0);
        lcd.print("Modo asistido:");
        lcd.setCursor(2, 1);
        lcd.print("Pausar deteccion");
        lcd.setCursor(2, 2);
        lcd.print("Terminar");
      } else if (estadoEntrenamiento == 2) {
        lcd.setCursor(2, 0);
        lcd.print("Modo asistido:");
        lcd.setCursor(2, 1);
        lcd.print("Retomar deteccion");
        lcd.setCursor(2, 2);
        lcd.print("Terminar");
      }
      fila = 1;
      escrituraEntrenamiento = 0;
    }
    // se toman los valores de presion.
    sensorValuePresion1 = analogRead(sensorPresion1);
    sensorValuePresion2 = analogRead(sensorPresion2);
    sensorValuePresion3 = analogRead(sensorPresion3);
    // se escalan los valores de presion a voltaje.
    lecturaPresion1 = 5 * (sensorValuePresion1 / 1023);
    lecturaPresion2 = 5 * (sensorValuePresion2 / 1023);
    lecturaPresion3 = 3.3 * (sensorValuePresion3 / 1023);
    // se transforma el valor de voltaje a un valor de presion real.
    presion1 = (((lecturaPresion1 / 5) + 0.00842) / 0.002421) - 100;
    presion2 = (((lecturaPresion2 / 5) + 0.00842) / 0.002421) - 100;
    presion3 = (((lecturaPresion3 / 3.3) - 0.04) / 0.018);
    // PWM.
    currentTime = millis();
    elapsedTimePWM = currentTime - oldTimePWM;
    elapsedTimePWM_canal1 = currentTime - oldTimePWM_canal1;
    elapsedTimePWM_canal2 = currentTime - oldTimePWM_canal2;
    if (estadoEntrenamiento == 1) {  // se lee el EMG solo cuando se prende la deteccion.
      Wire.requestFrom(9, 4);
      while (Wire.available()) {
        a = Wire.read();
        b = Wire.read();
        c = Wire.read();
        d = Wire.read();
        bigNum1 = a;
        bigNum1 = bigNum1 << 8 | b;
        bigNum2 = c;
        bigNum2 = bigNum2 << 8 | d;
      }
      totalEMG1 = totalEMG1 - readingsEMG1[readIndex];
      totalEMG2 = totalEMG2 - readingsEMG2[readIndex];
      readingsEMG1[readIndex] = bigNum1;
      readingsEMG2[readIndex] = bigNum2;
      totalEMG1 = totalEMG1 + readingsEMG1[readIndex];
      totalEMG2 = totalEMG2 + readingsEMG2[readIndex];
      if (readIndex == numReadings1 - 1) {
        readIndex = 0;
      } else {
        readIndex = readIndex + 1;
      }
      averageEMG1 = totalEMG1 / numReadings1;
      averageEMG2 = totalEMG2 / numReadings1;
      // se compara la media movil contra el umbral seteado.
      if (averageEMG1 >= umbralEMG1 && canal1 == 0) {  // para la muneca.
        digitalWrite(solenoide1, HIGH);
        canal1 = 1;
        lcd.setCursor(2, 3);
        lcd.print("                  ");
        lcd.setCursor(4, 3);
        lcd.write(9);
        lcd.setCursor(6, 3);
        if (canal2 == 1) {
          lcd.print("Conjunto");
        } else {
          lcd.print("Canal 1");
        }
      } else if (averageEMG2 >= umbralEMG2 && canal2 == 0) {
        digitalWrite(solenoide2, LOW);
        digitalWrite(solenoide3, LOW);
        canal2 = 1;
        lcd.setCursor(2, 3);
        lcd.print("                  ");
        lcd.setCursor(4, 3);
        lcd.write(9);
        lcd.setCursor(6, 3);
        if (canal1 == 1) {
          lcd.print("Conjunto");
        } else {
          lcd.print("Canal 2");
        }
      } else if (canal1 == 1 && averageEMG1 < umbralEMG1) {
        digitalWrite(bomba1, LOW);
        enCiclo = 0;
        canal1 = 0;
        lcd.setCursor(2, 3);
        lcd.print("                  ");
        lcd.setCursor(4, 3);
        lcd.write(9);
        lcd.setCursor(6, 3);
        if (canal2 == 1) {  // corte conjunto.
          lcd.print("Canal 2");
        } else {
          lcd.print("Sin esfuerzo");
        }
      } else if (canal2 == 1 && averageEMG2 < umbralEMG2) {
        digitalWrite(bomba2, LOW);
        digitalWrite(bomba3, LOW);
        enCiclo = 0;
        canal2 = 0;
        lcd.setCursor(2, 3);
        lcd.print("                  ");
        lcd.setCursor(4, 3);
        lcd.write(9);
        lcd.setCursor(6, 3);
        if (canal1 == 1) {  // corte conjunto
          lcd.print("Canal 1");
        } else {
          lcd.print("Sin esfuerzo");
        }
      }
      // logica para canal 1.
      if (canal1 == 1 && (frecuenciaInfladoPWM == 100 || DCInflado == 100)) {
        if (presion1 < presionMin1_escalada) {
          digitalWrite(bomba1, HIGH);
          digitalWrite(solenoide1, HIGH);
        } else if (presion1 > presionMax1_escalada) {
          digitalWrite(bomba1, LOW);
        }
      } else if (canal1 == 1 && frecuenciaInfladoPWM != 100) {  // entra al PWM de inflado.
        if (elapsedTimePWM_canal1 > miliTiempoOffInflado && enPWM_canal1 == 0) {
          if (presion1 < presionMin1_escalada) {
            digitalWrite(bomba1, HIGH);
            digitalWrite(solenoide1, HIGH);
          } else if (presion1 > presionMax1_escalada) {
            digitalWrite(bomba1, LOW);
          }
          oldTimePWM_canal1 = currentTime;
          enPWM_canal1 = 1;
        } else if (elapsedTimePWM_canal1 > miliTiempoOnInflado && enPWM_canal1 == 1) {
          digitalWrite(bomba1, LOW);
          oldTimePWM_canal1 = currentTime;
          enPWM_canal1 = 0;
        }
      } else if (canal1 == 0 && (velocidadDesinfladoManual == 100 || DCDesinflado == 100)) {
        digitalWrite(solenoide1, LOW);
      } else if (canal1 == 0 && frecuenciaDesinfladoPWM != 100) {  // entra al PMW de desinflado.
        if (elapsedTimePWM_canal1 > miliTiempoOffDesinflado && enPWM_canal1 == 0) {
          // señal de encendido a los solenoides
          oldTimePWM_canal1 = currentTime;
          enPWM_canal1 = 1;
        } else if (elapsedTimePWM_canal1 <= miliTiempoOnDesinflado && enPWM_canal1 == 1) {
          digitalWrite(solenoide1, LOW);
        } else if (elapsedTimePWM_canal1 > miliTiempoOnDesinflado && enPWM_canal1 == 1) {
          digitalWrite(solenoide1, HIGH);
          oldTimePWM_canal1 = currentTime;
          enPWM_canal1 = 0;
        }
      }
      // logica para canal 2.
      if (canal2 == 1 && (frecuenciaInfladoPWM == 100 || DCInflado == 100)) {
        if (presion2 < presionMin2_escalada) {
          digitalWrite(bomba2, HIGH);
        } else if (presion2 > presionMax2_escalada) {
          digitalWrite(bomba2, LOW);
        }
        if (presion3 < presionMin3_escalada) {
          digitalWrite(bomba3, HIGH);
        } else if (presion3 > presionMax3_escalada) {
          digitalWrite(bomba3, LOW);
        }
      } else if (canal2 == 1 && frecuenciaInfladoPWM != 100) {  // entra al PWM de inflado.
        if (elapsedTimePWM_canal2 > miliTiempoOffInflado && enPWM_canal2 == 0) {
          if (presion2 < presionMin2_escalada) {
            digitalWrite(bomba2, HIGH);
          } else if (presion2 > presionMax2_escalada) {
            digitalWrite(bomba2, LOW);
          }
          if (presion3 < presionMin3_escalada) {
            digitalWrite(bomba3, HIGH);
          } else if (presion3 > presionMax3_escalada) {
            digitalWrite(bomba3, LOW);
          }
          oldTimePWM_canal2 = currentTime;
          enPWM_canal2 = 1;
        } else if (elapsedTimePWM_canal2 > miliTiempoOnInflado && enPWM_canal2 == 1) {
          digitalWrite(bomba2, LOW);
          digitalWrite(bomba3, LOW);
          oldTimePWM_canal2 = currentTime;
          enPWM_canal2 = 0;
        }
      } else if (canal2 == 0 && (velocidadDesinfladoManual == 100 || DCDesinflado == 100)) {
        digitalWrite(solenoide2, HIGH);
        digitalWrite(solenoide3, HIGH);
      } else if (canal2 == 0 && frecuenciaDesinfladoPWM != 100) {  // entra al PMW de desinflado.
        if (elapsedTimePWM_canal2 > miliTiempoOffDesinflado && enPWM_canal2 == 0) {
          // señal de encendido a los solenoides
          oldTimePWM_canal2 = currentTime;
          enPWM_canal2 = 1;
        } else if (elapsedTimePWM_canal2 <= miliTiempoOnDesinflado && enPWM_canal2 == 1) {
          digitalWrite(solenoide2, HIGH);  // se abren los solenoides.
          digitalWrite(solenoide3, HIGH);
        } else if (elapsedTimePWM_canal2 > miliTiempoOnDesinflado && enPWM_canal2 == 1) {
          digitalWrite(solenoide2, LOW);
          digitalWrite(solenoide3, LOW);
          oldTimePWM_canal2 = currentTime;
          enPWM_canal2 = 0;
        }
      }
    }
  }
}

void to2(int ParMano[8], int ParMun[8], int RefMano[10], int RefMun[10]) {  // readIndex dicta todas las variables.
  Wire.requestFrom(9, 4);
  while (Wire.available()) {
    a = Wire.read();
    b = Wire.read();
    c = Wire.read();
    d = Wire.read();
    bigNum1 = a;
    bigNum1 = bigNum1 << 8 | b;
    bigNum2 = c;
    bigNum2 = bigNum2 << 8 | d;
  }
  readingsEMG1[readIndex] = bigNum1;
  readingsEMG2[readIndex] = bigNum2;
  if (readIndex == 99) {  // caso donde readIndex llega a la ultima posicion.
    // Suma.
    suma1 += readingsEMG1[readIndex] - readingsEMG1[0];
    suma2 += readingsEMG2[readIndex] - readingsEMG2[0];
    // calculo de promedios, paso intermedio.
    promedio1 = suma1 / 100.0;
    promedio2 = suma2 / 100.0;
    promedios1[readIndex] = promedio1;
    promedios2[readIndex] = promedio2;
    // Diff.
    dif += (readingsEMG1[readIndex] - readingsEMG2[readIndex]) - (readingsEMG1[0] - readingsEMG2[0]);
    // STD.
    sumaCuadradosSTD1 += pow(readingsEMG1[readIndex] - promedio1, 2) - pow(readingsEMG1[0] - promedios1[0], 2);
    sumaCuadradosSTD2 += pow(readingsEMG2[readIndex] - promedio2, 2) - pow(readingsEMG2[0] - promedios2[0], 2);
    // RMS.
    sumaCuadradosRMS1 += pow(readingsEMG1[readIndex], 2) - pow(readingsEMG1[0], 2);
    sumaCuadradosRMS2 += pow(readingsEMG2[readIndex], 2) - pow(readingsEMG2[0], 2);
  } else {
    // Suma.
    suma1 += readingsEMG1[readIndex] - readingsEMG1[readIndex + 1];
    suma2 += readingsEMG2[readIndex] - readingsEMG2[readIndex + 1];
    // calculo de promedios, paso intermedio.
    promedio1 = suma1 / 100.0;
    promedio2 = suma2 / 100.0;
    promedios1[readIndex] = promedio1;
    promedios2[readIndex] = promedio2;
    // Diff.
    dif += (readingsEMG1[readIndex] - readingsEMG2[readIndex]) - (readingsEMG1[readIndex + 1] - readingsEMG2[readIndex + 1]);
    // STD.
    sumaCuadradosSTD1 += pow(readingsEMG1[readIndex] - promedio1, 2) - pow(readingsEMG1[readIndex + 1] - promedios1[readIndex + 1], 2);
    sumaCuadradosSTD2 += pow(readingsEMG2[readIndex] - promedio2, 2) - pow(readingsEMG2[readIndex + 1] - promedios2[readIndex + 1], 2);
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

void to1(int ParMunSimple[5], int RefMunSimple[4]) {  // readIndex dicta todas las variables.
  Wire.requestFrom(9, 4);
  while (Wire.available()) {
    a = Wire.read();
    b = Wire.read();
    c = Wire.read();
    d = Wire.read();
    bigNum1 = a;
    bigNum1 = bigNum1 << 8 | b;
  }
  readingsEMG1[readIndex] = bigNum1;
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