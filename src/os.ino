#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioInputI2S            audioInput;
AudioAnalyzePeak         clkIn;
AudioConnection          patchCord1(audioInput, 0, clkIn, 0);

AudioOutputI2S           audioOutput;
AudioSynthWaveformDc     cvOut1;
AudioConnection          patchCord2(cvOut1, 0, audioOutput, 0);

int scales[1][5] = {
  { 0, 3, 5, 7, 10 }  // pentatonicMinC[5]
};

float octave = 1.0;
const float vPerSemitone = 0.08333;

int scale = 0, note = 0;
bool busy = false;

void setup() {
  Serial.begin(9600);
  AudioMemory(20);
}

void sync() {
  float gateVal = clkIn.read();
  if (gateVal > 0.9 && busy == false) {
    busy = true;
    float cv = (float) (octave + scales[scale][note++] * vPerSemitone) * 0.2;
    cvOut1.amplitude(cv);
    if (note > 4) {
      note = 0;
    }
  }
  if (gateVal < 0.2) {
    busy = false;
  }
}

void loop() {
  if (clkIn.available()) {
    sync();
  }
}
