// Definição dos pinos
const int inputPin = 2;
const int outputPin = 8;

// Definição do vetor de flag
int flag[8] = { 0, 1, 1, 1, 1, 1, 1, 0 };
int flagLength = sizeof(flag) / sizeof(flag[0]);
bool stopFlag = false;

// Definição do adress
const int adressLength = 8;
int adress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 0 };

// Definição da flag de sincronização
bool syncFlag = false;

// Definição do vetor de dados recebidos
const int receivedDataLength = 16;
int receivedData[receivedDataLength];

// Contador auxiliar
int auxCounter = 0;

// Tempo de recepção de bit
const int timeClock = 10;

// Contagem de sincronização
int syncCounter = 0;
int counter = 0;

// Configuração inicial do slave
void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

// Função de preenchimento do vetor auxiliar
void fillArray() {
  if (auxCounter >= flagLength) {
    syncFlag = syncFlag ? true : sync(flag, receivedData);
    Serial.println();
    auxCounter = 0;
  } else {
    receivedData[auxCounter] = digitalRead(inputPin);
    Serial.print(receivedData[auxCounter]);
    Serial.print(",");
    auxCounter++;
    delay(timeClock);
  }
}

// Função de sincronização
bool sync(int flag[], int auxRead[]) {
  for (int i = 0; i < flagLength; i++) {
    if (auxRead[i] != flag[i]) {
      return false;
    }
  }
  return true;
}

// Função de impressão dos dados recebidos
void printReceivedData(int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    Serial.print(receivedData[i]);
  }
  Serial.println();
}

// Função de checagem de endereço
bool checkAdress(int adress[], int receivedData[]) {
  for (int i = 0; i < adressLength; i++) {
    if (adress[i] != receivedData[i]) {
      return false;
    }
  }
  return true;
}

// Função de preenchimento do vetor de dados
void fillReceivedData(int receivedDataLength) {
  int counter = 0;
  while (counter < receivedDataLength) {
    receivedData[counter] = digitalRead(inputPin);
    counter++;
    delay(timeClock);
  }
}

// Função de recebimento de dados
void receiver() {
  if (!syncFlag) {
    fillArray();
  }
  if (!stopFlag) {
    if (syncFlag) {
      Serial.print("OpenFlag: ");
      printReceivedData(8);
      fillReceivedData(8);
      stopFlag = checkAdress(adress, receivedData);
      if (stopFlag) {
        Serial.print("Adress: ");
        printReceivedData(8);
        Serial.print("Control: ");
        fillReceivedData(8);
        printReceivedData(8);
        fillReceivedData(receivedDataLength);
        Serial.print("Data received: ");
        printReceivedData(receivedDataLength);
        fillReceivedData(8);
        Serial.print("CloseFlag: ");
        printReceivedData(8);
      } else {
        Serial.println("A msg não é para mim");
      }
    }
  }
}

// Loop principal
void loop() {
  int leitura = digitalRead(inputPin);
  digitalWrite(outputPin, leitura);
  receiver();
}
