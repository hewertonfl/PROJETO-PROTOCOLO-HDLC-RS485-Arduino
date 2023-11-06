// ********************* Definição dos pinos ********************* //
const int inputPin = 3;
const int outputPin = 8;


// ****************** Definição de vetores de bits **************** //
// Definição do vetor de flag
const int flagLength = 8;
int flag[flagLength] = { 0, 1, 1, 1, 1, 1, 1, 0 };

// Definição do adress
const int adressLength = 8;
int adress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 1 };


// Definição do vetor de controle de dados
const int controlLength = 8;
int control[controlLength];

// Definição do vetor de CRC
const int crcLength = 8;
int crc[crcLength];

// Definição do vetor de dados recebidos
const int receivedDataLength = 16;
const uint16_t frameLength = flagLength + controlLength + receivedDataLength + adressLength;
uint16_t receivedData[frameLength];

// ************************** Contadores ***************************** //
// Contagem de sincronização
int counter = 0;

// ***************************** Flags ******************************* //
// Definição da flag de status
bool stopFlag = false;
// Definição da flag de sincronização
bool syncFlag = false;

// Tempo de recepção de bit
const int timeClock = 10;


// Função de sincronização
bool sync(int array[], int auxRead[], int length) {
  for (int i = 0; i < length; i++) {
    if (array[i] != auxRead[i]) {
      return false;
    }
  }
  return true;
}

// Função de comparação CRC
bool compare(int array[], int start) {
  for (int i = 0; i < 8; i++) {
    if (array[i] != receivedData[i + start]) {
      return false;
    }
  }
  return true;
}
// Remetente
void sender(int data[], int dataLength) {
  for (int i = 0; i < dataLength; i++) {
    digitalWrite(outputPin, data[i]);
    delay(timeClock);
  }
}

// Função de impressão dos dados recebidos
void printReceivedData(int array[], int start, int arrayLength) {
  for (int i = start; i < arrayLength; i++) {
    Serial.print(array[i]);
  }
  Serial.println();
}

// Função de preenchimento do vetor auxiliar
void fillFrame(int flagLength, int frameLength) {
  int counter = 0;
  int auxCounter = flagLength;
  while (!syncFlag) {
    receivedData[counter] = digitalRead(inputPin);
    Serial.print(receivedData[counter]);
    delay(timeClock);
    counter++;

    if (counter > flagLength) {
      syncFlag = syncFlag ? true : sync(flag, receivedData, flagLength);
      Serial.println();
      counter = 0;
    }
  }
  while (auxCounter < frameLength) {
    receivedData[auxCounter] = digitalRead(inputPin);
    delay(timeClock);
    auxCounter++;
  }
}
// Função de cálculo do checksum
uint16_t calculateChecksum(int data[], int start, int receivedDataLength) {
  uint16_t checksum = 0;
  Serial.print("Checksum: ");
  for (int i = start; i < receivedDataLength; i++) {
    checksum += data[i];
  }
  Serial.println(checksum);
  return checksum;
}

// Função de conversão do checksum para binário
void checksumToBinaryArray(uint16_t checksum, int crc[], int crcLength) {
  Serial.print("CRC: ");
  int counter = 0;
  for (int i = crcLength - 1; i >= 0; i--) {
    crc[counter] = (checksum >> i) & 0x01;
    Serial.print(crc[counter]);
    counter++;
  }
  Serial.println();
}

// Função de recebimento de dados
void receiver() {
  //if (!syncFlag) {
  fillFrame(flagLength, frameLength);
  Serial.print("OpenFlag: ");
  printReceivedData(receivedData, 0, flagLength);

  // Checa se a mensagem é para mim
  // bool adressFlag = compare(adress, flagLength);

  //    Serial.println(adressFlag);
  // if (adressFlag) {
  Serial.print("Adress: ");
  printReceivedData(receivedData, flagLength, flagLength + adressLength);
  Serial.print("Control: ");
  printReceivedData(receivedData, flagLength + adressLength, flagLength + adressLength + controlLength);

  // // Checagem de erros
  // int checksum = calculateChecksum(receivedData, flagLength + adressLength + controlLength + crcLength, frameLength - flagLength);
  // checksumToBinaryArray(checksum, crc, crcLength);
  // bool crcFlag = compare(crc, flagLength + adressLength + controlLength);
  // bool sendingStatus = false;

  // Serial.println(crcFlag ? "CRC checked succeed!" : "CRC error check!");
  // Serial.println(crcFlag ? "Data status: Good!" : "Data Status: Bad!");

  //   if (crcFlag) {
  //     // Print dos dados recebidos
      Serial.print("Data received: ");
      printReceivedData(receivedData, frameLength - 2 * flagLength, frameLength - flagLength);

  //     // Print da closeFlag
      Serial.print("CloseFlag: ");
      printReceivedData(receivedData, frameLength - flagLength, frameLength);
  //     int control[controlLength] = { 0, 0, 0, 0, 1, 1, 1, 1 };

  //   if (!sendingStatus) {
  //     sender(control, controlLength);
  //   }
  // } else {
  //   int control[controlLength] = { 1, 1, 1, 1, 0, 0, 0, 0 };
  //   sender(control, controlLength);
  // }
  // }
  // else {
  //   Serial.println("A msg não é para mim");
  // }
// }
}
// Configuração inicial do slave
void setup() {
  Serial.begin(9600);
  //pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

// Loop principal
void loop() {
  //int leitura = digitalRead(inputPin);
  //digitalWrite(outputPin, leitura);
  receiver();
}
