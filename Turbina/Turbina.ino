#include <Servo.h>
Servo esc;
void setup() {
  esc.attach(8);
  esc.writeMicroseconds(1000);
  Serial.begin(900);
}

void loop() {
  int val;
  val = analogRead(A0);
  val = map(val, 0, 1023, 1500, 2000);
  esc.writeMicroseconds(val);

}
