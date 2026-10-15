#define BATT_PIN 15

// Calibration obtained from your measurements
const float ADC_TO_BATTERY = 7.005f / 3278.0f;

void setup()
{
    Serial.begin(115200);
    analogReadResolution(12);
    analogSetPinAttenuation(BATT_PIN, ADC_11db);
}

void loop()
{
    uint32_t sum = 0;

    for (int i = 0; i < 500; i++)
    {
        sum += analogRead(BATT_PIN);
        delayMicroseconds(100);
    }

    uint16_t raw = sum / 500;

    float batteryVoltage = raw * ADC_TO_BATTERY;

    Serial.print("Raw = ");
    Serial.print(raw);

    Serial.print("   Battery = ");
    Serial.print(batteryVoltage, 3);
    Serial.println(" V");

    delay(1000);
}