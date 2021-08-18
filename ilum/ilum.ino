const int LDR=A0;
int V;
int ilum;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
V= analogRead(LDR);

ilum = ((V/1023)*100);
Serial.print(ilum);
delay(1000);
}
