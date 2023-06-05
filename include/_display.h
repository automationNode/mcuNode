#ifndef display_h
#define display_h

#include <Arduino.h>
#include "Wire.h"     // libreria para bus I2C
#include "Adafruit_SSD1306.h"   // libreria para controlador SSD1306

Adafruit_SSD1306 oled(128, 64, &Wire, 4);  // crea objeto

void displayStart() {
  Wire.begin();         // inicializa bus I2C
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C); // inicializa pantalla con direccion 0x3C 
}

void displayPresentation(){
  
  oled.clearDisplay(); 
  oled.setTextColor(WHITE);   
  oled.setCursor(24, 0);    
  oled.setTextSize(2);     
  oled.print("FIRMATA"); 
  oled.setCursor (52, 28);  
  oled.setTextSize(2);     
  oled.print("V1");     
  oled.display();    

  delay(1000);
}

void displayConnecting(){
  
  oled.clearDisplay(); 
  oled.setTextColor(WHITE);   
  oled.setCursor(24, 0);    
  oled.setTextSize(2);     
  oled.print("FIRMATA"); 
  oled.setCursor (24, 36);  
  oled.setTextSize(1);     
  oled.print("CONNECTING...");     
  oled.display();    

  delay(1000);
}

void displayRestarting(){
  
  oled.clearDisplay(); 
  oled.setTextColor(WHITE);   
  oled.setCursor(24, 0);    
  oled.setTextSize(2);     
  oled.print("FIRMATA"); 
  oled.setCursor (16, 36);  
  oled.setTextSize(1);     
  oled.print("RESTARTING...");     
  oled.display();    

  delay(1000);
}

void displayInfo(){
  oled.clearDisplay(); 
  oled.setTextColor(WHITE);   
  oled.setCursor(24,0);    
  oled.setTextSize(2);     
  oled.print("FIRMATA");   
  oled.setCursor(0, 20); 
  oled.setTextSize(1);  
  oled.print(wifiname); 
  oled.setCursor(0, 32);
  oled.print(wifipassword);
  oled.setCursor(0, 44);
  oled.print(localIp);
  oled.display(); 
  delay(1000);
}
    
#endif
