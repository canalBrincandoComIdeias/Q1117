/****************************************************************************************
  Elevador com Motor DC
  
  Desenvolvido pela Fábrica de Programas - Brincando com Ideias (www.brincandocomideias.com)
  www.youtube.com/c/BrincandoComIdeias

  Autor Flavio Guimaraes
*****************************************************************************************/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define pinMotorA 9    //Motor DC
#define pinMotorB 10   //Motor DC
#define pinBotaoA 2    //Botão - Andar 1
#define pinBotaoB 3    //Botão - Andar 2
#define pinBotaoC 4    //Botão - Andar 3
#define pinBotaoD 5    //Botão - Andar 4
#define pinSensorA 8   //Sensor de Efeito Hall - Andar 1
#define pinSensorB 11  //Sensor de Efeito Hall - Andar 2
#define pinSensorC 12  //Sensor de Efeito Hall - Andar 3
#define pinSensorD 13  //Sensor de Efeito Hall - Andar 4
#define pinBuzzer 6    //Buzzer Passivo
#define pinLedA A0     //LED - Andar 1
#define pinLedB A1     //LED - Andar 2
#define pinLedC A2     //LED - Andar 3
#define pinLedD A3     //LED - Andar 4

#define velocidadeSobe 200
#define velocidadeDesce 90

LiquidCrystal_I2C lcd(0x27, 20, 4);

String ponteiro[2] = { "v", " " };
int ledAndar[4] = { pinLedA, pinLedB, pinLedC, pinLedD };
int enderecoAndar[4] = { 0x11, 0x12, 0x13, 0x14 };

bool estadoSensorA;
bool estadoSensorB;
bool estadoSensorC;
bool estadoSensorD;

bool estadoSensorAAnt;
bool estadoSensorBAnt;
bool estadoSensorCAnt;
bool estadoSensorDAnt;

int acaoElevador = 0;  //0- Parado, 1-Subindo, 2-Descendo
int acaoElevadorAnt = 0;

int andar = -1;
int andarAnt = 0;
int andarDestino = -1;
int andarDestinoAnt = -1;

void setup() {
  pinMode(pinBotaoA, INPUT_PULLUP);
  pinMode(pinBotaoB, INPUT_PULLUP);
  pinMode(pinBotaoC, INPUT_PULLUP);
  pinMode(pinBotaoD, INPUT_PULLUP);

  pinMode(pinLedA, OUTPUT);
  pinMode(pinLedB, OUTPUT);
  pinMode(pinLedC, OUTPUT);
  pinMode(pinLedD, OUTPUT);

  pinMode(pinSensorA, INPUT);
  pinMode(pinSensorB, INPUT);
  pinMode(pinSensorC, INPUT);
  pinMode(pinSensorD, INPUT);

  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinMotorA, OUTPUT);
  pinMode(pinMotorB, OUTPUT);

  Wire.begin();  // Inicia o I2C como mestre

  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Elevador montTUDO");

  lcd.setCursor(1, 3);              // 01234567890123456789   ->coordenadas
  lcd.print("(1)  (2)  (3)  (4)");  //  (1)  (2)  (3)  (4)

  digitalWrite(pinLedA, HIGH);
  digitalWrite(pinLedB, HIGH);
  digitalWrite(pinLedC, HIGH);
  digitalWrite(pinLedD, HIGH);

  tone(pinBuzzer, 800);
  delay(100);
  tone(pinBuzzer, 400);
  delay(100);
  tone(pinBuzzer, 800);
  delay(100);
  noTone(pinBuzzer);

  digitalWrite(pinLedA, LOW);
  digitalWrite(pinLedB, LOW);
  digitalWrite(pinLedC, LOW);
  digitalWrite(pinLedD, LOW);

  identificaAndar();

  //Atualiza o Display dos Andares e Apaga os LEDs
  for (int nL = 0; nL < 4; nL++) {
    Wire.beginTransmission(enderecoAndar[nL]);
    Wire.write(andar + '0');
    Wire.endTransmission();

    Wire.beginTransmission(enderecoAndar[nL]);
    Wire.write('S');
    Wire.endTransmission();

    Wire.beginTransmission(enderecoAndar[nL]);
    Wire.write('D');
    Wire.endTransmission();
  }
}

