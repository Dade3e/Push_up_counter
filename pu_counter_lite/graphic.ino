#define btn_size 2



int right_text(String testo){
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(testo, 0, 0, &x1, &y1, &w, &h);  // misura larghezza testo
  int x = SCREEN_WIDTH - w;
  return x;
}
int right_text_limit(String testo, int limit){
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(testo, 0, 0, &x1, &y1, &w, &h);  // misura larghezza testo
  int x = limit - w;
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
  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(1);
  
  if(index == 0){
    display.setCursor(0, 6);
    display.print(titoli_menu[index]);
    if(lampeggio)
      display.drawBitmap(56, 0, push_up_up, 56, 19, SSD1306_WHITE);
    else
      display.drawBitmap(56, 0, push_up_down, 56, 19, SSD1306_WHITE);
    lampeggio = !lampeggio;
    //display.drawFastHLine(30, 12, 68, SSD1306_WHITE);
    
  }
  else if(index == 1){
    display.setCursor(0, 11);
    display.print(titoli_menu[index]);
    display.drawBitmap(76, 0, settings, 32, 32, SSD1306_WHITE);
    
  }
  else if(index == 2){
    display.setCursor(0, 11);
    display.print(titoli_menu[index]);
    long distance = readUltrasonic();
    display.setCursor(center_text(String(distance)), 11);
    display.print(String(distance));
    if(battery_val < 2){
      if(lampeggio){
          display.drawBitmap(100, 11, battery, 16, 8, SSD1306_WHITE);
          display.fillRect(116-battery_val, 12, battery_val, 5, SSD1306_WHITE);
      }
      lampeggio = !lampeggio;
    }else{
      display.drawBitmap(100, 11, battery, 16, 8, SSD1306_WHITE);
      display.fillRect(116-battery_val, 12, battery_val, 5, SSD1306_WHITE);
    }
  }
  
  //chackbox
  int btn_space_menu = 8;
  int offset_y = (32-(btn_space_menu*(voci_menu-1)+btn_size)) / 2; //8 btn space
  int offset_x = 128 - (btn_size+2);;
  for(int i = 0; i<voci_menu; i++){
    if(i == index){
      display.fillRect(offset_x, offset_y+(i*btn_space_menu), btn_size, btn_size, SSD1306_WHITE);
      //display.fillRect(offsetx+(i*btn_space)+1, offset_y+1, btn_size-2, btn_size-2, SSD1306_WHITE);
      display.drawRect(offset_x-2, offset_y+(i*btn_space_menu)-2, btn_size+4, btn_size+4, SSD1306_WHITE);
    }
    else{
      display.fillRect(offset_x, offset_y+(i*btn_space_menu), btn_size, btn_size, SSD1306_WHITE);
    }
  }
  
  display.display();
}

void config_menu(int index){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setCursor(center_text(titoli_config[index]), 0);
  display.print(titoli_config[index]);

  if(index == 0){
    display.setTextSize(1);
    if(lampeggio){
      display.setCursor(16, 14);
      String tmp_p= String(soglia_ingresso) + "-"+String(perc)+"%";
      display.print(tmp_p);
      display.drawBitmap(74, 20, pu_up_s, 32, 10, SSD1306_WHITE);
      //display.drawFastHLine(20, 12, 40, SSD1306_WHITE);
    }else{
      display.setCursor(16, 24);
      String tmp_p= String(soglia_min) + "+"+String(perc)+"%";
      display.print(tmp_p);
      display.drawBitmap(74, 20, pu_down_s, 32, 10, SSD1306_WHITE);
      //display.drawFastHLine(20, 22, 40, SSD1306_WHITE);
    }
    lampeggio = !lampeggio;
    
  }
  else if(index == 1){
    display.setTextSize(2);
    String tmp = String(perc) + "%";
    display.setCursor(center_text(tmp), 14);
    display.print(tmp);
  }
  else if(index == 2){
    display.setTextSize(1);
    display.setCursor(center_text("le impostazioni"), 11);
    display.print("le impostazioni");
    display.setCursor(center_text("di default"), 22);
    display.print("di default");
  }
  else if(index == 3){
    display.drawBitmap(52, 8, undu, 24, 24, SSD1306_WHITE);
  }
  
  //chackbox
  int btn_space = 6;
  int offset_y = (32-(btn_space*(voci_config-1)+btn_size)) / 2;
  int offset_x = 128 - (btn_size+2);;
  for(int i = 0; i<voci_config; i++){
    if(i == index){
      display.fillRect(offset_x, offset_y+(i*btn_space), btn_size, btn_size, SSD1306_WHITE);
      //display.fillRect(offsetx+(i*btn_space)+1, offset_y+1, btn_size-2, btn_size-2, SSD1306_WHITE);
      display.drawRect(offset_x-2, offset_y+(i*btn_space)-2, btn_size+4, btn_size+4, SSD1306_WHITE);
    }
    else{
      display.fillRect(offset_x, offset_y+(i*btn_space), btn_size, btn_size, SSD1306_WHITE);
    }
  }
  
  display.display();
}

