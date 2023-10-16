// Definição dos pinos
const int inputPin = 2;
const int outputPin = 8;

// Definição do vetor de flag
int flag[8] = { 0, 1, 1, 1, 1, 1, 1, 0 };
int flagLength = sizeof(flag) / sizeof(flag[0]);
bool openFlag = true;
bool syncFlag = false;

// Definição do vetor de dados auxiliar de comparação
int auxRead[8];

// Definição do vetor de dados recebidos
int receivedData[8];

// Contador auxiliar
int auxCounter = 0;

// Tempo de transmissão de bit
const int timeClock = 100;

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
void printReceivedData() {
  for (int i = 0; i < 8; i++) {
    Serial.print(receivedData[i]);
  }
  Serial.println();
}

// Função de recebimento de dados
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

// Loop principal
void loop() {
  int leitura = digitalRead(inputPin);
  digitalWrite(outputPin, leitura);
  receiver();
}
