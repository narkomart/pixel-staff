//Этот скетч предназначен для работы на Arduino atmega32u4

#include <Adafruit_NeoPixel.h>
#include <Bounce2.h> //Библиотека для работы с кнопками
#include <EEPROM.h> //Библиотека для работы с ППЗУ

#define PIXEL_COUNT 37    //Количество пикселей
#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.
#define pressed_long 1500 // долговременное нажатие кнопки = 2 секунды 
#define num_modes 17 // Количество режимов

//Кнопка
Bounce bouncer = Bounce(); //создаем экземпляр класса Bounce
unsigned long pressed_moment; // момент нажатия кнопки
byte current_mode = 0; // текущий режим
boolean hold_mode = 0; // Включение режима ожидания
byte max_mode = num_modes + 1; // вспомогательная переменная


//Переменные ленты
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
int i = 0; //Вспомогательная переменная для хранения состояния диодной ленты
int i2 = 0; //Вспомогательная переменная для хранения состояния диодной ленты
byte ledState = LOW; //Храним состояние диода
unsigned long previousMillis = 0;        // храним время последнего переключения светодиода

byte FireballSymbol [10][40] = {
  {'b', 'b', 'b', 'o', 'o', 'o', 'o', 'o', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'y', 'y', 'y', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'},
  {'b', 'b', 'o', 'p', 'p', 'p', 'p', 'p', 'o', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'y', 'y', 'y', 'b', 'b', 'b', 'b', 'b', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b'},
  {'b', 'o', 'p', 'p', 'y', 'y', 'y', 'p', 'p', 'o', 'r', 'y', 'y', 'y', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'y', 'y', 'y', 'b', 'b', 'b', 'b', 'b', 'b', 'b'},
  {'o', 'p', 'p', 'y', 'y', 'y', 'y', 'y', 'p', 'p', 'o', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'y', 'y', 'y', 'r', 'r', 'r', 'r', 'y', 'y', 'y', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'r', 'r', 'b', 'b'},
  {'o', 'p', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'p', 'o', 'r', 'r', 'y', 'y', 'y', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b'},
  {'o', 'p', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'p', 'o', 'r', 'r', 'r', 'r', 'r', 'r', 'y', 'y', 'y', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'y', 'y', 'y', 'r', 'r', 'r', 'b', 'b', 'r', 'r', 'r'},
  {'o', 'p', 'p', 'y', 'y', 'y', 'y', 'y', 'p', 'p', 'o', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'y', 'y', 'y', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'y', 'y', 'y', 'y', 'b', 'b'},
  {'b', 'o', 'p', 'p', 'y', 'y', 'y', 'p', 'p', 'o', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'},
  {'b', 'b', 'o', 'p', 'p', 'p', 'p', 'p', 'o', 'r', 'r', 'r', 'y', 'y', 'y', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b'},
  {'b', 'b', 'b', 'o', 'o', 'o', 'o', 'o', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'y', 'y', 'y', 'y', 'b', 'b', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'},
};

boolean SweetSymbol [10][10] = {
  {1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 1, 1, 0, 0, 0, 0, 0, 1}
};

byte SweetSymbol2 [12][12] = {
  {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3},
  {1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 1},
  {1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 1, 1},
  {1, 2, 2, 2, 2, 3, 3, 3, 3, 1, 1, 1},
  {2, 2, 2, 2, 3, 3, 3, 3, 1, 1, 1, 1},
  {2, 2, 2, 3, 3, 3, 3, 1, 1, 1, 1, 2},
  {2, 2, 3, 3, 3, 3, 1, 1, 1, 1, 2, 2},
  {2, 3, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2},
  {3, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 2},
  {3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 2, 3},
  {3, 3, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3},
  {3, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3}
};

void setup() {

  pinMode(2 , INPUT); // кнопка на пине 2
  digitalWrite(2 , HIGH); // подключаем встроенный подтягивающий резистор. Нужно чтобы избежать дребезга кнопки. 
  bouncer .attach(2); // устанавливаем кнопкуна соответствующем пине.
  bouncer .interval(5); // устанавливаем параметр stable interval = 5 мс
  // Serial.begin(9600); //установка Serial-порта на скорость 9600 бит/сек
  current_mode = EEPROM.read(0); //Забераем режи работы ленты из ППЗУ который храним по адресу - 0
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void ClearStrip() {
  i = 0;
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  i = 0;
  strip.show(); // Initialize all pixels to 'off'
}

void ButModeChange() {
  if (bouncer.update())
  { //если произошло событие
    if (bouncer.read() == 0)
    { //если кнопка нажата
      pressed_moment = millis(); // запоминаем время нажатия
    }
    else
    { // кнопка отпущена
      if ((millis() - pressed_moment) < pressed_long)
      { // если кнопка была нажата кратковременно
        if (hold_mode) hold_mode = 0; // Если был режим ожидания, выходим из него.
        else {
          //Если режима ожидания не было
          current_mode++; // увеличиваем счетчик текушего режима
          current_mode %= max_mode; // остаток от целочисленного деления
          // if (current_mode == 0) current_mode = 1; // режим меняется от 1 до num_modes
        }
      }
      else
      { // кнопка удерживалась долго
        ClearStrip(); // Очищаем ленту
        hold_mode = 1; // Входим в режим hold
        pressed_moment = 0; // обнуляем момент нажатия
      }
      EEPROM.write(0, current_mode);
      //      Serial.println("Current mode:");
      //      Serial.println(current_mode); // выводим сообщение о текущем режиме
      ClearStrip();
      i = 0; //Обнуляем состояние диодной ленты
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t Wheel2(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(85 - WheelPos, 0, WheelPos);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos, 85 - WheelPos );
  }
  WheelPos -= 170;
  return strip.Color(WheelPos, 85 - WheelPos, 0);
}

uint32_t RanbowPixel(int counter, int offset) { //цвет пикселя меняется в зависимости от counter, offset - смещение от 0 до 768
  //   Serial.print (counter);
  //   Serial.print(",");
  counter = counter + offset;
  byte  RPixelColor = constrain(counter % 768, 0, 255) - constrain(counter % 768, 256, 511);
  byte  GPixelColor = constrain((counter + 255) % 768, 0, 255) - constrain((counter + 255) % 768, 256, 511);
  byte  BPixelColor = constrain((counter + 511) % 768, 0, 255) - constrain((counter + 511) % 768, 256, 511);
  /*  Serial.print (RPixelColor);
    Serial.print(",");
    Serial.print (GPixelColor);
      Serial.print(",");
    Serial.println (BPixelColor);
  */
  return strip.Color(RPixelColor, GPixelColor, BPixelColor);

}

void waiting(int wspeed, int mode) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;

    // если светодиод не горит, то зажигаем, и наоборот
    if (ledState == LOW) {
      strip.setPixelColor(PIXEL_COUNT / 2, Wheel(current_mode * 255 / max_mode));
      ledState = HIGH;
    }

    else {
      strip.setPixelColor(PIXEL_COUNT / 2, 0, 0, 0);
      ledState = LOW;
    }
    strip.show();
  }
}

/* void colorWipe(uint32_t c, int wspeed) { //Заполнение
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    strip.setPixelColor(strip.numPixels() / 2 - 1 - i, c);
    strip.setPixelColor(strip.numPixels() / 2 + 1 + i, c);
    // strip.setPixelColor(random(PIXEL_COUNT), c*8-1);
    strip.show();
    if (i <= strip.numPixels() / 2)  i++;
  }
  }
*/

void RedWipe(byte wspeed) { //Заполнение
  byte j = PIXEL_COUNT / 2;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    if ( 0 <= i & i < j) { //Последовательно зажигаем диоды вплоть до тех пор, пока диодная лента вся не загарится
      strip.setPixelColor(j - 1 - i, 128, 0, 0);
      strip.setPixelColor(j + 1 + i, 128, 0, 0);
      i++;
    }
  }

  if ( i == j) { //Зажигаем диодную ленту полностью и начинаем подмаргивать диодами
    for (byte x = 0 ; x < PIXEL_COUNT ; x++) strip.setPixelColor(x, 128, 0, 0);
    strip.setPixelColor(random(PIXEL_COUNT) - 1, 255, 0, 0);
  }
  strip.show();
}

void GreenWipe(byte wspeed) { //Заполнение
  byte j = PIXEL_COUNT / 2;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    if ( 0 <= i & i < j) { //Последовательно зажигаем диоды вплоть до тех пор, пока диодная лента вся не загарится
      strip.setPixelColor(j - 1 - i, 0, 100, 0);
      strip.setPixelColor(j + 1 + i, 0, 100, 0);
      i++;
    }
  }
  if ( i == j) { //Зажигаем диодную ленту полностью и начинаем подмаргивать диодами
    for (byte x = 0 ; x < PIXEL_COUNT ; x++) strip.setPixelColor(x, 0, 100, 0);
    strip.setPixelColor(random(PIXEL_COUNT) - 1, 0, 255, 0);
  }
  strip.show();
}

void BlueWipe(byte wspeed) { //Заполнение
  byte j = PIXEL_COUNT / 2;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    if ( 0 <= i & i < j) { //Последовательно зажигаем диоды вплоть до тех пор, пока диодная лента вся не загарится
      strip.setPixelColor(j - 1 - i, 0, 0, 128);
      strip.setPixelColor(j + 1 + i, 0, 0, 128);
      i++;
    }
  }
  if ( i == j) { //Зажигаем диодную ленту полностью и начинаем подмаргивать диодами
    for (byte x = 0 ; x < PIXEL_COUNT ; x++) strip.setPixelColor(x, 0, 0, 128);
    strip.setPixelColor(random(PIXEL_COUNT) - 1, 0, 0, 255);
  }
  strip.show();
}

void YellowWipe(byte wspeed) { //Заполнение
  byte j = PIXEL_COUNT / 2;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    if ( 0 <= i & i < j) { //Последовательно зажигаем диоды вплоть до тех пор, пока диодная лента вся не загарится
      strip.setPixelColor(j - 1 - i, 32, 32, 0);
      strip.setPixelColor(j + 1 + i, 32, 32, 0);
      i++;
    }
  }
  if ( i == j) { //Зажигаем диодную ленту полностью и начинаем подмаргивать диодами
    for (byte x = 0 ; x < PIXEL_COUNT ; x++) strip.setPixelColor(x, 32, 32, 0);
    strip.setPixelColor(random(PIXEL_COUNT) - 1, 255, 255, 0);
  }
  strip.show();
}

void Fire(byte wspeed) {  //// Огонь
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;

    byte j = PIXEL_COUNT / 2;
    for (int x = 0; x <= j; x++) { //bltv gjgbrctkmyj

      byte  RPixelColor1 = constrain(pow(2, (x - 8)), 0, 128) + (constrain(pow(2, (x - 8)), 0, 64) * (sin(i / 3) + 1)) + constrain(pow(2, (x - random(j))), 0, 63);
      byte  GPixelColor1 = 0;
      if (RPixelColor1 > 128)   GPixelColor1 = random(constrain(pow(2, (x - 12)), 0, 128)) * (sin(i / 3) + 1);
      byte  BPixelColor1 = 0;
      if (1 < RPixelColor1 < 64)   BPixelColor1 = random(constrain(pow(2, (x - 17)), 0, 4)) * (cos(i / 5) + 1);

      byte  RPixelColor2 = constrain(pow(2, (x - 8)), 0, 128) + (constrain(pow(2, (x - 8)), 0, 64) * (sin(i / 3) + 1)) + constrain(pow(2, (x - random(j))), 0, 63);
      byte  GPixelColor2 = 0;
      if (RPixelColor2 > 128)   GPixelColor2 = random(constrain(pow(2, (x - 12)), 0, 128)) * (sin(i / 3) + 1);
      byte  BPixelColor2 = 0;//0;
      if (1 < RPixelColor2 < 64)   BPixelColor2 = random(constrain(pow(2, (x - 17)), 0, 4)) * (cos(i / 5) + 1);

      strip.setPixelColor(j - x, strip.Color(RPixelColor1, GPixelColor1 / 2, BPixelColor1 / 2));
      strip.setPixelColor(j + x, strip.Color(RPixelColor2, GPixelColor2 / 2, BPixelColor2 / 2));
    }

    i++;
    strip.show();
  }
}

void Line(byte wspeed) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    byte j = PIXEL_COUNT / 2;
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    for (int x = 0; x <= PIXEL_COUNT; x++) strip.setPixelColor(x, 0); //гасим все диоды
    if (i < j) {
      strip.setPixelColor(j + i, Wheel(i2));
      strip.setPixelColor(j - i, Wheel(i2));
      if (i == 0)  strip.setPixelColor(i + 1, Wheel(i2));
    }
    if (i > j) {
      strip.setPixelColor(PIXEL_COUNT - i - 1, Wheel(i2));
      strip.setPixelColor(i, Wheel(i2));
      if (i > j + j / 2) strip.setPixelColor(i, Wheel(i2));
      if (i == PIXEL_COUNT - 1)  strip.setPixelColor(i - 1, Wheel(i2));
    }
    i++;
    if (i >= PIXEL_COUNT)  {
      i = 0;
      i2 = i2 + 31;
      if (i2 > 255)  i2 = 0;
    }
    strip.show();
  }
}

