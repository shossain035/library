/*
 Lithouse Wifly client 
 
 This sketch connects to api.lithouse.co through LithouseClient.
 It expects "on" or "off" command on incoming "LED" channel. 
 And, uploads normalized analog readings to "FSR" channel.  
 
 Circuit:
 * Described in getting started with Arduino tutorial in lithouse.co.
 
 Note:
 Lithouse client library will not compile with the original 
 SparkFun WiFly_Shield library. Please use the one included with LithouseWifly. 
 
 created 11 Nov 2013
 by Shah Hossain
 
 */


#include <SPI.h>
#include <WiFly.h>
#include <Lithouse.h>
#include "Credentials.h"

WiFlyClient  client;

//Copy deviceKey from Lithouse portal
char deviceKey [] = "c8a9892b-a4f2-4d24-9831-445e0cbe2f97";
//Incoming channel
char ledChannel [] = "LED";
//Outgoing channel
char fsrChannel [] = "FSR";

LithouseClient litClient ( client, deviceKey );
//Maximum channel number for simultaneous read or write
const int MAX_CHANNEL_COUNT = 1;
LithouseRecord records [MAX_CHANNEL_COUNT]; 

int LED_OUT_PIN = 5;
int FSR_IN_PIN = 0;
int fsrPressure = 0;
int fsrReading;

//Maximum length of a channel name or data string
const int MAX_LENGTH = 10;
char dataBuffer [MAX_LENGTH];
char channelBuffer [MAX_LENGTH];


void setup() {
  pinMode(LED_OUT_PIN, OUTPUT);
  
  Serial.begin ( 9600 );
  
  WiFly.begin ( );
  
  if ( !WiFly.join ( ssid, passphrase ) ) {
    Serial.println ( "Association failed." );
    while ( 1 ) {
      // Hang on failure.
    }
  }  
  Serial.println ( "Connected to wifi" );
  
  delay ( 10000 );
  
  Serial.println ( "Connecting to Lithouse" );
  
}

void loop() {
  downloadLedState ( );
  uploadFSRState ( );
  
  delay ( 1000 );        
}

void uploadFSRState ( ) {
  fsrReading = analogRead ( FSR_IN_PIN );
  Serial.print ( "FSR reading = " );
  Serial.println ( fsrReading );
  
  //Normalizing FSR reading 
  int currentPressure = (fsrReading > 700) ? 70 : 0;
  
  //Only upload reading if there was a change  
  if ( currentPressure != fsrPressure ) {
    fsrPressure = currentPressure;
    itoa ( fsrPressure, dataBuffer, 10 );
    records[0].updateRecord ( fsrChannel, dataBuffer );
    
    litClient.send ( records, 1 );   
  }
}

void downloadLedState ( ) {
  if ( litClient.receive ( records, MAX_CHANNEL_COUNT ) == 1 ) {
    
    records[0].getData ( dataBuffer );
    records[0].getChannel ( channelBuffer );
    
    if ( 0 == stricmp ( channelBuffer, ledChannel ) 
        && 0 == stricmp ( dataBuffer, "on" ) ) {
      
      Serial.println ( "turn led on" );
      digitalWrite(LED_OUT_PIN, HIGH);
    } else {
      
      Serial.println ( "turn led off" );
      digitalWrite ( LED_OUT_PIN , LOW );    
    }
  }
}

int stricmp(const char *s1, const char *s2) 
{ 
  while(tolower((unsigned char) *s1) == tolower((unsigned char) *s2)) { 
    if(*s1++ == '\0') 
      return 0; 
    s2++; 
  } 
  
  return (unsigned char) *s1 < (unsigned char) *s2 ? -1 : 1; 
}
