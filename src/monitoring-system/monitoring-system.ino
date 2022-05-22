int mqttCommand = 0;
// bomba
bool pumpOff = HIGH;
bool mqttStatus = false; // interação de desligamento da bomba via mqtt
int pumpLed = 13;
int pumpPin = 2;
// caixa de baixo
float bottomDistance = 0;
int bottomMaxLevel = 8; // distância em cm do sensor (depende do tamanho do reservátório utilizado)
int bottomMinLevel = 20; // distância em cm do sensor (depende do tamanho do reservátório utilizado)
int bottomMaxLevelLed = 11;
int bottomMinLevelLed = 10;
float bottomPercentageLevel = 0;
// sensor caixa de baixo
int bottomEchoPin = 6;
int bottomTriggerPin = 7;
// caixa de cima
float upperDistance = 0;
int upperMaxLevel = 8; // distância em cm do sensor (depende do tamanho do reservátório utilizado)
int upperMinLevel = 16; // distância em cm do sensor (depende do tamanho do reservátório utilizado)
int upperMaxLevelLed = 9;
int upperMinLevelLed = 8;
float upperPercentageLevel = 0;
// sensor caixa de cima
int upperEchoPin = 4;
int upperTriggerPin = 5;

// leitura do nível de água pelo sensor
long readUltrasonicDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  
  // define o pino trigger para HIGH por 10 microsegundos
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  // lê o pino echo e retorna o tempo de onda de som em microsegundos
  return pulseIn(echoPin, HIGH);
}

// muda o estado dos leds de nível, dependendo do nível de água
void changeLevelLeds(int level, int maxPin, int tankMaxLevel, int minPin, int tankMinLevel) {  
  if (level >= tankMinLevel) {
    digitalWrite(maxPin, HIGH);
    digitalWrite(minPin, LOW);
  } else if (level <= tankMaxLevel) {
    digitalWrite(maxPin, LOW);
    digitalWrite(minPin, HIGH);
  } else {
    digitalWrite(maxPin, LOW);
    digitalWrite(minPin, LOW);
  }
}

// lê informações via serial
bool checkSerial(bool status) {
  if (Serial.available() > 0) {
    mqttCommand = Serial.read();
    if (mqttCommand == 48) {
      status = HIGH;
      mqttStatus = true;
    } else if (mqttCommand == 49) {
      status = LOW;
      mqttStatus = false;
    }
  }
  
  return status;
}

void setup() {
  pinMode(bottomEchoPin, INPUT);
  pinMode(bottomMaxLevelLed, OUTPUT);
  pinMode(bottomMinLevelLed, OUTPUT);
  pinMode(bottomTriggerPin, OUTPUT);
  pinMode(upperEchoPin, INPUT);
  pinMode(upperMaxLevelLed, OUTPUT);
  pinMode(upperMinLevelLed, OUTPUT);
  pinMode(upperTriggerPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(pumpLed, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // converte a leitura da distância em cm
  bottomDistance = 0.01723 * readUltrasonicDistance(bottomTriggerPin, bottomEchoPin);
  upperDistance = 0.01723 * readUltrasonicDistance(upperTriggerPin, upperEchoPin);
  
  // converte a distância (cm) em porcentagem
  bottomPercentageLevel = map(bottomDistance, bottomMinLevel, bottomMaxLevel, 0, 100);
  upperPercentageLevel = map(upperDistance, upperMinLevel, upperMaxLevel, 0, 100);
  
  changeLevelLeds(bottomDistance, bottomMaxLevelLed, bottomMaxLevel, bottomMinLevelLed, bottomMinLevel);
  changeLevelLeds(upperDistance, upperMaxLevelLed, upperMaxLevel, upperMinLevelLed, upperMinLevel);

  // ativa ou desativa a bomba, via comunicação serial
  pumpOff = checkSerial(pumpOff);
  
  // ignora instruções via mqtt se caixa de baixo estiver cheia ou a de cima vazia
  if (mqttStatus && bottomDistance <= bottomMaxLevel || upperDistance >= upperMinLevel) {
    mqttStatus = false;
  }
  
  // checa o estado da bomba e os níveis de água das caixas (HIGH = desligado)
  if (!pumpOff) {
    if (bottomDistance >= bottomMinLevel || upperDistance <= upperMaxLevel) {
      pumpOff = HIGH;
    }
  } else {
    if (!mqttStatus) {
      if (bottomDistance < bottomMinLevel) {
        if (upperDistance > upperMaxLevel) {
          pumpOff = LOW; 
        }
      }
    }
  }
  
  // ativa (ou desativa) a bomba
  digitalWrite(pumpPin, pumpOff);
  digitalWrite(pumpLed, !pumpOff);
  
  // envia dados para dashboard, via porta serial
  Serial.print("bottomPercentageLevel:");
  Serial.println(bottomPercentageLevel);
  Serial.print("upperPercentageLevel:");
  Serial.println(upperPercentageLevel);
  Serial.print("pumpStatus:");
  Serial.println(!pumpOff);

  delay(1000); // espera 1000 milisegundos
}