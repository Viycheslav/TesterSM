#include <LiquidCrystal.h>
#include <EncButton.h>

// Инициализация дисплея (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Инициализация энкодера (A, B, кнопка)
EncButton enc(8, 9, 10);

// Пины управления драйвером
const int _pul = 6;
const int _dir = 7;
const int _ena = 13;
//задать номер меню для вывода на экран
uint8_t menu_num = 0; 

//переменная для работы меню
uint8_t flag_ret=0;

// задать номер строчки меню
uint8_t cursor_menu = 1;

//Параметры для ШД по умолчанию
uint16_t step = 200;  //количесто шагов на 1 шаг энкодера
uint8_t dir = 0;      //направление вращения 1 - положительное, 0 - отрицательное
uint16_t freq = 10;   //частота сигнала на выходе PUL (1..1000)
int var_mov = 0;   //количество PUL для вращения двигателя

int8_t pos = 0; //Переменная для работы с энкодером

uint16_t time_delay;  //задержка шага при работе двигателя

void setup() {
  // Настройка пинов управления драйвером как выходы
  pinMode(_ena, OUTPUT);
  pinMode(_dir, OUTPUT);
  pinMode(_pul, OUTPUT);

  digitalWrite(_ena, LOW);
  digitalWrite(_dir, LOW);
  digitalWrite(_pul, LOW);
 
  // Инициализация дисплея
  lcd.begin(16, 4); // WH1604 — 16 столбцов, 4 строки
  lcd.setCursor(4, 0);
  lcd.print("Tester");
  delay(10);
  lcd.setCursor(3, 1);
  lcd.print("STEP MOTOR");
  delay(10);
  lcd.setCursor(5, 3);
  lcd.print("v 1.0");
  delay(1000);
}

