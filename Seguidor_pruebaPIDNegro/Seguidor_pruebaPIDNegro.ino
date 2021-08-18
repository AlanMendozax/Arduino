
#include <QTRSensors.h>

#include<Servo.h>
 
Servo ESC; 
  //entradas
#define btn1      10
#define led_on    13   //~
   //~
#define mi1        8
#define mi2        7
#define pwmi       6
#define md1        3
#define md2        4
#define pwmd       5
#define stby       9
  
  

#define NUM_SENSORS             8  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  1 // average 4 analog samples per sensor reading
#define EMITTER_PIN             13   // emitter is controlled by digital pin 2

QTRSensorsAnalog qtra((unsigned char[]) {7, 6, 5, 4, 3, 2,1,0},NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
int proporcional = 0;
int derivativo = 0;
int integral = 0;
int salida_pwm = 0;
int proporcional_pasado = 0;
int position = 0;

////////////////////Parametros PID/////////////////////////////////
int velocidad = 100;
//float KP = 0.15, KD = 2.2, KI = 0.006; //reaccion rapida//KP=0.15, KD=2.2, KI=0.006
float KP=0.01, KD=0.15 , KI=0.0001; //reaccion rapida

int tur = 0;
int l = 90;
int r = -100;
///////////////////////////////////////////////////////

/////////////////parametros de sensado/////////////////////////////
int linea = 0; //0 linea negra, 1 para linea blanca
int flanco_color = 0 ;
int en_linea =  500 ;
int ruido = 30;
/////////////////////////////////////////////////////////
int boton1 = 7;
int boton2 = 7;

void setup()
{
 
    pinMode(led_on, OUTPUT);
    pinMode(mi1, OUTPUT);
    pinMode(mi2, OUTPUT);
    pinMode(pwmi, OUTPUT);
    pinMode(md1, OUTPUT);
    pinMode(md2, OUTPUT);
    pinMode(pwmd, OUTPUT);
pinMode(stby, OUTPUT);


 while(true)
   {
      botones();
      if(boton2 == 1) 
      {
       Serial.println("Esperando pulsacion");
        break;
      }
   }

   // turn on Arduino's LED to indicate we are in calibration mode
  for (int i = 0; i < 200; i++)  // make the calibration take about 10 seconds
  {
    qtra.calibrate();       // reads all sensors 10 times at 2.5 ms per six sensors (i.e. ~25 ms per call)
  }
    // turn off Arduino's LED to indicate we are through with calibration
 Serial.begin(9600);
   //Serial.println();

   while(true)
   {
      botones();
      if(boton2 == 1) 
      {
       Serial.println("Esperando pulsacion");
        break;
      }
   }
  

//bt.println("Hello, world?");
ESC.attach(11);
  
  //Activar el ESC
  ESC.writeMicroseconds(1000); //1000 = 1ms

 delay(5000); //Esperar 5 segundos para hacer la activacion
   
 
  Serial.setTimeout(10);

}

void loop()
{
  valores();
  pid(linea,velocidad,KP,KI,KD);
  frenos_contorno(600);
 ESC.writeMicroseconds(tur);
 Serial.print("       ");
 Serial.print("Freno izquierdo:  ");
 Serial.print(l);
 Serial.print("   ");
 Serial.print("Freno derecho:  ");
 Serial.print(r);
 Serial.print("       ");
 Serial.println(position);
 delay(2);
}

void pid(int linea, int velocidad, float Kp, float Ki, float Kd)
{
   
   position = qtra.readLine(sensorValues, QTR_EMITTERS_ON, linea, flanco_color, en_linea, ruido);
  //0 linea negra, 1 para linea blanca
  
      //  Serial.println(position);
  proporcional = (position) - 3500; // set point es 3500, asi obtenemos el error
  integral = integral + proporcional_pasado; //obteniendo integral
  derivativo = (proporcional - proporcional_pasado); //obteniedo el derivativo
  int ITerm = integral * KI;
  if(ITerm >= 255) ITerm = 255;
  if(ITerm <= -255) ITerm= -255;
  
  salida_pwm =(proporcional * KP) + (derivativo * KD) + (ITerm);
  
  if (salida_pwm > velocidad)  salida_pwm = velocidad; //limitamos la salida de pwm
  if (salida_pwm  < -velocidad)  salida_pwm = -velocidad;
  
  if (salida_pwm < 0)
 {
    int der = velocidad - salida_pwm; //(+)
    int izq = velocidad + salida_pwm;  //(-)
    if(der >= 120)der = 120;
    if(izq <= 0)izq = 0;
    motores(izq, der);
 }
 if (salida_pwm > 0)
 {
  int der = velocidad - salida_pwm; //(-)
  int izq = velocidad + salida_pwm; //(+)
  
  if(izq >= 120) izq = 120;
  if(der <= 0) der = 0;
  motores(izq ,der );
 }

 proporcional_pasado = proporcional;  
}


void valores() {

  Serial.print("kp= "); Serial.print(KP); Serial.print("  ki= ");Serial.print(KI); Serial.print("  kd= ");Serial.print(KD);Serial.print("  ve= ");Serial.print(velocidad);
  if (Serial.available())
  {
    String    dato = Serial.readStringUntil('\n');
    //Serial.println(dato);
    //Serial.println("Dato Resivido");
    //Serial.println(dato.substring(3));
    if (dato.substring(0, 2) == "l=") {
      l = dato.substring(2).toInt();
     // Serial.print(KP);

    }

    if (dato.substring(0, 2) == "r=") {
      r = dato.substring(2).toInt();
     // Serial.print(KP);

    }

 if (dato.substring(0, 3) == "tu=") {
      tur = dato.substring(3).toInt();
     // Serial.print(KP);

    }

    if (dato.substring(0, 3) == "kp=") {
      KP = dato.substring(3).toFloat();
     // Serial.print(KP);

    }

    if (dato.substring(0, 3) == "kd=") {
      KD = dato.substring(3).toFloat();
     // Serial.print(KD);
    }
    if (dato.substring(0, 3) == "ki=") {
      KI = dato.substring(3).toFloat();
     // Serial.print(KI);
    }

    if (dato.substring(0, 3) == "ve=") {
      velocidad = dato.substring(3).toInt();
     // Serial.print(velocidad);
    }
  }

}



void frenos_contorno(int flanco_comparacion)
{
    if (position <= 10) //si se salio por la parte derecha de la linea
    {
      while(true)
      { 
       
        motores(l,r);
        qtra.read(sensorValues); //lectura en bruto de sensor
        if   (sensorValues[3] < flanco_comparacion || sensorValues[4] < flanco_comparacion || sensorValues[5] < flanco_comparacion || sensorValues[6] < flanco_comparacion || sensorValues[7] < flanco_comparacion)
        {
          break;
        }
        
      }
    }

    if (position >= 6990) //si se salio por la parte izquierda de la linea
    {
      while(true)
      {
        
        motores(r,l); 
        qtra.read(sensorValues);
        if   (sensorValues[4] < flanco_comparacion || sensorValues[3] < flanco_comparacion || sensorValues[2] < flanco_comparacion || sensorValues[1] < flanco_comparacion || sensorValues[0] < flanco_comparacion)
    {
          break;
        }
      }
  }
 
}


void motores(int motor_izq, int motor_der)
{
  digitalWrite(stby, HIGH);
  if ( motor_izq >= 0 )  
  {
    digitalWrite(mi1, LOW);
    digitalWrite(mi2, HIGH); 
    analogWrite(pwmi, motor_izq); 
  }
  else
  {
    digitalWrite(mi1, HIGH); 
    digitalWrite(mi2, LOW);
    motor_izq = motor_izq * (-1); 
    analogWrite(pwmi, motor_izq);
  }

  if ( motor_der >= 0 ) //motor derecho
  {
    digitalWrite(md1, LOW);
    digitalWrite(md2, HIGH);
    analogWrite(pwmd, motor_der);
  }
  else
  {
    digitalWrite(md1, HIGH);
    digitalWrite(md2, LOW);
    motor_der = motor_der * (-1);
    analogWrite(pwmd, motor_der);
  }
}

void botones()
{
   
    boton2 = digitalRead(btn1); ///boton izquierdo, derecho
}
