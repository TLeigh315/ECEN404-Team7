
const int relay = 0;
const int buttonPin = 2;
const int switchPin = 3;
const int ledPin2 = 4; //RED
const int ledPin3 = 1; // GREEN


int buttonState = 0;
int buttonDown = 0;
int buttonUp = 0;
int state = 0;
int checkOff = 0;

/*
 * State 1 = off  
 * state 2 = weight has come off waiting to turn off
 * state 3 = Turning off Pi
 * state 4 = weight detected waiting to turn on
 * state 5 = on
 */

 
void setup() {
pinMode(relay, OUTPUT);
pinMode(buttonPin, INPUT);
}

bool checkPiOff()
{
   int sensorValue = analogRead(2); //P4 read input GPIO14
   return ((sensorValue/1024.0) < 0.1);
}


void loop() {

 buttonState = digitalRead(buttonPin);
 


 if (buttonState == LOW){
  //if pressure detected
    if (buttonDown <= 10){
        if(state == 5 && buttonUp<=10){
          //On but weight came off for a second
          state = 5;
          buttonUp=0;
        }
        else if(state == 2 && buttonUp<=10){
          // weight has come off for 10s but pressure has returned
          state = 2;
          buttonDown++;
          buttonUp=0;
    }
        else{
          //weight detected waiting to turn on
        buttonDown++;
        buttonUp = 0;
        state = 4;
        }
      
    }
     else if (buttonDown > 10){
      //weight has been on for awhile
      buttonDown++;
      state = 5;
     }

 }
 else if(buttonState == HIGH){
  //no pressure
    if (buttonUp<=100){
      if (state == 1){
        //off
        buttonUp++;
      }
      else if(state == 5){ 
        //if on but weight has come off
        buttonUp++;
        buttonDown =0;
        if(buttonUp<10){
          //if weight has come off for less than 10s
          state=5;
        }
       else if(buttonUp>=10){
        //if on but weight has come off for more than 10s start to turn off
          state=2;
          buttonUp = 0;
      }
      }
      if(state == 4){
        //waiting to turn on
        if(buttonDown <10 && buttonUp <10){
          //if weight has been on or off for less than 10s wait to turn on
          state=4;
        }
        else if(buttonUp>=10){
          //if waiting to turn on but weight has been off for more than 10s turn off
          state=1;
        }
      }
    else if(state == 3){
      //turning off pi soft shutdown
      if(checkOff<15){
        //if pi is off stay in stae 3 check 7 times
        state=3;
      }
      else if(checkOff>=15){
        //if pi is off and have checked 7 times turn off switch
        state=1;
        checkOff = 0;
      }
      
      
      
    }
        
   
    }
 }

  if(state == 1){
    //everything is off
    digitalWrite(switchPin, LOW); //switch is off
 digitalWrite(ledPin2,HIGH); //ledpin2 is on to signal everything is off
 digitalWrite(ledPin3,LOW); //ledpin3 is off
  digitalWrite(relay, LOW); // relay is off
  delay(100); // wait 1 second
  buttonUp++; // increase button up counter +1
  buttonDown=0; // botton down counter is = 0 
  }
else if(state==2){
  // waiting to turn off system
  if (buttonUp < 10 && buttonDown <10){
    //button was pressed but released flash led3 to tell us weight was removed
        digitalWrite(relay, HIGH);
        digitalWrite(ledPin2, HIGH);
       digitalWrite(ledPin3, HIGH);
        delay(400);
        digitalWrite(ledPin3, LOW);
        delay(400);
         digitalWrite(ledPin3, HIGH);
        delay(400);
        digitalWrite(ledPin3, LOW);
        delay(400);
        buttonUp++;
        }
        else if (buttonUp >= 10 && buttonDown < 10){
          // was pressed but was released for longer than 10s
          buttonDown = 0;
          buttonUp=0;
          digitalWrite(switchPin, HIGH);  //signal sent to turn off rpi
          delay(500);
          state = 3;
        }
        else if (buttonUp <10 && buttonDown >= 10){
          //button was pressed and released and pressed for 10s will stay on
          buttonUp = 0;
          state = 5;
        }
}


else if (state == 3){
  //turning off delay to let rpi turn off safely
    digitalWrite(switchPin, LOW);
    //digitalWrite(relay,HIGH);
    checkOff++;
   digitalWrite(ledPin3, HIGH);
   digitalWrite(ledPin2, LOW);
        delay(500);
   digitalWrite(ledPin3, LOW);
   digitalWrite(ledPin2, HIGH);
        delay(500);
   digitalWrite(ledPin3, HIGH);
   digitalWrite(ledPin2, LOW);
        delay(500);
   digitalWrite(ledPin3, LOW);
   digitalWrite(ledPin2, HIGH);
        delay(500);
   digitalWrite(ledPin3, HIGH);
   digitalWrite(ledPin2, LOW);
        delay(500);
   digitalWrite(ledPin3, LOW);
   digitalWrite(ledPin2, HIGH);
        delay(500);
   digitalWrite(ledPin3, HIGH);
   digitalWrite(ledPin2, LOW);
        delay(500);
   digitalWrite(ledPin3, LOW);
   digitalWrite(ledPin2, HIGH);
        delay(500);

     if(checkOff<20){
        //if pi is off stay in stae 3 check 7 times
        state=3;
      }
      else if(checkOff>=20){
        //if pi is off and have checked 7 times turn off switch
        state=1;
        checkOff = 0;
      }
      
    
    
}


 else if (state == 4){
        //Weight detected is off but waiting to turn on
         if (buttonUp < 10 && buttonDown <10){
          //flash green light to let us know weight was detected
        digitalWrite(ledPin2, HIGH);
       digitalWrite(ledPin3, HIGH);
        delay(200);
        digitalWrite(ledPin3, LOW);
        delay(200);
         digitalWrite(ledPin3, HIGH);
        delay(200);
        digitalWrite(ledPin3, LOW);
        delay(200);
        buttonUp++;
        }
        else if (buttonUp >= 10 && buttonDown < 10){
          //weight detected but was off for longer than 10s
          buttonDown = 0;
          buttonUp=0;
          state = 1;
        }
        else if (buttonUp <10 && buttonDown >= 10){
          //weight detected came off but stayed on for 10s turn on
          buttonUp = 0;
          state = 5;
        }
        
 }
  else if (state == 5){
    //system is on
    if( buttonState==LOW){
      //relay turns on and stays on
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3,LOW);
    digitalWrite(relay, HIGH);
    delay (500);
    buttonDown = 0;
    buttonUp=0;
    }
    else if(buttonState == HIGH && buttonUp < 10){
      //weight came off for a few seconds but goes back on  led2 
      //turns on but system stays on until weigh off for more than 10 s
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3,HIGH);
    digitalWrite(relay, HIGH);
    delay (500);
    buttonDown = 0;
    buttonDown++;
    }
    
    else if(buttonUp>=10 && buttonState == LOW){
      //weight off for more than 10s begin to turn off
      state =2;
    }
    

  }
}
  
