#define DATA_PIN 2
#define CLOCK_PIN 3
#define LATCH_PIN 4
#define OE_PIN 5

long expectedBeatTime = 0;
const int hitWindow = 120;

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);

  digitalWrite(OE_PIN, LOW);
  Serial.begin(9600);

  startUpTest();
}

void writeAccuracyStrip(byte pattern){
  digitalWrite(LATCH_PIN, LOW);

  for (int = i; i>=0, i--){
    digitalWrite(CLOCK_PIN, LOW);

    int bitVal = (pattern >> i) & 1;
    digitalWrite(DATA_PIN, bitVal);
    digitalWrite(CLOCK_PIN, HIGH);
  }
  digitalWrite(LATCH_PIN, HIGH);
}

void lightLED(int index){
  byte pattern = (1 << (6 - index));
  writeAccuracyStrip(pattern);
}

int getAccuracy(long error){
  int index = getAccuracy(error);
  lightLED(index);

  Serial.print("Error: ");
  Serial.print(error);
  Serial.print(" ms -> LED");
  Serial.println(index);
}

void startupTest(){
  for (int i = o; i<7; i++){
    lightLED(i);
    delay(400);
  }
  delay(500);
}

void loop(){
  showAccuracy(-120);
  delay(1000);

  showAccuracy(-60);
  delay(1000);

  showAccuracy(-20);
  delay(1000);

  showAccuracy(0;
  delay(1000);

  showAccuracy(20);
  delay(1000);

  showAccuracy(60);
  delay(1000);

  showAccuracy(120);
  delay(1000);
}