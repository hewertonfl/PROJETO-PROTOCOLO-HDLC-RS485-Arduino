// ********************* Definição dos pinos ********************* //
const int inputPin = 2;
const int outputPin = 23;


// ****************** Definição de vetores de bits **************** //
// Definição do vetor de flag
const int flagLength = 8;
int flag[flagLength] = { 0, 1, 1, 1, 1, 1, 1, 0 };
int counterSender = 0;

// Definição do adress
const int adressLength = 8;
int myAdress[adressLength] = { 1, 1, 0, 1, 1, 0, 0, 1 };

// Definição do vetor de dados recebidos
const int receivedDataLength = 24;
int receivedData[receivedDataLength];

// Definição do vetor de controle de dados
const int controlLength = 8;
int control[controlLength] = { 0, 0, 0, 0, 1, 1, 1, 1 };

// Definição do vetor de CRC
const int crcLength = 8;
int crc[crcLength];

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
bool executou = false;
bool sendingStatus = false;
bool crcFlag = false;
bool resetFlag = false;

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
    delay(8 * timeClock);
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
      crc[0] = 1;
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
        digitalWrite(outputPin, 1);
        stopFlag = true;
      }
      sendingStatus = true;
      Serial.println("Status: sending response...");
    } else {
      Serial.println("A msg não é para mim");
      syncFlag = false;
      checksum = 0;
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

void reset() {
  stopFlag = false;
  syncFlag = false;
  sendingStatus = false;
  crcFlag = false;
  resetFlag = false;
  digitalWrite(outputPin, 0);
}

// Configuração inicial do slave
void setup() {
  Serial.begin(115200);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  //digitalWrite(outputPin, 0);
}

// Loop principal
void loop() {
  if (!sendingStatus) {
    receiver();
    //Checa se o status da mensagem é good
  } else if (crcFlag && !digitalRead(inputPin)) {
    pinMode(inputPin, INPUT_PULLDOWN);
    int control[controlLength] = { 1, 0, 0, 0, 1, 1, 1, 0 };
    sender(control, controlLength);
    //Checa se o status da mensagem é bad
  } else if (!digitalRead(inputPin)) {
    pinMode(inputPin, INPUT_PULLDOWN);
    int control[controlLength] = { 1, 0, 0, 0, 1, 1, 1, 1 };
    for(int i = 0; i<20;i++){sender(control, controlLength);}
    pinMode(inputPin, INPUT);
    resetFlag = true;
  }

  else if (digitalRead(inputPin) && resetFlag) {
   reset();
 }
}
