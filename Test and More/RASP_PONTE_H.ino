#include <Encoder.h> // Biblioteca para uso de Encoders

// Definição dos pinos do motor
const int pinoPWM1 = 5;  
const int pinoDirecao1 = 4;
const int pinoDirecao2 = 10;
int ledPin = 13; // Pino do LED

// Definição dos pinos do sensor ultrassônico
const int trigPin = 11;  // Pino TRIG do sensor ultrassônico
const int echoPin = 12;  // Pino ECHO do sensor ultrassônico

// Variáveis de controle de distância
long duration;
float distance;
const float distanceThreshold = 30.0;  // Distância limite em cm para ativar o motor

// Inicializa o encoder
Encoder EncoderMotorNXT1(2, 8); // Pinos do encoder (ajuste conforme necessário)

// Variáveis para controlar o funcionamento do motor
bool motorRodou = false;
unsigned long tempoUltimoComando = 0; // Tempo do último comando
const unsigned long tempoEspera = 10000; // 10 segundos de espera
const unsigned long tempoSegurandoPorta = 30000; // 30 segundos para segurar a porta

void setup() {
  pinMode(pinoPWM1, OUTPUT);
  pinMode(pinoDirecao1, OUTPUT);
  pinMode(pinoDirecao2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Verifica se há dados recebidos via Serial
  if (Serial.available() > 0) {
    char comando = Serial.read();

    // Somente ativa o motor se o comando for '1', a distância estiver abaixo do limite, e o motor ainda não tiver rodado
    if (comando == '1' && medirDistancia() < distanceThreshold && !motorRodou) {  
      tempoUltimoComando = millis(); // Marca o tempo do último comando

      delay(5000);  // Espera 5 segundos antes de ligar o motor
      
      // Ativa o motor em uma velocidade contínua e direção anti-horária
      mover(1, 255, 1);  
      digitalWrite(ledPin, HIGH);  // Acende o LED para indicar que o motor está em movimento
      Serial.println("Motor ligado. Objeto próximo e comando recebido.");
      
      // Aguarda 10 segundos (tempo de espera)
      while (millis() - tempoUltimoComando < tempoEspera) {
        // Aqui pode-se adicionar qualquer outra lógica necessária durante esse tempo de espera
      }

      // Após 10 segundos, começa a segurar a porta por 30 segundos
      mover(1, 255, 1);    // Mantém o motor em movimento para segurar a porta
      digitalWrite(ledPin, HIGH);  // O LED continua aceso

      Serial.println("Segurando a porta por 30 segundos.");

      // Aguarda 30 segundos mantendo o motor em movimento
      delay(tempoSegurandoPorta); 

      // Após 30 segundos, desliga o motor
      mover(0, 100, 1);    // Para o motor
      digitalWrite(ledPin, LOW);  // Desliga o LED
      Serial.println("Motor desligado após 30 segundos.");

      motorRodou = true;  // Marca que o motor rodou uma vez
    } 
    else if (comando == '0') {  // Recebe '0' para desligar o motor imediatamente
      mover(1, 0, 1);    // Para o motor
      digitalWrite(ledPin, LOW);  // Desliga o LED
      Serial.println("Motor desligado.");
    }
  }
}

// Função para medir a distância com o sensor ultrassônico
float medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Calcula o tempo do pulso e converte em distância
  duration = pulseIn(echoPin, HIGH);
  float dist = (duration * 0.034) / 2;
  return dist;
}

// Função para controlar o motor
void mover(int motor, int potencia, int direcao) {
  potencia = constrain(potencia, 0, 255);
  direcao = constrain(direcao, 0, 1);

  if (motor == 1) {
    digitalWrite(pinoDirecao1, direcao == 0 ? HIGH : LOW);
    digitalWrite(pinoDirecao2, direcao == 0 ? LOW : HIGH);
    analogWrite(pinoPWM1, potencia);
  }
}
