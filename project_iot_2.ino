#include <WiFi.h>
#include <DHT.h>
#include <ESP32Servo.h>


const char* ssid     = "Redmi 9 Power";
const char* password = "nu123456789";

#define DHTPIN 4
#define DHTTYPE DHT11
float tem=0,humi=0;
DHT dht(DHTPIN, DHTTYPE);


#define servo_pin 5
Servo myservo;
int pos=115;

//for on off indicating
int lion_state=0,fnon_state=0,don_state=0;


WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(18, OUTPUT);
    digitalWrite(18,HIGH);     // set the LED pin mode
    dht.begin();
    myservo.attach(servo_pin);
    myservo.write(115);
    pinMode(19,OUTPUT); //Fan
    digitalWrite(19,HIGH);
    
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.println(
                           "<!DOCTYPE html>\
                           <html>\
                           <head>\
                           <title>\
                           Dash Board\
                           </title>\
                          <style>\
                            button{\
                                    display: inline-block;\
                                    padding: 10px 20px;\
                                    font-size: 16px;\
                                    text-align: center;\
                                    text-decoration: none;\
                                    border: none;\
                                    border-radius: 4px;\
                            }\
                            .on:active{\
                            background-color:green;\
                            }\
                            .off:active{\
                            background-color:red;\
                            }\
                          </style>\
                          </head>\
                          <body>"
            );

            //status 
              humi = dht.readHumidity();
              tem = dht.readTemperature();
              Serial.println(humi);
              Serial.println(tem);
               if (isnan(humi) || isnan(tem)) {
                  Serial.println(F("Failed to read from DHT sensor!"));
                  client.println(
                                  "<div>\
                                  <h2>Status: Failed to read from DHT sensor!</h2>\
                                  <button class=\"re\" type=\"button\" style=\"background-color:white; padding:10px; border-radius:4px; border:1px solid grey; font-weight:500; \">\
                                      <a href=\"/\" style=\"text-decoration:none; color:black;\">Refresh</a>\
                                  </button>\
                                  </div>" 
                  );
                }


              else{
                client.println(
                        "<div>\
                        <h2>Status: Good</h2>\
                        <button class=\"re\" type=\"button\" style=\"background-color:white; padding:10px; border-radius:4px; border:1px solid grey; \">\
                            <a href=\"/\" style=\"text-decoration:none; color:black; font-weight:500;\">Refresh</a>\
                        </button>\
                        </div>"
                );
                client.println(
                              "<div>\
                              <h3>Temperature: "
                );
                client.print(tem);
                client.println("&degC</h3>");
                client.print(
                              "<h3>Humidity: "
                );
                client.print(humi);
                client.println("%</h3></div>");
              }              




            //for light
            if(lion_state==0){
            client.println(
                         "<div>\
                            <h3 style=\"margin:0px;\">Light</h3>\
                            <button class=\"off\" type=\"button\" style=\"background-color:red; \">\
                             <a href=\"/L1L\" style=\"text-decoration:none; color:white;\">OFF</a>\
                             </button>\
                             <button class=\"on\" type=\"button\" style=\"background-color:white; border:1px solid grey; \">\
                             <a href=\"/L1H\" style=\"text-decoration:none; color:black;\">ON</a>\
                             </button>\
                             </div>"
            );
            }
            else{
            client.println(
                         "<div>\
                            <h3 style=\"margin:0px;\">Light</h3>\
                            <button class=\"off\" type=\"button\" style=\"background-color:white; border:1px solid grey;\">\
                             <a href=\"/L1L\" style=\"text-decoration:none; color:black;\">OFF</a>\
                             </button>\
                             <button class=\"on\" type=\"button\" style=\"background-color:green;\">\
                             <a href=\"/L1H\" style=\"text-decoration:none; color:white;\">ON</a>\
                             </button>\
                             </div>"
            );
            }//light end

            //for door
            if(don_state==0){
            client.println(
                         "<div>\
                            <h3 style=\"margin:0px;\">Door</h3>\
                            <button class=\"off\" type=\"button\" style=\"background-color:red; \">\
                             <a href=\"/D1L\" style=\"text-decoration:none; color:white;\">OFF</a>\
                             </button>\
                             <button class=\"on\" type=\"button\" style=\"background-color:white; border:1px solid grey; \">\
                             <a href=\"/D1H\" style=\"text-decoration:none; color:black;\">ON</a>\
                             </button>\
                             </div>"
            );
            }
            else{
            client.println(
                         "<div>\
                            <h3 style=\"margin:0px;\">Door</h3>\
                            <button class=\"off\" type=\"button\" style=\"background-color:white; border:1px solid grey;\">\
                             <a href=\"/D1L\" style=\"text-decoration:none; color:black;\">OFF</a>\
                             </button>\
                             <button class=\"on\" type=\"button\" style=\"background-color:green;\">\
                             <a href=\"/D1H\" style=\"text-decoration:none; color:white;\">ON</a>\
                             </button>\
                             </div>"
            );
            }//Door end

            //for fan
            if(fnon_state==0){
            client.println(
                         "<div>\
                            <h3 style=\"margin:0px;\">Fan</h3>\
                            <button class=\"off\" type=\"button\" style=\"background-color:red; \">\
                             <a href=\"/F1L\" style=\"text-decoration:none; color:white;\">OFF</a>\
                             </button>\
                             <button class=\"on\" type=\"button\" style=\"background-color:white; border:1px solid grey; \">\
                             <a href=\"/F1H\" style=\"text-decoration:none; color:black;\">ON</a>\
                             </button>\
                             </div>"
            );
            }
            else{
            client.println(
                         "<div>\
                            <h3 style=\"margin:0px;\">Fan</h3>\
                            <button class=\"off\" type=\"button\" style=\"background-color:white; border:1px solid grey;\">\
                             <a href=\"/F1L\" style=\"text-decoration:none; color:black;\">OFF</a>\
                             </button>\
                             <button class=\"on\" type=\"button\" style=\"background-color:green;\">\
                             <a href=\"/F1H\" style=\"text-decoration:none; color:white;\">ON</a>\
                             </button>\
                             </div>"
            );
            }//fan end




            // client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            // client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");
            client.print(
                          "<script>\
                          // function light_ON_off(button){\
                          // var element=document.getElementById(\"light\");\
                          // if(button==\"bt_li_off\"){\
                          //       document.getElementById(\"0ff\").style.background-color=\"red\";\
                          //       document.getElementById(\"on\").style.color=\"white\";}\
                          //       else if(button==\"bt_li_on){\
                          //          document.getElementById(\"on\").style.background-color=\"green\";\
                          //         document.getElementById(\"off\").style.color=\"white\";}}\
                          </script>"
            );
            client.print("</body>\
                          </html>"
            );

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } //response sending end
          else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } // getting new line end
        
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":


        /*if (currentLine.endsWith("GET /H")) {
          digitalWrite(2, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(2, LOW);                // GET /L turns the LED off
        }*/

        //for light
         if (currentLine.endsWith("GET /L1H")) {
          digitalWrite(18, LOW);               // GET /H turns the LED on
          lion_state=1;
        
        }
        else if (currentLine.endsWith("GET /L1L")) {
          digitalWrite(18, HIGH);     
          lion_state=0 ;       // GET /L turns the LED off
        }

         //for fan
         if (currentLine.endsWith("GET /F1H")) {
          digitalWrite(19, LOW);               // GET /H turns the LED on
           fnon_state=1;
        
        }
        else if (currentLine.endsWith("GET /F1L")) {
          digitalWrite(19, HIGH);     
          fnon_state=0 ;       // GET /L turns the LED off
        }







        //for door
         if (currentLine.endsWith("GET /D1H")) {
              if(pos>=114){
               for (pos = 115; pos >= 10; pos -= 1) { // goes from 180 degrees to 0 degrees
                  myservo.write(pos);  
                  Serial.println(pos);         // tell servo to go to position in variable 'pos'
                  delay(5);                       // waits 15ms for the servo to reach the position
                } }
             don_state=1;
          }

        else if (currentLine.endsWith("GET /D1L")) {
          if(pos<=11){
              for (pos = 10; pos <= 115; pos += 1) { // goes from 0 degrees to 180 degrees
                  // in steps of 1 degree
                  myservo.write(pos);// tell servo to go to position in variable 'pos'
                  Serial.println(pos);
                  delay(5);
                } 
              }    
          don_state=0 ;       // GET /L turns the LED off
        }//end fan

      } //client available ending
    } //while loop ending
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
