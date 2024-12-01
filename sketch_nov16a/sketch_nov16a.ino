#include <WiFi.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <AnimatedGIF.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char* ssid = "Your WiFi name";
const char* password = "Wifi Password";

#define DEVICE_ID "Arduino IoT device ID"
#define SECRET_KEY "Arduino IoT secret key"

#define SD_CS_PIN 5
#define LCD_BACK_LIGHT_PIN 21

TFT_eSPI tft = TFT_eSPI();
AnimatedGIF gif;

String gifFiles[] = {"/1.gif", "/2.gif", "/3.gif", "/4.gif", "/5.gif"};
int gifIndex = 0;

WiFiServer server(80);

bool bootCompleted = false;

bool backlightOn = true;
const int minBrightness = 30;
const int maxBrightness = 100;
int brightness = 90;

WiFiConnectionHandler ArduinoIoTPreferredConnection(ssid, password);

void onBacklightOnChange();
void onBrightnessChange();
void onGifIndexChange();
void initProperties();

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  uint32_t duty = (4095 * value) / valueMax;
  ledcWrite(channel, duty);
}

void setup() {
  Serial.begin(115200);

  initProperties();
  ArduinoCloud.setBoardId(DEVICE_ID);
  ArduinoCloud.setSecretDeviceKey(SECRET_KEY);
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  server.begin();
  Serial.println("WiFi connected!");

  tft.begin();
  tft.setRotation(1);
  tft.invertDisplay(1);
  tft.fillScreen(TFT_BLACK);

  pinMode(LCD_BACK_LIGHT_PIN, OUTPUT);
  ledcSetup(0, 5000, 12);
  ledcAttachPin(LCD_BACK_LIGHT_PIN, 0);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Error: Unable to initialize SD card!");
    tft.fillScreen(TFT_RED);
    tft.drawString("SD Error", 10, 10);
    while (true);
  }
  Serial.println("SD card initialized successfully.");

  gif.begin(BIG_ENDIAN_PIXELS);

  if (backlightOn) {
    onBacklightOnChange();
  }
}

void loop() {
  ArduinoCloud.update();

  WiFiClient client = server.available();
  if (client) {
    handleClient(client);
  }

  if (bootCompleted) {
    if (gif.open(gifFiles[gifIndex].c_str(), fileOpen, fileClose, fileRead, fileSeek, GIFDraw)) {
      tft.startWrite();
      while (gif.playFrame(true, NULL)) {
        yield();
      }
      gif.close();
      tft.endWrite();
    }
  }
}

void playCurrentGIF() {
  Serial.println("Playing GIF: " + gifFiles[gifIndex]);
  tft.fillScreen(TFT_BLACK);

  if (gif.open(gifFiles[gifIndex].c_str(), fileOpen, fileClose, fileRead, fileSeek, GIFDraw)) {
    tft.startWrite();
    while (gif.playFrame(true, NULL)) {
      yield();
    }
    gif.close();
    tft.endWrite();
  } else {
    Serial.println("Error: Unable to open GIF " + gifFiles[gifIndex]);
    tft.fillScreen(TFT_RED);
    tft.drawString("GIF Open Error", 10, 10);
  }
}

void handleClient(WiFiClient &client) {
  String request = client.readStringUntil('\r');
  client.flush();

  if (request.indexOf("/on") != -1) {
    if (!backlightOn) {
      backlightOn = true;
      onBacklightOnChange();
      client.println("HTTP/1.1 200 OK");
      client.println("LCD backlight turned on.");
    } else {
      client.println("HTTP/1.1 200 OK");
      client.println("LCD backlight is already on.");
    }
  } else if (request.indexOf("/off") != -1) {
    bootCompleted = false;
    backlightOn = false;
    onBacklightOnChange();
    client.println("HTTP/1.1 200 OK");
    client.println("LCD backlight turned off.");
  } else if (request.startsWith("GET /") && request.indexOf(".gif") != -1) {
    int startIdx = request.indexOf('/') + 1;
    int endIdx = request.indexOf(".gif");
    String gifName = request.substring(startIdx, endIdx);

    int gifNum = gifName.toInt();
    if (gifNum > 0 && gifNum <= sizeof(gifFiles) / sizeof(gifFiles[0])) {
      gifIndex = gifNum - 1;
      playCurrentGIF();
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Playing: " + gifFiles[gifIndex]);
    } else {
      client.println("HTTP/1.1 404 Not Found");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("GIF not found.");
    }
  } else if (request.indexOf("GET / ") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Current Brightness Level: " + String(brightness));
    client.println("Current GIF: " + gifFiles[gifIndex]);
  }

  client.stop();
}

void initProperties() {
  ArduinoCloud.addProperty(backlightOn, READWRITE, ON_CHANGE, onBacklightOnChange);
  ArduinoCloud.addProperty(brightness, READWRITE, ON_CHANGE, onBrightnessChange);
  ArduinoCloud.addProperty(gifIndex, READWRITE, ON_CHANGE, onGifIndexChange);
}

void onBacklightOnChange() {
  if (backlightOn && !bootCompleted) {
    ledcAnalogWrite(0, brightness);
    tft.fillScreen(TFT_BLACK);
    bootCompleted = true;
    playCurrentGIF();
  } else if (!backlightOn) {
    ledcAnalogWrite(0, 0);
    tft.fillScreen(TFT_BLACK);
    bootCompleted = false;
  }
}

void onBrightnessChange() {
  brightness = constrain(brightness, minBrightness, maxBrightness);
  if (backlightOn) {
    ledcAnalogWrite(0, brightness);
  }
}

void onGifIndexChange() {
  playCurrentGIF();
}

void* fileOpen(const char* filename, int32_t* pFileSize) {
  File* f = new File();
  *f = SD.open(filename, FILE_READ);
  if (*f) {
    *pFileSize = f->size();
    return (void*)f;
  }
  delete f;
  return NULL;
}

void fileClose(void* pHandle) {
  File* f = static_cast<File*>(pHandle);
  if (f != NULL) {
    f->close();
    delete f;
  }
}

int32_t fileRead(GIFFILE* pFile, uint8_t* pBuf, int32_t iLen) {
  File* f = static_cast<File*>(pFile->fHandle);
  if (f == NULL) return 0;
  int32_t iBytesRead = f->read(pBuf, iLen);
  pFile->iPos = f->position();
  return iBytesRead;
}

int32_t fileSeek(GIFFILE* pFile, int32_t iPosition) {
  File* f = static_cast<File*>(pFile->fHandle);
  if (f == NULL) return 0;
  f->seek(iPosition);
  pFile->iPos = f->position();
  return pFile->iPos;
}

void GIFDraw(GIFFILE* pFile, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t* pColors) {
  if (x < 0 || y < 0) return;
  tft.pushImage(x, y, width, height, pColors);
}
