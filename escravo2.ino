// ********************* Definição dos pinos ********************* //
const int inputPin = 2;
const int outputPin = 23;


// ****************** Definição de vetores de bits **************** //
// Definição do vetor de flag
const int flagLength = 8;
int flag[flagLength] = { 0, 1, 1, 1, 1, 1, 1, 0 };

// Definição do vetor de dados
const int dataLength = 8;
int data[dataLength] = { 1, 0, 0, 1, 0, 1, 1, 1 };


// Definição do adress
const int adressLength = 8;
int myAdress[adressLength] = { 1, 1, 0, 0, 1, 1, 0, 1 };

// Definição do vetor de controle de dados
const int controlLength = 8;
int control[controlLength] = { 0, 0, 0, 0, 1, 1, 1, 1 };

// Definição do vetor de CRC
const int crcLength = 8;
int crc[crcLength];

// Definição do vetor de dados final
const int finalDataLength = crcLength + dataLength;
int finalData[finalDataLength];

// Definição do vetor de dados recebidos
const int receivedDataLength = crcLength + dataLength + flagLength;
int receivedData[receivedDataLength];

// ************************** Contadores ***************************** //
// Contagem de sincronização
int counter = 0;
int checksum = 0;
// Contador auxiliar
int auxCounter = 0;

// ***************************** Flags ******************************* //
// Definição da flag de status
bool stopFlag = false;
// Definição da flag de sincronização
bool syncFlag = false;
bool sendingStatus = false;
bool receivingStatus = false;
bool crcFlag = false;
bool resetFlag = false;
bool questionFlag = true;
bool masterMode = false;
bool idleStatus = false;
bool readStatus = false;

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

// Remetente
void sender(int data[], int dataLength, int time = 0) {
  for (int i = 0; i < dataLength; i++) {
    digitalWrite(outputPin, data[i]);
    //Serial.print(data[i]);
    delay(timeClock);
  }
  delay(timeClock * time);
  //Serial.println();
}
// Envia o frame
void sendFrame(int adress[]) {
  sender(flag, flagLength);
  sender(adress, adressLength);
  sender(control, controlLength);
  sender(finalData, finalDataLength);
  sender(flag, flagLength);
}
// Detecta se o dispositivo está lendo ou não
void amIReading() {
  for (int i = 0; i < 10; i++) {
    int aux[8];
    if (digitalRead(inputPin)) {
      readStatus = true;
      return;
    }
    readStatus = false;
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
void fillReceivedData(int receivedDataLength, bool print = false) {
  int counter = 0;
  while (counter < receivedDataLength) {
    receivedData[counter] = digitalRead(inputPin);
    if (print) { Serial.print(receivedData[counter]); }
    counter++;
    delay(10);
  }
  if (print) { Serial.println(); }
}

// Função de cálculo do checksum
uint16_t calculateChecksum(int data[], int crcLength, int receivedDataLength) {
  uint16_t checksum = 0;
  //Serial.print("Checksum: ");
  for (int i = crcLength; i < receivedDataLength; i++) {
    checksum += data[i];
  }
  //Serial.println(checksum);
  return checksum;
}

// Função de conversão do checksum para binário
void checksumToBinaryArray(uint16_t checksum, int crc[], int crcLength) {
  Serial.print("Calculated CRC: ");
  int counter = 0;
  for (int i = crcLength - 1; i >= 0; i--) {
    crc[counter] = (checksum >> i) & 0x01;
    Serial.print(crc[counter]);
    counter++;
  }
  Serial.println();
}

// Função de conversão do checksum para binário
void checksumToBinaryArrayMM(uint16_t checksum, int crc[], int crcLength) {
  //Serial.print("CRC: ");
  for (int i = crcLength - 1; i >= 0; i--) {
    crc[i] = (checksum >> i) & 0x01;
    //Serial.print(crc[i]);
  }
  //Serial.println();
}

// Função de recebimento de dados
void receiver() {
  if (!stopFlag) {
    fillReceivedData(8, true);
    syncFlag = syncFlag ? true : sync(flag, receivedData, flagLength);
    if (!syncFlag) {
      delay(timeClock);
      return;
    }
    Serial.print("OpenFlag: ");
    printReceivedData(receivedData, 0, flagLength);
    //delay(8 * timeClock);
    fillReceivedData(8);
    checksum += calculateChecksum(receivedData, 0, 8);

    // Checa se a mensagem é para mim
    if (bool address = sync(myAdress, receivedData, adressLength)) {
      Serial.print("Adress: ");
      printReceivedData(receivedData, 0, adressLength);
      Serial.print("Control: ");
      fillReceivedData(8);
      printReceivedData(receivedData, 0, controlLength);
      checksum += calculateChecksum(receivedData, 0, 8);

      // Checagem de erros
      fillReceivedData(receivedDataLength);
      checksum += calculateChecksum(receivedData, crcLength, receivedDataLength - flagLength);
      checksumToBinaryArray(checksum, crc, crcLength);
      //crc[0] = 1;
      crcFlag = sync(crc, receivedData, crcLength);

      if (crcFlag) {
        Serial.println("CRC checked succeed!");
        Serial.println("Data status: Good!");
        Serial.print("CRC Received: ");
        printReceivedData(receivedData, 0, flagLength);

        // Print dos dados recebidos
        Serial.print("Data received: ");
        printReceivedData(receivedData, flagLength, receivedDataLength - flagLength);

        // Print da closeFlag
        Serial.print("CloseFlag: ");
        digitalWrite(outputPin, 1);
        printReceivedData(receivedData, receivedDataLength - flagLength, receivedDataLength);


      } else {
        Serial.println("CRC error check!");
        Serial.println("Data Status: Bad!");
        Serial.print("CRC Received: ");
        printReceivedData(receivedData, 0, flagLength);
        //digitalWrite(outputPin, 1);
      }
      sendingStatus = true;
      Serial.println("Status: sending response...");
    } else {
      Serial.println("A msg não é para mim");
      delay(8 * timeClock);
      syncFlag = false;
      checksum = 0;
    }
  }
}
void reset() {
  stopFlag = false;
  syncFlag = false;
  sendingStatus = false;
  crcFlag = false;
  resetFlag = false;
  readStatus = false;
  digitalWrite(outputPin, 0);
}

void slave() {
  if (!sendingStatus) {
    receiver();
  } else {
    //Checa se o status da mensagem é good
    int counter1 = 0;
    while (crcFlag) {
      pinMode(inputPin, INPUT_PULLDOWN);
      pinMode(outputPin, OUTPUT);
      int control[controlLength] = { 1, 0, 0, 0, 1, 1, 1, 0 };
      sender(control, controlLength);

      counter1++;
      delay(timeClock);
    }
    //Checa se o status da mensagem é bad
    int counter2 = 0;
    while (!crcFlag && counter2<5) {
      pinMode(inputPin, INPUT_PULLDOWN);
       pinMode(outputPin, OUTPUT);
      int control[controlLength] = { 0, 1, 1, 0, 1, 1, 0, 0 };
      sender(control, controlLength);
      resetFlag = true;

      counter2++;
      delay(timeClock);
    }
    if (resetFlag) {
      reset();
    }
  }
}
// Função de impressão de vetor
void printArray(int array[], int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    Serial.print(array[i]);
  }
  Serial.println();
}

// ****************** Master Mode **************** //
int adress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 1 };
// Função de cálculo do checksum
uint16_t calculateChecksumMM(int data[], int length) {
  uint16_t checksum = 0;
  //Serial.print("Checksum: ");
  for (int i = 0; i < length; i++) {
    checksum += data[i];
  }
  //Serial.print(checksum);
  return checksum;
}

