#include <Wire.h>  // подключим стандартную библиотеку I2C
#include <MPU6050.h> // подключаем библеотеку для работы с акселерометром
#include <RCSwitch.h> // подключаем библеотеку для работы с радиомодулем

RCSwitch mSwitch = RCSwitch(); // радиопередатчик
RCSwitch mySwitch = RCSwitch(); // радиоприемник
int duration1, cm1; // назначаем переменную "cm1" и "duration1" для показаний датчика растояния
int duration2, cm2; // назначаем переменную "cm2" и "duration2" для показаний датчика высоты
// датчик растояния
const int trigPin1 = 28;   //Триггер – зеленый проводник
const int echoPin1 = 30;   //Эхо – желтый проводник
// датчик высоты
const int trigPin2 = 14;   //Триггер – зеленый проводник
const int echoPin2 = 15;   //Эхо – желтый проводник

// цифровые данные, отвечающие за точки в 3-х осях (акселерометр)
int16_t axx, axy, axz;
int16_t gix, giy, giz;

// Создаем объект, символизирующий модуль датчика
MPU6050 axeler;

const int prx_pin1 = 38; //датчик препятствий передний левый
const int prx_pin2 = 42; //датчик препятствий задний правый
const int prx_pin3 = 37; //датчик препятствий передний правый
const int prx_pin4 = 41; //датчик препятствий задний левый
const int prx_pin5 = 36; //датчик препятствий передний 
const int prx_pin6 = 35; //датчик препятствий на подвижной платформе
byte v;
#define addr 0x1E // I2C адрес цифрового компаса HMC5883L
// первый уровень датчиков линии
 #define PIN_SENSOR1 50  // датчик линии передний правый
  #define PIN_SENSOR2 52  //датчик линии передний левый
  //второй уровень датчиков линии
   #define PIN_SENSOR3 48  // датчик линии передний правый
  #define PIN_SENSOR4 47  //датчик линии передний левый
 const int buttonPin = 16; // пин кнопки на погрузчике
 int button = 0; // состояние кнопки на погрузчике
 const int wheelF1 = 11; //колеса правые вперед
 const int wheelF2 = 9; //колеса левые вперед
 const int wheelR1 = 12; //колеса правые назад
 const int wheelR2 = 10; //колеса левые назад
 const int loaderUp = 5; // погрузчик движется вверх
 const int loaderDown = 4; // погрузчик движется вниз
 const int shim = 3; // скорость движения погрузчика (пин 3)
 const int dPin = 31; //светодиод (загорается при принятии радиосигнала)
