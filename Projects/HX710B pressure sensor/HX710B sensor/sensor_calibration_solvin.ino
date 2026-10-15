#define PRESSURE_PIN 34

float V0 = 2.5387;           // Measured zero voltage
float SLOPE_KPA = 164.65;    // Calculated from real calibration
float ZERO_DEADBAND_MMHG = 2.0;

float filteredVoltage = 0;
float alpha = 0.08;

void setup() {
  Serial.begin(115200);
  analogSetPinAttenuation(PRESSURE_PIN, ADC_11db);
}

float readFilteredVoltage() {
  long sum = 0;

  for(int i = 0; i < 60; i++)
    sum += analogReadMilliVolts(PRESSURE_PIN);

  float voltage = (sum / 60.0) / 1000.0;

  // IIR filter
  filteredVoltage = alpha * voltage + (1 - alpha) * filteredVoltage;

  return filteredVoltage;
}

void loop() {

  float voltage = readFilteredVoltage();

  float pressure_kPa = (V0 - voltage) * SLOPE_KPA;
  float pressure_mmHg = pressure_kPa * 7.50062;

  // Zero deadband
  if (abs(pressure_mmHg) < ZERO_DEADBAND_MMHG) {
    pressure_mmHg = 0;
    pressure_kPa = 0;
  }

  Serial.print("Pressure: ");
  Serial.print(pressure_kPa, 3);
  Serial.print(" kPa | ");
  Serial.print(pressure_mmHg, 1);
  Serial.println(" mmHg");

  delay(200);
}