#include <XBee.h> 
#include <math.h> 
#include <SoftwareSerial.h> 

SoftwareSerial xbee(2,3);

//IO pins
int blue = 7;
int green = 6;
int red = 5;
int button =8;

boolean isLeader;
boolean isInfected;
boolean isTimeout;
int leader_id;
int node_id;

void setup() {
  // Initialize an node
  xbee.begin(9600);
  Serial.begin(9600);
  isLeader = false;
  isInfected = false;
  isTimeout = true;

  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(button, INPUT);

  digitalWrite(green, HIGH);

  node_id = assign_id();
  leader_id = -1;
  Serial.println("My ID : "+ String(node_id)); 
  xbee.flush(); 
  Serial.println("Setup Complete"); 
}

void loop() {
  // put your main code here, to run repeatedly:

}

int assign_id() { 
   String s;

   // Enter configuration mode 
   delay(1000);    
   xbee.write("+++"); 
   delay(1000);   
   xbee.write("\r"); 
   delay(100); 
   // Get the OK and clear it out. 
   while (xbee.available() > 0) { 
     Serial.print(char(xbee.read())); 
   } 
   Serial.println(""); 
 
 
   // Send "ATNI" command.
   xbee.write("ATNI\r"); 
   delay(100); 
   while (xbee.available() > 0) { 
       s += char(xbee.read()); 
   } 
   delay(100); 
 
 
   // Exit configuration mode 
   xbee.write("ATCN\r"); 
   delay(1000); 
   // Flush Serial Buffer Before Start 
   while (xbee.available() > 0) { 
     Serial.print(char(xbee.read())); 
   } 
   Serial.println(""); 
   delay(100); 
 
 
   return s.toInt(); 
 } 

 
 

