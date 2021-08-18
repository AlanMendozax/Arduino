void setup() {
  // put your setup code here, to run once:
pinMode(9,OUTPUT);
pinMode(10,OUTPUT);
pinMode(8,OUTPUT);

pinMode(5,OUTPUT);
pinMode(6,OUTPUT);
pinMode(4,OUTPUT);
}

void motores (int vm1, int vm2)
{
digitalWrite(9,HIGH);
digitalWrite(10,LOW);
analogWrite(8,vm1);
//delay(3000);

digitalWrite(5,HIGH);
digitalWrite(4,LOW);
analogWrite(6,vm2);
delay(3000);
}


void loop() {
  // put your main code here, to run repeatedly:
motores (255,255);
motores (192,192);
motores (128,128);
}
