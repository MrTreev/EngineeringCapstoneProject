int sensorPin = A0; // Defining the voltage output pin
int analogInputFinal = 0;  // Initialising a variable to store the analog input
int analogInputRaw = 0;
float voltage = 0;  // Initialising the output voltage to 0
float voltageRaw = 0;  
float vs = 3.3; // The voltage source
float current = 0;  // Initialising the curren to 0
float shunt_resistance = 100000000; // The value of the know resistor
float resistance = 0; // A variable for the 
float skin_resistance = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Begins the serial monitor at 9600 bits per second
}

void loop() {
  // put your main code here, to run repeatedly:
  analogInputFinal = analogRead(sensorPin);  // reads in the value at A0
  voltage = analogInputFinal * (vs/674);  // converts the analog value at pin A0 to a voltage value
   
  Serial.println(voltage*1000);// Prints the voltage output
 
  delay(100);  // Delays the system 1 second before looping
}
