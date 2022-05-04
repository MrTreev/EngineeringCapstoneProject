#include <esp_adc_cal.h>

int analogInput = 0;
float voltage = 0;
float vs = 3.3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:

  analogInput = analogRead(38);
  voltage = analogInput * (vs/4095);
  
  Serial.println(voltage);
  delay(1000);
}
