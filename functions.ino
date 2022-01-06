//----------------------------------------------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
int statusCode;
String st;
String content;
String esid = "";
String epass = "";
//Establishing Local server at port 80 whenever required

ESP8266WebServer *server; //server for hotspot 

bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( 1 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    else if( analogRead(A0) > 600 )
    {
      ButoonPressedForConfigPage(1000);
    }
    else
    {
      delayIndecator(500,3);
      Serial.print("*");    
    }
  }
  
  return false;
}

void UpdateWifiList()
{
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
}

void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  UpdateWifiList();
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC :");
  Serial.println(WiFi.macAddress());
  String str = String(mac[4], HEX);
  str += String(mac[5], HEX);
  WiFi.softAP("WaterValveESP-" + str, "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}

void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server->begin();
  Serial.println("Server started");
}

void createWebServer()
{
  if(!server)
  {
    Serial.println("Init Hotspot Server");
    server = new ESP8266WebServer(80);  
    server->onNotFound([=](){serveNotFound();});
  }
  if(serverHtml)
  {
    Serial.println("deleting old Server");
    delete serverHtml;
    serverHtml = NULL;
  }
  
  if(server)
 {
    Serial.println("Init Hotspot Server complete");
    server->on("/", []() {
      Serial.println("Refreshing Page");
      createWebServer();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html><h3>Config Page ESP WaterValve</h3>";
      content += "<title>ESP WaterValve</title>";
      content += "<button onClick=\"window.location.reload();\">Re-Scan</button>";
      //content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += "<p>IP Addr: ";
      content += ipStr;
      content += "</p><p>";
      content += st;
      content += "</p><form method='get' action='setting'>";
      content += "<p> <label>SSID:</label> <input name='ssid' maxlength=\"32\" required> <label>Pass:</label> <input name='pass' maxlength=\"64\"> </p>";
      //content += "<p> <label>DeviceName:</label><input name='device' maxlength=\"32\" required> </p> <h3>Provide controling server details</h3> <p><label>Server IP: </label><input name='server_ip' maxlength=\"15\" required >"; 
      //content += "<label>Server PORT: </label><input name='server_port' type=\"number\" max=\"65535\" min =\"0\" required> </p><p> <input type='submit'></p>";
      content += "<p> <input type='submit'></p>";
      content += "</form>";
      content += "</html>";
      server->send(200, "text/html", content);
    });
    /*
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });
*/
    server->on("/setting", []() {
      String qsid = server->arg("ssid");
      String qpass = server->arg("pass");
     /* String device = server.arg("device");
      String server_ip = server.arg("server_ip");
      String server_port = server.arg("server_port");*/
      
      if (qsid.length() > 0)// && device.length() > 0 && server_ip.length() > 7 && device.length() > 0) 
      {
        WriteToEeprom(qsid,qpass);//,device,server_ip,server_port);
        content = "<!DOCTYPE HTML>\r\n<html><h2>{\"Success\":\"Rebooting. To change the config, long press reset button on the board\"}</h2></html>";
        statusCode = 200;
      } else {
        content = "<!DOCTYPE HTML>\r\n<html><h2>{\"Error\":\"Input details are not correct 404 not found\"}</h2></html>";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server->send(statusCode, "text/html", content);
      //server.sendHeader("Access-Control-Allow-Origin", "*");
      //server.send(statusCode, "application/json", content);
      delay(1000);
      if(statusCode == 200)
        ESP.reset();

    });

    server->on("/test", []() {     
      server->send(200, "text/html", "This is a test page");
    });

    Serial.println("Page reg complete");
  }
  else{
    Serial.println("Init failed, rebooting");
     ESP.reset();
    } 
}

void StartHotSpotWithConfigPage()
{
      WiFi.disconnect();
      Serial.println("Turning the HotSpot On");
      launchWeb();
      setupAP();// Setup HotSpot
      
      Serial.println();
      Serial.println("Waiting.");

      while ((WiFi.status() != WL_CONNECTED))
      {        
        delayIndecator(100,5);
        Serial.print(".");
        server->handleClient();
      }
}

void ReadFromEeprom()
{
  
  Serial.println("Reading EEPROM"); Serial.println();
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.print("SSID: ");
  Serial.println(esid);
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);
  Serial.println();
  
  /*for (int i = 96; i < 96 + 32; ++i)
  {
    DeviceName += char(EEPROM.read(i));
  }
  Serial.print("DeviceName: ");
  Serial.println(DeviceName);

  for (int i = 96 + 32; i < 96 + 32 + 15; ++i)
  {
    ServerIP += char(EEPROM.read(i));
  }
  Serial.print("ServerIP: ");
  Serial.println(ServerIP);

  for (int i = 96 + 32 + 15; i < 96 + 32 + 15 + 5; ++i)
  {
    ServerPort += char(EEPROM.read(i));
  }
  Serial.print("ServerPort: ");
  Serial.println(ServerPort);
  */
}

