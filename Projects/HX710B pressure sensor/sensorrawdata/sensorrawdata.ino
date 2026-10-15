#define HX_DATA_PIN  34   // Dout
#define HX_CLK_PIN   33   // SCK
#define PUMP_PIN     21
#define RELEASE_VALVE 42
#define VALVE1       26
#define VALVE2       47
#define VALVE3       39
#define VALVE4       40
#define VALVE5       41

long readHX710B() {
  unsigned long t = millis();
  while (digitalRead( HX_DATA_PIN) == HIGH) {
    if (millis() - t > 200) return -1L;
    delayMicroseconds(10);
  }
  delayMicroseconds(100);
  long raw = 0;
  for (int i = 0; i < 24; i++) {
    digitalWrite(HX_CLK_PIN, HIGH);
    delayMicroseconds(5);
    raw = (raw << 1) | digitalRead(HX_DATA_PIN);
    digitalWrite(HX_CLK_PIN, LOW);
    delayMicroseconds(5);
  }
  digitalWrite(HX_CLK_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(HX_CLK_PIN, LOW);
  delayMicroseconds(5);
  if (raw & 0x800000) raw |= 0xFF000000;
  Serial.println(raw);
  return raw;
}

void setup() {
  Serial.begin(115200);
  delay(1500);
    pinMode(PUMP_PIN, OUTPUT);
  pinMode(HX_CLK_PIN, OUTPUT);
  pinMode(HX_DATA_PIN, INPUT);
    // ── Force all outputs LOW during calibration ──
  pinMode(21, OUTPUT); digitalWrite(21, LOW);   // PUMP
  pinMode(42, OUTPUT); digitalWrite(42, LOW);   // RELEASE_VALVE
  pinMode(26, OUTPUT); digitalWrite(26, LOW);   // VALVE1
  pinMode(47, OUTPUT); digitalWrite(47, LOW);   // VALVE2
  pinMode(39, OUTPUT); digitalWrite(39, LOW);   // VALVE3
  pinMode(40, OUTPUT); digitalWrite(40, LOW);   // VALVE4
  pinMode(41, OUTPUT); digitalWrite(41, LOW);   // VALVE5
  Serial.println(F("\nVerveFlo Pressure Calibration"));
  Serial.println(F("type: raw | zero | stream | m <val> | show"));
  pinMode(HX_DATA_PIN , INPUT);
  pinMode(HX_CLK_PIN, OUTPUT);
  digitalWrite(HX_CLK_PIN, LOW);
}

void loop() {
  readHX710B();
  delay(200);
}