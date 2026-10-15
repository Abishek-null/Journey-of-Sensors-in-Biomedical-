#include <Arduino.h>
#include <math.h>

/* =========================================================
 *                  PIN CONFIGURATION
 * ========================================================= */

#define FSR_ADC_PIN       34

/* =========================================================
 *                  CIRCUIT CONFIGURATION
 * ========================================================= */

/*
 * Circuit:
 *
 *        3.3V
 *          |
 *         FSR
 *          |
 *          +-------- GPIO34
 *          |
 *        10 kΩ
 *          |
 *         GND
 */

#define VCC_FSR           3.3f
#define R_FIXED_OHMS      10000.0f

/* 12-bit ESP32 ADC */
#define ADC_MAX_VALUE     4095.0f

/* Number of samples to average */
#define ADC_SAMPLES       20


/* =========================================================
 *                  READ ADC
 * ========================================================= */

float readFSR_ADC(void)
{
    uint32_t adcTotal = 0;

    for (uint8_t i = 0; i < ADC_SAMPLES; i++)
    {
        adcTotal += analogRead(FSR_ADC_PIN);
        delayMicroseconds(500);
    }

    return (float)adcTotal / ADC_SAMPLES;
}


/* =========================================================
 *                  ADC TO VOLTAGE
 * ========================================================= */

float adcToVoltage(float adc)
{
    return (adc / ADC_MAX_VALUE) * VCC_FSR;
}


/* =========================================================
 *                VOLTAGE TO RESISTANCE
 *
 * Circuit:
 *
 *             3.3V
 *              |
 *             FSR
 *              |
 *              +------ ADC
 *              |
 *             10kΩ
 *              |
 *             GND
 *
 * VADC = VCC × R_FIXED / (R_FSR + R_FIXED)
 *
 * Therefore:
 *
 * R_FSR = R_FIXED × (VCC - VADC) / VADC
 *
 * ========================================================= */

float calculateFSRResistance(float voltage)
{
    if (voltage <= 0.001f)
    {
        return INFINITY;
    }

    if (voltage >= VCC_FSR)
    {
        return 0.0f;
    }

    return R_FIXED_OHMS *
           (VCC_FSR - voltage) /
           voltage;
}


/* =========================================================
 *                       SETUP
 * ========================================================= */

void setup(void)
{
    Serial.begin(115200);

    delay(1000);

    analogReadResolution(12);

    analogSetPinAttenuation(
        FSR_ADC_PIN,
        ADC_11db
    );

    Serial.println();
    Serial.println("==========================================");
    Serial.println("       ESP32 FSR RESISTANCE TEST");
    Serial.println("==========================================");

    Serial.println("Circuit:");
    Serial.println("3.3V -> FSR -> GPIO34 -> 10k -> GND");

    Serial.println();

    Serial.print("ADC Pin       : GPIO ");
    Serial.println(FSR_ADC_PIN);

    Serial.print("VCC           : ");
    Serial.print(VCC_FSR, 2);
    Serial.println(" V");

    Serial.print("Fixed Resistor: ");
    Serial.print(R_FIXED_OHMS, 0);
    Serial.println(" ohm");

    Serial.println();
    Serial.println("Apply force/weight to the FSR.");
    Serial.println();
}


/* =========================================================
 *                        LOOP
 * ========================================================= */

void loop(void)
{
    /* Read ADC */
    float adc = readFSR_ADC();

    /* ADC -> Voltage */
    float voltage = adcToVoltage(adc);

    /* Voltage -> FSR resistance */
    float resistance = calculateFSRResistance(voltage);


    /* =====================================================
     *                    SERIAL OUTPUT
     * ===================================================== */

    Serial.print("ADC = ");
    Serial.print(adc, 1);

    Serial.print(" | Voltage = ");
    Serial.print(voltage, 3);
    Serial.print(" V");

    Serial.print(" | FSR Resistance = ");

    if (isinf(resistance))
    {
        Serial.println("INF ohm");
    }
    else
    {
        Serial.print(resistance, 1);
        Serial.println(" ohm");
    }

    delay(200);
}