void Sinus4 (byte wspeed) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    byte j = PIXEL_COUNT / 2 + 1;
    for (int x = 0; x <= PIXEL_COUNT; x++) strip.setPixelColor(x, 0); //гасим все диоды

    float a = (float) 1 / 50.0;
    strip.setPixelColor(j * sin((float)i / 12.56) + j, RanbowPixel(i, 0));
    strip.setPixelColor(j * sin((float)i / 12.56 + 3.14) + j, RanbowPixel(i, 192));
    strip.setPixelColor(j * cos((float)i / 12.56) + j, RanbowPixel(i, 384));
    strip.setPixelColor(j * cos((float)i / 12.56 + 3.14) + j, RanbowPixel(i, 576));
    i++;
    if (i >= 768)  i = 0;
    strip.show();
  }
}

void Sinus2 (byte wspeed) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    byte j = PIXEL_COUNT / 2;
    for (int x = 0; x <= PIXEL_COUNT; x++) strip.setPixelColor(x, 0); //гасим все диоды

    float a = (float) 1 / 50.0;
    strip.setPixelColor(j * sin((float)i / 12.56) + j, RanbowPixel(i, 0));
    strip.setPixelColor(j * sin((float)i / 12.56 + 3.14) + j, RanbowPixel(i, 192));
    i++;
    if (i >= 768)  i = 0;
    strip.show();
  }
}

