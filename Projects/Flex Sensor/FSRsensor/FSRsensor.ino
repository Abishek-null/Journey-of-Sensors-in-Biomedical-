/*
 * FSR FORCE -> PRESSURE CONVERSION
 *
 * Conversion:
 * ADC
 *  ↓
 * FSR Resistance
 *  ↓
 * Force (kgf / kp)
 *  ↓
 * Force (N)
 *  ↓
 * Pressure (Pa)
 *  ↓
 * Pressure (mmHg)
 *
 * ESP32 Arduino
 */

#include <Arduino.h>
#include <math.h>

/* =========================================================
 *                    PIN CONFIGURATION
 * ========================================================= */

#define FSR_ADC_PIN       34
#define FSR_ZERO_ADC 1783.0f

/* =========================================================
 *                    ADC CONFIGURATION
 * ========================================================= */

#define ADC_MAX_VALUE     4095.0f
#define ADC_VREF          3.3f

/* ESP32 ADC attenuation */
#define FSR_ADC_ATTENUATION ADC_11db

/* =========================================================
 *                VOLTAGE DIVIDER CONFIGURATION
 *
 *        3.3V
 *         |
 *        R_FIXED
 *         |
 *         +------ ADC
 *         |
 *        FSR
 *         |
 *        GND
 *
 * Vadc = Vcc * R_FSR / (R_FIXED + R_FSR)
 *
 * Therefore:
 *
 * R_FSR = R_FIXED * Vadc / (Vcc - Vadc)
 * ========================================================= */

#define VCC_FSR            3.3f
#define R_FIXED_OHMS       10000.0f

/* =========================================================
 *                  MECHANICAL PARAMETERS
 * ========================================================= */

/*
 * Effective force area.
 *
 * Example:
 * 10 cm² = 0.001 m²
 *
 * CHANGE THIS VALUE according to your actual sensor/cuff
 * effective contact area.
 */

#define EFFECTIVE_AREA_CM2    10.0f

#define EFFECTIVE_AREA_M2 \
    (EFFECTIVE_AREA_CM2 * 0.0001f)

/* =========================================================
 *                  PHYSICAL CONSTANTS
 * ========================================================= */

#define KG_FORCE_TO_NEWTON    9.80665f
#define PA_PER_MMHG           133.322368f

/* =========================================================
 *                FSR CALIBRATION TABLE
 *
 * IMPORTANT:
 *
 * These are EXAMPLE values only.
 *
 * You MUST replace them with your measured calibration
 * values.
 *
 * ADC -> Force kgf
 *
 * The table should be generated using known weights.
 * ========================================================= */

#define CAL_POINTS 7

const float calibrationADC[CAL_POINTS] =
{
    100.0f,
    600.0f,
    1100.0f,
    1600.0f,
    2100.0f,
    2600.0f,
    3100.0f
};

const float calibrationKg[CAL_POINTS] =
{
    0.0f,
    0.5f,
    1.0f,
    1.5f,
    2.0f,
    2.5f,
    3.0f
};

/* =========================================================
 *                     FILTER CONFIG
 * ========================================================= */

#define FILTER_SAMPLES       20

float adcFiltered = 0.0f;

/* =========================================================
 *                     FUNCTION PROTOTYPES
 * ========================================================= */

float readFSRADC(void);
float calculateFSRResistance(float adc);
float interpolateForceKg(float adc);
float calculateForceNewton(float forceKg);
float calculatePressurePa(float forceNewton);
float calculatePressureMmHg(float forceNewton);

float lowPassFilter(float previous, float input, float alpha);

void printMeasurements(float adc,
                       float resistance,
                       float forceKg,
                       float forceN,
                       float pressurePa,
                       float pressureMmHg);
float zeroOffset = 0.0f;

void calibrateFSRZero(void)
{
    const uint16_t samples = 200;

    uint64_t total = 0;

    Serial.println("Remove all force from FSR...");
    delay(3000);

    for (uint16_t i = 0; i < samples; i++)
    {
        total += analogRead(FSR_ADC_PIN);
        delay(10);
    }

    zeroOffset = (float)total / samples;

    Serial.print("FSR ZERO ADC = ");
    Serial.println(zeroOffset, 2);
}

/* =========================================================
 *                         SETUP
 * ========================================================= */

void setup(void)
{
    Serial.begin(115200);

    delay(1000);

    analogReadResolution(12);

    analogSetPinAttenuation(
        FSR_ADC_PIN,
        FSR_ADC_ATTENUATION
    );

    Serial.println();
    Serial.println("========================================");
    Serial.println("       FSR FORCE / PRESSURE TEST");
    Serial.println("========================================");

    Serial.print("FSR ADC Pin          : ");
    Serial.println(FSR_ADC_PIN);

    Serial.print("Fixed resistor       : ");
    Serial.print(R_FIXED_OHMS);
    Serial.println(" ohm");

    Serial.print("Effective area       : ");
    Serial.print(EFFECTIVE_AREA_CM2);
    Serial.println(" cm2");

    Serial.print("Effective area       : ");
    Serial.print(EFFECTIVE_AREA_M2, 6);
    Serial.println(" m2");

    Serial.println();
    Serial.println("Starting measurements...");
    Serial.println();
    calibrateFSRZero();
}

/* =========================================================
 *                          LOOP
 * ========================================================= */

