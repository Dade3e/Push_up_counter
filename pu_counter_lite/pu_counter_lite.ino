#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <EEPROM.h>
#define EEPROM_SIZE 16
#define ADDR_MARKER 0
#define ADDR_MAX    2
#define ADDR_MIN    4
#define ADDR_PERC   6
#define ADDR_MODE   7

// Definizione display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#include "./icons.h"

#define DEF_SOGLIA_INGRESSO 450
#define DEF_SOGLIA_MIN 130
#define DEF_PERC 15
#define DEF_MODE 0

// Pin sensore ultrasonico
#define TRIG_PIN 2
#define ECHO_PIN 3

#define MINUS_PIN 5
#define PLUS_PIN 6
#define PLAY_PIN 7

#define BUZZER_PIN 4

#define voci_menu 3
#define voci_config 5

#define CHECKBAT 0
#define BATEN 1
#define BAT0 digitalWrite(BATEN, LOW)
#define BAT1 digitalWrite(BATEN, HIGH)

bool plotter = false;

String titoli_menu[voci_menu] = {"Conta\npush-ups", "Impostazioni", "Debug"};
String titoli_config[voci_config] = {"Limiti push-up", "Tolleranza misura", "Modalita' misura", "Ripristina", "Menu iniziale"};

unsigned long counter = 0;
unsigned long workout_time = 0;
bool in_pausa = false;

int modalita = 0;

int debounce = 20;

bool lampeggio = true;

uint16_t soglia_ingresso = DEF_SOGLIA_INGRESSO;
uint16_t soglia_min      = DEF_SOGLIA_MIN;
uint8_t  perc            = DEF_PERC;

int stato_pushup = 0;

//contatori millis
unsigned long start = 0;
unsigned long old_counter = 0;
unsigned long old_millis = 0;
unsigned long timer = 0;
unsigned long battery_timer = 0;

//indici macchine a stati
int state_machine = -1;
int state_machine_pu = 0;
int pu_index = 0;
int j = 0;

int media_pu = 0;
int counter_media = 1;

int battery_val = 0; //tensione batteria

void load_config_init() {
  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(ADDR_MARKER) != 0x42) {
    // EEPROM vuota → scrivo valori di default
    soglia_ingresso = DEF_SOGLIA_INGRESSO;
    soglia_min = DEF_SOGLIA_MIN;
    perc = DEF_PERC;
    modalita = DEF_MODE;

    EEPROM.write(ADDR_MARKER, 0x42); // marker
    EEPROM.put(ADDR_MAX,  (uint16_t)soglia_ingresso);
    EEPROM.put(ADDR_MIN,  (uint16_t)soglia_min);
    EEPROM.write(ADDR_PERC, (uint8_t)perc);
    EEPROM.write(ADDR_MODE, (uint8_t)modalita);
    EEPROM.commit();
  } else {
    // leggo i valori salvati
    EEPROM.get(ADDR_MAX, soglia_ingresso);
    EEPROM.get(ADDR_MIN, soglia_min);
    perc = EEPROM.read(ADDR_PERC);
    modalita = EEPROM.read(ADDR_MODE);
  }
}

void load_config() {
  EEPROM.begin(EEPROM_SIZE);
  // EEPROM vuota → scrivo valori di default
  soglia_ingresso = DEF_SOGLIA_INGRESSO;
  soglia_min = DEF_SOGLIA_MIN;
  perc = DEF_PERC;
  modalita = DEF_MODE;

   // scrivo in EEPROM con indirizzi coerenti
  EEPROM.put(ADDR_MAX, (uint16_t)soglia_ingresso);         // 2 byte
  EEPROM.put(ADDR_MIN, (uint16_t)soglia_min);              // 2 byte
  EEPROM.write(ADDR_PERC, (uint8_t)perc);                  // 1 byte
  EEPROM.write(ADDR_MODE, (uint8_t)modalita);                  // 1 byte
  EEPROM.commit();
  // leggo i valori salvati
  EEPROM.get(ADDR_MAX, soglia_ingresso);
  EEPROM.get(ADDR_MIN, soglia_min);
  perc = EEPROM.read(ADDR_PERC);
  modalita = EEPROM.read(ADDR_MODE);
}