void Sinus1 (byte wspeed) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    byte j = PIXEL_COUNT / 2;
    for (int x = 0; x <= PIXEL_COUNT; x++) strip.setPixelColor(x, 0); //гасим все диоды

    float a = (float) 1 / 50.0;
    strip.setPixelColor(j * sin((float)i / 12.56) + j, RanbowPixel(i, 0));
    i++;
    if (i >= 768)  i = 0;
    strip.show();
  }
}

void Rainbow (byte wspeed) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis;
    for (byte k = 0; k < PIXEL_COUNT ; k++) strip.setPixelColor(k, Wheel2(i + k * 10));
    i++;
    strip.show();
  }
}

void Flag (byte wspeed) {
  unsigned long currentMillis = millis();
  //   if(currentMillis - previousMillis > wspeed) {
  // сохраняем время последнего переключения
  previousMillis = currentMillis;
  byte j = PIXEL_COUNT / 2;
  for (byte x = 0 ; x <= PIXEL_COUNT ; x++) {
    if (0 <= x & x <= 4)    {
      strip.setPixelColor(x, 32, 32, 32); //,белый
      if (x == random(PIXEL_COUNT)) strip.setPixelColor(x, 64, 64, 64);
    }
    if (5 <= x & x <= 10)   {
      strip.setPixelColor(x, 0, 0, 128); //,синий
      if (x == random(PIXEL_COUNT)) strip.setPixelColor(x, 0, 0, 255);
    }
    if (10 <= x & x <= 14)  {
      strip.setPixelColor(x, 128, 0, 0); //,красный
      if (x == random(PIXEL_COUNT)) strip.setPixelColor(x, 255, 0, 0);
    }
    if (PIXEL_COUNT - 5 <= x & x <= PIXEL_COUNT - 1)    {
      strip.setPixelColor(x, 32, 32, 32); //,белый
      if (x == random(PIXEL_COUNT)) strip.setPixelColor(x, 64, 64, 64);
    }
    if (PIXEL_COUNT - 10 <= x & x <= PIXEL_COUNT - 6)   {
      strip.setPixelColor(x, 0, 0, 128); //,синий
      if (x == random(PIXEL_COUNT)) strip.setPixelColor(x, 0, 0, 255);
    }
    if (PIXEL_COUNT - 15 <= x & x <= PIXEL_COUNT - 11)  {
      strip.setPixelColor(x, 128, 0, 0); //,красный
      if (x == random(PIXEL_COUNT)) strip.setPixelColor(x, 255, 0, 0);
    }
  }
  i++;
  strip.show();
  //  }
}

