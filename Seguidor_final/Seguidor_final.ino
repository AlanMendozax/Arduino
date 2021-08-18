#include <QTRSensors.h>
#define NUM_SENSORS   8  //numero de sensores usados
#define TIMEOUT       2500  // tiempo de espera para dar resultado en uS
#define EMITTER_PIN   6    //pin led on
///////////////pines arduino a utilizar/////////////////////
#define led1          13 
#define led2          4  
#define mot_i         7
#define mot_d         8
#define sensores      6
#define boton_1       2  //pin para boton
#define pin_pwm_i     9
#define pin_pwm_d     10

QTRSensorsRC qtrrc((unsigned char[]) {19, 18, 17, 16,15,14,11,12}
,NUM_SENSORS, TIMEOUT, EMITTER_PIN);

//variables para almacenar valores de sensores y posicion
unsigned int sensorValues[NUM_SENSORS];
unsigned int position=0;

/// variables para el pid
int  derivativo=0, proporcional=0, integral=0; //parametros
int  salida_pwm=0, proporcional_pasado=0;


//_______AQUI CAMBIEREMOS LOS PARAMETROS DE NUESTRO ROBOT_________________
int velocidad=200; //variable para la velocidad, el maximo es 255
float Kp=0.18;
float Kd=2;
float Ki=0.01;  //constantes
//variables para el control del sensado
int linea=0;                //  0 para lineas negra, 1 para lineas blancas
int flanco_color =0;      // aumenta o disminuye el valor del sensado
int en_linea=500;         //valor al que considerara si el sensor esta en linea o no
int ruido=  50;          //valor al cual el valor del sensor es considerado como ruido
//________________________________________________________________________________

void setup()
{
  delay(800);
 pinMode(mot_i, OUTPUT);//pin de direccion motor izquierdo
 pinMode(mot_d, OUTPUT);//pin de direccion motor derecho
 pinMode(led1, OUTPUT); //led1
 pinMode(led2, OUTPUT); //led2
        pinMode(boton_1, INPUT); //boton 1 como pull up
        
        for (int i = 0; i < 50; i++)  //calibracion durante 2.5 segundos,
 {                                 //para calibrar es necesario colocar los sensores sobre la superficie negra y luego 
  digitalWrite(led1, HIGH);  //la blanca
                delay(20);
  qtrrc.calibrate();    //funcion para calibrar sensores   
  digitalWrite(led1, LOW);  
                delay(20);
 }
        digitalWrite(led1, LOW); //apagar sensores para indicar fin 
                                  //de calibracion 
        delay(400); 
        digitalWrite(led2,HIGH); //encender led 2 para indicar la
                                 // espera  de pulsacion de boton
       
        while(true)
        {
          int x=digitalRead(boton_1); //leemos y guardamos el valor
                                      // del boton en variable x
                                      delay(100);
          if(x==0) //si se presiona boton 
          {
             digitalWrite(led2,LOW); //indicamos que se presiono boton
             digitalWrite(led1,HIGH); //encendiendo led 1
             delay(100);
             break; //saltamos hacia el bucle principal
          }
        }
}       



void loop()
{

                    
  pid(linea, velocidad, Kp, Ki, Kd, flanco_color, en_linea, ruido); //funcion para algoritmo pid(modificado )
    //(tipo_linea,velocidad,kp,ki,kd,flanco_color,en_linea,ruido)

         
  frenos_contorno(linea,700); //funcion para frenado en curvas tipo 
                  //flanco de comparación va desde 0 hasta 1000 , esto para ver 
                 //si esta en negro o blanco
}


////////funciones para el control del robot////

//aqui esta modificado la funcion del pid para que reciba los nuevos parametros  para la libreria modificada
 void pid(int linea, int velocidad, float Kp, float Ki, float Kd,int flanco_color, int en_linea,int ruido)
{
  position = qtrrc.readLine(sensorValues, QTR_EMITTERS_ON, linea,flanco_color, en_linea, ruido ); //0 para linea 
                                                //negra, 1 para linea blanca
  proporcional = (position) - 3500; // set point es 3500, asi obtenemos el error
  integral=integral + proporcional_pasado; //obteniendo integral
  derivativo = (proporcional - proporcional_pasado); //obteniedo el derivativo
  if (integral>1000) integral=1000; //limitamos la integral para no causar problemas
  if (integral<-1000) integral=-1000;
  salida_pwm =( proporcional * Kp ) + ( derivativo * Kd )+(integral*Ki);
  
  if (  salida_pwm > velocidad )  salida_pwm = velocidad; //limitamos la salida de pwm
  if ( salida_pwm < -velocidad )  salida_pwm = -velocidad;
  
  if (salida_pwm < 0)
 {
  motores(velocidad+salida_pwm, velocidad);
 }
 if (salida_pwm >0)
 {
  motores(velocidad, velocidad-salida_pwm);
 }

 proporcional_pasado = proporcional;  
}

void motores(int motor_izq, int motor_der)
{
  
  if ( motor_izq >= 0 )  //motor izquierdo
 {
  digitalWrite(mot_i,HIGH); // con high avanza
  analogWrite(pin_pwm_i,255-motor_izq); //se controla de manera
                                                 //inversa para mayor control
 }
 else
 {
  digitalWrite(mot_i,LOW); //con low retrocede
  motor_izq = motor_izq*(-1); //cambio de signo
  analogWrite(pin_pwm_i,motor_izq); 
 }


  if ( motor_der >= 0 ) //motor derecho
 {
  digitalWrite(mot_d,HIGH);
  analogWrite(pin_pwm_d,255-motor_der);
 }
 else
 {
  digitalWrite(mot_d,LOW);
  motor_der= motor_der*(-1);
  analogWrite(pin_pwm_d,motor_der);
 }

  
}

void frenos_contorno(int tipo,int flanco_comparacion)
{
  
if(tipo==0)
{
  if (position<=500) //si se salio por la parte derecha de la linea
 {
  motores(-80,90); //debido a la inercia, el motor 
                                  //tendera a seguri girando
                                  //por eso le damos para atras , para que frene
                                 // lo mas rapido posible 
  while(true)  
  {
   qtrrc.read(sensorValues); //lectura en bruto de sensor   
if ( sensorValues[0]>flanco_comparacion || sensorValues[1]>flanco_comparacion ) 
//asegurar que esta en linea
   {
    break;
   } 
  }
 }

 if (position>=6500) //si se salio por la parte izquierda de la linea
 { 
  motores(90,-80);
  while(true)
  {
   qtrrc.read(sensorValues);
if (sensorValues[7]>flanco_comparacion || sensorValues[6]>flanco_comparacion )
   {
    break;
   }  
  }
 }
}

//*******
if(tipo==1) //para linea blanca con fondo negro
{
  if (position<=500) //si se salio por la parte derecha de la linea
 {
  motores(-80,90); //debido a la inercia, el motor 
                                  //tendera a seguri girando
                                  //por eso le damos para atras , 
                                  //para que frene lo mas rapido posible 
  while(true)  
  {
   qtrrc.read(sensorValues); //lectura en bruto de sensor
if ( sensorValues[0]<flanco_comparacion || sensorValues[1]<flanco_comparacion )
//asegurar que esta en linea
   {
    break;
   }
   
  }
 }

 if (position>=6500) //si se salio por la parte izquierda de la linea
 { 
  motores(90,-80);
  while(true)
  {
   qtrrc.read(sensorValues);
  if (sensorValues[7]<flanco_comparacion || sensorValues[6]<flanco_comparacion)
   {
    break;
   }  
  }
 }
}

}



