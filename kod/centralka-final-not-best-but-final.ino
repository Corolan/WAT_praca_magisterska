#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAddress.h>
#include <LiquidCrystal_I2C.h>

int lcdColumns = 16;// set the LCD number of columns and rows
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);// set LCD address, number of columns and rows

// The remote service we wish to connect to.
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static int timesRead = 0;
long int freeMem;
const int howManySensors = 2;
static std::string validSensors[howManySensors] = {"7c:9e:bd:f8:a9:c2", "7c:9e:bd:f9:f9:fa"};

const byte led_gpio[howManySensors] = {16, 17};
const byte led_error = 18;

std::string value;
float temperature, pressure, humidity;

static bool connection_status[howManySensors] = {false, false};
static byte failed_connections[howManySensors] = {0, 0};
static bool skipLoop = false;
//static bool connected_succesfully = false;

static BLERemoteCharacteristic *pRemoteCharacteristics[howManySensors];
static BLERemoteService *pRemoteServices[howManySensors];
static BLEAddress *pServerAddress[howManySensors];
static BLEClient *pClients[howManySensors];

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
      //connected_succesfully = true;
      Serial.println("Połączono");
    }

    void onDisconnect(BLEClient* pclient) {
      //connected_succesfully = false;
      Serial.println("Rozłączono");
      delay(3000);
      digitalWrite(led_error, HIGH);
      //Serial.print("onDiscconnect: "); //Serial.println(connected_succesfully);
      //ESP.restart();
    }
};


static MyClientCallback *clientCallback;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();// turn on LCD backlight
  lcd.clear();
  BLEDevice::init("Cent_Dev");
  clientCallback = new MyClientCallback();

  for (int i = 0; i < howManySensors; i++) {
    pinMode(led_gpio[i], OUTPUT);
    digitalWrite(led_gpio[i], LOW);
  }
  pinMode(led_error, OUTPUT);
  digitalWrite(led_error, LOW);

  for (int i = 0; i < howManySensors; i++) {
    Serial.print("Sensor nr: "); Serial.println(i + 1);
    pServerAddress[i] = new BLEAddress(validSensors[i]);
    pClients[i]  = BLEDevice::createClient();
    delay(100);
    pClients[i]->setClientCallbacks(clientCallback);
    delay(100);
    Serial.print("SETUP ENDED for sensor nr: "); Serial.println(i + 1);
  }

  //  for (int i = 0; i < howManySensors; i++) {
  //    Serial.print("BEFORE CONNECTION: SENSOR "); Serial.println(i + 1);
  //    pClients[i]->connect(*pServerAddress[i]);
  //
  //
  //    Serial.print("CONNECTION STATUS: "); Serial.println(connected_succesfully);
  //    delay(100);
  //    if (connected_succesfully == true) {
  //      digitalWrite(led_gpio[i], HIGH);
  //      delay(100);
  //      Serial.print("AFTER CONNECTION: SENSOR "); Serial.println(i + 1);
  //      delay(100);
  //      pRemoteServices[i] = pClients[i]->getService(serviceUUID);
  //      pRemoteCharacteristics[i] = pRemoteServices[i]->getCharacteristic(charUUID);
  //      delay(100);
  //      digitalWrite(led_gpio[i], LOW);
  //    } else if (connected_succesfully == false) {
  //      Serial.print("SENSOR "); Serial.print(i + 1); Serial.println(" NOT FOUND");
  //    }
  //  }
}



void loop() {
  //freeMem = ESP.getFreeHeap(); Serial.print("Remaining memory "); Serial.println(freeMem);
  for (int i = 0; i < howManySensors; i++) {
    failed_connections[i] = 0;
  }
  digitalWrite(led_error, LOW);

  for (int i = 0; i < howManySensors; i++) {

    Serial.print("BEFORE CONNECTION: SENSOR "); Serial.println(i + 1);
    if (connection_status[i] == false) {
      pClients[i]->connect(*pServerAddress[i]);
      delay(100);
    }
    //Serial.print("CONNECTION STATUS: "); Serial.println(connected_succesfully);
    connection_status[i] = pClients[i]->isConnected();
    Serial.print("Wynik isConnected(): "); Serial.println(connection_status[i]);
    if (connection_status[i] == true) {
      digitalWrite(led_gpio[i], HIGH);
      delay(100);
      Serial.print("AFTER CONNECTION: SENSOR "); Serial.println(i + 1);
      delay(100);
      pRemoteServices[i] = pClients[i]->getService(serviceUUID);
      pRemoteCharacteristics[i] = pRemoteServices[i]->getCharacteristic(charUUID);
      delay(2000);
      digitalWrite(led_gpio[i], LOW);
    } else {
      Serial.print("SENSOR "); Serial.print(i + 1); Serial.println(" NOT FOUND");
    }
  }
  delay(300);

  while (1) {
    //freeMem = ESP.getFreeHeap(); Serial.print("Remaining memory "); Serial.println(freeMem);

    for (int i = 0; i < howManySensors; i++) {
      //connection_status[i] = pClients[i]->isConnected();
      //Serial.print("Wynik isConnected(): "); Serial.println(connection_status[i]);
      if (connection_status[i] == true && pRemoteCharacteristics[i]->canRead()) {
        digitalWrite(led_gpio[i], HIGH);
        //        if (pRemoteCharacteristics[i]->canRead()) {
        value = pRemoteCharacteristics[i]->readValue();
        char * strtokIndx; // this is used by strtok() as an index

        strtokIndx = strtok((char *)value.c_str(), ",");     // get the first part
        temperature = atof(strtokIndx); // convert this part to a float
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        pressure = atof(strtokIndx);     // convert this part to a float
        strtokIndx = strtok(NULL, ",");
        humidity = atof(strtokIndx);     // convert this part to a float
        //Serial.print("T: "); Serial.print(temperature); Serial.print(" P: "); Serial.print(pressure); Serial.print(" H: "); Serial.println(humidity);
        Serial.print(i); Serial.print(", "); Serial.print(temperature); Serial.print(", "); Serial.print(pressure); Serial.print(", "); Serial.println(humidity);
        Serial.println();

        lcd.setCursor(0, i);
        lcd.print(temperature); lcd.print(","); lcd.print(pressure); lcd.print(","); lcd.println(humidity);
        //lcd.setCursor(0, 3*i);
        //lcd.print("SENSOR ");lcd.print(i+1);lcd.print("T: ");lcd.print(temperature);
        //lcd.setCursor(0, 3*i+1);
        //lcd.print("P: ");lcd.print(pressure);lcd.print("H: ");lcd.print(humidity);
        //        } else {
        //          Serial.print("NIE MOZNA ODCZYTAC: S "); Serial.println(i+1);
        //        }
        digitalWrite(led_gpio[i], LOW);
        delay(200);
        //pClients[i]->disconnect();
      } else {
        Serial.print(i); Serial.print(", "); Serial.print("00"); Serial.print(", "); Serial.print("00"); Serial.print(", "); Serial.println("00");
        Serial.println();
        digitalWrite(led_error, HIGH);
        delay(1000);
        if (connection_status[i] == true) {
          pClients[i]->disconnect();
        }
        if (failed_connections[i]++ >= 5) skipLoop = true;

        //break;//ESP.restart();//TU MUSI BYC FLAGA WYJSCAI Z WHILE
      }
    }
    Serial.print("Obiegi: "); Serial.println(timesRead++);
    delay(350);
    if (skipLoop == true) {
      skipLoop = false;
      Serial.println("WYSKAKUJE!");
      break;
    }
  }//while
}
