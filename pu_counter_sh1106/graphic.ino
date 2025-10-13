#define btn_size 3
#define btn_space 16



int right_text(String testo){
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(testo, 0, 0, &x1, &y1, &w, &h);  // misura larghezza testo
  int x = SCREEN_WIDTH - w;
  return x;
}

int center_text(String testo){
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(testo, 0, 0, &x1, &y1, &w, &h);  // misura larghezza testo
  int x = (SCREEN_WIDTH - w)/2;
  return x;
}

void init_menu(int index){
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  
  display.setTextSize(1);
  display.setCursor(center_text(titoli_menu[index]), 1);
  display.print(titoli_menu[index]);
  if(index == 0){
    if(lampeggio)
      display.drawBitmap(36, 28, push_up_up, 56, 19, SH110X_WHITE);
    else
      display.drawBitmap(36, 28, push_up_down, 56, 19, SH110X_WHITE);
    lampeggio = !lampeggio;
    display.drawFastHLine(30, 46, 68, SH110X_WHITE);
  }
  else if(index == 1){
    display.drawBitmap(48, 20, settings, 32, 32, SH110X_WHITE);
  }
  else if(index == 2){
    long distance = readUltrasonic();
    display.setCursor(center_text(String(distance)), 29);
    display.print(String(distance));
    if(battery_val < 2){
      if(lampeggio){
          display.drawBitmap(112, 0, battery, 16, 8, SH110X_WHITE);
          display.fillRect(127-battery_val, 1, battery_val, 5, SH110X_WHITE);
      }
      lampeggio = !lampeggio;
    }else{
      display.drawBitmap(112, 0, battery, 16, 8, SH110X_WHITE);
      display.fillRect(127-battery_val, 1, battery_val, 5, SH110X_WHITE);
    }
  }
  
  //chackbox
  int offsetx = (128-(btn_space*(voci_menu-1)+btn_size)) / 2;
  int offset_y = 64 - (btn_size+2);
  for(int i = 0; i<voci_menu; i++){
    if(i == index){
      display.fillRect(offsetx+(i*btn_space), offset_y, btn_size, btn_size, SH110X_WHITE);
      //display.fillRect(offsetx+(i*btn_space)+1, offset_y+1, btn_size-2, btn_size-2, SSD1306_WHITE);
      display.drawRect(offsetx+(i*btn_space)-2, offset_y-2, btn_size+4, btn_size+4, SH110X_WHITE);
    }
    else{
      display.fillRect(offsetx+(i*btn_space), offset_y, btn_size, btn_size, SH110X_WHITE);
    }
  }
  
  display.display();
}

void config_menu(int index){
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  
  display.setTextSize(1);
  display.setCursor(center_text(titoli_config[index]), 1);
  display.print(titoli_config[index]);

  if(index == 0){
    display.setTextSize(1);
    if(lampeggio){
      display.setCursor(20, 24);
      String tmp_p= String(soglia_ingresso) + "-"+String(perc)+"%";
      display.print(tmp_p);
      display.drawBitmap(74, 34, pu_up_s, 32, 10, SH110X_WHITE);
      display.drawFastHLine(20, 32, 40, SH110X_WHITE);
    }else{
      display.setCursor(20, 34);
      String tmp_p= String(soglia_min) + "+"+String(perc)+"%";
      display.print(tmp_p);
      display.drawBitmap(74, 34, pu_down_s, 32, 10, SH110X_WHITE);
      display.drawFastHLine(20, 42, 40, SH110X_WHITE);
    }
    lampeggio = !lampeggio;
    
  }
  else if(index == 1){
    display.setTextSize(3);
    String tmp = String(perc) + "%";
    display.setCursor(center_text(tmp), 24);
    display.print(tmp);
  }
  else if(index == 2){
    if(buzzer){
      display.setTextSize(3);
      display.setCursor(center_text("SI"), 24);
      display.print("SI");
    }else{
      display.setTextSize(3);
      display.setCursor(center_text("NO"), 24);
      display.print("NO");
    }
  }
  
  else if(index == 3){
    display.setTextSize(1);
    display.setCursor(center_text("le impostazioni"), 15);
    display.print("le impostazioni");
    display.setCursor(center_text("di default"), 29);
    display.print("di default");
  }
  else if(index == 4){
    display.drawBitmap(52, 24, undu, 24, 24, SH110X_WHITE);
  }
  

  int offsetx = (128-(btn_space*(voci_config-1)+btn_size)) / 2;
  int offset_y = 64 - (btn_size+2);
  for(int i = 0; i<voci_config; i++){
    if(i == index){
      display.fillRect(offsetx+(i*btn_space), offset_y, btn_size, btn_size, SH110X_WHITE);
      //display.fillRect(offsetx+(i*btn_space)+1, offset_y+1, btn_size-2, btn_size-2, SSD1306_WHITE);
      display.drawRect(offsetx+(i*btn_space)-2, offset_y-2, btn_size+4, btn_size+4, SH110X_WHITE);
    }
    else{
      display.fillRect(offsetx+(i*btn_space), offset_y, btn_size, btn_size, SH110X_WHITE);
    }
  }
  
  display.display();
}