#define s2  7//порт 2 датчика цвета
#define s3  6// порт 3 датчика цвета
#define out 32 // вывод цвета
byte red = 0;
byte green = 0;
byte blue = 0;
int y = 0; // флаг на 3 режима работы датчиков препятствий
const int motorAnalog1 = 8; //скорость моторов, которые располагаются справа (пин 8)
const int motorAnalog2 = 13; // скорость моторов, которые располагаются слева (пин 13)
 void setup() {
  //устанавливаем режимы работ
   Serial.begin(9600);
   pinMode(dPin,OUTPUT);
   mSwitch.enableTransmit(2); 
   mySwitch.enableReceive(0);
   pinMode(trigPin1, OUTPUT);
   pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
   pinMode(echoPin2, INPUT);
   pinMode(s2, OUTPUT);
   pinMode(s3, OUTPUT);
   pinMode(out, INPUT);
   pinMode(buttonPin, INPUT);
   pinMode(loaderUp, OUTPUT);
   pinMode(loaderDown, OUTPUT);
   pinMode(prx_pin1, INPUT);
   pinMode(prx_pin2, INPUT);
   pinMode(prx_pin3, INPUT);
   pinMode(prx_pin4, INPUT);
   pinMode(prx_pin5, INPUT);
   pinMode(prx_pin6, INPUT);
   pinMode(wheelF1, OUTPUT);
   pinMode(wheelF2, OUTPUT);
   pinMode(wheelR1, OUTPUT);
   pinMode(wheelR2, OUTPUT);
   pinMode(PIN_SENSOR1,INPUT);
   pinMode(PIN_SENSOR2,INPUT);
   pinMode(PIN_SENSOR3,INPUT);
   pinMode(PIN_SENSOR4,INPUT);
 }
 void loop() {
  if((digitalRead(PIN_SENSOR3)== 1) || (digitalRead(PIN_SENSOR4) == 1)){ // проверка работоспособности первой линии датчиков
    y = 1; // если срабатывает вторая линия, то машина переключается на работу со второй линией
  }
  if ((digitalRead(prx_pin1)== 1) || (digitalRead(prx_pin2) == 1) || (digitalRead(prx_pin3)== 1) || (digitalRead(prx_pin4) == 1)){ //если срабатывает хоть один датчик препятствия
    y = 2;
    digitalWrite(wheelR2,LOW); // машина останавливается
            digitalWrite(wheelF1,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelF2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   mSwitch.send(B0100,4);//  машина посылает радиосигнал в пункт управления
  }
  if(y == 0){
  drive(true); // едет вперед с помощью первой линии датчиков
  }
  else if(y == 1){
  nextLevelDrive(true); // едет вперед с помощью второй линии датчиков
  }
   if (blue >= 200){ // блок при обнаружении синего светодиода
     //поворот вправо и захват ящика со второго яруса
        while(!(compass() == 90)){
    // поворот на 90 градусов вправо
     digitalWrite(wheelF1,LOW); 
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);
}
//остановка
   digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   //спуск платформы до упора через 500 мс после остановки 
   delay(500);
   while(button == 0){
            digitalWrite(loaderDown,HIGH);
            digitalWrite(loaderUp,LOW);
             digitalWrite(shim,255);
             button = digitalRead(buttonPin);
   }
   // остановка платформы на пол секунды
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             delay(500);
             // подьем до фиксированного значения датчика высоты
             while(!(distance(2) == 20)){ // значение датчика высоты должно стать - 20 см
         digitalWrite(loaderUp,HIGH);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,255);}
            //остановка платформы на 500 мс
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             delay(500);
             // передвижение вперед пока датчик препятствия 1 - ый или 2 - ой не сработает
             while((prx_pin5 == 0) || (prx_pin6 == 0)){
             digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
             }
      // остановка
   digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   // поднятие груза
    digitalWrite(loaderUp,HIGH);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,255);
             delay(2000);
             // остановка платформы
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             // передвижение назад до фиксированного значения датчика растояния
             while(!(distance(1) == 7)){ // значение датчика растояния должно стать - 7 см
               digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
   }
   // остановка
    digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   // отправка радиосигнала в пункт управления
       mSwitch.send(B1000, 4);
       while(!( mySwitch.available() )){  //ждем радиосигнала, отправленного из пункта управления
        int value = mySwitch.getReceivedValue();
        if( value == B0100 ){ 
            // радиосигнал, отправленный из пункта управления, принят
            // зажигание светодиода на 500 мс
        digitalWrite(dPin,HIGH);
            delay(500);
                    digitalWrite(dPin,LOW);
                // спуск платформы до упора
                while(button == 0){
               digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,HIGH);
             digitalWrite(shim,255);
             button = digitalRead(buttonPin);}
            // остановка платформы
               digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             //поворот на 90 градусов вправо
              while(!(compass() == 90)){
                     digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);   
        }        
        }              
        mySwitch.resetAvailable(); // перезагрузка радиоприемника
    }
}
else if ((blue >= 100) && (red >= 100)){ // блок при обнаружении фиолетового светодиода
     //поворот вправо и захват ящика с первого яруса
        while(!(compass() == 90)){
    // поворот на 90 градусов вправо
     digitalWrite(wheelF1,LOW); 
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);
}
//остановка
   digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   //спуск платформы до упора через 500 мс после остановки 
   delay(500);
   while(button == 0){
            digitalWrite(loaderDown,HIGH);
            digitalWrite(loaderUp,LOW);
             digitalWrite(shim,255);
             button = digitalRead(buttonPin);
   }
   // остановка платформы на пол секунды
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             delay(500);
             // подьем до фиксированного значения датчика высоты
             while(!(distance(2) == 5)){ // значение датчика высоты должно стать - 5 см
         digitalWrite(loaderUp,HIGH);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,255);}
            //остановка платформы на 500 мс
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             delay(500);
             // передвижение вперед пока датчик препятствия 1 - ый или 2 - ой не сработает
             while((prx_pin5 == 0) || (prx_pin6 == 0)){
             digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
             }
      // остановка
   digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   // поднятие груза
    digitalWrite(loaderUp,HIGH);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,255);
             delay(2000);
             // остановка платформы
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             // передвижение назад до фиксированного значения датчика растояния
             while(!(distance(1) == 7)){ // значение датчика растояния должно стать - 7 см
               digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
   }
   // остановка
    digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   // отправка радиосигнала в пункт управления
       mSwitch.send(B1000, 4);
       while(!( mySwitch.available() )){  //ждем радиосигнала, отправленного из пункта управления
        int value = mySwitch.getReceivedValue();
        if( value == B0100 ){ 
            // радиосигнал, отправленный из пункта управления, принят
            // зажигание светодиода на 500 мс
        digitalWrite(dPin,HIGH);
            delay(500);
                    digitalWrite(dPin,LOW);
                // спуск платформы до упора
                while(button == 0){
               digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,HIGH);
             digitalWrite(shim,255);
             button = digitalRead(buttonPin);}
            // остановка платформы
               digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             //поворот на 90 градусов вправо
              while(!(compass() == 90)){
                     digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);   
        }        
        }              
        mySwitch.resetAvailable(); // перезагрузка радиоприемника
    }
}
else if ((blue >= 100) && (green >= 100)){ // блок при обнаружении голубого светодиода
     //поворот влево и захват ящика со второго яруса
        while(!(compass() == 90)){
    // поворот на 90 градусов влево
     digitalWrite(wheelF1,HIGH); 
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);
}
//остановка
   digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   //спуск платформы до упора через 500 мс после остановки 
   delay(500);
   while(button == 0){
            digitalWrite(loaderDown,HIGH);
            digitalWrite(loaderUp,LOW);
             digitalWrite(shim,255);
             button = digitalRead(buttonPin);
   }
   // остановка платформы на пол секунды
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             delay(500);
             // подьем до фиксированного значения датчика высоты
             while(!(distance(2) == 20)){ // значение датчика высоты должно стать - 20 см
         digitalWrite(loaderUp,HIGH);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,255);}
            //остановка платформы на 500 мс
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             delay(500);
             // передвижение вперед пока датчик препятствия 1 - ый или 2 - ой не сработает
             while((prx_pin5 == 0) || (prx_pin6 == 0)){
             digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
             }
      // остановка
   digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   // поднятие груза
    digitalWrite(loaderUp,HIGH);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,255);
             delay(2000);
             // остановка платформы
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             // передвижение назад до фиксированного значения датчика растояния
             while(!(distance(1) == 7)){ // значение датчика растояния должно стать - 7 см
               digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
   }
   // остановка
    digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   // отправка радиосигнала в пункт управления
       mSwitch.send(B1000, 4);
       while(!( mySwitch.available() )){  //ждем радиосигнала, отправленного из пункта управления
        int value = mySwitch.getReceivedValue();
        if( value == B0100 ){ 
            // радиосигнал, отправленный из пункта управления, принят
            // зажигание светодиода на 500 мс
        digitalWrite(dPin,HIGH);
            delay(500);
                    digitalWrite(dPin,LOW);
                // спуск платформы до упора
                while(button == 0){
               digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,HIGH);
             digitalWrite(shim,255);
             button = digitalRead(buttonPin);}
            // остановка платформы
               digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             //поворот на 90 градусов влево
              while(!(compass() == 90)){
                     digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);   
        }        
        }              
        mySwitch.resetAvailable(); // перезагрузка радиоприемника
    }
}
else if ((red >= 100) && (green >= 100)){ // блок при обнаружении желтого светодиода
     //поворот влево и захват ящика с первого яруса
        while(!(compass() == 90)){
    // поворот на 90 градусов влево
     digitalWrite(wheelF1,HIGH); 
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);
}
//остановка
   digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   //спуск платформы до упора через 500 мс после остановки 
   delay(500);
   while(button == 0){
            digitalWrite(loaderDown,HIGH);
            digitalWrite(loaderUp,LOW);
             digitalWrite(shim,255);
             button = digitalRead(buttonPin);
   }
   // остановка платформы на пол секунды
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             delay(500);
             // подьем до фиксированного значения датчика высоты
             while(!(distance(2) == 5)){ // значение датчика высоты должно стать - 5 см
         digitalWrite(loaderUp,HIGH);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,255);}
            //остановка платформы на 500 мс
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             delay(500);
             // передвижение вперед пока датчик препятствия 1 - ый или 2 - ой не сработает
             while((prx_pin5 == 0) || (prx_pin6 == 0)){
             digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
             }
      // остановка
   digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   // поднятие груза
    digitalWrite(loaderUp,HIGH);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,255);
             delay(2000);
             // остановка платформы
             digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             // передвижение назад до фиксированного значения датчика растояния
             while(!(distance(1) == 7)){ // значение датчика растояния должно стать - 7 см
               digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
   }
   // остановка
    digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,0);
   analogWrite(motorAnalog1,0);
   // отправка радиосигнала в пункт управления
       mSwitch.send(B1000, 4);
       while(!( mySwitch.available() )){  //ждем радиосигнала, отправленного из пункта управления
        int value = mySwitch.getReceivedValue();
        if( value == B0100 ){ 
            // радиосигнал, отправленный из пункта управления, принят
            // зажигание светодиода на 500 мс
        digitalWrite(dPin,HIGH);
            delay(500);
                    digitalWrite(dPin,LOW);
                // спуск платформы до упора
                while(button == 0){
               digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,HIGH);
             digitalWrite(shim,255);
             button = digitalRead(buttonPin);}
            // остановка платформы
               digitalWrite(loaderUp,LOW);
            digitalWrite(loaderDown,LOW);
             digitalWrite(shim,0);
             //поворот на 90 градусов влево
              while(!(compass() == 90)){
                     digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);   
        }        
        }              
        mySwitch.resetAvailable(); // перезагрузка радиоприемника
    }
}
else if(red >= 200) {  //блок при обнаружении красного светодиода
  while(!(compass() == 90)){
    // поворот на 90 градусов влево
     digitalWrite(wheelF1,HIGH); 
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);
}
  if(y == 0){
  drive(true); // едет вперед с помощью первой линии датчиков
  }
  else if(y == 1){
  nextLevelDrive(true); // едет вперед с помощью второй линии датчиков
  }
