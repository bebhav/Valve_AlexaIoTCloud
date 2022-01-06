extern volatile int LowerValveState;
extern volatile int UpperValveState;

void HandleMotorSensor(EnumValves *valve, uint8_t motorP, uint8_t motorN, uint8_t volveOn, uint8_t volveOff, unsigned int cmdStartTime, String valveName )
{
  unsigned int cmdTime = VALVEO_OPERATION_TIME;
  
  if(*valve != eValveIdea)
  {
    static unsigned int lastPrintTime;
    cmdTime = millis() - cmdStartTime;
    if(lastPrintTime <= millis()) // for debug
    {
      lastPrintTime = millis()+2000;
      Serial.print(valveName);
      Serial.print(" time = ");
      Serial.print(cmdTime);
      Serial.print(" SensorOn = ");
      Serial.print(digitalRead(volveOn));
      Serial.print(" SensorOff = ");
      Serial.print(digitalRead(volveOff));
      Serial.print(" Oper = ");
      Serial.println(*valve);
    }
  }
  
  if(*valve == eValveOn)
  {
    bool sensor = digitalRead(volveOn);
    if(sensor){
      int cnt = 10;
      while(sensor = digitalRead(volveOn)){
        delay(1);
        if(!cnt--)break;
      }
    }
    if(cmdTime < VALVEO_OPERATION_TIME && sensor!= HIGH)
    {
      digitalWrite(LED_BUILTIN,LOW);
      digitalWrite(motorP,MOTORHIGH);
      digitalWrite(motorN,MOTORLOW);
      int per = ((cmdTime * 100)/VALVEO_OPERATION_TIME);
      if(per%10==0)
        msg = valveName + " " + per;
      if(valveName=="Lower") LowerPercent = per; else if(valveName=="Upper") UpperPercent = per;
    }
    else
    {
      Serial.println("Operation on Complete");
      digitalWrite(LED_BUILTIN,HIGH);
      msg = "Operation on Complete "+  valveName;
      if(valveName=="Lower") {LowerValveState = 1; LowerPercent = 100;} else if(valveName=="Upper"){ UpperValveState = 1;UpperPercent = 100;}
      *valve = eValveIdea;
      digitalWrite(motorP,MOTORLOW);
      digitalWrite(motorN,MOTORLOW);
      Serial.print(" time = ");
      Serial.print(cmdTime);
      Serial.print(" SensorOn = ");
      Serial.print(sensor);
      
    }
  }
  else if(*valve == eValveOff)
  {
    bool sensor = digitalRead(volveOff);
    if(sensor){
      int cnt = 10;
      while(sensor = digitalRead(volveOff)){
        delay(1);
        if(!cnt--)break;
      }
    }
    if(cmdTime < VALVEO_OPERATION_TIME && sensor != HIGH)
    {
      digitalWrite(LED_BUILTIN,LOW);
      int per = (100-((cmdTime * 100)/VALVEO_OPERATION_TIME));
      if(per%10==0)
        msg = valveName + " " + per;
      if(valveName=="Lower") LowerPercent = per; else if(valveName=="Upper") UpperPercent = per;
      digitalWrite(motorP,MOTORLOW);
      digitalWrite(motorN,MOTORHIGH);
    }
    else
    {
      Serial.println("Operation off Complete");
      digitalWrite(LED_BUILTIN,HIGH);
      msg = "Operation off Complete " + valveName ;
      if(valveName=="Lower") {LowerValveState = 0; LowerPercent = 0;} else if(valveName=="Upper"){ UpperValveState = 0;UpperPercent = 0;}
      *valve = eValveIdea;
      digitalWrite(motorP,MOTORLOW);
      digitalWrite(motorN,MOTORLOW);
      Serial.print(" time = ");
      Serial.print(cmdTime);
      Serial.print(" SensorOn = ");
      Serial.print(sensor);
    }
  }
}

void initMillis(EnumValves *Valve,unsigned int *time)
{
  if(*Valve==eValveIdea)
      {
        Serial.print("Fresh Start : Time = "+ String(*time ) );
        *time = millis();
        Serial.println(" Updated to = "+ String(*time ) + " oper = " + String(*Valve));
      }
      else
      {
        Serial.print("Toggle oper : Time = "+ String(*time ) );
        *time = millis() + (millis() - *time) - VALVEO_OPERATION_TIME ;
        Serial.println(" Updated to = "+ String(*time ) + " Mill = "+ String(millis()) + " oper = " + String(*Valve));
      }
}

void CmdOperateValve(bool OpenClose, String LowerUpper)
{
  EnumValves *Valve = &nullValve;
 if(LowerUpper == "Lower")
 {
  Valve = &StartLower; 
  Serial.print(" SensorOn = ");
  Serial.print(digitalRead(VALVE1ON));
  Serial.print(" SensorOff = ");
  Serial.println(digitalRead(VALVE1OFF));
  if(LowerValveState != OpenClose)
  {
    if(OpenClose)
    {
      if(*Valve!=eValveOn)
      {
        initMillis(Valve,&LowerTimer);
        *Valve = eValveOn;
        Serial.print("Valve Lower Turning On ");
        LowerValveState = -1;
      }
      else{
        Serial.print("Already in the process of on ");
      }
    }
    else
    {
      if(*Valve!=eValveOff)
      {
        initMillis(Valve,&LowerTimer);
        *Valve = eValveOff;
        Serial.print("Valve Lower Turning Off ");
        LowerValveState =-1;
      }
      else{
        Serial.print("Already in the process of on ");
      }
    }
  }else Serial.print("No change is valve ");
 }
 else if(LowerUpper == "Upper")
 {
  Valve = &StartUpper; 
  Serial.print(" SensorOn = ");
  Serial.print(digitalRead(VALVE2ON));
  Serial.print(" SensorOff = ");
  Serial.println(digitalRead(VALVE2OFF));
  if(UpperValveState != OpenClose)
  {
    if(OpenClose)
    {
      if(*Valve!=eValveOn)
      {
        initMillis(Valve,&UpperTimer);
        *Valve = eValveOn;
        Serial.print("Valve Lower Turning On ");
        UpperValveState = -1;
      }
      else{
        Serial.print("Already in the process of on ");
      }
    }
    else
    {
      if(*Valve!=eValveOff)
      {
        initMillis(Valve,&UpperTimer);
        *Valve = eValveOff;
        Serial.print("Valve Lower Turning Off ");
        UpperValveState =-1;
      }
    }
  }else Serial.print("No change is valve ");
 }
 else if(LowerUpper == "STOP_ALL")
 {
   LowerTimer = UpperTimer =  millis() + VALVEO_OPERATION_TIME ;
   Serial.print("Stopping all valves ");
 }
 Serial.print(OpenClose);
 Serial.println("%");
}

//callback from elexa
void ValveLowerChanged(bool OpenClose) {
  CmdOperateValve(OpenClose, "Lower");
  Serial.println("Operating Lower Valve");
    
}

// call back from elexa
void ValveUpperChanged(bool OpenClose) {
  CmdOperateValve(OpenClose, "Upper");
  Serial.println("Operating Upper Valve");

}

int ValveState(int pinOn, int pinOff)
{
  bool OnState  = digitalRead(pinOn);
  bool OffState = digitalRead(pinOff);
  if( OnState && OffState )
  {
    return 25;
  }
  else if( !OnState && !OffState )
  {
    return 75;
  }
  else if( !OnState && OffState )
  {
    return 0;
  }
  else //if( OnState && !OffState )
  {
    return 100;
  }
}
