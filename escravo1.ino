int inputPin = 3;
int outputPin = 5;
int flag[8] = { 0, 1, 1, 1, 1, 1, 1, 0 };
int flagLength = sizeof(flag) / sizeof(flag[0]);
bool openFlag = true;
int auxRead[] = {};
int receivedData[] = {};
uint64_t receiverCounter = 0;
uint64_t auxCounter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

void fillArray(int i) {
  if (i > 7) {
    Serial.println();
  } else {
    auxRead[i] = digitalRead(inputPin);
    // Serial.print(auxRead[i]);
    // Serial.print(",");
  }
}

bool sync(int flag[], int auxRead[]) {
  for (int i = 0; i < flagLength; i++) {
    if (auxRead[i] != flag[i]) {
      return false;
    }
  }
  return true;
}

void receiver(int receiverCounter, int auxCounter) {
  fillArray(auxCounter);
  Serial.println(auxCounter);
  bool syncFlag = syncFlag ? true : sync(flag, auxRead);

  if (syncFlag) {
    receivedData[receiverCounter] = digitalRead(inputPin);
    receiverCounter++;
  }
}


void loop() {
  //int leitura = digitalRead(inputPin);
  //digitalWrite(outputPin, leitura);
  receiver(receiverCounter, auxCounter);
  delay(1000);
  //Serial.println(auxCounter);
  auxCounter = auxCounter>7 ? 0 : auxCounter+1;
}
