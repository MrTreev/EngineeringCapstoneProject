#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <SPI.h>

#define SERVICE_UUID 		"7aa93166-d243-441b-bd31-234ce6e72470"	// Create Service UUID
#define CHARACTERISTIC_UUID 	"6466966b-97a1-44cf-b05f-bd8cb60d491c"	// Create Characteristic UUID

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
int BAUDRATE 		= 	9600; 					// Set Baudrate
int PulseSensor		= 	0;  					// Pulse Sensor Pin Definition
int AHT20_SDA  		= 	34; 					// AHT20 SDA Pin Definition
int AHT20_SCL 		= 	35; 					// AHT20 SCL Pin Definition
int GSR 		= 	38; 					// GSR Signal Pin Definition
int analogInput		= 	38; 					// Initialise Analog Input Variable
bool deviceConnected 	= 	false;					// Initialise Connected Device Bool
uint32_t value 		= 	0;    					// Initialise Counter Value

void setup() {
	// put your setup code here, to run once:
	Serial.begin(BAUDRATE); 					// Begins the serial monitor at Baudrate
	Serial.println("Starting Serial and BLE"); 			// Initial Serial Message
	BLEDevice::init("Oliver_ESP32"); 				// Names the BLE device
	BLEServer *pServer = BLEDevice::createServer(); 		//
	BLEService *pService = pServer->createService(SERVICE_UUID); 	//
	pCharacteristic = pService->createCharacteristic( 		//
		CHARACTERISTIC_UUID, 					//
		BLECharacteristic::PROPERTY_READ | 			//
		BLECharacteristic::PROPERTY_WRITE | 			//
		BLECharacteristic::PROPERTY_NOTIFY 			//
	);

	pCharacteristic->setValue("Hello World"); 			//
	pService->start(); 						// Start the service
	BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); 	//
	pAdvertising->addServiceUUID(SERVICE_UUID); 			//
	pAdvertising->setScanResponse(true); 				//
	pAdvertising->setMinPreferred(0x06); 				// functions that help with iPhone connections issue
	pAdvertising->setMinPreferred(0x12); 				//
	BLEDevice::startAdvertising(); 					//
}

void loop() {
	// put your main code here, to run repeatedly:
	analogInput = analogRead(PulseSensor); 				// Read pulse sensor pin
	Serial.println(analogInput); 					// Send Pulse Sensor Data to Serial
	pCharacteristic->setValue((uint8_t*)&value, 4); 	//
	pCharacteristic->notify(); 				//
	value++; 						//
	delay(1000); 							// delay 1 sec
}