delay(500); //пол секунды на то, чтобы отъехать от светодиода
}
else if(green >= 200) {  //блок при обнаружении зеленого светодиода
  while(!(compass() == 90)){
    // поворот на 90 градусов вправо
     digitalWrite(wheelF1,LOW); 
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,200);
   analogWrite(motorAnalog1,200);
}
  if(y == 0){
  drive(true); // едет вперед с помощью первой линии датчиков
  }
  else if(y == 1){
  nextLevelDrive(true); // едет вперед с помощью второй линии датчиков
  }
delay(500); //пол секунды на то, чтобы отъехать от светодиода
}
  delay(150); // цикл повторяется через каждые 150 мс
  } 
 void drive(boolean n){ // первый уровень датчиков линии
  if(n){ // передвижение вперед
  if((digitalRead(PIN_SENSOR2)== 0) && (digitalRead(PIN_SENSOR1)) == 0) { // Датчик не срабатывает
      digitalWrite(wheelR2,LOW);
            digitalWrite(wheelF1,HIGH);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelF2,HIGH);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
     Serial.println("прямо");
     

  }

  else if ((digitalRead(PIN_SENSOR2)== 1) && (digitalRead(PIN_SENSOR1) == 0)){ // Датчик срабатывает
 digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,180);
   analogWrite(motorAnalog1,180);
     Serial.println("влево");

  }
  else if ((digitalRead(PIN_SENSOR2)== 0) && (digitalRead(PIN_SENSOR1) == 1)){ // Датчик срабатывает
     digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog1,180);
   analogWrite(motorAnalog2,180);
     Serial.println("вправо");
  }
  }else{ // передвижение назад
    if((digitalRead(PIN_SENSOR2)== 0) && (digitalRead(PIN_SENSOR1)) == 0) { // Датчик не срабатывает
      digitalWrite(wheelR2,HIGH);
            digitalWrite(wheelF1,LOW);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelF2,LOW);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
     Serial.println("назад");
  }

  else if ((digitalRead(PIN_SENSOR2)== 1) && (digitalRead(PIN_SENSOR1) == 0)){ // Датчик срабатывает
 digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,120);
   analogWrite(motorAnalog1,120);
     Serial.println("влево"); // если смотреть на движущуюся против нас машину 

  }
  else if ((digitalRead(PIN_SENSOR2)== 0) && (digitalRead(PIN_SENSOR1) == 1)){ // Датчик срабатывает
     digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog1,120);
   analogWrite(motorAnalog2,120);
     Serial.println("вправо"); // если смотреть на движущуюся против нас машину 
  }
    }
      }
      void nextLevelDrive(boolean n){ // второй уровень датчиков линии
         if(n){ // передвижение вперед
  if((digitalRead(PIN_SENSOR3)== 0) && (digitalRead(PIN_SENSOR4)) == 0) { // Датчик не срабатывает
      digitalWrite(wheelR2,LOW);
            digitalWrite(wheelF1,HIGH);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelF2,HIGH);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
     Serial.println("прямо");
     

  }

  else if ((digitalRead(PIN_SENSOR3)== 1) && (digitalRead(PIN_SENSOR4) == 0)){ // Датчик срабатывает
 digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog2,180);
   analogWrite(motorAnalog1,180);
     Serial.println("влево");

  }
  else if ((digitalRead(PIN_SENSOR3)== 0) && (digitalRead(PIN_SENSOR4) == 1)){ // Датчик срабатывает
     digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog1,180);
   analogWrite(motorAnalog2,180);
     Serial.println("вправо");
  }
  }else{ // передвижение назад
    if((digitalRead(PIN_SENSOR3)== 0) && (digitalRead(PIN_SENSOR4)) == 0) { // Датчик не срабатывает
      digitalWrite(wheelR2,HIGH);
            digitalWrite(wheelF1,LOW);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelF2,LOW);        
  analogWrite(motorAnalog2,80);
   analogWrite(motorAnalog1,80);
     Serial.println("назад");
  }

  else if ((digitalRead(PIN_SENSOR3)== 1) && (digitalRead(PIN_SENSOR4) == 0)){ // Датчик срабатывает
 digitalWrite(wheelF1,LOW);
            digitalWrite(wheelF2,HIGH);
      digitalWrite(wheelR1,HIGH);
            digitalWrite(wheelR2,LOW);        
  analogWrite(motorAnalog2,120);
   analogWrite(motorAnalog1,120);
     Serial.println("влево"); // если смотреть на движущуюся против нас машину 

  }
  else if ((digitalRead(PIN_SENSOR3)== 0) && (digitalRead(PIN_SENSOR4) == 1)){ // Датчик срабатывает
     digitalWrite(wheelF1,HIGH);
            digitalWrite(wheelF2,LOW);
      digitalWrite(wheelR1,LOW);
            digitalWrite(wheelR2,HIGH);        
  analogWrite(motorAnalog1,120);
   analogWrite(motorAnalog2,120);
     Serial.println("вправо"); // если смотреть на движущуюся против нас машину 
  }
    }
      }

      int distance(int i){
        if(i == 1){ // датчик растояния
        // Датчик срабатывает и генерирует импульсы шириной 10 мкс или больше

// Генерируем короткий LOW импульс, чтобы обеспечить «чистый» импульс HIGH:

digitalWrite(trigPin1, LOW);

delayMicroseconds(5);

digitalWrite(trigPin1, HIGH);

delayMicroseconds(10);

digitalWrite(trigPin1, LOW);

// Считываем данные с ультразвукового датчика: значение HIGH, которое

// зависит от длительности (в микросекундах) между отправкой

// акустической волны и ее обратном приеме на эхолокаторе.

pinMode(echoPin1, INPUT);

duration1 = pulseIn(echoPin1, HIGH);

// преобразование времени в расстояние

cm1 = (duration1/2) / 29.1;

      return cm1;}
else{ // датчик высоты
// Датчик срабатывает и генерирует импульсы шириной 10 мкс или больше

// Генерируем короткий LOW импульс, чтобы обеспечить «чистый» импульс HIGH:

digitalWrite(trigPin2, LOW);

delayMicroseconds(5);

digitalWrite(trigPin2, HIGH);

delayMicroseconds(10);

digitalWrite(trigPin2, LOW);

// Считываем данные с ультразвукового датчика: значение HIGH, которое

// зависит от длительности (в микросекундах) между отправкой

// акустической волны и ее обратном приеме на эхолокаторе.

pinMode(echoPin2, INPUT);

duration2 = pulseIn(echoPin2, HIGH);

// преобразование времени в расстояние

cm2 = (duration2/2) / 29.1;

      return cm2;
}
}
      
      int compass(){ // функция выдает градус поворота
        int x;
        Wire.beginTransmission(addr);
  Wire.write(0x03); // переходим к регистру 0x03
  Wire.endTransmission();
  
  Wire.requestFrom(addr, 6); // запрашиваем 6 байтов
  if ( Wire.available() )  
  {
    axeler.getMotion6(&axx, &axy, &axz, &gix, &giy, &giz); 
    int h = Wire.read(); // старший байт значения по оси X
    int l = Wire.read(); // младший байт значения по оси X
     x = word(h, l);  // объединяем в двухбайтовое число
  }
      return x;}
 void color() // процедура, определяющая значения цветов RGB
{
 // если 2 и 3 порты отключить, то получим значение красного цвета
 digitalWrite(s2, LOW);
 digitalWrite(s3, LOW);
 red = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);

 // если 3 порт включить, а 2 отключить, то получим синий цвет
 digitalWrite(s3, HIGH);
 blue = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);

 // если 2 включить, а 3 отключить, то получим зеленый цвет
 digitalWrite(s2, HIGH);
 green = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH); 
}
