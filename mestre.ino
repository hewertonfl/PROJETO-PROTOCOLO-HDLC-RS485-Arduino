// ********************* Definição dos pinos ********************* //
const int outputPin = 2;
const int inputPin = 8;
int option = 2;

// ****************** Definição de vetores de bits **************** //
// Definição do vetor de dados
const int dataLength = 8;
int data[dataLength] = { 0, 0, 0, 0, 0, 1, 0, 1 };

// Definição do vetor de flag
const int flagLength = 8;
int flag[flagLength] = { 0, 1, 1, 1, 1, 1, 1, 0 };

// Definição do vetor adress de destino
const int adressLength = 8;
int myAdress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 1 };

// Definição do vetor de controle de dados
const int controlLength = 8;
// Vetor de controle que pergunta para o escravo se ele quer se comunicar com o segundo escravo
int control[controlLength] = { 0, 0, 0, 0, 0, 1, 0, 1 };

// Definição do vetor de CRC
const int crcLength = 8;
int crc[crcLength];

// Definição do vetor de dados final
const int finalDataLength = crcLength + dataLength;
int finalData[finalDataLength];
// Definição do vetor de dados recebidos
const int receivedDataLength = 24;
int receivedData[receivedDataLength];

// Tempo de recepção de bit
const int timeClock = 10;
bool sendingStatus = true;

// ***************************** Flags **************************** //
// Definição de status de print
bool printStatus = false;
bool dataStatus = false;
bool stopFlag = false;
bool syncFlag = false;
bool crcFlag = false;
bool resetFlag = false;
bool questionFlag = true;
bool slaveMode = false;
bool requestFlag = false;
bool frameReceived = false;
bool readStatus = false;
bool badStatus = false;
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

// Função de impressão de vetor
void printArray(int array[], int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    Serial.print(array[i]);
  }
  Serial.println();
}


void sendFrame(int adress[]) {
  sender(flag, flagLength);
  sender(adress, adressLength);
  sender(control, controlLength);
  sender(finalData, finalDataLength);
  sender(flag, flagLength);
}

// Função de cálculo do checksum
uint16_t calculateChecksum(int data[], int length) {
  uint16_t checksum = 0;

  for (int i = 0; i < length; i++) {
    checksum += data[i];
  }
  return checksum;
}

// Função de conversão do checksum para binário
void checksumToBinaryArray(uint16_t checksum, int crc[], int crcLength) {
  //Serial.print("CRC: ");
  for (int i = crcLength - 1; i >= 0; i--) {
    crc[i] = (checksum >> i) & 0x01;
    //Serial.print(crc[i]);
  }
  //Serial.println();
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
// Função de preenchimento do vetor de dados
void fillReceivedData(int receivedDataLength, bool print = false) {
  int counter = 0;
  while (counter < receivedDataLength) {
    delay(timeClock);
    receivedData[counter] = digitalRead(inputPin);
    if (print) { Serial.print(receivedData[counter]); }
    counter++;
  }
  if (print) { Serial.println(); }
}

// Configuração inicial do Arduino
void setup() {
  Serial.begin(115200);
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}


void selectAdress() {
  if (option == 1) {
    // Escravo 1
    int adress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 1 };
    int checksum = calculateChecksum(data, 8) + calculateChecksum(adress, 8) + calculateChecksum(control, 8);
    checksumToBinaryArray(checksum, crc, crcLength);
    concatenateArrays(crc, data, crcLength, dataLength);
    sendFrame(adress);
  } else if (option == 2) {
    //Escravo 2
    int adress[adressLength] = { 1, 1, 0, 0, 1, 1, 0, 1 };
    int checksum  = calculateChecksum(data, 8) + calculateChecksum(adress, 8) + calculateChecksum(control, 8);
    checksumToBinaryArray(checksum , crc, crcLength);
    concatenateArrays(crc, data, crcLength, dataLength);
    sendFrame(adress);
    // Comunicação escravo-escravo
  } else if (option == 3) {
    Serial.println("Status: sending received frame by master to slave...");
    sendFrameSM();
  }
}
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

