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
//Время свечения от кнопки в мин
const long BT_LIGHT_ON_MIN = 15;
long lightTime = 0;
//Текущее время в сек
long currentTime = 0;
//Текущий день недели
int dayOfWeek = 0;

class lightPeriodByWeekDays 
{
  private:
    long _lightSt;
    long _lightEnd;
    bool _dayOn[7];
  public: 
    lightPeriodByWeekDays (long lightSt = 0,long lightEnd = 0)
    {
      _lightSt = lightSt;
      _lightEnd = lightEnd;
      SetDays();
    };
    void SetDays(bool Mon=true, bool Tue=true, bool Wed=true, bool Thu=true, bool Fri=true, bool Sat=true, bool Sun=false) 
    {
      _dayOn[0]=Sun;
      _dayOn[1]=Mon;
      _dayOn[2]=Tue;    
      _dayOn[3]=Wed; 
      _dayOn[4]=Thu;
      _dayOn[5]=Fri;
      _dayOn[6]=Sat;    
    };
    void View ()
    {
      Serial.println("День недели");        
      Serial.println(_dayOn[0]);          
      Serial.println(_dayOn[1]);  
      Serial.println(_dayOn[2]);  
      Serial.println(_dayOn[3]);    
      Serial.println(_dayOn[4]); 
      Serial.println(_dayOn[5]); 
      Serial.println(_dayOn[6]); 
      Serial.println("Конец");
    }
    bool LightON(long sec, int d)
    {
      bool inRange = sec > _lightSt && sec < _lightEnd && _dayOn[d];
      //Необходимо для отладки
      //View();       
      return   inRange;
    }
};

  lightPeriodByWeekDays e=lightPeriodByWeekDays(70200L, 82800L);
  lightPeriodByWeekDays tm[2]={lightPeriodByWeekDays(21600L,25200L),e};
  
  
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
  //time.settime(10,28,19,11,1,20,6);     // сек, мин, час, день, месяц, год, день недели
  //e.SetDays(false,false,true,false,false,true,false);
  //tm[1]=e;
}

void loop()
{ 
  Serial.println(time.gettime("w - H:i:s"));   // выводим время
  
  currentTime = (long)(time.seconds + time.minutes * 60L + time.hours * 3600L);

  dayOfWeek = (int)(time.weekday);

  if (time.midday == 1)
    currentTime += 43200L;

  if (currentTime == 0L)
    lightTime -= 86400L;
    
  Serial.println(currentTime);
  Serial.println(dayOfWeek);
  
  if(bt.scanState())
  {
    digitalWrite(LED_PIN, HIGH);
    lightTime = currentTime + (BT_LIGHT_ON_MIN * 60L);
    Serial.println("BT_LIGHT_ON");
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
      if(tm[i].LightON(currentTime,dayOfWeek))
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
