#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad bleGamepad;

const int potPin = 34;                // Potentiometer is connected to GPIO 34 (Analog ADC1_CH6)
const int numberOfPotSamples = 5;     // Number of pot samples to take (to smooth the values)
const int delayBetweenSamples = 4;    // Delay in milliseconds between pot samples
const int delayBetweenHIDReports = 5; // Additional delay in milliseconds between HID reports

// The following defines which pins are connected where

const int xPin = 39;
const int yPin = 36;
const int aButton = 27;
const int bButton = 26;

int aPrev = LOW, bPrev = LOW, aCurr, bCurr;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE work!");
    bleGamepad.begin();

    pinMode(xPin, INPUT);
    pinMode(yPin, INPUT);
    pinMode(aButton, INPUT);
    pinMode(bButton, INPUT);
}

void loop()
{
    if (bleGamepad.isConnected())
    {
        int xValues[numberOfPotSamples], yValues[numberOfPotSamples]; // Array to store pot readings
        int xValue = 0, yValue = 0;                  // Variable to store calculated pot reading average

        // Populate readings
        for (int i = 0; i < numberOfPotSamples; i++)
        {
            xValues[i] = analogRead(xPin);
            yValues[i] = analogRead(yPin);
            xValue += xValues[i];
            yValue += yValues[i];
            delay(delayBetweenSamples);
        }

        // Calculate the average
       xValue /= numberOfPotSamples;
       yValue /= numberOfPotSamples;

        // Map analog reading from 0 ~ 4095 to 32737 ~ 0 for use as an axis reading
        int adjustedXValue = map(xValue, 0, 4095, 32737, 0);
        int adjustedYValue = map(yValue, 0, 4095, 32737, 0);

        // Update X axis and auto-send report
        bleGamepad.setX(adjustedXValue);
        bleGamepad.setY(adjustedYValue);
        aCurr = digitalRead(aButton);
        bCurr = digitalRead(bButton);

        if (aCurr != aPrev)
          if (aCurr == HIGH)
            bleGamepad.press(BUTTON_1);
           else
            bleGamepad.release(BUTTON_1);
        
        if (bCurr != bPrev)
          if (bCurr == HIGH)
            bleGamepad.press(BUTTON_2);
           else
            bleGamepad.release(BUTTON_2);
            
        aPrev = aCurr;
        bPrev = bCurr;
        delay(delayBetweenHIDReports);
    }
}
