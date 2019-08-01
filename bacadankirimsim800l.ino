#include <gprs.h>
#include <softwareserial.h>
#define TIMEOUT    5000
#define LED_PIN    13
GPRS gprs;
void setup() { 
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
 
  Serial.println("Starting SIM800 Auto Read SMS");
  gprs.preInit();
  delay(1000);
 
  while(0 != gprs.init()) {
      delay(1000);
      Serial.print("init error\r\n");
  }
    if(0 != gprs.sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CNMI");
    return;
  }
  if(0 != gprs.sendCmdAndWaitForResp("AT+CNMI=2,2,0,0,0\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CNMI");
    return;
  }
  Serial.println("Init success");

}

void loop() {
  if(gprs.serialSIM800.available()){
    char lastCharRead = gprs.serialSIM800.read();
    if(lastCharRead == '\r' || lastCharRead == '\n'){
        String lastLine = String(currentLine);
        if(lastLine.startsWith("+CMT:")){
          Serial.println(lastLine);
          nextLineIsMessage = true;
        } else if (lastLine.length() > 0) {
          if(nextLineIsMessage) {
            Serial.println(lastLine);
            if(lastLine.indexOf("HIDUP") >= 0){
              digitalWrite(LED_PIN, HIGH);
              gprs.sendSMS("08xx","Lampu Hidup");
            } else if(lastLine.indexOf("MATI") >= 0) {
              gprs.sendSMS("08xx","Lampu Mati");
              digitalWrite(LED_PIN, HIGH);
            }
            nextLineIsMessage = false;
          }  
        }
        for( int i = 0; i < sizeof(currentLine);  ++i ) {
         currentLine[i] = (char)0;
        }
        currentLineIndex = 0;
    } else {
      currentLine[currentLineIndex++] = lastCharRead;
    }
  }

}
