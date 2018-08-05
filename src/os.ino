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

AudioControlSGTL5000     sgtl5000_1;

int upperPotInput = 20;
int lowerPotInput = 21;

float octave = 1.0;
const float vPerSemitone = 0.08333;

int scale = 0, note = 0;
bool busy = false;

int scales[4][5] = {
  { 0, 3, 5, 7, 10 },               // Minor C Pentatonic
  { 0, 4, 7, 11, 12 },              // Paradiso
  { 8, 14, 12, 17, 19 }             // Pygmy
};

String style[] = { "Up", "Down", "Up/Down", "Down/Up", "Random" };

void setup() {
  Serial.begin(9600);
  AudioMemory(20);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.82);
  sgtl5000_1.adcHighPassFilterDisable();
  sgtl5000_1.lineInLevel(0,0);
  sgtl5000_1.unmuteHeadphone();
}

void listenPots() {
  int upperPotValue = analogRead(upperPotInput); // scale selector
  scale = map(upperPotValue, 0, 1023, 0, 2);

  // int lowerPotValue = analogRead(lowerPotInput); // arpeggio style
  delay(5);
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
  listenPots();

  if (clkIn.available()) {
    sync();
  }
}
