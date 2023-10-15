int inputPin = 2;
int outputPin = 8;
int flag[8] = { 0, 1, 1, 1, 1, 1, 1, 0 };
int flagLength = sizeof(flag) / sizeof(flag[0]);
bool openFlag = true;
bool syncFlag = false;
int auxRead[8];
int receivedData[8];
int auxCounter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

void fillArray() {
  if (auxCounter >= 7) {
    syncFlag = syncFlag ? true : sync(flag, flag);
    // Serial.println();
    auxCounter = 0;
  } else {
    auxRead[auxCounter] = digitalRead(inputPin);
    //Serial.print(auxRead[auxCounter]);
    //Serial.print(",");
    auxCounter++;
    delay(10);
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
void printReceivedData() {
  for (int i = 0; i < 8; i++) {
    Serial.print(receivedData[i]);
  }
  Serial.println();
}


void receiver() {
  fillArray();
  if (syncFlag) {
    // Serial.println("Inicio sincronizado com sucesso!");
    if (auxCounter == 7) {
      Serial.println();

    } else {
      receivedData[auxCounter] = digitalRead(inputPin);
      Serial.print(receivedData[auxCounter]);

      delay(10);
    }
  }
}

void loop() {
  int leitura = digitalRead(inputPin);
  digitalWrite(outputPin, leitura);
  receiver();
}
