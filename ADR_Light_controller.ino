#include <EButton.h>
#include <iarduino_RTC.h>

//Определяем пины подключения модуля DS1302 к плате Arduino
#define DS_DAT 7
#define DS_CLK 6
#define DS_RST 8
//Определяем пин управления реле
#define TL_SIG 11
//Определяем пин кнопки
#define BUTT_PIN 2
#define LED_PIN 13
//Создаём объект time класса iarduino_RTC с указанием пинов подключения
iarduino_RTC time(RTC_DS1302, DS_RST, DS_CLK, DS_DAT);
//Создаём объект bt класса EButton кол-во повторений 11 (для устранения дребезга контактов)
EButton bt(BUTT_PIN,11);
//По умолчанию свет отключен
bool lightFlag = false;
//Массив периодов включения в сек
long lightOnRange[2][2] = {{21600L,28800L},{64800L, 79200L}};
//Время свечения от кнопки в мин
const long BT_LIGHT_ON_MIN = 15;
long lightTime = 0;
long currentTime = 0;
  
void setup()
{
  //Активация пин TL_SIG
  pinMode(TL_SIG, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  //По умолчанию реле отключено
  digitalWrite(TL_SIG, LOW);
  digitalWrite(LED_PIN, LOW);
  //Запускаем модуль DS1302
  time.begin();
  Serial.begin(9600);   
  //Установка времени
  //time.settime(10,51,14,8,12,19,7);     // сек, мин, час, день, месяц, год, день недели
}

void loop()
{
  //Выполняем задержку чтобы не выводить данные слишком часто
  Serial.println(time.gettime("H:i:s"));   // выводим время
  
  currentTime = (long)(time.seconds + time.minutes * 60L + time.hours * 3600L);
  
  if (time.midday == 1)
    currentTime += 3600L * 12L;

  if (currentTime == 0L)
    lightTime = 0L;
    
  Serial.println(currentTime);
  
  if(bt.scanState())
  {
    digitalWrite(LED_PIN, HIGH);
    lightTime = currentTime + (BT_LIGHT_ON_MIN * 60L);
    Serial.println("BT_LIGHT_ON_MIN");
    delay(1000);
    digitalWrite(LED_PIN, LOW);
  }
  
  if(currentTime < lightTime)
  {
    lightFlag=true;
    Serial.println("Ok");
    Serial.println(lightTime);
    Serial.println(currentTime);
  }
  else
  {
    lightTime=0;
    for(int i=0; i<2; i++)
    {
      Serial.println("<Элемент ");
      Serial.println(currentTime);
      Serial.println(i);
      Serial.println(lightOnRange[i][0]);
      Serial.println(lightOnRange[i][1]);
      Serial.println("/Элемент>");
             
      if(currentTime > lightOnRange[i][0] && currentTime < lightOnRange[i][1])
      {
        lightFlag=true;
        break;
      }else
      {
        if(i==1)
          lightFlag=false;
      }
    }
  }
  turnLight(lightFlag);
}

void turnLight(bool lightOnFlag){
  if(lightOnFlag){
    digitalWrite(TL_SIG, HIGH);
    Serial.println("TURN_ON");}
  else{
    digitalWrite(TL_SIG, LOW);
    Serial.println("TURN_OFF");}
  }
