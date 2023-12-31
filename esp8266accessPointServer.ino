//todos
//try list all sd files


// Import required libraries
//#include <Arduino.h>
//#include <ESP8266WiFi.h>
//#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>   // Include the SPIFFS library

//String humanReadableSize(const size_t bytes);

const char* ssid     = "ESP";
const char* password = "";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
</head>
<body>
//  <p><h1>File Upload file</h1></p>
//  <p>Free Storage: %FREESPIFFS% | Used Storage: %USEDSPIFFS% | Total Storage: %TOTALSPIFFS%</p>
//  <form method="POST" action="/upload" enctype="multipart/form-data"><input type="file" name="data"/><input type="submit" name="upload" value="Upload" title="Upload File"></form>

  <img src="displayImage" id="mainImage">


  <p>%FILELIST%</p>

<script>
  setInterval(function() 
    {
      getImage();
    }, 1100);
    
function getImage() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("mainImage").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "displayImage", true);
  xhttp.send();

  document.getElementById("mainImage").src = 'http://192.168.4.1/displayImage';
}
</script>
</body>
</html>
)rawliteral";


// list all of the files, if ishtml=true, return html rather than simple text
String listFiles(bool ishtml) {
  String returnText = "";
  Serial.println("List files");
  File foundfile = SPIFFS.open("/displayImage.jpg", "r");

  if (!foundfile) {
    Serial.println("Failed to open file for reading");
    return "";
  }

//  while (foundfile.available()) {
//    Serial.write(foundfile.read());
//  }


//  File foundfile = root.openNextFile();
  if (ishtml) {
    returnText += "<table><tr><th align='left'>Name</th><th align='left'>Size</th></tr>";
  }
//  while (foundfile) {
  if (ishtml) {
      returnText += "<tr align='left'><td>" + String(foundfile.name()) + "</td><td>" + foundfile.size() + "</td></tr>";
    } else {
      returnText += "File: " + String(foundfile.name()) + "\n";
    }
//    foundfile = root.openNextFile();
//  }

  if (ishtml) {
    returnText += "</table>";
  }
  
//  root.close();
  foundfile.close();
  return returnText;
}

// Replaces placeholders
String processor(const String& var) {
  if (var == "FILELIST") {
    return listFiles(true);
  }
//  FSInfo info;
//  SPIFFS.info(info);
  
//  if (var == "FREESPIFFS") {  
//    return String((info.totalBytes - info.usedBytes));
//  }
//
//  if (var == "USEDSPIFFS") {
//    return String(info.usedBytes);
//  }
//
//  if (var == "TOTALSPIFFS") {
//    return String(info.totalBytes);
//  }

  return String();
}

void configureWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
//    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();
//    Serial.println(logmessage);
    request->send_P(200, "text/html", index_html, processor);
  });

  // run handleUpload function when any file is uploaded
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
      }, handleUpload);

//change it to user reqeust->arg(0); to pass img name in request argument
  server.on("/displayImage", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println(String(request->url()));
    
    request->send(SPIFFS, "/displayImage.jpg", "image/png");
  });
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

  if (!index) {
    Serial.println(filename);
    request->_tempFile = SPIFFS.open("/displayImage.jpg", "w");
    
  }

  if (len) {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
  }

  if (final) {
    String logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len) + ", img len:" +String(len);
    Serial.println(logmessage);
    
    // close the file handle as the upload is now done
    request->_tempFile.close();
    
//    request->redirect("/");
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(1000000);
  
  Serial.print("\nSetting AP (Access Point)…\n");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(IP);

  configureWebServer();
  
  SPIFFS.begin();                           // Start the SPI Flash Files System
  SPIFFS.format();

  // Start server
  server.begin();
}
 
void loop(){ 
  delay(2000);
  client_status(); 
 }

 void client_status() {

  unsigned char number_client;
  struct station_info *stat_info;
  
  struct ip_addr *IPaddress;
  IPAddress address;
  int i=1;
  
  number_client= wifi_softap_get_station_num();
  stat_info = wifi_softap_get_station_info();
  
  Serial.print("Connected Clients: ");
  Serial.println(number_client);
}
 
// Make size of files human readable
// source: https://github.com/CelliesProjects/minimalUploadAuthESP32
//String humanReadableSize(const size_t bytes) {
//  if (bytes < 1024) return String(bytes) + " B";
//  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
//  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
//  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
//}
