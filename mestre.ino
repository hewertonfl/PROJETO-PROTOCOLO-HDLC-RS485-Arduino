int data[] = { 1, 0, 0, 1, 0, 1, 1, 1, 0 };
int arrayLenght = sizeof(data) / sizeof(data[0]);
int outputPin = 23;

void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  sender(data, arrayLenght);
}
// Remetente
void sender(int data[], int arrayLenght) {
  for (int i = 0; i < arrayLenght; i++) {
    digitalWrite(outputPin, data[i]);
    delay(1000);
  }
}

void loop() {
  sender(data, arrayLenght);
}