void Spectrum(byte wspeed) {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    previousMillis = currentMillis;
    if (i % 3 == 0) for (byte k = 0; k < PIXEL_COUNT; k++) strip.setPixelColor(k, 128, 0, 0);
    if (i % 3 == 1) for (byte k = 0; k < PIXEL_COUNT; k++) strip.setPixelColor(k, 0, 128, 0);
    if (i % 3 == 2) for (byte k = 0; k < PIXEL_COUNT; k++) strip.setPixelColor(k, 0, 0, 128);
    strip.show();
  }
  i++;
  if (i >= 256) i = 0;

}

void Fireball(byte wspeed) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > wspeed) {
    previousMillis = currentMillis;
    if (i < 40) {
      for (byte x = 0 ; x < 10 ; x++) {
        if (FireballSymbol[x][i] == 'b')  strip.setPixelColor(x, 0, 0, 0);
        if (FireballSymbol[x][i] == 'r')  strip.setPixelColor(x, 255, 0, 0);
        if (FireballSymbol[x][i] == 'o')  strip.setPixelColor(x, 255, 33, 0);
        if (FireballSymbol[x][i] == 'p')  strip.setPixelColor(x, 255, 99, 0);
        if (FireballSymbol[x][i] == 'y')  strip.setPixelColor(x, 255, 120, 0);
      }
    }
    else for (int x = 0 ; x < PIXEL_COUNT / 2 ; x++) strip.setPixelColor(x, 0, 0, 0);

    if (i2 < 40) {
      for (byte x = 0 ; x < 10 ; x++) {
        if (FireballSymbol[x][i2] == 'b')  strip.setPixelColor(PIXEL_COUNT - x, 0, 0, 0);
        if (FireballSymbol[x][i2] == 'r')  strip.setPixelColor(PIXEL_COUNT - x, 255, 0, 0);
        if (FireballSymbol[x][i2] == 'o')  strip.setPixelColor(PIXEL_COUNT - x, 255, 33, 0);
        if (FireballSymbol[x][i2] == 'p')  strip.setPixelColor(PIXEL_COUNT - x, 255, 99, 0);
        if (FireballSymbol[x][i2] == 'y')  strip.setPixelColor(PIXEL_COUNT - x, 255, 120, 0);
      }
    }
    else for (int x = PIXEL_COUNT / 2 + 1 ; x <= PIXEL_COUNT ; x++) strip.setPixelColor(x, 0, 0, 0);

    strip.show();
    i++;
    if (i >= 50) i = 0;
    i2 = (i + 25) % 50;
  }
}

