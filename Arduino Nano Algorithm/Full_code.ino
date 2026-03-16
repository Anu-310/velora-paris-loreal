#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <DHT.h>

MAX30105 particleSensor;

#define DHTPIN 2
#define DHTTYPE DHT11
                     
DHT dht(DHTPIN, DHTTYPE);

long lastBeat = 0;
float beatsPerMinute;

int ledGreen = 6;
int ledYellow = 7;

int solenoid1 = 9;
int solenoid2 = 10;

int bpmReadings[5];
int count = 0;
int ignoreCount = 0;

bool measurementDone = false;

void setup()
{
  Serial.begin(115200);

  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);

  pinMode(solenoid1, OUTPUT);
  pinMode(solenoid2, OUTPUT);

  digitalWrite(ledGreen, LOW);
  digitalWrite(ledYellow, LOW);

  digitalWrite(solenoid1, LOW);
  digitalWrite(solenoid2, LOW);

  dht.begin();

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    Serial.println("MAX30101 not found");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  Serial.println("Place finger on sensor...");
}

void loop()
{
  long irValue = particleSensor.getIR();

  // Reset if finger removed
  if (irValue < 50000)
  {
    Serial.println("Waiting for finger...");

    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYellow, LOW);

    count = 0;
    ignoreCount = 0;
    measurementDone = false;

    delay(200);
    return;
  }

  if (!measurementDone && checkForBeat(irValue))
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute > 20 && beatsPerMinute < 220)
    {

      // Ignore unstable beats
      if (ignoreCount < 3)
      {
        ignoreCount++;
        Serial.println("Ignoring unstable reading...");
        return;
      }

      bpmReadings[count] = beatsPerMinute;

      Serial.print("Reading ");
      Serial.print(count + 1);
      Serial.print(": ");
      Serial.println(beatsPerMinute);

      count++;

      if (count == 5)
      {
        int sum = 0;

        for (int i = 0; i < 5; i++)
        {
          sum += bpmReadings[i];
        }

        int avgBPM = sum / 5;

        Serial.print("Average BPM: ");
        Serial.println(avgBPM);

        // Read temperature & humidity
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");

        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");

        // LED + Solenoid decision based on BPM
        if (avgBPM >= 70)
        {
          digitalWrite(ledGreen, HIGH);
          digitalWrite(ledYellow, LOW);

          digitalWrite(solenoid1, HIGH);
          delay(800);
          digitalWrite(solenoid1, LOW);
        }
        else
        {
          digitalWrite(ledGreen, LOW);
          digitalWrite(ledYellow, HIGH);

          digitalWrite(solenoid2, HIGH);
          delay(800);
          digitalWrite(solenoid2, LOW);
        }

        measurementDone = true;
      }
    }
  }
}