void loop(void)
{
    /* ---------------------------------------------
     * 1. Read ADC
     * --------------------------------------------- */

    float adc = readFSRADC();

    /* ---------------------------------------------
     * 2. Filter ADC
     * --------------------------------------------- */

    adcFiltered = lowPassFilter(
        adcFiltered,
        adc,
        0.15f
    );

    /* ---------------------------------------------
     * 3. Calculate FSR resistance
     * --------------------------------------------- */

    float resistance =
        calculateFSRResistance(adcFiltered);

    /* ---------------------------------------------
     * 4. Convert ADC -> kgf
     * --------------------------------------------- */

    float forceKg =
        interpolateForceKg(adcFiltered);

    /* ---------------------------------------------
     * 5. kgf -> Newton
     * --------------------------------------------- */

    float forceN =
        calculateForceNewton(forceKg);

    /* ---------------------------------------------
     * 6. Newton -> Pascal
     * --------------------------------------------- */

    float pressurePa =
        calculatePressurePa(forceN);

    /* ---------------------------------------------
     * 7. Pascal -> mmHg
     * --------------------------------------------- */

    float pressureMmHg =
        calculatePressureMmHg(forceN);

    /* ---------------------------------------------
     * 8. Print results
     * --------------------------------------------- */

    printMeasurements(
        adcFiltered,
        resistance,
        forceKg,
        forceN,
        pressurePa,
        pressureMmHg
    );

    delay(200);
}

/* =========================================================
 *                    READ FSR ADC
 * ========================================================= */

float readFSRADC(void)
{
    uint32_t total = 0;

    for (uint8_t i = 0; i < FILTER_SAMPLES; i++)
    {
        total += analogRead(FSR_ADC_PIN);

        delayMicroseconds(500);
    }

    return (float)total / FILTER_SAMPLES;
}

/* =========================================================
 *                ADC -> FSR RESISTANCE
 * ========================================================= */

float calculateFSRResistance(float adc)
{
    if (adc <= 0.0f)
    {
        return INFINITY;
    }

    if (adc >= ADC_MAX_VALUE)
    {
        return 0.0f;
    }

    float voltage =
        (adc / ADC_MAX_VALUE) * VCC_FSR;

    if (voltage >= VCC_FSR)
    {
        return 0.0f;
    }

    float resistance =
        R_FIXED_OHMS *
        voltage /
        (VCC_FSR - voltage);

    return resistance;
}

/* =========================================================
 *            ADC -> FORCE USING LOOKUP TABLE
 *
 * Linear interpolation between calibration points.
 * ========================================================= */

float interpolateForceKg(float adc)
{
    /*
     * Below minimum calibration point
     */

    if (adc <= calibrationADC[0])
    {
        return calibrationKg[0];
    }

    /*
     * Above maximum calibration point
     */

    if (adc >= calibrationADC[CAL_POINTS - 1])
    {
        return calibrationKg[CAL_POINTS - 1];
    }

    /*
     * Find calibration interval
     */

    for (uint8_t i = 0; i < CAL_POINTS - 1; i++)
    {
        if (adc >= calibrationADC[i] &&
            adc <= calibrationADC[i + 1])
        {
            float adc1 = calibrationADC[i];
            float adc2 = calibrationADC[i + 1];

            float kg1 = calibrationKg[i];
            float kg2 = calibrationKg[i + 1];

            float ratio =
                (adc - adc1) /
                (adc2 - adc1);

            return kg1 +
                   ratio * (kg2 - kg1);
        }
    }

    return 0.0f;
}

/* =========================================================
 *                  kgf -> NEWTON
 * ========================================================= */

float calculateForceNewton(float forceKg)
{
    return forceKg * KG_FORCE_TO_NEWTON;
}

/* =========================================================
 *                  NEWTON -> PASCAL
 * ========================================================= */

float calculatePressurePa(float forceNewton)
{
    if (EFFECTIVE_AREA_M2 <= 0.0f)
    {
        return 0.0f;
    }

    return forceNewton /
           EFFECTIVE_AREA_M2;
}

/* =========================================================
 *                 NEWTON -> mmHg
 * ========================================================= */

float calculatePressureMmHg(float forceNewton)
{
    float pressurePa =
        calculatePressurePa(forceNewton);

    return pressurePa /
           PA_PER_MMHG;
}

/* =========================================================
 *                    LOW PASS FILTER
 * ========================================================= */

float lowPassFilter(float previous,
                    float input,
                    float alpha)
{
    return previous +
           alpha * (input - previous);
}

/* =========================================================
 *                    PRINT DATA
 * ========================================================= */

void printMeasurements(float adc,
                       float resistance,
                       float forceKg,
                       float forceN,
                       float pressurePa,
                       float pressureMmHg)
{
    Serial.print("ADC = ");
    Serial.print(adc, 1);

    Serial.print(" | R = ");

    if (isinf(resistance))
    {
        Serial.print("INF");
    }
    else
    {
        Serial.print(resistance, 1);
        Serial.print(" ohm");
    }

    Serial.print(" | Force = ");
    Serial.print(forceKg, 3);
    Serial.print(" kgf");

    Serial.print(" | ");
    Serial.print(forceKg, 3);
    Serial.print(" kp");

    Serial.print(" | ");
    Serial.print(forceN, 3);
    Serial.print(" N");

    Serial.print(" | Pressure = ");
    Serial.print(pressurePa, 1);
    Serial.print(" Pa");

    Serial.print(" | ");
    Serial.print(pressureMmHg, 2);
    Serial.println(" mmHg");
}