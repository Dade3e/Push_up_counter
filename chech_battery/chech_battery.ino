#define CHECKBAT 0
#define BATEN 1
#define BAT0 digitalWrite(BATEN, LOW)
#define BAT1 digitalWrite(BATEN, HIGH)
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  pinMode(BATEN, OUTPUT);
  pinMode(CHECKBAT, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  BAT1;
  delay(20);
  int sensorValue = analogRead(CHECKBAT);
  BAT0;
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.print(" ; tensione: ");
  int tens = (sensorValue-2950) /50; //da 1 a 10
  Serial.println(tens);
  delay(100);  // delay in between reads for stability
}