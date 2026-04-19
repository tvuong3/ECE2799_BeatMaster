#include metronome.h

#define PIEZO_PIN 14
#define LED_STRIP_PIN 
#define LED_RING_PIN 
#define SPEAKER_PIN 

Metronome metro(SPEAKER_PIN);

int hitThreshold = 
bool hitDetected = false;

unsigned long exerciseStartTime = 0;
unsigned long expectedBeatTime = 0;
const unsigned long exercise_Length = 30000;
bool exerciseRun = false;

int totalHits;
int correctHits;

void setup() {
// put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_STRIP_PIN, OUTPUT);
  pinMode(LED_RING_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(PIEZO_PIN, INPUT);

  metro.begin();
  metro.setBeatsPerMinute();
  metro.setMeasure(4);
  metro.start();

  exerciseStartTime = millis();
  excersieRun true;
}

void loop() {
// put your main code here, to run repeatedly:
  if (!exerciseRun) return;
  metro.check();
  readSensor();
  runMetronome();
  runExerciseTimer();
}

void readSensor() {
  int sensorValue = analogRead(PIEZO_PIN);

  if (sensorValue > hitThreshold && millis() - lastHitTime > debounceTime) {
    lastHitTime = millis();
    processHit(lastHitTime);
  }
}

void runExerciseTimer() {
  if (millis() - exerciseStartTime >= exercise_Length){
    exerciseRun = false;

    Serial.print("Hits: ");
    Serial.println(totalHits);
    Serial.print("Correct: ");
    Serial.printlnn(correctHits);
  }
}

void processHit() {
  totalHits++;

  long error = hitTime - expectedBeatTime;
  long absError = abs(error);

  if (abs(error) <= 30){
    correctHits++;
    showFeedback(1); //perfect hit
  }
  else if (abs(error) <= 80){
    showFeedback(2); //close hit
  }
  else{
    showFeedback(3); //missed hit
  }
}

void showFeedback(){
// LED STRIP - RED|YELLOW|YELLOW|GREEN|YELLOW|YELLOW|RED
            //  0     1     2      3     4      5     6

  if (type == 1)
  if (type == 2)
  if (type == 3)
}
