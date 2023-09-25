// Import required libraries
//#include <Arduino.h>
//#include <ESP8266WiFi.h>
//#include <Hash.h>
//#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>   // Include the SPIFFS library

//String humanReadableSize(const size_t bytes);

const char* ssid     = "ESP";
const char* password = "1234";

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
  <p><h1>File Upload filezzzz</h1></p>
  <p>Free Storage: %FREESPIFFS% | Used Storage: %USEDSPIFFS% | Total Storage: %TOTALSPIFFS%</p>
  <form method="POST" action="/upload" enctype="multipart/form-data"><input type="file" name="data"/><input type="submit" name="upload" value="Upload" title="Upload File"></form>

  <img src="sun">

  <p>%FILELIST%</p>
</body>
</html>
)rawliteral";


// list all of the files, if ishtml=true, return html rather than simple text
String listFiles(bool ishtml) {
  String returnText = "";
  Serial.println("List files");
  File foundfile = SPIFFS.open("/sun.jpg", "r");

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
  FSInfo info;
  SPIFFS.info(info);
  
  if (var == "FREESPIFFS") {  
    return String((info.totalBytes - info.usedBytes));
  }

  if (var == "USEDSPIFFS") {
    return String(info.usedBytes);
  }

  if (var == "TOTALSPIFFS") {
    return String(info.totalBytes);
  }

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

  server.on("/sun", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("/" + String(request->url()));
    
    request->send(SPIFFS, String(request->url()) + ".jpg", "image/png");
  });
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

  File f;

  if (!index) {
//    logmessage = "Upload Start: " + String(filename);
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open("/" + filename, "w");
//    f = SPIFFS.open("/" + filename, "w");
//    Serial.println(logmessage);
  }

  if (len) {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
//      f.write(data, len);
  }

  if (final) {
    String logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
    Serial.println(logmessage);
    
    // close the file handle as the upload is now done
    request->_tempFile.close();

//    f.close();
    
    request->redirect("/");
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
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

//  File file = SPIFFS.open("/file.txt", "w");
// 
//  if (!file) {
//    Serial.println("Error opening file for writing");
//    return;
//  }
// 
//  int bytesWritten = file.print("TEST SPIFFS");
// 
//  if (bytesWritten == 0) {
//    Serial.println("File write failed");
//    return;
//  }
// 
//  file.close();
}
 
void loop(){  

 }
 
// Make size of files human readable
// source: https://github.com/CelliesProjects/minimalUploadAuthESP32
//String humanReadableSize(const size_t bytes) {
//  if (bytes < 1024) return String(bytes) + " B";
//  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
//  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
//  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
//}
