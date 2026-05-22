#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// PIN DEFINITIONS
#define SENSOR_RX 2
#define SENSOR_TX 3
#define KAYIT_BUTON 4
#define SERVO_PIN 5
#define BUZZER_PIN 7
#define SILME_BUTON 8
#define YESIL_LED 11
#define KIRMIZI_LED 12 

// OBJECTS
SoftwareSerial mySerial(SENSOR_RX, SENSOR_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal_I2C lcd(0x27, 16, 2); 
Servo kilitMotoru;

int kayitID = 1;

void setup() {
  Serial.begin(9600);
  
  // Pin Configurations
  pinMode(KAYIT_BUTON, INPUT);
  pinMode(SILME_BUTON, INPUT);
  pinMode(YESIL_LED, OUTPUT);
  pinMode(KIRMIZI_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize Servo and LCD
  kilitMotoru.attach(SERVO_PIN);
  kilitMotoru.write(0); // Locked initially
  
  lcd.init();
  lcd.backlight();
  lcd.print("Sistem Basliyor");
  
  // Initialize Fingerprint Sensor
  finger.begin(57600);
  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.print("Sensor Hazir");
    finger.getTemplateCount();
  } else {
    lcd.clear();
    lcd.print("Sensor Hatasi!");
    while (1);
  }
  delay(1500);
  anaEkran();
}

void loop() {
  // ENROLL BUTTON CONTROL
  if (digitalRead(KAYIT_BUTON) == HIGH) {
    delay(50); // Software debounce to prevent button bouncing
    if (digitalRead(KAYIT_BUTON) == HIGH) {
      while(digitalRead(KAYIT_BUTON) == HIGH); // Halt execution until the button is released
      yeniKayitModu();
      anaEkran();
    }
  }
  
  // DELETE BUTTON CONTROL
  if (digitalRead(SILME_BUTON) == HIGH) {
    delay(50); 
    if (digitalRead(SILME_BUTON) == HIGH) {
      while(digitalRead(SILME_BUTON) == HIGH); 
      parmakSilmeModu();
      anaEkran();
    }
  }

  // METHOD 1: DELETE SPECIFIC ID VIA SERIAL PORT

  if (Serial.available() > 0) {
    int silinecekID = Serial.parseInt(); // Read input value from the Serial Monitor
    
    if (silinecekID > 0) {
      lcd.clear();
      lcd.print("Kontrol...");
      lcd.setCursor(0, 1);
      lcd.print("ID: "); lcd.print(silinecekID);
      delay(500);
      
      // Execute deletion only if the ID slot is occupied
      if (finger.loadModel(silinecekID) == FINGERPRINT_OK) {
        uint8_t p = finger.deleteModel(silinecekID); 
        if (p == FINGERPRINT_OK) {
          lcd.clear();
          lcd.print("ID: "); lcd.print(silinecekID);
          lcd.setCursor(0, 1);
          lcd.print("SILINDI");
        }
      } else {
        hataMesaji("ID Zaten Bos!");
      }
      delay(2000);
      anaEkran();
    }
  }

  // Standard Fingerprint Verification
  parmakDogrulama();
  delay(50);
}

// FUNCTIONS

void parmakDogrulama() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return;

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Erisim Onaylandi");
    digitalWrite(YESIL_LED, HIGH);
    kilitMotoru.write(90); // Unlock the mechanism
    delay(3000);
    kilitMotoru.write(0); // Lock the mechanism again
    digitalWrite(YESIL_LED, LOW);
    anaEkran();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);       
    lcd.print("Erisim");        
    lcd.setCursor(0, 1);       
    lcd.print("Reddedildi");   
    
    digitalWrite(KIRMIZI_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(KIRMIZI_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    anaEkran();
  }
}

void yeniKayitModu() {
  lcd.clear();
  lcd.print("YENI KAYIT");
  lcd.setCursor(0, 1);
  lcd.print("Iptal:KayitButon"); 
  delay(1500);
  
  // DYNAMIC EMPTY ID SEARCH ALGORITHM
  int bosID = 1;
  int maksimumKapasite = 127;
  
  while (finger.loadModel(bosID) == FINGERPRINT_OK && bosID <= maksimumKapasite) {
    bosID++; 
  }
  
  if (bosID > maksimumKapasite) {
    hataMesaji("Hafiza Dolu!");
    return; 
  }
  
  kayitID = bosID; 
  
  int p = -1;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parmaginizi");
  lcd.setCursor(0, 1);
  lcd.print("Koyunuz (ID:"); lcd.print(kayitID); lcd.print(")");

  while (p != FINGERPRINT_OK) { 
    p = finger.getImage(); 
    
    if (digitalRead(KAYIT_BUTON) == HIGH) { 
       delay(50);
       while(digitalRead(KAYIT_BUTON) == HIGH); 
       lcd.clear();
       lcd.print("Islem Iptal");
       delay(1000);
       return; 
    }
  }
  
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) { hataMesaji("Okuma Hatasi"); return; }
  
  lcd.clear();
  lcd.print("Parmagi Kaldirin");
  delay(1500);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) { p = finger.getImage(); }
  
  p = -1;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parmagi Tekrar");
  lcd.setCursor(0, 1);
  lcd.print("Koyunuz");

  while (p != FINGERPRINT_OK) { 
    p = finger.getImage(); 
    
    if (digitalRead(KAYIT_BUTON) == HIGH) { 
       delay(50);
       while(digitalRead(KAYIT_BUTON) == HIGH);
       lcd.clear();
       lcd.print("Islem Iptal");
       delay(1000);
       return; 
    }
  }
  
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) { hataMesaji("Eslesmedi"); return; }
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    p = finger.storeModel(kayitID);
    if (p == FINGERPRINT_OK) {
      lcd.clear();
      lcd.print("Kayit Basarili");
      lcd.setCursor(0, 1);
      lcd.print("ID: "); lcd.print(kayitID);
      delay(2000);
    }
  } else {
    hataMesaji("Kayit Hatasi");
  }
}


