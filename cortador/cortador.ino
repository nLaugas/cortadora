  
#include <HX711.h>
#include <ArduinoSTL.h>
#include <TM1637Display.h>
#include <TimerOne.h>

#define SCALE_SLEEP

#define BTN_POWER 2
#define FIN_CARRERA_ALTO 4
#define FIN_CARRERA_BAJO 3

#define DIR_PIN 9
#define STEP_PIN 8
#define ENABLE 7

#define HX711_DT A1
#define HX711_CLK A0

#define TM1637_CLK 5
#define TM1637_DIO 6

#define INICIO_CORTE 6
#define FIN_CORTE 4

volatile int state = 0;
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
      scale.power_up();
    }
  },
  []() { // Estado 1 - BAJANDO
    if ((digitalRead(BTN_POWER) == LOW && millis() - tiempo > 1000)){
      digitalWrite(DIR_PIN, 1);
      state = 3;
      
    }
    measure = scale.get_units(10);
    if(measure > INICIO_CORTE){
      state = 2;
    }
  }, 
  []() { // Estado 2 - CORTANDO
    double m = scale.get_units(10);
    if((digitalRead(BTN_POWER) == LOW && millis() - tiempo > 1000) || measure < FIN_CORTE){
      state = 3;
      digitalWrite(DIR_PIN, 1);
    }else{
      measure = m;
    }
  },
  []() { // Estado 3 - SUBIENDO
    if(digitalRead(FIN_CARRERA_ALTO) == LOW){
      state = 0;
      Serial.print(state);
      digitalWrite(ENABLE,HIGH);
#ifdef SCALE_SLEEP
      scale.power_down();
#endif
    }     
  }
};



void setup() {
  pinMode(ENABLE, OUTPUT);
  digitalWrite(ENABLE,HIGH);
  Serial.begin(9600);
  pinMode(BTN_POWER,INPUT_PULLUP);
  
  pinMode(FIN_CARRERA_ALTO,INPUT_PULLUP);
  pinMode(FIN_CARRERA_BAJO,INPUT_PULLUP);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  attachInterrupt( digitalPinToInterrupt(FIN_CARRERA_BAJO), ServicioBoton, FALLING);
  
  Timer1.initialize(900);
  Timer1.pwm(STEP_PIN, 512);
  
  scale.set_scale();
  scale.set_scale(22192);
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
        Serial.print("Measured: "); Serial.println(measure);
        Serial.print("State: "); Serial.println(state);
        display.showNumberDec((int)((int)measure % 1000), false);
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
