
#include <SD.h>
#include <Process.h>

#include <SPI.h>
#include <pcmConfig.h>
#include <pcmRF.h>
#include <TMRpcm.h>

int incomingByte;
const int led3 = 3;
const int led2 = 2;
const int led4 = 4;
const int led5 = 5;

const int buttonPin = 7;
int buttonState = 0;

const int recordLed = 8;

TMRpcm audio;
int file_number = 0;
char filePrefixname[50] = "record";
char exten[10] = ".wav";
const int mic_pin = A0;
const int sample_rate = 19200;

#define SD_CSPin 10
void wait_min(int mins) {
  int count = 0;
  int secs = mins * 60;
  while (1) {
    Serial.print('.');
    delay(1000);
    count++;
    if (count == secs) {
      count = 0;
      break;
    }
  }
  Serial.println();
  return ;
}


void setup() {
  Serial.begin(115200);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);

  pinMode(buttonPin, INPUT);
  pinMode(mic_pin, INPUT);
  pinMode(recordLed, OUTPUT);


  Serial.println("loading... SD card");
  if (!SD.begin(SD_CSPin)) {
    Serial.println("An Error has occurred while mounting SD");
  }
  while (!SD.begin(SD_CSPin)) {
    Serial.print(".");
    delay(500);
  }
  audio.CSPin = SD_CSPin;
  Serial.println("SD card... OK");
  delay(500);
  //Serial.println("device ready");

}

void loop() {

  buttonState = digitalRead(buttonPin);

  int sensorValue = analogRead(A0);

  char fileSlNum[20] = "";
  itoa(file_number, fileSlNum, 10);
  char file_name[50] = "";
  strcat(file_name, filePrefixname);
  strcat(file_name, fileSlNum);
  strcat(file_name, exten);

  if (buttonState == HIGH) {
    //digitalWrite(recordLed, HIGH);
    Serial.print("New File Name: ");
    //delay(1000);
    Serial.println(file_name);
    //delay(1000);
    digitalWrite(recordLed, HIGH);
    audio.startRecording(file_name, sample_rate, mic_pin);
    Serial.println("startRecording ");
    delay(12000);
    digitalWrite(recordLed, LOW);
    audio.stopRecording(file_name);
    Serial.println("stopRecording");
    file_number++;
    delay(1500);
    File myFile = SD.open(file_name);
    if (myFile) {
      Serial.println("START_FILE");
      while (myFile.available() > 0) {
        Serial.write(myFile.read());
      }
      myFile.close();
      Serial.println("");
      Serial.println("file sent");
    } else {
      Serial.println("error opening file");
    }
    digitalWrite(recordLed, LOW);
  }
  else {
    digitalWrite(recordLed, LOW);
  }
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (incomingByte == '1') {
      digitalWrite(led2, HIGH);
      delay(5000);
      digitalWrite(led2, LOW);
    }
    if (incomingByte == '2') {
      digitalWrite(led3, HIGH);
      delay(5000);
      digitalWrite(led3, LOW);
    }
    if (incomingByte == '3') {
      digitalWrite(led4, HIGH);
      delay(5000);
      digitalWrite(led4, LOW);
    }
    if (incomingByte == '4') {
      digitalWrite(led5, HIGH);
      delay(5000);
      digitalWrite(led5, LOW);
    }
    if (incomingByte == '5') {
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      delay(5000);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
    }
  }
}
