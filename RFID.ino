#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#include <Servo.h>
Servo myservo;

#define doorLock 7

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN,RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte card_ID[4];                              //ID THẺ CÓ KÍCH CỠ 4 BYTE
byte Name1[4] = {0x53, 0xA8, 0x9B, 0x02};     //UID_CARD THỨ NHẤT
byte Name2[4] = {0x67, 0xC6, 0xB0, 0x60};     //UID_CARD THỨ HAI

int NumbCard[2]; // đây là mảng có nội dung số lượng thẻ, trong trường hợp của nhóm, chỉ sử dụng hai thẻ
int j=0;
int statu[2];
int s=0;

int const RedLed = 6;
int const GreenLed = 5;   // định nghĩa chân
int const Buzzer = 8;

// String Log
String Name;    // tên tương ứng với biển số xe sử dụng
long Number;    // biến đặt số cho xe
int n;          // số lượng thẻ muốn học

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600); // khởi tạo giao tiếp với PC
  SPI.begin();        // khởi tạo chuẩn SPI
  mfrc522.PCD_Init(); // khởi tạo RFID (mfrc522)

  myservo.attach(doorLock);
  myservo.write(25);     // BAN ĐẦU SERVO ĐÓNG

  Serial.println("CLEARSHEET");   // XÓA BẮT ĐẦU TỪ HÀNG 1 EXCEL
  Serial.println("LABEL,Date,Name,Number,Time Out,Time In");

  pinMode(RedLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  delay(100);

  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("SYSTEM READY ");
  lcd.setCursor(1,1);
  lcd.print("SCAN YOUR CARD ");
  delay(2000);
}

void loop() {
  // TÌM KIẾM THẺ MỚI
  if(!mfrc522.PICC_IsNewCardPresent())
  {
    return; // BẮT ĐẦU VÒNG LẶP KHI KHÔNG CÓ THẺ Ở HIỆN TẠI
  }
  // LỰA CHỌN 1 THẺ
  if(!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  for(byte i=0; i<mfrc522.uid.size;i++){
    card_ID[i]=mfrc522.uid.uidByte[i];

    if(card_ID[i]==Name1[i]){      // ID CỦA THẺ ĐƯỢC GÁN VỚI TÊN
      Name = "99A-999.99";       // BIỂN SỐ XE CÓ THỂ THAY ĐỔI
      Number = 1;                 // XE SỐ 1
      j = 0;
      s = 0;
    }
    else if(card_ID[i]==Name2[i]){ // ID CỦA THẺ ĐƯỢC GÁN VỚI TÊN
      Name = "35A-696.69";       // BIỂN SỐ XE CÓ THỂ THAY ĐỔI
      Number = 2;                 // XE SỐ 2
      j = 1;
      s = 1;
    }
    else {                            // TRƯỜNG HỢP MÃ THẺ UID CỦA XE KHÔNG HỢP LỆ
      digitalWrite(GreenLed, LOW);    // LED XANH TẮT
      digitalWrite(RedLed, HIGH);     // LED ĐỎ SÁNG TRONG 2S
      digitalWrite(Buzzer, HIGH);     // LOA KÊU TRONG 2S
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("THE KHONG HOP LE!"); // HIỂN THỊ LÊN LCD
      lcd.setCursor(0, 1);
      lcd.print(" XIN THU LAI ");
      delay(2000);
              myservo.write(25);      //ĐỘNG CƠ RÀO CHẮN ĐÓNG
      digitalWrite(Buzzer, LOW);      //SAU 2S LOA TẮT
      //Serial.println("  NOT ALLOWED!  ");
      goto cont;
    }
  }
  if (NumbCard[j]==1)         // TRƯỜNG HỢP THẺ HỢP LỆ
  {
    Serial.println("Already Exist");
    NumbCard[j] = 0;
    statu[s] = 1;
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("XE VAO BEN");     //QUẸT THẺ LẦN 1 HIỆN THỊ LÊN LCD XE VÀO BẾN
    lcd.setCursor(3, 1);
    lcd.print(Name);             // HIỆN THỊ BIỂN SỐ XE LÊN LCD
    Serial.print("DATA,DATE," + Name);
    Serial.print(",");
    Serial.print(Number);
    Serial.print(",");
    Serial.print("");           // XUẤT DỮ LIỆU LÊN EXCEL NGÀY, BIỂN SỐ XE
    Serial.print(",");                    
    Serial.println("TIME");         // HIỂN THỊ THỜI GIAN
    digitalWrite(GreenLed, HIGH);   // LED XANH SÁNG
    digitalWrite(RedLed, LOW);      // LED ĐỎ TẮT
    digitalWrite(Buzzer, HIGH);     // LOA KÊU 1 TIẾNG BÍP
    delay(2000);
    digitalWrite(Buzzer, LOW);

    myservo.write(140);
    delay(5000);                  // ĐỘNG CƠ SẼ MỞ KHI QUẸT THẺ, ĐÚNG SAU 5S SẼ TỰ ĐỘNG ĐÓNG
    myservo.write(25);
  }
  else {
    NumbCard[j] = 1;
    n++;
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("XE RA BEN");     // QUẸT THẺ LẦN 2 SẼ HIỆN THỊ XE RA BẾN
    lcd.setCursor(3, 1);
    lcd.print(Name);
    Serial.print("DATA,DATE," + Name);    // HIỆN THỊ TÊN BIỂN SỐ XE, NGÀY THÁNG NĂM
    Serial.print(",");
    Serial.print(Number);
    Serial.print(",");
    Serial.print("TIME");     // HIỂN THỊ THỜI GIAN XE RA BẾN LÊN LCD
    Serial.print(",");
    Serial.println("");
    digitalWrite(GreenLed, HIGH); // LED XANH SÁNG
    digitalWrite(RedLed, LOW);    // LED ĐỎ TẮT
    digitalWrite(Buzzer, HIGH);   // LOA KÊU 1 TIẾNG BÍP
    delay(2000);
    digitalWrite(Buzzer, LOW);

    myservo.write(140);
    delay(5000);
    myservo.write(25);
    Serial.println("SAVEWORKBOOKAS,Name/WorkNames");
  }
cont:
  delay(100);
  digitalWrite(GreenLed, LOW);
  digitalWrite(RedLed, LOW);
  myservo.write(25);
  if(n==6){
    Serial.println("FORCEEXCELQUIT");
  }
}
