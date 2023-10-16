int inputPin = 2;
int outputPin = 8;
int flag[8] = { 0, 1, 1, 1, 1, 1, 1, 0 };
int flagLength = sizeof(flag) / sizeof(flag[0]);
bool openFlag = true;
bool syncFlag = false;
int auxRead[8];
int receivedData[8];
int auxCounter = 0;
int timeClock = 100;
int syncCounter = 0;
int counter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

void fillArray() {
  if (auxCounter >= flagLength) {
    syncFlag = syncFlag ? true : sync(flag, auxRead);
    //Serial.println();
    auxCounter = 0;
  } else {
    auxRead[auxCounter] = digitalRead(inputPin);
    //Serial.print(auxRead[auxCounter]);
    //Serial.print(",");
    auxCounter++;
    delay(timeClock);
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
  if (!syncFlag){
    fillArray();
    }
  if (syncFlag) {
      receivedData[counter] = digitalRead(inputPin);
      if(counter<8){
        counter=counter + 1;
      } else{
        printReceivedData();
        counter = 0;
      }
      delay(timeClock);
   // }
  }
}

void loop() {
  int leitura = digitalRead(inputPin);
  digitalWrite(outputPin, leitura);
  receiver();
}
