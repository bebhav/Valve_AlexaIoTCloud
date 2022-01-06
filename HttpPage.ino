void handleRoot() 
{
 #include "index.h"
 String s = webpage;
 serverHtml->send(200, "text/html", s);
}

void UpTime()
{
  String TimeValue;
  unsigned int hour = 0 ,minute = 0, days = 0;
  unsigned int sec = millis()/1000;
  
  if(sec > 60 )
  {
    minute = sec/60; 
    sec = sec%60;
    if(minute > 60 )
    {
      hour = minute/60; 
      minute = minute%60;
      if(hour > 24) 
      {
        days = hour/ 24; 
        hour = hour%24;
      }
    }
  }
  TimeValue = String(days)+ "D: " + String(hour) +"H: "+String(minute)+"m: "+String(sec)+"s";
  
 serverHtml->send(200, "text/plane", TimeValue);
}

void StatusPage() 
{
 String StatusMsg;

 StatusMsg += "LowerOn : "   + String(digitalRead(VALVE1ON));
 StatusMsg += ", LowerOff : " + String(digitalRead(VALVE1OFF));
 StatusMsg += ", UpperOn : "  + String(digitalRead(VALVE2ON));
 StatusMsg += ", UpperOff : " + String(digitalRead(VALVE2OFF));
 StatusMsg += ", LowerPersent : " + String(LowerPercent);
 StatusMsg += ", UpperPersent : " + String(UpperPercent);

 serverHtml->send(200, "text/plane", StatusMsg);
}

void SetValve() 
{
  String valve_state = serverHtml->arg("state");
  String pageVal = "Unexpected Input " + valve_state;
  String additionalMsg = "";
  
  if(valve_state.endsWith("UP_ON"))
  {
    onValve1=true;
    pageVal = "Upper Valve Turning On";
  }
  else if(valve_state.endsWith("UP_OFF"))
  {
    if(onValve2 == false )
    {
      onValve2 = true;   
      additionalMsg = " Lower Valve Turning On";
    }
    onValve1=false;
    pageVal = "Upper Valve Turning Off" + additionalMsg;
  }
  else if(valve_state.endsWith("LO_ON"))
  {
    onValve2 = true;
    pageVal = "Lower Valve Turning On";
  }
  else if(valve_state.endsWith("LO_OFF"))
  {
    if(onValve1 == false)
    {
      onValve1 = true;
      additionalMsg = " Lower Valve Turning On";
    }
    onValve2 = false;
    pageVal = "Lower Valve Turning Off";
  }
  else if(valve_state.endsWith("STOP_ALL"))
  {
    CmdOperateValve(false, "STOP_ALL");
    pageVal = "Stoping All valve";
    serverHtml->send(200, "text/plane", pageVal);
    return;
  }
  
  CmdOperateValve(onValve1, "Upper");
  CmdOperateValve(onValve2, "Lower");
  onBothValve = onValve1 == true && onValve2 == true;
  serverHtml->send(200, "text/plane", pageVal);
}

//not found URI (only if internal webserver is used)
void serveNotFound()
{
    serverHtml->send(404, "text/plain", "This Page Not Found");
}

void RegisterWebPage()
{
  if(!serverHtml)
  {
    serverHtml = new ESP8266WebServer(80);  
    serverHtml->onNotFound([=](){serveNotFound();});
  }
  if(serverHtml)
  {
    serverHtml->on("/", handleRoot);
    serverHtml->on("/uptime", UpTime);
    serverHtml->on("/Status", StatusPage);
    serverHtml->on("/Set_Valve", SetValve);
    serverHtml->begin();
  }
}
