#include <LowPower.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

#define INPUT_PIN_INT   2

// target 2mhz clock
#define CLK_PRESCALE_FACTOR 4
#define CLK_PRESCALE_HEX 0x02

volatile byte count = 255;
byte digits[] = {12, 13};
byte segments[] = {11, 10, 5, 8, 3, 16, 15};
byte num[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1}
};

void int_isr(void)
{
  count = 20;
}

void setup(void)
{
  Serial.begin(9600);
  while (!accel.begin())
  {
    Serial.println("Accel init failed.");
  }

  Serial.println("Accel init complete.");

  accel.setRange(ADXL343_RANGE_16_G);

  pinMode(INPUT_PIN_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN_INT), int_isr, RISING);

  int_config g_int_config_enabled = { 0 };
  int_config g_int_config_map = { 0 };

  g_int_config_enabled.bits.activity = true;
  accel.enableInterrupts(g_int_config_enabled);

  g_int_config_map.bits.activity = ADXL343_INT2;
  accel.mapInterrupts(g_int_config_map);

  accel.writeRegister(ADXL343_REG_ACT_INACT_CTL, 0x70);
  accel.writeRegister(ADXL343_REG_THRESH_ACT, 20);
  accel.writeRegister(ADXL343_REG_BW_RATE, 0x07 | (1 << 4));

  Serial.println("accel setup complete");

  for (byte i = 0; i < 7; ++i)
  {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(6, OUTPUT);

  // set ADC reference to 1.1v
  analogReference(INTERNAL);

  // slow down clock
  CLKPR = 0x80;
  CLKPR = CLK_PRESCALE_HEX;
}

void print7(byte c)
{
  byte a, b;

  a = c / 10;
  b = c % 10;

  if (a != 0)
  {
    for (byte i = 0; i < 7; ++i)
    {
      digitalWrite(segments[i], num[a][i]);
    }
    digitalWrite(digits[1], 0);
    delay(5 / CLK_PRESCALE_FACTOR);
    digitalWrite(digits[1], 1);
  }

  for (byte i = 0; i < 7; ++i)
  {
    digitalWrite(segments[i], num[b][i]);
  }
  digitalWrite(digits[0], 0);
  delay(5 / CLK_PRESCALE_FACTOR);
  digitalWrite(digits[0], 1);
}

void clear(void)
{
  for (byte i = 0; i < 7; ++i)
  {
    digitalWrite(segments[i], 0);
  }
  digitalWrite(13, 0);
}

void showVoltage(byte v)
{
  unsigned long time = millis();
  
  Serial.println(v);
  while (1)
  {
      print7(v);
      if (millis() - time > 1000 / CLK_PRESCALE_FACTOR)
      {
        break;
      }
      delay(10 / CLK_PRESCALE_FACTOR);
  }
}
byte v;
void loop(void)
{
  v = analogRead(A7) / 31.03;
  sensors_event_t event;
  accel.getEvent(&event);

  unsigned long time = millis();
  byte tmp = count;
  bool interrupt = tmp != 255;
  while (tmp != 255)
  {
    accel.checkInterrupts();
    print7(tmp);
    if (millis() - time > 1000 / CLK_PRESCALE_FACTOR)
    {
      count--;
      time = millis();
    }
    delay(10 / CLK_PRESCALE_FACTOR);
    tmp = count;
  }

  if (interrupt)
  {
    showVoltage(v);
  }
  
  clear();
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}
