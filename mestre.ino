// ********************* Definição dos pinos ********************* //
const int outputPin = 2;
const int inputPin = 8;

// ****************** Definição de vetores de bits **************** //
// Definição do vetor de dados
const int dataLength = 8;
int data[dataLength] = { 1, 0, 0, 1, 0, 1, 1, 1 };

// Definição do vetor de flag
const int flagLength = 8;
int flag[flagLength] = { 0, 1, 1, 1, 1, 1, 1, 0 };

// Definição do vetor adress de destino
const int adressLength = 8;
int adress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 1 };

// Definição do vetor de controle de dados
const int controlLength = 8;
int control[controlLength] = { 0, 0, 0, 0, 0, 0, 0, 1 };

// Definição do vetor de CRC
const int crcLength = 8;
int crc[crcLength];

// Definição do vetor de dados final
const int finalDataLength = crcLength + dataLength;
int finalData[finalDataLength];
// Definição do vetor de dados recebidos
const int receivedDataLength = 8;
int receivedData[receivedDataLength];

// Tempo de recepção de bit
const int timeClock = 10;
int status = 0;

// Função de impressão de vetor
void printArray(int array[], int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    Serial.print(array[i]);
  }
  Serial.println();
}
// ***************************** Flags **************************** //
// Definição de status de print
bool printStatus = false;
bool response = false;
bool dataStatus = false;
// Remetente
void sender(int data[], int dataLength) {
  for (int i = 0; i < dataLength; i++) {
    digitalWrite(outputPin, data[i]);
    //Serial.print(data[i]);
    delay(timeClock);
  }
  //Serial.println();
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
void fillReceivedData(int receivedDataLength) {
  int counter = 0;
  while (counter < receivedDataLength) {
    receivedData[counter] = digitalRead(inputPin);
    counter++;
    delay(10);
  }
}

// Configuração inicial do Arduino
void setup() {
  Serial.begin(115200);
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}


void selectAdress(int option) {
  if (option == 1) {
    int adress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 1 };
    int checksum = calculateChecksum(data, 8) + calculateChecksum(adress, 8) + calculateChecksum(control, 8);
    checksumToBinaryArray(checksum, crc, crcLength);
    concatenateArrays(crc, data, crcLength, dataLength);
    sendFrame(adress);
  } else if (option == 2) {
    int adress[adressLength] = { 1, 1, 0, 1, 1, 0, 0, 1 };
    int checksum = calculateChecksum(data, 8) + calculateChecksum(adress, 8) + calculateChecksum(control, 8);
    checksumToBinaryArray(checksum, crc, crcLength);
    concatenateArrays(crc, data, crcLength, dataLength);
    sendFrame(adress);
  } else if (option == 3) {
    Serial.println("Em construção");
  }
}
// Loop principal
void loop() {
  if (!dataStatus) {
    if (!digitalRead(inputPin)) {
      Serial.println("Status: sending frame...");
      selectAdress(1);
    } else {
      //if (!response) {
      pinMode(inputPin, INPUT_PULLUP);
      Serial.println("Status: receiving response...");
      fillReceivedData(8);
      if (receivedData[0] && !receivedData[1] && !receivedData[2] && !receivedData[3] && receivedData[4] && receivedData[5] && receivedData[6] && !receivedData[7]) {
        Serial.println("Data status: Good!");
        printArray(receivedData, 8);
        dataStatus = true;
      } else if (receivedData[0] && !receivedData[1] && !receivedData[2] && !receivedData[3] && receivedData[4] && receivedData[5] && receivedData[6] && receivedData[7]) {
        Serial.println("Data status: Bad!");
        printArray(receivedData, 8);
        dataStatus = false;
        pinMode(inputPin, INPUT);
      }
    }
  }
}