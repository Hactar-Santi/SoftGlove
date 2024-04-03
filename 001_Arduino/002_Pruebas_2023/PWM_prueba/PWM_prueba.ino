#define SENSOR_PIN 0
#define PWM_Bomba 8
#define PWM_Sole 3


float sensorValue;
int cont;
float lecturaPresion;
float presion;


unsigned long currentTime;
unsigned long firstTime;

int escrituraPWM1 = 0;


void setup() {
  // Set up serial port
  pinMode(5, OUTPUT);
  Serial.begin(57600);
  currentTime = millis();
  firstTime = currentTime;
  pinMode(PWM_Bomba, OUTPUT);
  pinMode(PWM_Sole, OUTPUT);
  digitalWrite(PWM_Bomba, LOW);
  digitalWrite(PWM_Sole, LOW);
}

void loop() {
  currentTime = millis();
  sensorValue = analogRead(SENSOR_PIN);
  lecturaPresion = 5 * (sensorValue / 1023);
  // con este ajuste se tiene un error de 1 Kpa. Puede que se trabaje en una zona no lineal, se podria mejorar.
  presion = (((lecturaPresion / 5) + 0.00842) / 0.002421) - 100;

  if (currentTime - firstTime < 13000) {
    //si es menor a lim inf infla
    if (presion < 30) {
      digitalWrite(PWM_Bomba, HIGH);
      digitalWrite(PWM_Sole, LOW);

    //si es mayor a lim sup no infles mas
    } else if (presion > 40) {
      digitalWrite(PWM_Bomba, LOW);
      digitalWrite(PWM_Sole, LOW);
    }
  } else {
    //si supera el tiempo liberame el aire pana
    digitalWrite(PWM_Bomba, LOW);
    digitalWrite(PWM_Sole, HIGH);
  }




  if (currentTime - firstTime < 15000) {
    //graficame neneeee
    Serial.print(0); // To freeze the lower limit
    Serial.print(" ");
    Serial.print(100); // To freeze the upper limit
    Serial.print(" ");
    Serial.println(presion);
  }
}
