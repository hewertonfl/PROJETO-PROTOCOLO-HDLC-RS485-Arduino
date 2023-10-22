// Definição do vetor de dados
const int dataLength = 8;
int data[dataLength] = { 1, 0, 0, 1, 0, 1, 1, 1 };

// Definição do vetor de flag
const int flagLength = 8;
int flag[flagLength] = { 0, 1, 1, 1, 1, 1, 1, 0 };

// Definição do vetor adress de destino
const int adressLength = 8;
int adress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 0 };

// Definição do vetor de controle de dados
const int controlLength = 8;
int control[controlLength] = { 0, 0, 0, 0, 0, 0, 0, 1 };

// Definição do vetor de CRC
const int crcLength = 8;
int crc[crcLength];

const int finalDataLength = crcLength + dataLength;
int finalData[finalDataLength];

// Definição do pino de saída
int outputPin = 2;

// Tempo de recepção de bit
const int timeClock = 10;

// Configuração inicial do Arduino
void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  int checksum = calculateChecksum(data, 8);
  checksumToBinaryArray(checksum, crc, crcLength);
  concatenateArrays(crc, data, crcLength, dataLength);
}

void printArray(int array[], int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    Serial.print(array[i]);
  }
  Serial.println();
}

// Remetente
void sender(int data[], int dataLength) {
  for (int i = 0; i < dataLength; i++) {
    digitalWrite(outputPin, data[i]);
    delay(timeClock);
  }
}

uint16_t calculateChecksum(int data[], int length) {
  uint16_t checksum = 0;

  for (int i = 0; i < length; i++) {
    checksum += data[i];
  }

  return checksum;
}

void checksumToBinaryArray(uint16_t checksum, int crc[], int crcLength) {
  Serial.print("CRC: ");
  for (int i = crcLength - 1; i >= 0; i--) {
    crc[i] = (checksum >> i) & 0x01;
    Serial.print(crc[i]);
  }
  Serial.println();
}

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
  Serial.print("Final data: ");
  printArray(finalData, finalDataLength);
}


// Loop principal
void loop() {
  sender(flag, flagLength);
  sender(adress, adressLength);
  sender(control, controlLength);
  sender(finalData, finalDataLength);
  sender(flag, flagLength);
}