// Função de concatenação de vetores
void concatenateArrays(int array1[], int array2[], int size1, int size2) {
  // Copie os elementos do primeiro array para o vetor final
  int counter = 0;
  for (int i = size1 - 1; i >= 0; i--) {
    finalData[counter] = array1[i];
    counter++;
  }

  // Copie os elementos do segundo array para o vetor final
  for (int i = 0; i < size2; i++) {
    finalData[size1 + i] = array2[i];
  }
  //Serial.print("Final data: ");
  //printArray(finalData, finalDataLength);
}

void master() {
  if (!digitalRead(inputPin) && masterMode) {
    Serial.println("Master Mode");
    int checksum = calculateChecksumMM(data, 8) + calculateChecksumMM(adress, 8) + calculateChecksumMM(control, 8);
    checksumToBinaryArrayMM(checksum, crc, crcLength);
    concatenateArrays(crc, data, crcLength, dataLength);
    sendFrame(adress);
  } else {
    pinMode(inputPin, INPUT_PULLUP);
    Serial.println("Status: receiving response...");
    fillReceivedData(8);
    if (receivedData[0] && !receivedData[1] && !receivedData[2] && !receivedData[3] && receivedData[4] && receivedData[5] && receivedData[6] && !receivedData[7]) {
      Serial.println("Frame sended!");
      masterMode = false;
      questionFlag = false;
    }
  }
}

void sendAck() {
  int control[controlLength] = { 0, 0, 0, 0, 1, 0, 1, 0 };
  Serial.println("sending ACK...");
  for (int i = 0; i < 20; i++) { sender(control, controlLength, i); }
}

// ****************** Configuração inicial do slave **************** //
void setup() {
  Serial.begin(115200);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, 0);
}

// ************************* Loop principal *************************** //
void loop() {
  if (!idleStatus && questionFlag) {
    readStatus = true;
    while (!masterMode && !receivingStatus && readStatus) {
      Serial.println("Sending request...");
      pinMode(inputPin, INPUT_PULLDOWN);
      int control[controlLength] = { 0, 0, 1, 1, 1, 0, 1, 0 };
      sender(control, controlLength);
      amIReading();
    }

    amIReading();
    while (readStatus && !receivingStatus) {
      fillReceivedData(8);
      printArray(receivedData, 8);
      if (receivedData[0] && receivedData[1] && receivedData[2] && !receivedData[3] && receivedData[4] && !receivedData[5] && receivedData[6] && !receivedData[7]) {
        Serial.println("Request sended!");
        printArray(receivedData, 8);
        masterMode = true;
        receivingStatus = true;
        readStatus = false;
      }
    }

    if (masterMode) {
      Serial.println("Master Mode");
      int checksum = calculateChecksumMM(data, 8) + calculateChecksumMM(adress, 8) + calculateChecksumMM(control, 8);
      checksumToBinaryArrayMM(checksum, crc, crcLength);
      concatenateArrays(crc, data, crcLength, dataLength);
      sendFrame(adress);

      amIReading();
      while (readStatus) {
        fillReceivedData(8);
        printArray(receivedData, 8);
        if (!receivedData[0] && !receivedData[1] && !receivedData[2] && !receivedData[3] && receivedData[4] && !receivedData[5] && receivedData[6] && !receivedData[7]) {
          Serial.println("sended success!");
          printArray(receivedData, 8);
          idleStatus = true;
          readStatus = false;
        }
        //delay(timeClock);
      }
    }
  } else if (questionFlag) {
    readStatus = false;
    while (!readStatus) {
      digitalWrite(outputPin, 1);
      amIReading();
    }
    Serial.println("Acabou!");
    digitalWrite(outputPin, 0);
    questionFlag = false;
  } else if (!questionFlag) {
    pinMode(outputPin, INPUT_PULLDOWN);
    slave();
  }
}