void master(int mode = 0) {
  if (!dataStatus) {
    if (mode == 0) {
      amIReading();
      while (readStatus) {
        if(!badStatus){Serial.println("Status: receiving response...");}
        //printArray(receivedData, 8);
        fillReceivedData(8);
        if (receivedData[0] && !receivedData[1] && !receivedData[2] && !receivedData[3] && receivedData[4] && receivedData[5] && receivedData[6] && !receivedData[7]) {
          Serial.println("Data status: Good!");
          printArray(receivedData, 8);
          dataStatus = true;
          readStatus = false;
          return;
        } else if (!receivedData[0] && receivedData[1] && receivedData[2] && !receivedData[3] && receivedData[4] && receivedData[5] && !receivedData[6] && !receivedData[7]) {
          Serial.println("Data status: Bad!");
          printArray(receivedData, 8);
          pinMode(inputPin, INPUT);
          badStatus = true;
          readStatus = false;
        } else if (!receivedData[0] && !receivedData[1] && receivedData[2] && receivedData[3] && receivedData[4] && !receivedData[5] && receivedData[6] && !receivedData[7]) {
          Serial.println("Master change to slave mode!");
          slaveMode = true;
          readStatus = false;
          return;
        }
      }
      Serial.println("Status: sending frame...");
      selectAdress();
    } else if (mode == 1) {
      Serial.println("Status: sending frame...");
      selectAdress();
    }
  }
}

// ************************** Slave MODE ***************************** //
// Definição dos vetores recebidos
int RopenFlag[flagLength];

// Definição do vetor de dados
int Rdata[dataLength];

// Definição do adress
int Radress[adressLength];

// Definição do vetor de controle de dados
int Rcontrol[controlLength];

// Definição do vetor de CRC
int Rcrc[crcLength];

// Definição do vetor de dados final
int RfinalData[finalDataLength];

// Definição de vetor close flag
int RcloseFlag[flagLength];

// Função de sincronização
bool sync(int array[], int auxRead[], int length) {
  for (int i = 0; i < length; i++) {
    if (array[i] != auxRead[i]) {
      return false;
    }
  }
  return true;
}

void fillVector(int vector[], int receivedData[], int length) {
  for (int i = 0; i < length; i++) {
    vector[i] = receivedData[i];
  }
}

// Função de impressão dos dados recebidos
void printReceivedData(int array[], int start, int arrayLength) {
  for (int i = start; i < arrayLength; i++) {
    Serial.print(array[i]);
  }
  Serial.println();
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

void sendFrameSM() {
  sender(RopenFlag, flagLength);
  sender(Radress, adressLength);
  sender(Rcontrol, controlLength);
  sender(RfinalData, finalDataLength);
  sender(RcloseFlag, flagLength);
}

void printFrame() {
  // Print da openFlag
  Serial.print("OpenFlag: ");
  printReceivedData(RopenFlag, 0, flagLength);
  // Print do Adress
  Serial.print("Adress: ");
  printReceivedData(Radress, 0, adressLength);
  // Print do controle
  Serial.print("Control: ");
  printReceivedData(Rcontrol, 0, controlLength);
  // Print do CRC
  Serial.print("CRC: ");
  printReceivedData(RfinalData, 0, 8);
  // Print dos dados recebidos
  Serial.print("Data received: ");
  printReceivedData(RfinalData, 8, 16);
  // Print da closeFlag
  Serial.print("CloseFlag: ");
  printReceivedData(RcloseFlag, 0, flagLength);
}

// Função de recebimento de dados
void receiver() {
  if (!stopFlag) {
    while (!syncFlag) {
      fillReceivedData(8, true);
      syncFlag = syncFlag ? true : sync(flag, receivedData, flagLength);
    }
    // Saving openFlag
    fillVector(RopenFlag, receivedData, flagLength);
    // Saving Adress
    //delay(8 * timeClock);
    fillReceivedData(8);
    fillVector(Radress, receivedData, adressLength);
    // Saving controle
    fillReceivedData(8);
    fillVector(Rcontrol, receivedData, adressLength);
    // Saving CRC + Data
    fillReceivedData(16);
    fillVector(RfinalData, receivedData, finalDataLength);
    // Saving closeFlag
    fillReceivedData(8);
    fillVector(RcloseFlag, receivedData, flagLength);
    printFrame();
    stopFlag = true;
    frameReceived = true;
    readStatus = false;
  }
}

void sendAck(int control[], int index) {
  Serial.print("sending ACK ");
  Serial.print(index);
  Serial.println("...");
  sender(control, controlLength);
}
// Loop principal
void loop() {
  if (!frameReceived) {
    while (slaveMode && !requestFlag && !frameReceived && !readStatus) {
      int control[controlLength] = { 1, 1, 1, 0, 1, 0, 1, 0 };
      sendAck(control, 1);
      amIReading();
    }
    amIReading();
    while (readStatus && slaveMode && !requestFlag) {
      if (!stopFlag) { Serial.println("Slave Mode"); }
      requestFlag = true;
      receiver();
    }
  } else if (slaveMode) {
    while (frameReceived && !readStatus) {
      int control[controlLength] = { 0, 0, 0, 0, 1, 0, 1, 0 };
      sendAck(control, 2);
      amIReading();
    }
    readStatus = false;
    slaveMode = false;
  }
  if (!slaveMode && frameReceived) {
    option = 3;
    master(1);
  } else {
    master();
  }
}

//}