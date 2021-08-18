/* 
Programa: Sensor de Proximidad con Señal Acustica por Ultrasonidos
Autor:	Humberto Higinio
Web: www.humbertohiginio.com
Canal de Youtube: https://www.youtube.com/user/HHSolis
Video Exclusivo para mi canal de Youtube
Todos los Derechos Reservados - 2015
Código de Dominio Público
 */

#include "Ultrasonic.h"     //Libreria del sonar
Ultrasonic ultrasonic(6,7); //Trigger, Echo

// Señal acustica
#include "pitches.h"        //Libreria que contiene las notas musicales
int sonido = NOTE_E5;       //Declaramos la nota musical elegida como el sonido

int sonar;                  //Declaramos la variable sonar
long dis;
long tiem;

void setup()
{
   Serial.begin(9600); //Inicio de la comunicacion serie a 9600 baudios
  pinMode(6, OUTPUT); //salida del pulso generado por el sensor ultrasónico
  pinMode(7, INPUT);//entrada del pulso generado por el sensor ultrasónico
  pinMode(11, INPUT);  //alarma de la distancia(encenderá el led)   
}

void loop()
{

   
   sonar = ultrasonic.Ranging(CM); //Leemos la distancia del sonar  
                                   
                                   //La funcion ultrasonic.ranging(cm) viene declarada en la libreria del sonar
                                   //Calcula la distancia a la que rebota una señal enviada basandose en el 
                                   //tiempo que tarda en recorrer dicha distancia, devolviendonos la distancia
                                   //en centimetros, lista para utilizar en casos de medicion por ultrasonidos.
                                    
   while (sonar < 60)             //Mientras que la distancia sea menor a 60 cm
   {   
       noTone(8);                  //Mantenemos el sonido apagado
       
       delay(sonar*10);            //Delay dependiente del valor del sonar. Si la distancia se reduce
                                   //el delay es menor y la señal acustica sonara con mayor frecuencia.
                                   //Si la distancia aumenta, el delay aumenta, disminuyendo la frecuencia
                                   //con la que suenan los pitidos.
       
       tone(8, sonido);            //Señal acustica de aviso 
       
       delay(100);                 //Delay para mantener la señal acustica 0,1 segundos minimo
       
       noTone(8);                  //Apagamos el sonido
       //digitalWrite(6,LOW);//recibimiento del pulso.
       //delayMicroseconds(5);
       //digitalWrite(6, HIGH);//envió del pulso.
       //delayMicroseconds(10);
       //tiem=pulseIn(7, HIGH);//fórmula para medir el pulso entrante.
       //dis= long(0.017*tiem);//fórmula para calcular la distancia del sensor ultrasónico.
       //if(dis>10)//comparativo para la alarma se ingresa la distancia en la que encenderá o apagara.
       //{ 
        // digitalWrite(11, HIGH);
       //}
       //else
       //{
         //digitalWrite(11,LOW);
       //}
         //Serial.println("LA DISTANCIA MEDIDA ES:");
         //Serial.println(dis);
         //Serial.println("cm");
         //delay(500);
    
       while (sonar < 10)          //Si la distancia del sonar es menor que 10 cm
       {
         tone(8, sonido);                //Suena sin interrupciones indicando la proximidad del objeto
         sonar = ultrasonic.Ranging(CM); //Distancia del sonar  
         
       //digitalWrite(6,LOW);//recibimiento del pulso.
       //delayMicroseconds(5);
       //digitalWrite(6, HIGH);//envió del pulso.
       //delayMicroseconds(10);
       //tiem=pulseIn(7, HIGH);//fórmula para medir el pulso entrante.
       //dis= long(0.017*tiem);//fórmula para calcular la distancia del sensor ultrasónico.
       //if(dis>10)//comparativo para la alarma se ingresa la distancia en la que encenderá o apagara.
       //{ 
         //digitalWrite(11, HIGH);
       //}
       //else
       //{
         //digitalWrite(11,LOW);
       //}
         //Serial.println("LA DISTANCIA MEDIDA ES:");
         //Serial.println(dis);
         //Serial.println("cm");
         //delay(500);
       }  
       
       sonar = ultrasonic.Ranging(CM); //Leemos la distancia del sonar para volver a empezar
   }
}