void WriteToEeprom(String qsid, String qpass)//,String device,String server_ip, String server_port)
{
        Serial.println("clearing eeprom");
        for (int i = 0; i < 512; ++i) 
        {
          EEPROM.write(i, 0);
        }
        
        Serial.println(qsid);Serial.println("");
        Serial.println(qpass);Serial.println("");
        /*Serial.println(device);Serial.println("");
        Serial.println(server_ip);Serial.println("");
        Serial.println(server_port);Serial.println("");*/

        EepromWriteOperation(0,                   qsid);
        EepromWriteOperation(32,                  qpass);
        /*EepromWriteOperation(32 + 64,             device);
        EepromWriteOperation(32 + 64 + 32,        server_ip);
        EepromWriteOperation(32 + 64 + 32 + 15,   server_port);*/
        EEPROM.commit();
}

void EepromWriteOperation(int start, String str )
{
   Serial.print("Writing " + str + " At location = ");
   Serial.print( start );
   
    for (int i = 0; i < str.length() ; i++ )
    {
      EEPROM.write(i + start, str[i]);
    }
}

void ButoonPressedForConfigPage(int cnt)
{
  int dly= 0;
  static int SkipCount;
  if(SkipCount++ > cnt)
  {
    SkipCount = 0;
    while( analogRead(A0) > 600 )   // read the input pin
    {
      Serial.println("Inside skip cnt");
      dly++;
      delayIndecatorFast(100,100);
      if(dly > 50) // 5sec
      {
        StartHotSpotWithConfigPage();
      }
    }
  }
}

// each time led will flash for flashDuration mSec and off for offDuration mSec and in last off for one sec
void LedStateIndicater(int flashCount, int flashDuration, int offDuration) 
{
  static int delayTimer = 0;
  static int currentFlashCount = 0; 
  static int maxDelayValue = 0;
  const int blinkCycle = flashDuration + offDuration;
  
  if(flashCount!= -1 && currentFlashCount != flashCount)
  {
   currentFlashCount = flashCount   ;
   maxDelayValue = (flashCount)*(blinkCycle) + (1000 - offDuration); // 800 + last 200 off = 1 sec.
   delayTimer = 0;
   Serial.printf("Led: Flash count = %d\n\r",flashCount);
  }

  delay(1);
  delayTimer = (delayTimer+1)%maxDelayValue;
  
  if(maxDelayValue - (1000 - offDuration) >= delayTimer)
  {
    int reminder = delayTimer %(blinkCycle);
    if(reminder == 1)
      digitalWrite(LED_BUILTIN, LOW);
    else if(reminder == 1+flashDuration)
      digitalWrite(LED_BUILTIN, HIGH);
  }
}

void delayIndecator(long timeInMiliSec , int flashCnt)
{
  for (long i = 0 ; i < timeInMiliSec ; i++)
    LedIndication(flashCnt);
}

void delayIndecatorFast(long timeInMiliSec , int flashCnt)
{
  for (long i = 0 ; i < timeInMiliSec ; i++)
    LedIndicationFast(flashCnt);
}

void LedIndication(int flashCnt)
{
  LedStateIndicater(flashCnt,5,2000);
}

void LedIndicationFast(int flashCnt)
{
  LedStateIndicater(flashCnt,50,50);
}

void LedIndicationWithArduinoIoT(int flashCnt)
{
  static int skipcnt;
  if(flashCnt >1) delay(500);
  if(skipcnt++>5)
  {
    skipcnt =0 ;
    for(int i = 0; i<flashCnt; i++)
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(5);
      digitalWrite(LED_BUILTIN, HIGH);
      if(i!=flashCnt-1) delay(200);
    }
  }
}


void setupWifi()
{
  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");

  //---------------------------------------- Read eeprom 
  
  Serial.println("Reading EEPROM pass");
  ReadFromEeprom();

  Serial.print("MAC :");
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.println(WiFi.macAddress());
  
  WiFi.softAPdisconnect(true);
  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Succesfully Connected!!!");
    return;
  }

// Not Connected to wifi   
  StartHotSpotWithConfigPage();

}
