
#include <LBLE.h>
#include <LBLECentral.h>
String AT = "AT\r";
bool has_target=false;
uint16_t target_major;
uint16_t target_minor;
int target_power;
int target_rssi=-9999;
LBLEUuid check_uuid="E2C56DB5-DFFB-48D2-B060-D0F5A71096E0";
void setup() {
  pinMode(10, INPUT);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial1.print('\r');
  Serial1.print(AT);
  LBLE.begin();
  while (!LBLE.ready()) {
    delay(10);
  }
  Serial.println("Ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(10)==HIGH) {
    scan_beacon();
    send_message();
    reset();
    Serial.println("(Finished)");
  }
  


}


void read_lora(){
  while (Serial1.available() > 0) {
    String inByte1 = Serial1.readStringUntil('\n');
    Serial.println(inByte1);
  }
}



void scan_beacon() {
  Serial.println("BLE ready, start scan (wait 10 seconds)");
  LBLECentral.scan();
  for (int i = 0; i < 10; ++i)
  {
    delay(1000);
    Serial.print(".");
  }

  // list advertisements found.
  Serial.print("Total ");
  Serial.print(LBLECentral.getPeripheralCount());
  Serial.println(" devices found:");
  Serial.println("idx\taddress\t\t\tflag\tRSSI");
  for (int i = 0; i < LBLECentral.getPeripheralCount(); ++i) {
    printDeviceInfo(i);
  }
  LBLECentral.stopScan();
  Serial.println("------scan stopped-------");
}

void printDeviceInfo(int i) {
  Serial.print(i);
  Serial.print("\t");
  Serial.print(LBLECentral.getAddress(i));
  Serial.print("\t");
  Serial.print(LBLECentral.getAdvertisementFlag(i), HEX);
  Serial.print("\t");
  Serial.print(LBLECentral.getRSSI(i));
  Serial.print("\t");
  const String name = LBLECentral.getName(i);
  Serial.print(name);
  if (name.length() == 0)
  {
    Serial.print("(Unknown)");
  }
  Serial.print(" by ");
  const String manu = LBLECentral.getManufacturer(i);
  Serial.print(manu);
  Serial.print(", service: ");
  if (!LBLECentral.getServiceUuid(i).isEmpty()) {
    Serial.print(LBLECentral.getServiceUuid(i));
  } else {
    Serial.print("(no service info)");
  }

  if (LBLECentral.isIBeacon(i)) {
    
    LBLEUuid uuid;
    uint16_t major = 0, minor = 0;
    int8_t txPower = 0;
   
    LBLECentral.getIBeaconInfo(i, uuid, major, minor, txPower);
    Serial.print(" ");
    Serial.print("iBeacon->");
    Serial.print("  UUID: ");
    Serial.print(uuid);
    Serial.print("\tMajor:");
    Serial.print(major);
    Serial.print("\tMinor:");
    Serial.print(minor);
    Serial.print("\ttxPower:");
    Serial.print(txPower);
     int rssi=LBLECentral.getRSSI(i);
     if(rssi>target_rssi and check_uuid==uuid){
      has_target=true;
      target_major=major;
      target_minor=minor;
      target_power=txPower;
      target_rssi=rssi;
    }
  }

  Serial.println();

}
void send_message(){
  /*
  float beacon_distance;
  int ratio = target_rssi*1.0/target_power;
  if (ratio < 1.0) {
    beacon_distance=pow(ratio,10);
  }else {
    beacon_distance =  (0.89976)*pow(ratio,7.7095) + 0.111;   
  }
  */
  String help_message="Help";
  Serial.println("send->"+help_message);
  lora_send(help_message);
  if (has_target){
    Serial.println("Have ibeacon around");
    String major_message="major:"+String(target_major);
    String minor_message="minor:"+String(target_minor);
    String rssi_message="rssi:"+String(target_rssi);
    Serial.println("send->"+major_message);
    lora_send(major_message);
    Serial.println("send->"+minor_message);
    lora_send(minor_message);
    Serial.println("send->"+rssi_message);
    lora_send(rssi_message);
    //Serial.println(beacon_distance);
  }else{
    Serial.println("No ibeacon around");
  }
  
}
void lora_send(String message){
  unsigned int lastStringLength = message.length();
  String answer="AT+DTX="+String(lastStringLength)+",\""+message+"\"";
  Serial1.println(answer);
  delay(2500);
  //read_lora();
}
void reset(){
  has_target=false;
  target_rssi=-9999;
}
