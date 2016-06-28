
#include <SPI.h>

#include <MFRC522.h>

#include <LiquidCrystal.h>

/*
 * Define pins for connection with RFID controls
 */
#define SDA_PIN 53
#define RST_PIN 5

MFRC522 mfrc522(SDA_PIN,RST_PIN);

/*
 * Initialize LCD with following connections
 */
LiquidCrystal lcd(12, 11, 22, 23, 24, 25);

/*
 * Define pins for connection with barriers (servo motors)
 */
#define ENTRY_IR 6
#define EXIT_IR 7

/*
 * Define variables for barrier control
 */
 int state;
 int entry_init_state;
 int exit_init_state;

byte data;
int status;
int overloaded;

/*
Typical pin layout used:
* -----------------------------------------------------------------------------------------
*             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
*             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
* Signal      Pin          Pin           Pin       Pin        Pin              Pin
* -----------------------------------------------------------------------------------------
* RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
* SPI SS      SDA(SS)      10            53        D10        10               10
* SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
* SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
* SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

void setup() {

  Serial.begin(9600);	// Initialize serial communications with the PC
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();	// Init MFRC522 card

  /*
   * Barrier initialization
   */
   pinMode(ENTRY_IR, INPUT);
   pinMode(EXIT_IR,INPUT);

   entry_init_state = digitalRead(ENTRY_IR);
   exit_init_state = digitalRead(EXIT_IR);

   /*
    * Barrier initialization
    */
   lcd.begin(20, 4);
   lcd.setCursor(0, 1);

}

void loop() {
  lcd.clear();

  /*
   * monitor entry barrier
   */
  monitor_barrier(ENTRY_IR, entry_init_state);

  /*
   * Display first welcome note and delay for 300ms
   */
  display_notification("WELCOME..");
  delay(1000);

  /*
   * Clear screen an display tag scanning notification
   */

  lcd.clear();
  display_notification("SCANNING TAG");
  lcd.setCursor(0, 2);
  display_notification("PLEASE WAIT...");
  delay(1000);

  read_tag_id();

  //Get validation status
  status = get_status();

  if(status == 1) {

    lcd.clear();
    display_notification("Truck Authorized");
    delay(1000);
    lcd.setCursor(0, 1);
    display_notification("Measuring GVM....");

    measure_gvm();
    lcd.setCursor(0, 2);
    display_notification("34.98 tonnes");
    delay(1000);

    //get overloaded status
    overloaded = get_status();

    if (overloaded == 1) {
      measure_gvm();
      lcd.clear();
      display_notification("Truck is overloaded");
      lcd.setCursor(0, 2);
      display_notification("Enter holding area");
      delay(1000);

    } else {
      //Display goodbye note
      lcd.clear();
      display_notification("Truck is OK");
      lcd.setCursor(0, 2);
      display_notification("Have a safe trip");
      delay(1000);

      monitor_barrier(EXIT_IR, exit_init_state);
    }

  } else {

    lcd.clear();
    display_notification("Invalid Truck");
    lcd.setCursor(0, 2);
    display_notification("Enter holding area");
    delay(1000);
  }

  lcd.clear();
  display_notification("SCANNING TAG");
  delay(1000);

}

void dump_byte_array(byte *buffer, byte bufferSize) {

  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
}

void read_tag_id() {
  // Serial.println("wait...");
  while ( ! mfrc522.PICC_IsNewCardPresent()) {}

  while ( ! mfrc522.PICC_ReadCardSerial()) {}

  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println("");
  Serial.flush();
  mfrc522.PICC_HaltA(); //stop reading

}

int get_status() {
  while( ! Serial.available()) {}

  return Serial.parseInt();
}

/*
 * Function to monitor if a truck crossed the barrier
 */

void monitor_barrier(int ir, int ir_state) {
  /*
   * Read the current state of push button
   */

  int current_state = digitalRead(ir);

  /*
   * Monitor for any change of ir barrier state
   */

   while(current_state == ir_state) {
    delay(100);
    /*
     * Check ir state again
     */

    current_state = digitalRead(ir);
  }

  /*
   * Check if initial state of ir barrier is restored
   */

   while(current_state != ir_state) {
    delay(100);
    /*
     * Check button state again
     */

    current_state = digitalRead(ir);
  }
}


void measure_gvm() {
  float f = 40.65;
  byte * b = (byte *) &f;
  Serial.write(b[0]);
  Serial.write(b[1]);
  Serial.write(b[2]);
  Serial.write(b[3]);
  Serial.println();
}

/*
 * Function to display notification
 */

void display_notification(String message) {

  lcd.print(message);

}