void setup() {
  Serial.begin(115200);
  
  // Inizializza I2C con i pin di default ESP32-C3 (SDA=8, SCL=9)
  Wire.begin();

  // Inizializza display
  if (!display.begin(0x3C, true)) {
    Serial.println("Display SH1106 non trovato!");
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

   // Inizializza display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // indirizzo tipico 0x3C
    Serial.println("SSD1306 non trovato!");
    while (true);
  }

  load_config_init();
  Serial.print(", soglia ingresso: ");
  Serial.print(soglia_ingresso);
  Serial.print(", soglia min: ");
  Serial.print(soglia_min);
  Serial.print(", perc: ");
  Serial.print(perc);
  Serial.print("%");
  Serial.print(", modalita: ");
  Serial.print(modalita);

  int tmp_soglia_ingresso = soglia_ingresso - soglia_ingresso*perc/100;
  int tmp_soglia_min = soglia_min + soglia_min*perc/100;

  Serial.print(", soglia max %: ");
  Serial.print(tmp_soglia_ingresso);
  Serial.print(", soglia min %: ");
  Serial.println(tmp_soglia_min);


  // Inizializza pin ultrasonico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(MINUS_PIN, INPUT_PULLUP);
  pinMode(PLUS_PIN, INPUT_PULLUP);
  pinMode(PLAY_PIN, INPUT_PULLUP);

  pinMode(BATEN, OUTPUT);
  pinMode(CHECKBAT, INPUT);

  BAT1;
  delay(20);
  int sensorValue = analogRead(CHECKBAT);
  BAT0;

  battery_val = (sensorValue-2950) /60; //da 1 a 10

  battery_timer = millis();

}

String formatMillis(unsigned long ms) {
  unsigned long totalSeconds = ms / 1000;
  unsigned int hours = totalSeconds / 3600;
  unsigned int minutes = (totalSeconds % 3600) / 60;
  unsigned int seconds = totalSeconds % 60;

  char buffer[9]; // "HH:MM:SS"
  sprintf(buffer, "%02u:%02u:%02u", hours, minutes, seconds);
  return String(buffer);
}

void analisi_piegamento_mode_0(int distance){
  if(distance > 2 && distance < 1000){
      
      if(plotter) Serial.println(distance);
      else Serial.print(distance);

      if(! plotter) Serial.print(" mm");
      int tmp_soglia_ingresso = soglia_ingresso - (soglia_ingresso*perc/100);
      int tmp_soglia_min = soglia_min + soglia_min*perc/100;
      if(! plotter) Serial.print(" ^ ");
      if(! plotter) Serial.print(soglia_ingresso);
      if(! plotter) Serial.print("; ^% ");
      if(! plotter) Serial.print(tmp_soglia_ingresso);
      if(! plotter) Serial.print("; v ");
      if(! plotter) Serial.print(soglia_min);
      if(! plotter) Serial.print("; v% ");
      if(! plotter) Serial.print(tmp_soglia_min);
      if(distance > tmp_soglia_ingresso && stato_pushup < 2 ){
        stato_pushup = 0;
      }
      else if(distance >= tmp_soglia_min && distance <= tmp_soglia_ingresso && stato_pushup == 0){
        stato_pushup = 1;
      }
      else if(distance > 0 && distance <= tmp_soglia_min && stato_pushup == 1){
        stato_pushup = 2;
      }
      else if(distance > tmp_soglia_ingresso && stato_pushup == 2){
        stato_pushup = 3;
      }
      
      if(! plotter) Serial.print("; Stato pushup: ");
      if(! plotter) Serial.println(stato_pushup);
    }
  if(stato_pushup == 3){  //piegamento eseguito
    stato_pushup = 0;
    counter += 1;
  }
}

