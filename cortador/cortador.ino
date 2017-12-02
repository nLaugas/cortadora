#include <HX711.h>
#include <ArduinoSTL.h>
#include <TM1637Display.h>

#define BTN_POWER 2
#define FIN_CARRERA 3
#define DIR_PIN 8
#define STEP_PIN 9
#define ENABLE 7

#define HX711_DT A1
#define HX711_CLK A0

#define TM1637_CLK 5
#define TM1637_DIO 6

int state = 0;
int sentido;
int stepDelay = 700;
bool b;
unsigned long tiempo;
HX711 scale(HX711_DT, HX711_CLK);
TM1637Display display(TM1637_CLK, TM1637_DIO);

double measure;

void motorAction(int sentido){
  int a;
  if(sentido != 0){
    if (sentido == 1)
    {
      a=0;
    }
    else{
      a=1; 
    }
    b=!b;
    digitalWrite(DIR_PIN, a);
    digitalWrite(STEP_PIN, b);
    delayMicroseconds(stepDelay);
  }
}

std::vector<void (*)()>  states = {
  []() { // Estado 0 - Detenido
    if(digitalRead(BTN_POWER)== LOW){
      state = 1;
      tiempo = millis();
      digitalWrite(ENABLE,LOW);
      scale.power_up();
      scale.tare();
    }
    motorAction(0);
  },
  []() { // Estado 1 - Bajando
    if((digitalRead(BTN_POWER) == LOW) && ( (millis() - tiempo) > 1000) ){
      state = 3;
    }
    motorAction(-1);
    double m = scale.get_value();
    Serial.print("Measured: "); serial.println(m);
    if(m > 100){
      state = 2;
    }
  }, 
  []() { // Estado 2 - Cortando
  
  },
  []() { // Estado 3 - Subiendo
    if(digitalRead(FIN_CARRERA) == LOW){
      state = 0;
      digitalWrite(ENABLE,HIGH);
      scale.power_down();
    }     
    motorAction(1);
  }
};

int c = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BTN_POWER,INPUT_PULLUP);
  pinMode(FIN_CARRERA,INPUT_PULLUP);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(ENABLE, OUTPUT);  
  digitalWrite(ENABLE,HIGH);
  scale.power_down();
  display.setBrightness(0x0f);
}

void loop() {
     states[state]();
     display.showNumberDec(measure, false);
}
