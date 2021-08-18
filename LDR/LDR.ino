const long A= 1.000;
const int B= 15;
const int Re= 10;
const int LDR=A0;

int V;
int ilum;
void setup() {
pinMode(motor, OUTPUT);
Serial.begin(9600);
}

void loop() {
 V=analogRead(LDR);
 ilum = ((V/1023)*100);
 Serial.print(ilum);
 delay(1000);
digitalWrite(motor, HIGH);
analogWrite(motor, 128);
}