void push_up_limit(int index, int sec){
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  if(index == 0){
    display.setTextSize(1);
    display.setCursor(0, 1);
    display.print("Imposta limiti");
    display.drawBitmap(4, 34, pu_up_s, 32, 10, SH110X_WHITE);
    display.drawFastHLine(2, 43, 38, SH110X_WHITE);
    display.setCursor(54, 24);
    display.print("Premi Play");
    display.setCursor(54, 34);
    display.print("e rimani in");
    display.setCursor(54, 44);
    display.print("posizione di");
    display.setCursor(54, 54);
    display.print("partenza");
    
  }
  else if(index == 1){
    display.setTextSize(1);
    display.setCursor(0, 1);
    display.print("Limite alto");
    display.drawBitmap(4, 34, pu_up_s, 32, 10, SH110X_WHITE);
    display.drawFastHLine(2, 43, 38, SH110X_WHITE);
    display.setCursor(54, 24);
    display.print("Rimani in");
    display.setCursor(54, 34);
    display.print("posizione di");
    display.setCursor(54, 44);
    display.print("partenza");
    display.setCursor(54, 54);
    display.print("per ");
    display.print(sec);
    display.print(" sec");
   
  }
  else if(index == 2){
    display.setTextSize(1);
    display.setCursor(0, 1);
    display.print("Limite superiore");
    display.setCursor(0, 10);
    display.print("impostato");
    display.drawBitmap(4, 34, pu_down_s, 32, 10, SH110X_WHITE);
    display.drawFastHLine(2, 43, 38, SH110X_WHITE);
    display.setCursor(54, 24);
    display.print("Premi Play");
    display.setCursor(54, 34);
    display.print("e rimani in");
    display.setCursor(54, 44);
    display.print("posizione al");
    display.setCursor(54, 54);
    display.print("suolo");
  }
  else if(index == 3){
    display.setTextSize(1);
    display.setCursor(0, 1);
    display.print("Limite inferiore");
    display.drawBitmap(4, 34, pu_down_s, 32, 10, SH110X_WHITE);
    display.drawFastHLine(2, 43, 38, SH110X_WHITE);
    display.setCursor(54, 24);
    display.print("Rimani in");
    display.setCursor(54, 34);
    display.print("posizione");
    display.setCursor(54, 44);
    display.print("al suolo");
    display.setCursor(54, 54);
    display.print("per ");
    display.print(sec);
    display.print(" sec"); 
  }
  else if(index == 4){
    display.setTextSize(1);
    display.setCursor(0, 1);
    display.print("Limiti impostati");
    if(lampeggio)
      display.drawBitmap(4, 34, pu_up_s, 32, 10, SH110X_WHITE);
    else
      display.drawBitmap(4, 34, pu_down_s, 32, 10, SH110X_WHITE);
    display.drawFastHLine(2, 43, 38, SH110X_WHITE);
    lampeggio = !lampeggio;
    display.setCursor(54, 24);
    display.print("Premi Play");
    display.setCursor(54, 34);
    display.print("per tornare");
    display.setCursor(54, 44);
    display.print("al menu");
  }
  display.display();
}

void display_set_prec(){
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 1);
  display.print("Imposta tolleranza");
  display.setTextSize(2);
  display.setCursor(10, 30);
  display.print(perc);
  display.print("%");
  display.setTextSize(1);
  display.setCursor(54, 20);
  display.print("Imposta la");
  display.setCursor(54, 30);
  display.print("tolleranza");
  display.setCursor(54, 40);
  display.print("delle misure");
  display.setCursor(54, 50);
  display.print("e premi Play");
  display.display();
}

void display_show(){
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  if(in_pausa){
    if(lampeggio)
      display.drawBitmap(0, 0, PauseIcon, 8, 8, SH110X_WHITE);
  }
  
  //battery
  if(battery_val < 2){
     if(lampeggio){
        display.drawBitmap(112, 0, battery, 16, 8, SH110X_WHITE);
        display.fillRect(127-battery_val, 1, battery_val, 5, SH110X_WHITE);
     }
  }else{
    display.drawBitmap(112, 0, battery, 16, 8, SH110X_WHITE);
    display.fillRect(127-battery_val, 1, battery_val, 5, SH110X_WHITE);
  }
  

  lampeggio = !lampeggio;

  //numero pushups
  display.setTextSize(4);
  display.setCursor(12, 12);
  display.print(String(counter));

  //tempo
  display.setTextSize(2);
  display.setCursor(12, 50);
  display.print(formatMillis(workout_time));
  display.display();
}