// Definição do vetor de dados
int data[] = { 1, 0, 0, 1, 0, 1, 1, 1};
int dataLength = sizeof(data) / sizeof(data[0]);

// Definição do vetor de flag
const int flagLength = 8;
int flag[flagLength] = { 0, 1, 1, 1, 1, 1, 1, 0 };

// Definição do vetor master adress
const int adressLength = 8;
int adress[adressLength] = [1, 1, 0, 0, 0, 0, 0, 0]

// Definição do vetor de controle de dados
const int controlLength = 8;
const int control[controlLength];

// Definição do vetor de CRC
const int crcLength = 8;
const int crc[crcLength];

// Definição do pino de saída
int outputPin = 2;

// Configuração inicial do Arduino
void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  //sender(data, dataLength);
}
// Remetente
void sender(int data[], int dataLength) {
  for (int i = 0; i < dataLength; i++) {
    digitalWrite(outputPin, data[i]);
    delay(100);
  }
}

// Loop principal
void loop() {
  sender(flag, flagLength);
  sender(data, dataLength);
}