void Sweet(byte wspeed) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > wspeed) {
    previousMillis = currentMillis;
    if (i < 10) {
      for (byte x = 0 ; x < 10 ; x++) {
        if (SweetSymbol[x][i] == 0)  strip.setPixelColor(x, 0, 0, 255);
        if (SweetSymbol[x][i] == 1)  strip.setPixelColor(x, 255, 0, 0);
        if (SweetSymbol[x][i] == 0)  strip.setPixelColor(PIXEL_COUNT - x - 1, 255, 0, 0);
          if (SweetSymbol[x][i] == 1)  strip.setPixelColor(PIXEL_COUNT - x - 1, 0, 0, 255);
      }
    }
    else for (byte x = 0 ; x < PIXEL_COUNT ; x++) strip.setPixelColor(x, 0, 0, 0);
    strip.setPixelColor(18, random(255), random(255), random(255));
    strip.show();
    i++;
    if (i >= 10) i = 0;
  }
}

void Spoke(byte wspeed) {
  unsigned long currentMillis = millis();
  byte dim = random(PIXEL_COUNT / 2);
  if (currentMillis - previousMillis > wspeed) {
    previousMillis = currentMillis;
    if (i == 0) {
      for (byte x = dim ; x < PIXEL_COUNT / 2 ; x++) strip.setPixelColor(x, 0, 0, 255);
      for (byte x = PIXEL_COUNT / 2 + 1 ; x <= PIXEL_COUNT / 2 + dim + 1 ; x++) strip.setPixelColor(x, 0, 0, 255);
    }
    else for (byte x = 0 ; x < PIXEL_COUNT ; x++) strip.setPixelColor(x, 0, 0, 0);
    strip.setPixelColor(18, random(255), random(255), random(255));
    strip.show();
    i++;
    if (i >= wspeed) i = 0;
  }
}

