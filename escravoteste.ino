// ********************* Definição dos pinos ********************* //
const int inputPin = 2;
const int outputPin = 8;


// ****************** Definição de vetores de bits **************** //
// Definição do vetor de flag
const int flagLength = 8;
int flag[flagLength] = { 0, 1, 1, 1, 1, 1, 1, 0 };

// Definição do adress
const int adressLength = 8;
int adress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 1 };

// Definição do vetor de dados recebidos
const int receivedDataLength = 24;
int receivedData[receivedDataLength];

// Definição do vetor de controle de dados
const int controlLength = 8;
int control[controlLength];

// Definição do vetor de CRC
const int crcLength = 8;
int crc[crcLength];

// ************************** Contadores ***************************** //
// Contagem de sincronização
int counter = 0;

// Contador auxiliar
int auxCounter = 0;

// ***************************** Flags ******************************* //
// Definição da flag de status
bool stopFlag = false;
// Definição da flag de sincronização
bool syncFlag = false;
bool sendingStatus = false;

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

// Função de preenchimento do vetor auxiliar
void fillArray() {
  if (auxCounter >= flagLength) {
    syncFlag = syncFlag ? true : sync(flag, receivedData, flagLength);
    Serial.println();
    auxCounter = 0;
  } else {
    receivedData[auxCounter] = digitalRead(inputPin);
    Serial.print(receivedData[auxCounter]);
    auxCounter++;
    delay(timeClock);
  }
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

// Função de preenchimento do vetor de dados
void fillReceivedData(int receivedDataLength) {
  int counter = 0;
  while (counter < receivedDataLength) {
    receivedData[counter] = digitalRead(inputPin);
    counter++;
    delay(timeClock);
  }
}

// Função de cálculo do checksum
uint16_t calculateChecksum(int data[], int crcLength, int receivedDataLength) {
  uint16_t checksum = 0;
  Serial.print("Checksum: ");
  for (int i = crcLength; i < receivedDataLength; i++) {
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
  // Checagem de parada
  if (stopFlag) { return; }
  while (!syncFlag) {
    fillArray();
  }
  Serial.print("OpenFlag: ");
  printReceivedData(receivedData,0, adressLength);
  fillReceivedData(8);

  // Checa se a mensagem é para mim
  if (bool adressFlag = sync(adress, receivedData, adressLength)) {
    Serial.print("Adress: ");
    printReceivedData(receivedData, 0, adressLength);
    Serial.print("Control: ");
    fillReceivedData(8);
    printReceivedData(receivedData, 0, flagLength);

    // Checagem de erros
    fillReceivedData(receivedDataLength);
    int checksum = calculateChecksum(receivedData, crcLength, receivedDataLength - flagLength);
    checksumToBinaryArray(checksum, crc, crcLength);
    bool crcFlag = sync(crc, receivedData, crcLength);

    if (!crcFlag) {
      int control[controlLength] = { 1, 1, 1, 1, 0, 0, 0, 0 };
      sender(control, controlLength);
      //syncFlag = false;
      return;
    }

    Serial.println(crcFlag ? "CRC checked succeed!" : "CRC error check!");
    Serial.println(crcFlag ? "Data status: Good!" : "Data Status: Bad!");

    // Print dos dados recebidos
    Serial.print("Data received: ");
    printReceivedData(receivedData, 0, receivedDataLength - flagLength);

    // Print da closeFlag
    Serial.print("CloseFlag: ");
    printReceivedData(receivedData, receivedDataLength - flagLength, receivedDataLength);
    stopFlag = true;

    sender(control, controlLength);
    sendingStatus = false;
    int control[controlLength] = { 0, 0, 0, 0, 1, 1, 1, 1 };
    return;

  } else {
    syncFlag = false;
    Serial.println("A msg não é para mim");
  }
}
// Configuração inicial do slave
void setup() {
  Serial.begin(115200);
  //pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

// Loop principal
void loop() {
  receiver();
}