/*
  UDPSendReceive.pde:
 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender
 
 A Processing sketch is included at the end of file that can be used to send 
 and received messages for testing with a computer.
 
 created 21 Aug 2010
 by Michael Margolis
 
 This code is in the public domain.
 */


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <Wire.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  
  0x90, 0xA2, 0xDA, 0x0F, 0x9E, 0x4C };
IPAddress ip(10,0,0,127);
const int ledPin = 9;
unsigned long localPort = 50074;      // local port to listen on
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged";       // a string to send back
const int LONGSIZE = sizeof(long int);
// An EthernetUDP instance to let us send and receive packets over UDP

unsigned char buf[LONGSIZE];

uint8_t adres;
uint8_t command;
long value;

EthernetUDP Udp;

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  Wire.begin();
  Serial.begin(9600);
  
}

void loop() {
  // if there's data available, read a packet

  int packetSize = Udp.parsePacket();
  int i;
  if(packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());
    for(i=0; i < UDP_TX_PACKET_MAX_SIZE; i++)
    packetBuffer[i]=0;
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    sscanf(packetBuffer, "%1d %1d %ld", &adres, &command, &value);
    Serial.println(adres);
    Serial.println(command);
    Serial.println(value);
    sendCommand(adres, command, value);
    /*send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
    */
  }


}

void sendCommand(uint8_t wireAdres, uint8_t _command, long _value) {
  int i;
  digitalWrite(ledPin, HIGH);

  for(i=0; i < LONGSIZE; i++)
    buf[i]=0;
  memcpy(buf, &_value, 4);
  Wire.beginTransmission(wireAdres); 
     for(i=0; i < LONGSIZE; i++) {
        Serial.print(buf[i], DEC); Serial.print('-');
        Wire.write(buf[i]); }       // send long
     Wire.write(_command);        // sends byte DMX1
  Wire.endTransmission();    // stop transmitting
  Serial.println();
  digitalWrite(ledPin, LOW);
}
/*
  Processing sketch to run with this example
 =====================================================
 
 // Processing UDP example to send and receive string data from Arduino 
 // press any key to send the "Hello Arduino" message
 
 
 import hypermedia.net.*;
 
 UDP udp;  // define the UDP object
 
 
 void setup() {
 udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
 //udp.log( true ); 		// <-- printout the connection activity
 udp.listen( true );           // and wait for incoming message  
 }
 
 void draw()
 {
 }
 
 void keyPressed() {
 String ip       = "192.168.1.177";	// the remote IP address
 int port        = 8888;		// the destination port
 
 udp.send("Hello World", ip, port );   // the message to send
 
 }
 
 void receive( byte[] data ) { 			// <-- default handler
 //void receive( byte[] data, String ip, int port ) {	// <-- extended handler
 
 for(int i=0; i < data.length; i++) 
 print(char(data[i]));  
 println();   
 }
 */