void loop() {

  //Controle dos botões
  if (!digitalRead(pinBotaoA)) andarDestino = 1;
  if (!digitalRead(pinBotaoB)) andarDestino = 2;
  if (!digitalRead(pinBotaoC)) andarDestino = 3;
  if (!digitalRead(pinBotaoD)) andarDestino = 4;

  //Controle das chamadas dos andares
  for (int nL = 0; nL < 4; nL++) {
    Wire.requestFrom(enderecoAndar[nL], 1);
    if (Wire.available()) {
      char command = Wire.read();
      if (command != 'N') {
        andarDestino = nL + 1;
      }
    }
  }

  //Controle das ações
  if (andar == andarDestino) acaoElevador = 0;  //Elevador precisa parar
  if (andar < andarDestino) acaoElevador = 1;   //Elevador precisa subir
  if (andar > andarDestino) acaoElevador = 2;   //Elevador precisa descer

  //Controle dos andares
  estadoSensorA = !digitalRead(pinSensorA);
  estadoSensorB = !digitalRead(pinSensorB);
  estadoSensorC = !digitalRead(pinSensorC);
  estadoSensorD = !digitalRead(pinSensorD);

  if (estadoSensorA && !estadoSensorAAnt) andar = 1;
  if (estadoSensorB && !estadoSensorBAnt) andar = 2;
  if (estadoSensorC && !estadoSensorCAnt) andar = 3;
  if (estadoSensorD && !estadoSensorDAnt) andar = 4;

  //Executa a ação de acordo com o estado do elevador e os comandos recebidos pelos botões
  if (acaoElevador == 0) {
    if (acaoElevadorAnt == 1) {  //Estava subindo. Entao freia para baixo.
      analogWrite(pinMotorA, 255);
      analogWrite(pinMotorB, 255);
      tone(pinBuzzer, 800);
      delay(40);
      noTone(pinBuzzer);
    }
    if (acaoElevadorAnt == 2) {  //Estava descendo. Entao freia para cima.
      analogWrite(pinMotorA, 0);
      analogWrite(pinMotorB, velocidadeSobe);
      tone(pinBuzzer, 800);
      delay(40);
      noTone(pinBuzzer);
    }
    analogWrite(pinMotorA, 255);
    analogWrite(pinMotorB, 255);

    digitalWrite(pinLedA, LOW);
    digitalWrite(pinLedB, LOW);
    digitalWrite(pinLedC, LOW);
    digitalWrite(pinLedD, LOW);

    Wire.beginTransmission(enderecoAndar[andarDestino - 1]);
    Wire.write('S');
    Wire.endTransmission();

    Wire.beginTransmission(enderecoAndar[andarDestino - 1]);
    Wire.write('D');
    Wire.endTransmission();
  } else {
    digitalWrite(ledAndar[andarDestino - 1], HIGH);
  }

  if (acaoElevador == 1) {  //Subir o elevador
    analogWrite(pinMotorA, 0);
    analogWrite(pinMotorB, velocidadeSobe);
  }

  if (acaoElevador == 2) {  //Descer o elevador
    analogWrite(pinMotorA, velocidadeDesce);
    analogWrite(pinMotorB, 0);
  }

  //Atualiza o ponteiro dos andares no display
  lcd.setCursor(2, 2);
  lcd.print(ponteiro[digitalRead(pinSensorA)]);
  lcd.setCursor(7, 2);
  lcd.print(ponteiro[digitalRead(pinSensorB)]);
  lcd.setCursor(12, 2);
  lcd.print(ponteiro[digitalRead(pinSensorC)]);
  lcd.setCursor(17, 2);
  lcd.print(ponteiro[digitalRead(pinSensorD)]);

  //Atualiza o display dos andares
  for (int nL = 0; nL < 4; nL++) {
    Wire.beginTransmission(enderecoAndar[nL]);
    Wire.write(andar + '0');
    Wire.endTransmission();
  }

  estadoSensorAAnt = estadoSensorA;
  estadoSensorBAnt = estadoSensorB;
  estadoSensorCAnt = estadoSensorC;
  estadoSensorDAnt = estadoSensorD;

  acaoElevadorAnt = acaoElevador;
  andarAnt = andar;
  andarDestinoAnt = andarDestino;
}

void identificaAndar() {

  estadoSensorA = !digitalRead(pinSensorA);
  estadoSensorB = !digitalRead(pinSensorB);
  estadoSensorC = !digitalRead(pinSensorC);
  estadoSensorD = !digitalRead(pinSensorD);

  andar = -1;
  if (estadoSensorA) andar = 1;
  if (estadoSensorB) andar = 2;
  if (estadoSensorC) andar = 3;
  if (estadoSensorD) andar = 4;

  //Caso nenhum sensor detectado, desce o elevador até que um seja encontrado.
  if (andar == -1) {
    analogWrite(pinMotorA, velocidadeDesce);
    analogWrite(pinMotorB, 0);

    do {

      estadoSensorA = !digitalRead(pinSensorA);
      estadoSensorB = !digitalRead(pinSensorB);
      estadoSensorC = !digitalRead(pinSensorC);
      estadoSensorD = !digitalRead(pinSensorD);

      if (estadoSensorA) andar = 1;
      if (estadoSensorB) andar = 2;
      if (estadoSensorC) andar = 3;
      if (estadoSensorD) andar = 4;

      if (andar != -1) {
        analogWrite(pinMotorA, 0);
        analogWrite(pinMotorB, velocidadeSobe);
        tone(pinBuzzer, 800);
        delay(40);
        noTone(pinBuzzer);
        analogWrite(pinMotorA, 255);
        analogWrite(pinMotorB, 255);

        break;
      }
    } while (andar == -1);
  }

  andarDestino = andar;
}
