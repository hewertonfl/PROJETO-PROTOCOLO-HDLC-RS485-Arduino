// ********************* Definição dos pinos ********************* //
const int inputPin = 2;
const int outputPin = 8;


// ****************** Definição de vetores de bits **************** //
// Definição do vetor de flag
int flag[8] = { 0, 1, 1, 1, 1, 1, 1, 0 };
int flagLength = sizeof(flag) / sizeof(flag[0]);

// Definição do adress
const int adressLength = 8;
int adress[adressLength] = { 1, 1, 0, 0, 0, 0, 0, 0 };

// Definição do vetor de dados recebidos
const int receivedDataLength = 16;
int receivedData[receivedDataLength];

// Definição do vetor de CRC
const int crcLength = 8;
int crc[crcLength];

// ************************** Contadores ***************************** //
// Contagem de sincronização
int counter = 0;

// Contador auxiliar
int auxCounter = 0;

// ***************************** Flags **************************** //
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

// Função de impressão dos dados recebidos
void printReceivedData(int array[], int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
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
  if (!syncFlag) {
    fillArray();
  }
  if (!stopFlag) {
    if (syncFlag) {
      Serial.print("OpenFlag: ");
      printReceivedData(receivedData, 8);
      fillReceivedData(8);
      // Checa se a mensagem é para mim
      stopFlag = sync(adress, receivedData, adressLength);
      if (stopFlag) {
        Serial.print("Adress: ");
        printReceivedData(receivedData, 8);
        Serial.print("Control: ");
        fillReceivedData(8);
        printReceivedData(receivedData, 8);
        // Checagem de erros
        fillReceivedData(receivedDataLength);
        int checksum = calculateChecksum(receivedData, crcLength, receivedDataLength);
        checksumToBinaryArray(checksum, crc, crcLength);
        crc [0] = 1;
        bool crcFlag = sync(crc, receivedData, crcLength);
        if (crcFlag) {
          Serial.println("CRC checked succeed!");
          Serial.println("Data status: Good!");
          Serial.print("Data received: ");
          printReceivedData(receivedData, receivedDataLength);
          fillReceivedData(8);
          Serial.print("CloseFlag: ");
          printReceivedData(receivedData, 8);
        } else {
          Serial.println("CRC error check!");
          Serial.println("Data Status: Bad!");
        }
      } else {
        Serial.println("A msg não é para mim");
      }
    }
  }
}

// Configuração inicial do slave
void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

// Loop principal
void loop() {
  int leitura = digitalRead(inputPin);
  digitalWrite(outputPin, leitura);
  receiver();
}
