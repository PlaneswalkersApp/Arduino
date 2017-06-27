#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522::MIFARE_Key rfidkey;

byte sector = 1;
byte blockAddr = 4;
byte trailerBlock = 4;
MFRC522::StatusCode status;
byte buffer[18];
byte size = sizeof(buffer);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();
  
  for (byte i = 0; i < 6; i++) {
    rfidkey.keyByte[i] = 0xFF;
  }  
}

void loop() {
  //Serial.println(mfrc522.PICC_IsNewCardPresent());
  if (mfrc522.PICC_IsNewCardPresent()) {
    getUID();
  }
}

int getUID() {
  if (! mfrc522.PICC_ReadCardSerial()) return -1;

  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &rfidkey, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    return -1;
  }

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  String uid = uid_from_byte_array(buffer, 6);
  emit_uid_to_server(uid);

  mfrc522.PICC_HaltA(); // Stop reading
  mfrc522.PCD_StopCrypto1();
  return 1;
}

void emit_uid_to_server(String uid) {
  Serial.println(uid);
}

String uid_from_byte_array(byte *buffer, byte bufferSize) {
    String uid = "";
    
    for (byte i = 0; i < bufferSize; i++) {
        uid += String(buffer[i]);
    }
    
    return uid;
}