void push_up_limit(int index, int sec){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  if(index == 0){
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Limite alto: premi");
    //display.drawBitmap(4, 10, pu_up_s, 32, 10, SSD1306_WHITE);
    //display.drawFastHLine(0, 19, 38, SSD1306_WHITE);
    display.setCursor(0, 11);
    display.print("PLAY e rimani in");
    display.setCursor(0, 22);
    display.print("posizione di partenza");
  }
  else if(index == 1){
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Limite alto");
    display.drawBitmap(10, 21, pu_up_s, 32, 10, SSD1306_WHITE);
    display.drawFastHLine(6, 30, 38, SSD1306_WHITE);
    display.setTextSize(4);
    display.setCursor(96, 2);
    display.print(sec);
   
  }
  else if(index == 2){
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Limite basso: premi");
    //display.drawBitmap(4, 34, pu_down_s, 32, 10, SSD1306_WHITE);
    //display.drawFastHLine(2, 43, 38, SSD1306_WHITE);
    display.setCursor(0, 11);
    display.print("PLAY e rimani in");
    display.setCursor(0, 22);
    display.print("posizione al suolo");
  }
  else if(index == 3){
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Limite basso");
    display.drawBitmap(10, 21, pu_down_s, 32, 10, SSD1306_WHITE);
    display.drawFastHLine(6, 30, 38, SSD1306_WHITE);
    display.setTextSize(4);
    display.setCursor(96, 2);
    display.print(sec);
  }
  else if(index == 4){
    display.setTextSize(1);
    display.setCursor(0, 1);
    display.print("Limiti impostati");
    if(lampeggio)
      display.drawBitmap(10, 21, pu_up_s, 32, 10, SSD1306_WHITE);
    else
      display.drawBitmap(10, 21, pu_down_s, 32, 10, SSD1306_WHITE);
    display.drawFastHLine(6, 30, 38, SSD1306_WHITE);
    lampeggio = !lampeggio;
    display.setCursor(60, 22);
    display.print("Premi Play");
  }
  display.display();
}

void display_set_prec(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Imposta la tolleranza");
  display.setTextSize(2);
  display.setCursor(2, 14);
  display.print(perc);
  display.print("%");
  display.setTextSize(1);
  display.setCursor(54, 10);
  display.print("delle misure");
  display.setCursor(54, 20);
  display.print("e premi Play");
  display.display();
}

void display_show(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  if(in_pausa){
    if(lampeggio)
      display.drawBitmap(0, 0, PauseIcon, 8, 8, SSD1306_WHITE);
    
  }
  
  //battery
  if(battery_val < 2){
     if(lampeggio){
        display.drawBitmap(112, 0, battery, 16, 8, SSD1306_WHITE);
        display.fillRect(126-battery_val, 1, battery_val, 5, SSD1306_WHITE);
     }
  }else{
    display.drawBitmap(112, 0, battery, 16, 8, SSD1306_WHITE);
    display.fillRect(126-battery_val, 1, battery_val, 5, SSD1306_WHITE);
  }

  lampeggio = !lampeggio;

  display.setTextSize(3);
  //numero pushups
  if(counter < 9999)
    display.setCursor(right_text_limit(String(counter),73), 0);
  else
    display.setCursor(0, 0);
  display.print(String(counter));


  //tempo
  display.setTextSize(1);
  display.setCursor(right_text(formatMillis(workout_time)), 24);
  display.print(formatMillis(workout_time));
  display.display();
}