// METHOD 2 & 3: HYBRID DELETION (BY SCANNING OR SELECTING)

void parmakSilmeModu() {
  // Check if there are any enrolled fingerprints in the system first
  finger.getTemplateCount();
  if (finger.templateCount == 0) {
    hataMesaji("KayitliParmakYok");
    return;
  }

  int secilenID = 0; // 0 = CANCEL
  int maksimumKapasite = 127;
  
  lcd.clear();
  lcd.print("SILME MODU");
  delay(1000);
  
  while(true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    if (secilenID == 0) {
      lcd.print("Iptal:KayitButon");
    } else {
      lcd.print("Silinecek ID:"); lcd.print(secilenID);
    }
    
    lcd.setCursor(0, 1);
    lcd.print("Okut veya Sec");
    
    // LOOP TO LISTEN TO BOTH BUTTONS AND SENSOR SIMULTANEOUSLY
    uint8_t p = FINGERPRINT_NOFINGER;
    
    while(digitalRead(SILME_BUTON) == LOW && digitalRead(KAYIT_BUTON) == LOW) {
      p = finger.getImage(); 
      if (p == FINGERPRINT_OK) {
        break; 
      }
      delay(50);
    }
    
    // CASE 1: FINGERPRINT SCANNED
    if (p == FINGERPRINT_OK) {
      p = finger.image2Tz();
      if (p == FINGERPRINT_OK) {
        p = finger.fingerSearch();
        if (p == FINGERPRINT_OK) {
          uint8_t id = finger.fingerID; 
          p = finger.deleteModel(id);   
          if (p == FINGERPRINT_OK) {
            lcd.clear();
            lcd.print("Parmak Tanindi");
            lcd.setCursor(0, 1);
            lcd.print("ID: "); lcd.print(id); lcd.print(" SILINDI");
            delay(2000);
            return; 
          }
        } else {
          hataMesaji("Kayitsiz Parmak!");
          return;
        }
      }
    }
    
    // CASE 2: DELETE BUTTON PRESSED (Smart ID Selection)
    if (digitalRead(SILME_BUTON) == HIGH) {
      delay(50); while(digitalRead(SILME_BUTON) == HIGH); // Debounce
      
      bool doluBulundu = false;
      
      // Increment the ID number until an occupied slot is found
      while (!doluBulundu) {
        secilenID++;
        
        // Reset to beginning (Cancel) if maximum capacity is reached
        if(secilenID > maksimumKapasite) {
          secilenID = 0; 
          break;
        }
        
        // Break the loop and display if this ID slot is occupied
        if (finger.loadModel(secilenID) == FINGERPRINT_OK) {
          doluBulundu = true;
        }
      }
    }
    
    // CASE 3: ENROLL BUTTON PRESSED (ID Confirmation)
    if (digitalRead(KAYIT_BUTON) == HIGH) {
      delay(50); while(digitalRead(KAYIT_BUTON) == HIGH); // Debounce
      
      if (secilenID == 0) {
        lcd.clear();
        lcd.print("Iptal Edildi");
        delay(1000);
        return; 
      }
      
      lcd.clear();
      lcd.print("Kontrol...");
      delay(500);

      if (finger.loadModel(secilenID) == FINGERPRINT_OK) {
        p = finger.deleteModel(secilenID); 
        if (p == FINGERPRINT_OK) {
          lcd.clear();
          lcd.print("ID: "); lcd.print(secilenID);
          lcd.setCursor(0, 1);
          lcd.print("SILINDI");
          delay(2000);
          return; 
        }
      } else {
        hataMesaji("ID Zaten Bos!");
        return; 
      }
    }
  }
}

void anaEkran() {
  lcd.clear();
  lcd.print("Sistem Hazir");
  lcd.setCursor(0,1);
  lcd.print("Giris Bekleniyor");
}

void hataMesaji(String msg) {
  lcd.clear();
  lcd.print(msg);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  delay(1500);
}