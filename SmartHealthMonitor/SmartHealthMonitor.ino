#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MLX90614.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);


Adafruit_MLX90614 mlx = Adafruit_MLX90614();


int pulsePin = A0;
int signal;


float tempAvg = 0;
float signalAvg = 0;


int BPM = 0;
int lastValidBPM = 0;

unsigned long lastBeatTime = 0;
int threshold = 480;  


float smoothFloat(float newVal, float &avg, float alpha) {
  avg = (avg * (1 - alpha)) + (newVal * alpha);
  return avg;
}

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  mlx.begin();

  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(3000);
  lcd.clear();
}

void loop() {

  
  float temp = mlx.readObjectTempC();
  temp = smoothFloat(temp, tempAvg, 0.2);

  
  signal = analogRead(pulsePin);
  signal = smoothFloat(signal, signalAvg, 0.4);

 
  static int lastSignal = 0;

  if (signal > threshold && lastSignal <= threshold) {
    unsigned long currentTime = millis();

    if (lastBeatTime != 0) {
      int interval = currentTime - lastBeatTime;

      if (interval > 300 && interval < 2000) { 
        BPM = 60000 / interval;

        
        lastValidBPM = BPM;
      }
    }

    lastBeatTime = currentTime;
  }

  lastSignal = signal;

  
  if (BPM == 0) {
    BPM = lastValidBPM;
  }

  
  String pulseStatus;
  if (BPM >= 60 && BPM <= 100) pulseStatus = "Normal";
  else if (BPM < 60) pulseStatus = "Low";
  else pulseStatus = "High";

  
  String tempStatus;
  if (temp >= 36.5 && temp <= 37.5) tempStatus = "Normal";
  else if (temp < 36.5) tempStatus = "Low";
  else tempStatus = "High";

  
  lcd.setCursor(0, 0);
  lcd.print("Tem:");
  lcd.print(temp, 1);
  lcd.print(" ");
  lcd.print(tempStatus);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print("Beat:");
  lcd.print(BPM);
  lcd.print(" ");
  lcd.print(pulseStatus);
  lcd.print("   ");

  
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" | BPM: ");
  Serial.print(BPM);
  Serial.print(" | Signal: ");
  Serial.println(signal);

  delay(50); 
}



