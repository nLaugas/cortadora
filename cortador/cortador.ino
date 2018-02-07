#include <HX711.h>
#include <ArduinoSTL.h>
#include <TM1637Display.h>
#include <TimerOne.h>

#define SCALE_SLEEP

#define FIN_CARRERA_ALTO 4
#define FIN_CARRERA_BAJO 3
#define BTN_POWER 2

#define DIR_PIN 8
#define STEP_PIN 9
#define ENABLE 7

#define HX711_DT A1
#define HX711_CLK A0

#define TM1637_CLK 6
#define TM1637_DIO 5

#define MS0 13
#define MS1 12
#define MS2 11

#define INICIO_CORTE 60
#define FIN_CORTE 40

volatile int state = 3;
unsigned long tiempo;

HX711 scale(HX711_DT, HX711_CLK);
TM1637Display display(TM1637_CLK, TM1637_DIO);

double measure;

std::vector<void (*)()>  states = {
  []() { // Estado 0 - DETENIDO
    if(digitalRead(BTN_POWER)== LOW){
      state = 1;
      tiempo = millis();
      digitalWrite(ENABLE,LOW);
      digitalWrite(DIR_PIN, 0);
      measure = 0;
      scale.power_up();
    }
  },
  []() { // Estado 1 - BAJANDO
    if ((digitalRead(BTN_POWER) == LOW && millis() - tiempo > 1000)){
      digitalWrite(DIR_PIN, 1);
      state = 3;
      
    }
    measure = scale.get_units(5);
    if(measure > INICIO_CORTE){
      state = 2;
    }
  }, 
  []() { // Estado 2 - CORTANDO
    double m = scale.get_units(10);
    if((digitalRead(BTN_POWER) == LOW && millis() - tiempo > 1000) || m < FIN_CORTE){
      state = 3;
      digitalWrite(DIR_PIN, 1);
    }else if(measure < m){
      measure = m;
    }
  },
  []() { // Estado 3 - SUBIENDO
    if(digitalRead(FIN_CARRERA_ALTO) == LOW){
      state = 0;
      digitalWrite(ENABLE,HIGH);
#ifdef SCALE_SLEEP
      scale.power_down();
#endif
    }     
  }
};



void setup() {
  
  pinMode(MS0,OUTPUT);
  pinMode(MS1,OUTPUT);
  pinMode(MS2,OUTPUT);
  digitalWrite(MS0,HIGH);
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  
  pinMode(ENABLE, OUTPUT);
  
  Serial.begin(9600);
  pinMode(BTN_POWER,INPUT_PULLUP);
  
  pinMode(FIN_CARRERA_ALTO,INPUT_PULLUP);
  pinMode(FIN_CARRERA_BAJO,INPUT_PULLUP);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  digitalWrite(ENABLE,LOW);
  
  attachInterrupt( digitalPinToInterrupt(FIN_CARRERA_BAJO), ServicioBoton, FALLING);
  
  Timer1.initialize(900);
  Timer1.pwm(STEP_PIN, 512);
  
  
  scale.set_scale();
  scale.tare();
  //scale.set_scale(222515);
  scale.set_scale(227);
  scale.tare();
#ifdef SCALE_SLEEP
      scale.power_down();
#endif
  display.setBrightness(0x0f);
  measure = 0;
}

void loop() {
     states[state]();
     static int next_change = 0; int t = millis();
     if(t > next_change){
        if(state > 0){
          Serial.print("Measured: "); Serial.println(measure);
          Serial.print("State: "); Serial.println(state);
        }
        if(measure > 0)
          display.showNumberDec((int)measure % 10000, false);
        else
          display.showNumberDec(0, false);
        next_change = t + 1000;
        if(Serial.available())
        {
          scale.set_scale(Serial.parseInt());
        }
     }
}

void ServicioBoton (){
      digitalWrite(DIR_PIN,1);
      state = 3;
}

