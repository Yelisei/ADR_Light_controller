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
//Массив периодов включения в сек: вкл, выкл, кроме день недели
long lightOnRange[2][2] = {{21600L,28800L},{64800L, 79200L}};
//Время свечения от кнопки в мин
const long BT_LIGHT_ON_MIN = 15;
long lightTime = 0;
long currentTime = 0;
int dayOfWeek = 8;

class lp 
{
  private:
    long _lightSt;
    long _lightEnd;
    bool _dayOn[7];
  public: 
    lp (long lightSt = 0,long lightEnd = 0)
    {
      _lightSt = lightSt;
      _lightEnd = lightEnd;
      setDays();
    };
    void setDays(bool Mon=true, bool Tue=true, bool Wed=true, bool Thu=true, bool Fri=true, bool Sat=true, bool Sun=false) 
    {
      _dayOn[0]=Sun;
      _dayOn[1]=Mon;
      _dayOn[2]=Tue;    
      _dayOn[3]=Wed; 
      _dayOn[4]=Thu;
      _dayOn[5]=Fri;
      _dayOn[6]=Sat;    
    };
    bool on(long sec, int d)
    {
      bool inRange = sec > _lightSt && sec < _lightEnd && _dayOn[d];
      Serial.println("Вот эта пижда");
      
      Serial.println(d);
      
      Serial.println(_dayOn[0]);          
      Serial.println(_dayOn[1]);  
      Serial.println(_dayOn[2]);  
      Serial.println(_dayOn[3]);    
      Serial.println(_dayOn[4]); 
      Serial.println(_dayOn[5]); 
      Serial.println(_dayOn[6]); 

      Serial.println("Вот эта пижда");    
          
      return   inRange;
    }
};

  lp e=lp(64800L, 79200L);
  lp tm[2]={lp(21600L,28800L),e};
  
  
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
  e.setDays(false,false,true,false,false,true,false);
  tm[1]=e;
}

void loop()
{
  for(int i=0; i<2; i++)
    {
      Serial.print("<Object ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(tm[i].on(currentTime,dayOfWeek));
      Serial.print(" /Object>");
      Serial.println( );
    }
  
  Serial.println(time.gettime("w - H:i:s"));   // выводим время
  
  currentTime = (long)(time.seconds + time.minutes * 60L + time.hours * 3600L);

  dayOfWeek = (int)(time.weekday);

  if (time.midday == 1)
    currentTime += 43200L;

  if (currentTime == 0L)
    lightTime = 0L;
    
  Serial.println(currentTime);
  Serial.println(dayOfWeek);
  
  if(bt.scanState())
  {
    digitalWrite(LED_PIN, HIGH);
    lightTime = currentTime + (BT_LIGHT_ON_MIN * 60L);
    Serial.println("BT_LIGHT_ON");
    delay(1000);
    digitalWrite(LED_PIN, LOW);
  }
  
  if(currentTime < lightTime)
  {
    lightFlag=true;
    Serial.println("Turn on for 15 min");
    Serial.println(lightTime);
  }
  else
  {
    lightTime=0;
    for(int i=0; i<2; i++)
    {
      Serial.print("<Элемент ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(lightOnRange[i][0]);
      Serial.print(" ");
      Serial.print(lightOnRange[i][1]);
      Serial.print(" /Элемент>");
      Serial.println( );
            
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
