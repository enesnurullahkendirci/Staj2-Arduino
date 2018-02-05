// Author : ChungYi Fu (Taiwan)
// ESP8266 
// Server static IP: 192.168.4.1
// Turn On : http://192.168.4.1/?on
// Turn Off : http://192.168.4.1/?off
// Client dynammic IP: 
// Turn On : http://192.168.xxx.xxx/?on
// Turn Off : http://192.168.xxx.xxx/?off

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // Arduino RX:10, TX:11 

String SSID = "id";
String PWD = "pwd";

void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);

  SendData("AT+RST",10000);
  SendData("AT+CWMODE=3",2000);
  SendData("AT+CIPMUX=1",2000);
  SendData("AT+CIPSERVER=1,80",2000);
  //SendData("AT+CIPSTA=\"192.168.0.3\",\"192.168.0.1\",\"255.255.255.0\"",2000);
  SendData("AT+CWJAP=\""+SSID+"\",\""+PWD+"\"",10000);
}

void loop() 
{
  String str="", cmd="";
  int getstate = 0;
  if (mySerial.available())
  {
    while (mySerial.available())
    {
      char c = mySerial.read();
      delay(10);
      str = str + String(c);
      if (String(c).indexOf("?")!= -1) getstate=1;
      if (String(c).indexOf(" ")!= -1) getstate=0;
      if ((getstate==1)&&(String(c).indexOf("?")== -1)) cmd = cmd + String(c);
    }  
    Serial.println(str);
  }
  
  if (str.indexOf(" HTTP")!= -1)
  {
    Serial.println("");
    Serial.println("command: " + cmd);
    while (mySerial.available())
    {
      mySerial.read();
    }
    
    if (cmd=="on")
    {
      pinMode(2,OUTPUT);
      digitalWrite(2,HIGH);
      feedback(str,"<font color=red>TURN ON</font>",32);
    }
    else if (cmd=="off")
    {
      pinMode(2,OUTPUT);
      digitalWrite(2,LOW);  
      feedback(str,"<font color=blue>TURN OFF</font>",34);
    }
    else if (cmd=="ip")
    {
      mySerial.println("AT+CIFSR");
      mySerial.flush();
      delay(100);
      while (mySerial.available())
      {
        char cip = mySerial.read();
        Serial.print(cip);
      }
      feedback(str,"IP",4);
    }    
    else if (cmd=="your command")
    {
      // you can do anything
      // feedback(str,yourfeedback,(yourfeedback.length()+2));
    }
    else 
    {
      feedback(str,"FAIL",6);
    }  
  }
}

void SendData(String data,int timelimit)
{
  mySerial.println(data);
  mySerial.flush();
  delay(20);
  waitreply(timelimit);
}

void feedback(String str,String response,int len)
{
    String CID = String(str.charAt(str.indexOf("IPD,")+4));
    
    SendData("AT+CIPSEND="+CID+",8",0);
    SendData("<html>",2000);
    SendData("AT+CIPSEND="+CID+",8",0);
    SendData("<head>",2000);
    SendData("AT+CIPSEND="+CID+",69",0);
    SendData("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">",10000);
    SendData("AT+CIPSEND="+CID+",9",0);
    SendData("</head>",2000);
    SendData("AT+CIPSEND="+CID+",8",0);
    SendData("<body>",2000);
    SendData("AT+CIPSEND="+CID+","+String(len),0);
    SendData(response,10000);
    SendData("AT+CIPSEND="+CID+",16",0);
    SendData("</body></html>",2000);
    SendData("AT+CIPCLOSE="+CID,2000);
}

void waitreply(int timelimit)
{
    int st = 0;
    long int starttime = millis();
    while( (starttime + timelimit) > millis())
    {
        while(mySerial.available())
        {
            char c = mySerial.read();
            delay(10);
            //Serial.print(c);
            st=1;
        }
        if (st==1) return;
    } 
}