void analisi_piegamento_mode_1(int distance){
  if(distance > 2 && distance < 1000){
      
      if(plotter) Serial.println(distance);
      else Serial.print(distance);

      if(! plotter) Serial.print(" mm");
      int tmp_soglia_min = soglia_min + soglia_min*perc/100;
      int tmp_soglia_ingresso = tmp_soglia_min + 50;
      
      if(! plotter) Serial.print(" ^ ");
      if(! plotter) Serial.print(soglia_ingresso);
      if(! plotter) Serial.print("; ^% ");
      if(! plotter) Serial.print(tmp_soglia_ingresso);
      if(! plotter) Serial.print("; v ");
      if(! plotter) Serial.print(soglia_min);
      if(! plotter) Serial.print("; v% ");
      if(! plotter) Serial.print(tmp_soglia_min);
      if(distance > tmp_soglia_ingresso && stato_pushup < 2 ){
        stato_pushup = 0;
      }
      else if(distance >= tmp_soglia_min && distance <= tmp_soglia_ingresso && stato_pushup == 0){
        stato_pushup = 1;
      }
      else if(distance > 0 && distance <= tmp_soglia_min && stato_pushup == 1){
        stato_pushup = 2;
      }
      else if(distance > tmp_soglia_ingresso && stato_pushup == 2){
        stato_pushup = 3;
      }
      
      if(! plotter) Serial.print("; Stato pushup: ");
      if(! plotter) Serial.println(stato_pushup);
    }
  if(stato_pushup == 3){  //piegamento eseguito
    stato_pushup = 0;
    counter += 1;
  }
}


long readUltrasonic() {
  long durata, mm;
  // Trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Leggi durata
  durata = pulseInLong(ECHO_PIN, HIGH, 15000); // max 15ms
  mm = durata / 5.8;
  return mm; // return the distance value
}

void workout(int distance){
  if(! in_pausa)
    if(modalita == 0)
      analisi_piegamento_mode_0(distance);
    else if(modalita == 1)
      analisi_piegamento_mode_1(distance);
  
  if(state_machine_pu == 0){
    if(counter == 1 && start == 0){  //primo piegamento eseguito
      start = millis();
      state_machine_pu = 1;
      workout_time = millis() - start;
      old_millis = millis();
      display_show();
    }
    if(millis() - old_millis > 1000){
      old_millis = millis();
      display_show();
    }
    if (digitalRead(PLAY_PIN) == LOW) {
      timer = millis();
      start = millis();
      state_machine_pu = 1;
      workout_time = millis() - start;
      old_millis = millis();
      display_show();
      while(digitalRead(PLAY_PIN) == LOW){
        if(millis() - timer > 3000 ){
          timer = millis();
          state_machine = -1;
          j = 0;
        }
        delay(10);
      }
    }
  }
  else if(state_machine_pu == 1){
    if(counter != old_counter){
      old_counter = counter;
      old_millis = 0;
    }
    if(in_pausa){
      Serial.print("; Stato pausa: ");
      Serial.print(in_pausa);
      Serial.print("; start: ");
      Serial.println(start);
      start = millis() - workout_time;
    }
    if (digitalRead(PLUS_PIN) == LOW ) {
      counter = counter +1;
      workout_time = millis() - start;
      old_millis = 0;
      while(digitalRead(PLUS_PIN) == LOW )delay(10);
      
    }
    if (digitalRead(MINUS_PIN) == LOW ) {
      if(counter > 0){
        counter = counter -1;
      }
      workout_time = millis() - start;
      old_millis = 0;
      while(digitalRead(MINUS_PIN) == LOW )delay(10);
    }
    
    if (digitalRead(PLAY_PIN) == LOW) {
      Serial.println("Pulsante Pausa premuto");
      in_pausa = !in_pausa;
      timer = millis();
      while(digitalRead(PLAY_PIN) == LOW){
        if(millis() - timer > 3000 ){
          timer = millis();
          state_machine = -1;
          state_machine_pu = 0;
          j = 0;
          counter = 0;
          start = 0;
          workout_time = 0;
          in_pausa = false;
          old_millis = millis();
          display_show();
        }
        delay(10);
      }
    }
    if(millis() - old_millis > 1000){
      workout_time = millis() - start;
      old_millis = millis();
      display_show();
    }
  }
}



