#define INTERVAL_SENSOR   17000             //定义传感器采样时间间隔  597000
#define INTERVAL_NET      17000             //定义发送时间
//传感器部分================================   
#include <Wire.h>                                  //调用库  
#include "ESP8266.h"
#include "I2Cdev.h"                                //调用库
#define SSID           "contradictio"                   // cannot be longer than 32 characters!
#define PASSWORD       "99593699"
#define IDLE_TIMEOUT_MS  3000
#define HOST_NAME   "api.heclouds.com"
#define DEVICEID   "503135834"
#define PROJECTID "184050"
#define HOST_PORT   (80)
String apiKey="ndDzDA=1nFUl9sT4nEYAtAnTAKo=";
char buf[10];
#define INTERVAL_sensor 2000
unsigned long sensorlastTime = millis();
#define INTERVAL_OLED 1000
String mCottenData;
String jsonToSend;
#include <SoftwareSerial.h>
#define EspSerial mySerial
#define UARTSPEED  9600
SoftwareSerial mySerial(2, 3); /* RX:D3, TX:D2 */
ESP8266 wifi(&EspSerial);
unsigned long net_time1 = millis();                          //数据上传服务器时间
unsigned long sensor_time = millis();                        //传感器采样时间计时器
String postString;


int Sensor1 = 8;//接收传感器的信号
int Sensor2 = 10;
int buffer=0,num=0;
void setup()
{
  Wire.begin();
  Serial.begin(115200);   
  while(!Serial);
  pinMode(Sensor1, INPUT); // 设置为输入
  pinMode(Sensor2,INPUT);
  while (!Serial); // wait for Leonardo enumeration, others continue immediately
  Serial.print(F("setup begin\r\n"));
  delay(100);

  WifiInit(EspSerial, UARTSPEED);

  Serial.print(F("FW Version: "));
  Serial.println(wifi.getVersion().c_str());


  if (wifi.setOprToStation()) {
    Serial.print(F("to station ok\r\n"));
  } else {
    Serial.print(F("to station err\r\n"));
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print(F("Join AP success\r\n"));
    Serial.print(F("IP: "));
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print(F("Join AP failure\r\n"));
  }

  Serial.print(F("setup end\r\n"));
}
void loop()
{
    
  int val1,val2;
  val1 = digitalRead(Sensor1); // 读取引脚电平
  val2 = digitalRead(Sensor2);
  if (val1)  // 若检测到有人，则val值为1
      {
        if(buffer<0)
        {num--;}
        buffer++;
      }
  if(val2)
  {if(buffer>0)
    num++;
    buffer--;
  }

  
    if (net_time1 > millis())  net_time1 = millis();
  if (millis() - net_time1 > INTERVAL_NET)                  //发送数据时间间隔
  {                
    updateSensorData();                                     //将数据上传到服务器的函数
    net_time1 = millis();
  }

  
  delay(100);
}

void updateSensorData() {

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据

    Serial.print("create tcp ok\r\n");
    jsonToSend="{\"num\":";

    dtostrf(num,1,0,buf);

    jsonToSend+="\""+String(buf)+"\"";

    jsonToSend+="}";


    postString="POST /devices/";

    postString+=DEVICEID;

    postString+="/datapoints?type=3 HTTP/1.1";

    postString+="\r\n";

    postString+="api-key:";

    postString+=apiKey;

    postString+="\r\n";

    postString+="Host:api.heclouds.com\r\n";

    postString+="Connection:close\r\n";

    postString+="Content-Length:";

    postString+=jsonToSend.length();

    postString+="\r\n";

    postString+="\r\n";

    postString+=jsonToSend;

    postString+="\r\n";

    postString+="\r\n";

    postString+="\r\n";



  const char *postArray = postString.c_str();                 //将str转化为char数组

  Serial.println(postArray);

  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)

  Serial.println("send success");   

     if (wifi.releaseTCP()) {                                 //释放TCP连接

        Serial.print("release tcp ok\r\n");

        } 

     else {

        Serial.print("release tcp err\r\n");

        }

      postArray = NULL;                                       //清空数组，等待下次传输数据

  

  } else {

    Serial.print("create tcp err\r\n");

  }

  

}

