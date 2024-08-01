#include <Wire.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <MUIU8g2.h>
#include <EEPROM.h>

  // Konstruktor
  U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);  // U8G2_SH1106_128X64_NONAME_1_SW_I2C(const u8g2_cb_t *rotation, uint8_t clock, uint8_t data, uint8_t reset = U8X8_PIN_NONE)
 
  enum Ausrichtung
  {
    OBEN = 1,
    UNTEN,
    MITTE
  };

  enum Ausrichtung enum_align_obstacle;
  enum Ausrichtung enum_align_cursor;
  int rnd_nr=0, wait_new_round_rnd=0, rnd_width=0, rnd_dice=0;
  int globalPosX = 118;
  int score=0, round_counter=0; // Punktezahl
  int highscore = 0; 
  int speed = 1;
  int pin_button_down = 14;    // Pin 14 = UNTEN digitalRead(14)
  int pin_button_up = 15;      // Pin 15 = OBEN digitalRead(15)
  int pin_poti = 16;           // Pin 16 = Potentiometer  
  int val_poti = 0;            // Einlesen Poti Wert Wertebereich: 0 bis 1023
  int brightnessValue = 0;
  int y_cursor[3] = {22,42,32};   // y-Werte von Cursor auf Standartposition (mitte)
  bool isCircle = false;         
  

void setup() {
  pinMode(pin_button_down, INPUT);       // Pin 14
  pinMode(pin_button_up, INPUT);         // Pin 15
  pinMode(pin_poti, INPUT);              // Pin 16
  //U8G2 graphics; 
  Serial.begin(9600);
  u8g2.begin();
  title();                            // Ruft Titelbildschirm auf
  menu();                             // Blendet Menü ein

  rnd_nr = random(1, 4);                // Zufallszahl für Ausrichtung der Hindernisse
  rnd_dice = random(1, 20);             // Zufallszahl für Bonus
  rnd_width = random(10,100);           // Generiert Zufallszahl für breite der Hindernisse in px 
}

void loop() {
  
  display_game(rnd_width);           // Zeichnet Grafiken und bewegt den Cursor

  Serial.print("Dice: ");
  Serial.println(rnd_dice, DEC); 
    Serial.print("globalPosX: ");
  Serial.println(globalPosX, DEC); 
      Serial.print("rnd_width: ");
  Serial.println(rnd_width, DEC); 
  if (globalPosX<=(-rnd_width)){
    // Runde abgeschlossen
    isCircle = false;
    globalPosX = 118;
    score++; 
    round_counter++;
    rnd_nr = random(1, 4);
    rnd_dice = random(1, 20);
    rnd_width = random(10,100);                  // Generiert Zufallszahl für breite der Hindernisse in px 
     
    if (round_counter % 3 == 0) {speed++;}    // Erhöht Geschwindigkeit nach jeder dritten Runde

    // 30 px: Breite des Cursor, 10 px: Abstand vom Cursor zu x=0
    // Kollisioneerkennung tritt auf, wenn das Hindernis auf den Cursor trifft
    // Wenn kein Hindernis vorliegtm wird der neue xpos Wert berechnet 
  }else if(globalPosX <= 30 and globalPosX >= 10) {
      if (isCircle == true) {
        rnd_dice = random(1, 20);
        collision_detection_circle();
      } else {
        collision_detection_block();
      }
    } else {globalPosX=globalPosX-speed;}     
      Serial.print("Zufallszahl: ");
      Serial.println(rnd_nr, DEC);
}

// drawTriangle(x0,y0,x1,y1,x2,y2)
void move_player_upwards() {
    u8g2.drawTriangle(10, 0, 10, 20, 30, 10); 
  }

void move_player_downwards() {
   u8g2.drawTriangle(10, 44, 10, 64, 30, 54); 
  }

void move_player_middle() {
  u8g2.drawTriangle(10, 22, 10, 42, 30, 32); 
  }

void draw_block_upside(int width) {
  u8g2.drawBox(globalPosX, 0, width, 42);
  } 

// drawBox(x, y, w, h)
void draw_block_downside(int witdth) {
  u8g2.drawBox(globalPosX, 22, witdth, 42);
  } 

void draw_block_middle(int width) {
  u8g2.drawBox(globalPosX, 0, width, 20);
  u8g2.drawBox(globalPosX, 44, width, 20);
  } 

void draw_circle_upside(){
  u8g2.drawDisc(globalPosX, 10, 5, U8G2_DRAW_ALL);
  }

void draw_circle_middle(){
  u8g2.drawDisc(globalPosX, 32, 5, U8G2_DRAW_ALL);
  }

 void draw_circle_downside(){
  u8g2.drawDisc(globalPosX, 54, 5, U8G2_DRAW_ALL);
  }


 // Pin 14 = UNTEN
 // Pin 15 = OBEN
void display_game(int width) {
   u8g2.firstPage();
    do {     
      if (digitalRead(14) == false) {
         move_player_downwards();
         enum_align_cursor = UNTEN;
        } else if((digitalRead(15) == false)) {
          move_player_upwards();
          enum_align_cursor = OBEN;
        } else { 
          move_player_middle(); 
          enum_align_cursor = MITTE;
        }

       align_obstacle(width);   // Ruft draw_block_...() auf    
  } while ( u8g2.nextPage() );
}

