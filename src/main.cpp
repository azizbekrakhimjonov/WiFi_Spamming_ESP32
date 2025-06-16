#include <WiFi.h>
#include "esp_wifi.h"

// Beacon Packet buffer
uint8_t packet[128] = { 
  0x80, 0x00,             // Frame Control
  0x00, 0x00,             // Duration
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff,   // Destination address
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,   // Source address
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,   // BSSID
  0x00, 0x00,             // Sequence Control
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Timestamp
  0x64, 0x00,             // Beacon Interval
  0x31, 0x04,             // Capability info
  0x00                    // SSID Parameter
};

char ssids[40][32] = {
  "Free_WiFi_1", "CoffeeShop_2", "Airport_Free_3", "Hotel_Guest_4",
  "Public_5", "Mall_WiFi_6", "Library_7", "Cafe_8",
  "Restaurant_9", "Student_10", "Employee_11", "Guest_12",
  "Conference_13", "Meeting_14", "Lobby_15", "Secure_16",
  "Open_17", "Fast_18", "Reliable_19", "Starbucks_20",
  "McDonalds_21", "KFC_22", "BurgerKing_23", "Subway_24",
  "Train_25", "Bus_26", "Taxi_27", "Metro_28",
  "Shopping_29", "Park_30", "Museum_31", "Gallery_32",
  "University_33", "School_34", "Office_35", "Work_36",
  "Home_37", "Family_38", "Friends_39", "Neighbor_40"
};

bool broadcasting = false;
String serialCommand;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  WiFi.mode(WIFI_MODE_AP);
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_AP);
  esp_wifi_start();

  broadcasting = true; // ESP yoqilganda avtomatik namoyishni boshlash

  Serial.println("40 ta WiFi Beacon Spam Tool");
  Serial.println("Namoyish avtomatik boshlandi");
  Serial.println("Buyruqlar:");
  Serial.println("start - Namoyishni boshlash");
  Serial.println("stop - Namoyishni to'xtatish");
  Serial.println("list - Joriy SSIDlarni ko'rish");
  Serial.println("set <pozitsiya> <nomi> - Yangi SSID o'rnatish (0-39)");
  Serial.println("status - Joriy holatni ko'rsatish");
}


void handleSerialCommands() {
  if (Serial.available()) {
    serialCommand = Serial.readStringUntil('\n');
    serialCommand.trim();
    
    if (serialCommand == "start") {
      broadcasting = true;
      Serial.println("Namoyish boshlandi");
    }
    else if (serialCommand == "stop") {
      broadcasting = false;
      Serial.println("Namoyish to'xtatildi");
    }
    else if (serialCommand == "list") {
      Serial.println("Joriy SSIDlar:");
      for (int i = 0; i < 40; i++) {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(ssids[i]);
      }
    }
    else if (serialCommand == "status") {
      Serial.print("Namoyish holati: ");
      Serial.println(broadcasting ? "Faol" : "To'xtatilgan");
    }
    else if (serialCommand.startsWith("set ")) {
      int pos = serialCommand.substring(4, serialCommand.indexOf(' ', 4)).toInt();
      String newSSID = serialCommand.substring(serialCommand.indexOf(' ', 4) + 1);
      
      if (pos >= 0 && pos < 40 && newSSID.length() > 0 && newSSID.length() < 32) {
        newSSID.toCharArray(ssids[pos], 32);
        Serial.print(pos);
        Serial.print(" pozitsiyadagi SSID yangilandi: ");
        Serial.println(ssids[pos]);
      } else {
        Serial.println("Noto'g'ri pozitsiya yoki SSID uzunligi (0-39, max 31 belgi)");
      }
    }
  }
}

void loop() {
  handleSerialCommands();
  
  if (broadcasting) {
    for(int i = 0; i < 40; i++) {
      // Tasodifiy MAC manzilini o'rnatish
      packet[10] = packet[16] = random(256);
      packet[11] = packet[17] = random(256);
      packet[12] = packet[18] = random(256);
      packet[13] = packet[19] = random(256);
      packet[14] = packet[20] = random(256);
      packet[15] = packet[21] = random(256);
      
      // SSID uzunligini o'rnatish
      int ssidLen = strlen(ssids[i]);
      packet[37] = ssidLen;
      
      // SSID ni o'rnatish
      for(int j = 0; j < ssidLen; j++) {
        packet[38 + j] = ssids[i][j];
      }
      
      // Paketni yuborish
      esp_wifi_80211_tx(WIFI_IF_AP, packet, 38 + ssidLen, false);
      delay(1);
    }
  }
}