void Flag2(byte wspeed) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > wspeed) {
    previousMillis = currentMillis;
    if (i < 12) {
      for (byte x = 0 ; x < 12 ; x++) {
        if (SweetSymbol2[x][i] == 1)  strip.setPixelColor(x, 100, 100, 100);
        if (SweetSymbol2[x][i] == 2)  strip.setPixelColor(x, 0, 0, 255);
        if (SweetSymbol2[x][i] == 3)  strip.setPixelColor(x, 255, 0, 0);
        if (SweetSymbol2[x][i] == 1)  strip.setPixelColor(PIXEL_COUNT - x - 1, 100, 100, 100);
        if (SweetSymbol2[x][i] == 2)  strip.setPixelColor(PIXEL_COUNT - x - 1, 0, 0, 255);
        if (SweetSymbol2[x][i] == 3)  strip.setPixelColor(PIXEL_COUNT - x - 1, 255, 0, 0);
      }
    }
    else for (byte x = 0 ; x < PIXEL_COUNT ; x++) strip.setPixelColor(x, 0, 0, 0);
    strip.setPixelColor(18, random(255), random(255), random(255));
    strip.show();
    i++;
    if (i >= 12) i = 0;
  }
}

void Georg(byte wspeed) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > wspeed) {
    previousMillis = currentMillis;
    if (i < 12) {
      for (byte x = 0 ; x < 12 ; x++) {
        if (SweetSymbol2[x][i] == 1)  strip.setPixelColor(x, 255, 60, 0);
        if (SweetSymbol2[x][i] == 2)  strip.setPixelColor(x, 0, 0, 0);
        if (SweetSymbol2[x][i] == 3)  strip.setPixelColor(x, 0, 0, 0);
        if (SweetSymbol2[x][i] == 1)  strip.setPixelColor(PIXEL_COUNT - x - 1, 255, 60, 0);
        if (SweetSymbol2[x][i] == 2)  strip.setPixelColor(PIXEL_COUNT - x - 1, 0, 0, 0);
        if (SweetSymbol2[x][i] == 3)  strip.setPixelColor(PIXEL_COUNT - x - 1, 0, 0, 0);
      }
    }
    else for (byte x = 0 ; x < PIXEL_COUNT ; x++) strip.setPixelColor(x, 0, 0, 0);
    strip.setPixelColor(18, random(255), random(255), random(255));
    strip.show();
    i++;
    if (i >= 12) i = 0;
  }
}

void loop() {
  ButModeChange();
  //  waiting(100);
  if (hold_mode) {
    waiting(500, current_mode); // Режим ожидания
  }
  else
    switch (current_mode) { //Это переключалка режимов
      case 0: RedWipe(50);
        break;
      case 1: GreenWipe(50);
        break;
      case 2: BlueWipe(50);
        break;
      case 3: YellowWipe(50);
        break;
      case 4: Flag(100);
        break;
      case 5: Flag2(100);
        break;
      case 6: Fire(1);
        break;
      case 7: Fireball(25);
        break;
      case 8: Sinus4(10);
        break;
      case 9: Sinus4(5);
        break;
      case 10: Sinus2(10);
        break;
      case 11: Sinus1(1);
        break;
      case 12: Line(10);
        break;
      case 13: Spectrum(6);
        break;
      case 14: Sweet(10);
        break;
      case 15: Rainbow(1);
        break;
      case 16: Spoke(2);
        break;
      case 17: Georg(2);
        break;
    }
}