void loop() {

  if(millis() - battery_timer > 600){
    battery_timer = millis();
    BAT1;
    delay(10);
    int sensorValue = analogRead(CHECKBAT);
    BAT0;

    battery_val = (sensorValue-2950) /60; //da 1 a 10
  }
  

  if(state_machine == -1){
    if(millis() - old_millis > 500 ){
      old_millis = millis();
      init_menu(j%voci_menu);
    }
    if (digitalRead(MINUS_PIN) == LOW) {
      if(j >= 0)
        j -= 1;
      if(j < 0)
        j = voci_menu-1;
      old_millis = millis();
      init_menu(j%voci_menu);
      while(digitalRead(MINUS_PIN) == LOW)delay(10);
    }
    if ( digitalRead(PLUS_PIN) == LOW) {
      if(j<voci_menu)
        j += 1;
      if(j == voci_menu)
        j = 0;
      old_millis = millis();
      init_menu(j%voci_menu);
      while(digitalRead(PLUS_PIN) == LOW)delay(10);
    }
    if (digitalRead(PLAY_PIN) == LOW) {
      state_machine = j;
      j = 0;
      while(digitalRead(PLAY_PIN) == LOW)delay(10);
    }
  }


  // Push ups counter
  else if(state_machine == 0){
    long distance = readUltrasonic();
    workout(distance);
  }
  
  // Config menu
  else if(state_machine == 1){
    if(millis() - old_millis > 750 ){
      old_millis = millis();
      config_menu(j%voci_config);
    }
    if (digitalRead(MINUS_PIN) == LOW) {
      if(j >= 0)
        j -= 1;
      if(j < 0)
        j = voci_config-1;
      old_millis = millis();
      config_menu(j%voci_config);
      while(digitalRead(MINUS_PIN) == LOW)delay(10);
    }
    if ( digitalRead(PLUS_PIN) == LOW) {
      if(j<voci_config)
        j += 1;
      if(j == voci_config)
        j = 0;
      old_millis = millis();
      config_menu(j%voci_config);
      while(digitalRead(PLUS_PIN) == LOW)delay(10);
    }
    if (digitalRead(PLAY_PIN) == LOW) {
      state_machine = voci_menu+j;
      j = 0;
      timer = millis();
      while(digitalRead(PLAY_PIN) == LOW){
        if(millis() - timer > 3000 ){
          timer = millis();
          state_machine = -1;
          j = 0;
        }
        delay(10);
      }
    }
  }
  else if(state_machine == 2){          //stato menu 2 -> debug
    state_machine = -1;
    j = 2;
  }
  else if(state_machine == voci_menu){  //stato config 0 -> limiti push up
    long distance = readUltrasonic();
    if(pu_index == 0){
      if(millis() - old_millis > 1000 ){
        old_millis = millis();
        push_up_limit(pu_index, 5 );
      }
      timer = millis();
      if (digitalRead(MINUS_PIN) == LOW  || digitalRead(PLUS_PIN) == LOW ) {
        state_machine = 1;
        j = 0;
        while(digitalRead(MINUS_PIN) == LOW || digitalRead(PLUS_PIN) == LOW )delay(10);
      }
      if (digitalRead(PLAY_PIN) == LOW) {
        pu_index += 1;
        while(digitalRead(PLAY_PIN) == LOW)delay(10);
        delay(1000);
        timer = millis();
      }
    }

    //soglia massima
    else if(pu_index == 1){
      Serial.print("Pu up ");
      media_pu += distance;
      Serial.print(distance);
      Serial.print(" counter: ");
      Serial.println(counter_media);
      counter_media += 1;
      if(millis() - old_millis > 200 ){
        old_millis = millis();
        push_up_limit(pu_index, 5 - ((millis() - timer)/1000) );
      }
      if( ((millis() - timer)) > 5000){   //5 sec
        pu_index += 1;
        old_millis = 0;
      }
      delay(100);
    }
    else if(pu_index == 2){
      if(media_pu != 0){
        soglia_ingresso = media_pu/(counter_media-1);
        Serial.print("Valore max: ");
        Serial.println(soglia_ingresso);
      }

      media_pu = 0;
      counter_media = 1;

      if(millis() - old_millis > 1000 ){
        old_millis = millis();
        push_up_limit(pu_index, 5 );
      }
      timer = millis();
      if (digitalRead(MINUS_PIN) == LOW  || digitalRead(PLUS_PIN) == LOW ) {
        state_machine = 1;
        j = 0;
        while(digitalRead(MINUS_PIN) == LOW || digitalRead(PLUS_PIN) == LOW )delay(10);
      }
      if (digitalRead(PLAY_PIN) == LOW) {
        pu_index += 1;
        while(digitalRead(PLAY_PIN) == LOW)delay(10);
        delay(1000);
        timer = millis();
      }
    }

    //soglia minima
    else if(pu_index == 3){
      media_pu += distance;
      Serial.print("Pu down ");
      Serial.print(distance);
      Serial.print(" counter: ");
      Serial.println(counter_media);
      counter_media += 1;
      if(millis() - old_millis > 200 ){
        old_millis = millis();
        push_up_limit(pu_index, 5 - ((millis() - timer)/1000) );
      }
      if( ((millis() - timer)) > 5000){   //5 sec
        pu_index += 1;
      }
      delay(100);
    }
    else if(pu_index == 4){
      if(media_pu != 0){
        soglia_min = media_pu/(counter_media-1);
        Serial.print("soglia min: ");
        Serial.println(soglia_min);
        
        media_pu = 0;
        counter_media = 1;
      }
      if(millis() - old_millis > 500 ){
        old_millis = millis();
        push_up_limit(pu_index, 0);
      }
      timer = millis();
      if (digitalRead(MINUS_PIN) == LOW  || digitalRead(PLUS_PIN) == LOW ) {
        state_machine = 1;
        j = 0;
        while(digitalRead(MINUS_PIN) == LOW || digitalRead(PLUS_PIN) == LOW )delay(10);
      }
      if (digitalRead(PLAY_PIN) == LOW) {
        pu_index += 1;
        while(digitalRead(PLAY_PIN) == LOW)delay(10);
      }
      
    }
    else if(pu_index == 5){
      //if(soglia_ingresso > soglia_min){
      //  soglia_ingresso =  soglia_ingresso - int((soglia_ingresso - soglia_min)/2);
      //}else{
      //  soglia_ingresso = DEF_SOGLIA_INGRESSO;
      //  soglia_min = DEF_SOGLIA_MIN;
      //}
      EEPROM.put(ADDR_MAX, soglia_ingresso);   // occupa 2 byte
      EEPROM.put(ADDR_MIN, soglia_min);   // occupa 2 byte
      EEPROM.commit();
      Serial.print("soglia ingresso: ");
      Serial.println(soglia_ingresso);
      
      state_machine = 1;
      pu_index = 0;
    }
  }
  else if(state_machine == voci_menu+1){  //stato config 1 -> perc
    if(pu_index == 0){
      if(millis() - old_millis > 1000 ){
        old_millis = millis();
        display_set_prec();
      }
      if (digitalRead(MINUS_PIN) == LOW   ) {
        if(perc >= 5)
          perc -= 5;
        old_millis = millis();
        display_set_prec();
        while(digitalRead(MINUS_PIN) == LOW  )delay(10);
      }
      if (digitalRead(PLUS_PIN) == LOW   ) {
        if(perc < 25)
          perc += 5;
        old_millis = millis();
        display_set_prec();
        while(digitalRead(PLUS_PIN) == LOW  )delay(10);
      }
      if (digitalRead(PLAY_PIN) == LOW) {
        pu_index += 1;
        timer = millis();
        while(digitalRead(PLAY_PIN) == LOW){
          if(millis() - timer > 5000 ){
            timer = millis();
            state_machine = -1;
            j = 0;
            pu_index = 0;
          }
          delay(10);
        }
      }
    }
    else if(pu_index == 1){
      EEPROM.write(ADDR_PERC, (uint8_t)perc);
      EEPROM.commit();
      state_machine = 1;
      j = 1;
      pu_index = 0;
    }
  
  }
  else if(state_machine ==  voci_menu+2){  //stato config 2 -> modalita
    modalita = (modalita * -1) +1;
    EEPROM.write(ADDR_MODE, (uint8_t)modalita);
    EEPROM.commit();
    state_machine = 1;
    j = 2;
  }
  else if(state_machine ==  voci_menu+3){  //stato config 3 -> Ripristina
    load_config();
    state_machine = 1;
    j = 0;
  }
  else if(state_machine ==  voci_menu+4){  //stato config 4 -> undu
    state_machine = -1;
    j = 0;
  }
  else{
    Serial.println("Condizione non gestita");
    state_machine = -1;
  }
  delay(debounce);  // la logica avviene in notifyCallback
}