void loop() {
  
  switch (menu_num)
  {
    case 0:
      /* settings */
      lcd.clear();  
      
      // вывод меню на экран
      lcd.setCursor(3, 0);
      lcd.print("SETTINGS");
      lcd.setCursor(3, 2);
      lcd.print("Parameters");
      lcd.setCursor(3, 3);
      lcd.print("Start StM");
      delay(50);
      //обработка энкодера
      for(;;){
        pos = enc.tickISR();//получить направление вращения по часовой -1, против часовой 1, нет вращения 0
        cursor_menu=cursor_menu-pos;
        /*контроль границ меню*/
        if (cursor_menu>2) cursor_menu = 2; 
        if (cursor_menu<1) cursor_menu = 1;

        /*вывод стрелки ">" на текущий пункт меню*/
        if(cursor_menu==1){ lcd.setCursor(0, 2); lcd.print(">"); lcd.setCursor(0, 3); lcd.print(" "); }
        if(cursor_menu==2){ lcd.setCursor(0, 3); lcd.print(">"); lcd.setCursor(0, 2); lcd.print(" "); }

        /*обработка нажатия*/
        if (enc.readBtn()) { //переход в подменю в соответствии с выбором
          while (enc.readBtn());
          if(cursor_menu==1) menu_num = 1;
          if(cursor_menu==2) menu_num = 2;
          flag_ret=1; //разрешить управление в подменю
          break;
        }
      }
      break;

    case 1: 
      /* parameters */
      lcd.clear();
      char buf[17];
      // вывод меню на экран с параметрами
      lcd.setCursor(0, 0);
      lcd.print("   PARAMETERS   ");

      lcd.setCursor(0, 1);
      sprintf(buf, "  Step: %4d", step);
      lcd.print(buf);

      lcd.setCursor(0, 2);
      sprintf(buf,"  Dir: %c", dir?'+':'-');
      lcd.print(buf);
      
      lcd.setCursor(0, 3);
      sprintf(buf, "  Freq: %4d", freq);
      lcd.print(buf);
      delay(50);
     
      cursor_menu = 1;    //курсор в исходное - указывает на 1 строку
      
      while(flag_ret){
        /*обработка вращения*/
        pos = enc.tickISR();//получить направление вращения по часовой -1, против часовой 1, нет вращения 0
        cursor_menu=cursor_menu-pos;
        /*контроль границ меню*/
        if (cursor_menu>3) cursor_menu = 3; 
        if (cursor_menu<0) cursor_menu = 1;

        /*вывод стрелки ">" на текущий пункт меню*/
        if (cursor_menu==0) { lcd.setCursor(0, 0); lcd.print("^"); lcd.setCursor(0, 1); lcd.print(" "); lcd.setCursor(0, 2); lcd.print(" "); lcd.setCursor(0, 3); lcd.print(" ");}
        if (cursor_menu==1) { lcd.setCursor(0, 1); lcd.print(">"); lcd.setCursor(0, 0); lcd.print(" "); lcd.setCursor(0, 2); lcd.print(" "); lcd.setCursor(0, 3); lcd.print(" ");}
        if (cursor_menu==2) { lcd.setCursor(0, 2); lcd.print(">"); lcd.setCursor(0, 0); lcd.print(" "); lcd.setCursor(0, 1); lcd.print(" "); lcd.setCursor(0, 3); lcd.print(" ");}
        if (cursor_menu==3) { lcd.setCursor(0, 3); lcd.print(">"); lcd.setCursor(0, 0); lcd.print(" "); lcd.setCursor(0, 1); lcd.print(" "); lcd.setCursor(0, 2); lcd.print(" ");}

        /*обработка нажатия*/
        if (enc.readBtn()) {
          while (enc.readBtn());
          delay(10);
          /*выход из меню*/
          if (cursor_menu==0){
            flag_ret=0;   //выход из бесконечного цикла опроса энкодара - > выход в меню верхнего уровня
            menu_num=0;
            break;
          }

          /*=======*/
          switch (cursor_menu)
          {
            case 1:
              /* изменение параметра step */
              for(;;){
                pos = enc.tickISR();//получить направление вращения по часовой -1, против часовой 1, нет вращения 0
                step = step-pos;
                //Определяем границы
                if (step<1) step = 1;
                if (step>500) step = 500;
                lcd.setCursor(0, 1);
                sprintf(buf, "* Step: %4d", step);
                lcd.print(buf);

                if (enc.readBtn()) { //переход в подменю в соответствии с выбором
                  while (enc.readBtn());
                  delay(10);
                  lcd.setCursor(0, 1);
                  sprintf(buf, "  Step: %4d", step);
                  lcd.print(buf);
                  break;
                }
              }
              break;
            
            /*--------------------------*/
            case 2:
              /* изменение параметра dir */
              for(;;){
                pos = enc.tickISR();
                if(pos>0) dir=0;
                if(pos<0) dir=1;
                /*формирование строки вывода и вывод*/
                lcd.setCursor(0, 2);
                sprintf(buf, "* Dir: %c", (dir?'+':'-'));
                lcd.print(buf);

                if (enc.readBtn()) { //переход в подменю в соответствии с выбором
                  while (enc.readBtn());
                  delay(10);
                  lcd.setCursor(0, 2);
                  sprintf(buf, "  Dir: %c", (dir?'+':'-'));
                  lcd.print(buf);
                  break;
                }
              }
              break;

            /*--------------------------*/
            case 3:
              /* изменение параметра freq */
              for(;;){
                pos = enc.tickISR();//получить направление вращения по часовой -1, против часовой 1, нет вращения 0
                freq = freq-pos;
                //Определяем границы
                if (freq<1) freq = 1;
                if (freq>500) freq = 500;
                lcd.setCursor(0, 3);
                sprintf(buf, "* Freq: %4d", freq);
                lcd.print(buf);

                if (enc.readBtn()) { //переход в подменю в соответствии с выбором
                  while (enc.readBtn());
                  delay(10);
                  lcd.setCursor(0, 3);
                  sprintf(buf, "  Freq: %4d", freq);
                  lcd.print(buf);
                  break;
                }
              }
              break;

          }
        }
        
      }
      break;

    case 2:
      /* start stepper motor */
      lcd.clear();
      // вывод меню на экран
      lcd.setCursor(0, 0);
      lcd.print("   TEST MOTOR   ");

      lcd.setCursor(0, 1);
      sprintf(buf, "  Move: %+i", var_mov); //вывод текущего значения для вращения двигателя
      lcd.print(buf);

      lcd.setCursor(0, 3);
      sprintf(buf, "  Drive %s", 0?" Run ":" Stop"); //вывод текущего значения для вращения двигателя
      lcd.print(buf);

      //включить двигатель
      digitalWrite(_ena, HIGH);
      
      uint16_t time_delay=(uint16_t) 1000/freq;

      flag_ret=1;

      //обработка энкодера 
      for(;;){
        pos = enc.tickISR();//получить направление вращения по часовой -1, против часовой 1, нет вращения 0
        var_mov = var_mov-pos*step;
        //Определяем границы
        if (var_mov< (-10000)) var_mov = -10000;
        if (var_mov>10000) var_mov = 10000;

        lcd.setCursor(0, 1);
        sprintf(buf, "  Move: %+i    ", var_mov); //вывод текущего значения для вращения двигателя
        lcd.print(buf);

        lcd.setCursor(0, 3);
        sprintf(buf, "  Drive %s", (var_mov!=0)?" Run ":" Stop"); //вывод текущего значения для вращения двигателя
        lcd.print(buf);

        if (var_mov<0) {
          digitalWrite(_dir, dir?LOW:HIGH);
          digitalWrite(_pul, HIGH);
          delay(time_delay);
          digitalWrite(_pul,LOW);
          delay(time_delay);
          var_mov++;
        }

        if (var_mov>0) {
          digitalWrite(_dir, dir?HIGH:LOW);
          digitalWrite(_pul, HIGH);
          delay(time_delay);
          digitalWrite(_pul,LOW);
          delay(time_delay);
          var_mov--;
        }

        /*обработка нажатия*/
        if (enc.readBtn()) {
          while (enc.readBtn());
          delay(10);
          menu_num = 0;
          digitalWrite(_ena, LOW);
          break;
        }
      }
      
      break;
    
  }
 
}