void align_obstacle(int width) {
  switch (rnd_nr) {
        case OBEN: 
          if (speed >= 3 and rnd_dice==10 or isCircle==true){
            draw_circle_upside();
            enum_align_obstacle = OBEN;
            isCircle = true;
          } else {
            draw_block_upside(width);
            enum_align_obstacle = OBEN;
          }     
          break;
        case UNTEN: 
          if (speed >= 3 and rnd_dice==10 or isCircle==true){
             draw_circle_downside();
             enum_align_obstacle = UNTEN;
             isCircle = true;
          } else {
            draw_block_downside(width);
            enum_align_obstacle = UNTEN;
          }
          break;
        //case MITTE: draw_block_middle(width);
        case MITTE: 
          if (speed >= 3 and rnd_dice==10 or isCircle==true){
            draw_circle_middle();
            enum_align_obstacle = MITTE;
            isCircle = true;
          } else {
            draw_block_middle(width);
            enum_align_obstacle = MITTE;
          }
        
          break;
      } // end switch
  }

void title() {
    u8g2.firstPage();
do {
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0,15,"Retro Game");
   } while ( u8g2.nextPage() );
      delay(3000);
}

void menu() {
   u8g2.firstPage();
do {
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(15,15,"Starten");
  
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawGlyph(0, 15, 0x23f6);  /* Arrow Up */

  u8g2.drawGlyph(0, 45, 0x23f7);  /* Arrow Down */
  
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(15,45,"Helligkeit");

   } while ( u8g2.nextPage() );
  delay(50);

  while(true) {
  Serial.print("Button down: ");
  Serial.println(digitalRead(15));
  Serial.print("Button up: ");
  Serial.println(digitalRead(14));
    if (digitalRead(14) != HIGH or digitalRead(15) != HIGH) {
      if (digitalRead(14) == LOW) {
        display_brightness();
        }
        break;
      }
    }
  }

void display_brightness() {
int box_width = 0;
  
do {
    val_poti = analogRead(pin_poti);              // Einlesen Poti-Wert
    Serial.print("Poti-Wert: ");
    Serial.println(val_poti);
    
    brightnessValue = mapValue(val_poti, 0, 1023, 0, 255);  
    u8g2.setContrast(brightnessValue);            // Justierung der Helligkeit 
    box_width = mapValue(brightnessValue, 0, 255, 0, 118);
   
    u8g2.firstPage();
    do {
          u8g2.setFont(u8g2_font_unifont_t_symbols);
          u8g2.drawGlyph(0, 55, 0x23f6);  /* Arrow Up */

          u8g2.drawGlyph(0, 62, 0x23f7);  /* Arrow Down */
          u8g2.setFont(u8g2_font_ncenB14_tr);
          u8g2.drawStr(0,15,"Helligkeit");
           
          u8g2.setFont(u8g2_font_ncenB14_tr);
          u8g2.drawStr(15,60,"Speichern");
        
          u8g2.drawFrame(10,20,118,20);
          u8g2.drawBox(10,20,box_width,20);
    
       } while ( u8g2.nextPage() );
  delay(50);  
 
  } while (digitalRead(14) != false or digitalRead(15) != false);
  
    menu();
  }

void game_over() {
    int eeprom_highscore;
    
    eeprom_highscore = EEPROMReadInt(0);
    if (eeprom_highscore < score) {
      EEPROMWriteInt(0, score);
    }
    
   u8g2.firstPage();
do {
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0,15,"Game Over");
  u8g2.setCursor(0, 40);
  u8g2.print("Score: ");
  u8g2.print(u8x8_u8toa(score,2));
    if (score > highscore) {
      if (eeprom_highscore > score) {
        highscore = eeprom_highscore;
      } else {
        highscore = score;
      }
        }
  u8g2.setCursor(0, 55);
  u8g2.print("Rekord: ");
  u8g2.print(u8x8_u8toa(highscore,3));
   } while ( u8g2.nextPage() );
      delay(5000);
      globalPosX = 118;
      score = 0;
      speed = 1;
      menu();
  }

/*
 * Kollisionserkennung 
 * Ausrichtung von Curser und Hindernis erforderlich
 * Kollision tritt auf, wenn Ausrichtung von Curser und Hindernis identisch sind
 */
void collision_detection_block() {
    Serial.print("Hindernis: ");
    Serial.println(enum_align_obstacle);
    Serial.print("Cursor: ");
    Serial.println(enum_align_cursor);
      
      switch (enum_align_obstacle) {
        case OBEN: 
          if (enum_align_cursor == OBEN or enum_align_cursor == MITTE) {
            game_over();
            } else { globalPosX=globalPosX-speed;}
          break;
        case UNTEN:
          if (enum_align_cursor == UNTEN or enum_align_cursor == MITTE) {
            game_over();
            } else {globalPosX=globalPosX-speed;}
          break;
        case MITTE:
          if (enum_align_cursor == OBEN or enum_align_cursor == UNTEN) {
            game_over();
          } else {globalPosX=globalPosX-speed;}
          break;
        }
    }

void collision_detection_circle() {
    // Kollisionserkennung 
    // Ausrichtung von Curser und Hindernis erforderlich
      switch (enum_align_obstacle) {
        case OBEN: 
          if (enum_align_cursor == OBEN) {
            speed = speed-1;
            score++;
            globalPosX = 118; 
            isCircle = false;          
            } else { globalPosX=globalPosX-speed;}
          break;
        case UNTEN:
          if (enum_align_cursor == UNTEN) {
            speed = speed-1;
            score++;
            globalPosX = 118;
            isCircle = false;
            } else {globalPosX=globalPosX-speed;}
          break;
        case MITTE:
          if (enum_align_cursor == MITTE) {
            speed = speed-1;
            score++;
            globalPosX = 118;
            isCircle = false;
          } else {globalPosX=globalPosX-speed;}
          break;
        }
      
    }

int mapValue(int value, int inMin, int inMax, int outMin, int outMax) {
    // Lineare Skalierung mit expliziten Datentypen
    return (int)((long)(value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
}

void EEPROMWriteInt(int address, int value)
{
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);
  
  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}
 
int EEPROMReadInt(int address)
{
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);
 
  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}
  