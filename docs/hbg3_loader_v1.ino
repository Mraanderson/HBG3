#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <WiFiManager.h>  // by tzapu

// ---------- Pins ----------
#define BOOT_BUTTON_PIN   0      // GPIO0 (BOOT)
#define STATUS_LED_PIN    2      // Adjust if needed

// ---------- OLED ----------
#define SCREEN_WIDTH      128
#define SCREEN_HEIGHT     64
#define OLED_RESET        -1

Adafruit_SSD1306  oled_ssd1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SH1106G  oled_sh1106(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool              oled_ok = false;
enum OledType { OLED_NONE, OLED_SSD1306, OLED_SH1106 };
OledType          oled_type = OLED_NONE;

// ---------- WiFi / AP ----------
const char *ap_prefix = "HBG3-Setup-";
String      ap_ssid;

// ---------- OTA ----------
const char *ota_url = "https://rtr.ca/hbg3/firmware.bin";

// ---------- Web server ----------
WebServer server(80);

// ---------- State ----------
bool autoUpdateRan     = false;
bool autoUpdateFailed  = false;

// ---------- Helpers ----------

void oledPrintCentered(const String &line1, const String &line2 = "") {
  if (!oled_ok) return;

  if (oled_type == OLED_SSD1306) oled_ssd1306.clearDisplay();
  else if (oled_type == OLED_SH1106) oled_sh1106.clearDisplay();

  int16_t x1, y1;
  uint16_t w, h;

  if (oled_type == OLED_SSD1306) {
    oled_ssd1306.setTextSize(1);
    oled_ssd1306.setTextColor(SSD1306_WHITE);
    oled_ssd1306.getTextBounds(line1, 0, 0, &x1, &y1, &w, &h);
    oled_ssd1306.setCursor((SCREEN_WIDTH - w) / 2, 16);
    oled_ssd1306.println(line1);

    if (line2.length()) {
      oled_ssd1306.getTextBounds(line2, 0, 0, &x1, &y1, &w, &h);
      oled_ssd1306.setCursor((SCREEN_WIDTH - w) / 2, 32);
      oled_ssd1306.println(line2);
    }

    oled_ssd1306.display();
  } else if (oled_type == OLED_SH1106) {
    oled_sh1106.setTextSize(1);
    oled_sh1106.setTextColor(SH110X_WHITE);
    oled_sh1106.getTextBounds(line1, 0, 0, &x1, &y1, &w, &h);
    oled_sh1106.setCursor((SCREEN_WIDTH - w) / 2, 16);
    oled_sh1106.println(line1);

    if (line2.length()) {
      oled_sh1106.getTextBounds(line2, 0, 0, &x1, &y1, &w, &h);
      oled_sh1106.setCursor((SCREEN_WIDTH - w) / 2, 32);
      oled_sh1106.println(line2);
    }

    oled_sh1106.display();
  }
}

void oledInitAuto() {
  Wire.begin();
  if (oled_ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    oled_type = OLED_SSD1306;
    oled_ok   = true;
    oled_ssd1306.clearDisplay();
    oled_ssd1306.display();
    return;
  }
  if (oled_sh1106.begin(0x3C, true)) {
    oled_type = OLED_SH1106;
    oled_ok   = true;
    oled_sh1106.clearDisplay();
    oled_sh1106.display();
    return;
  }
  oled_type = OLED_NONE;
  oled_ok   = false;
}

// LED patterns
void ledIdle() {
  digitalWrite(STATUS_LED_PIN, millis() / 500 % 2); // slow blink
}

void ledUpdating() {
  digitalWrite(STATUS_LED_PIN, millis() / 150 % 2); // fast blink
}

void ledSuccess() {
  digitalWrite(STATUS_LED_PIN, HIGH);
}

void ledError() {
  digitalWrite(STATUS_LED_PIN, LOW);
}

// ---------- OTA logic (HTTPS/TLS + progress) ----------

bool performOTA(const char *url) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[OTA] WiFi not connected");
    oledPrintCentered("OTA failed", "No WiFi");
    return false;
  }

  Serial.println("[OTA] Starting OTA from URL:");
  Serial.println(url);
  oledPrintCentered("Updating...", url);

  WiFiClientSecure client;
  client.setInsecure();  // allow HTTPS without certificate

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(15000);

  if (!http.begin(client, url)) {
    Serial.println("[OTA] HTTP begin failed");
    oledPrintCentered("OTA failed", "HTTP begin");
    return false;
  }

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("[OTA] HTTP GET failed, code: %d\n", httpCode);
    oledPrintCentered("OTA failed", String("HTTP ") + httpCode);
    http.end();
    return false;
  }

  int contentLength = http.getSize();
  if (contentLength <= 0) {
    Serial.println("[OTA] Invalid content length");
    oledPrintCentered("OTA failed", "Bad length");
    http.end();
    return false;
  }

  if (!Update.begin(contentLength)) {
    Serial.println("[OTA] Update.begin failed");
    oledPrintCentered("OTA failed", "No space?");
    http.end();
    return false;
  }

  WiFiClient *stream = http.getStreamPtr();
  uint8_t buff[1024];
  int written = 0;
  unsigned long lastProgressUpdate = 0;

  while (http.connected() && (written < contentLength || contentLength == -1)) {
    ledUpdating();
    size_t size = stream->available();
    if (size) {
      int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
      if (c > 0) {
        if (Update.write(buff, c) != c) {
          Serial.println("[OTA] Write failed");
          oledPrintCentered("OTA failed", "Write error");
          http.end();
          Update.end();
          return false;
        }
        written += c;

        if (millis() - lastProgressUpdate > 250 && contentLength > 0) {
          int percent = written * 100 / contentLength;
          Serial.printf("[OTA] %d%% (%d/%d bytes)\n", percent, written, contentLength);

          char line1[32];
          snprintf(line1, sizeof(line1), "Updating... %d%%", percent);
          oledPrintCentered(line1);

          lastProgressUpdate = millis();
        }
      }
    }
    delay(1);
  }

  if (!Update.end()) {
    Serial.printf("[OTA] Update.end error: %s\n", Update.errorString());
    oledPrintCentered("OTA failed", "Finalize error");
    http.end();
    return false;
  }

  http.end();

  if (!Update.isFinished()) {
    Serial.println("[OTA] Update not finished");
    oledPrintCentered("OTA failed", "Not finished");
    return false;
  }

  Serial.println("[OTA] Update successful");
  oledPrintCentered("Update complete", "Rebooting...");
  ledSuccess();
  delay(2000);
  ESP.restart();
  return true;
}

// ---------- Auto-update flow ----------

void runAutoUpdateOnce() {
  if (autoUpdateRan) return;
  autoUpdateRan = true;

  // 2-second grace period
  Serial.println("[AUTO] Preparing update (2s grace)");
  oledPrintCentered("Preparing update...", "Press BOOT to cancel");
  unsigned long start = millis();
  bool cancelled = false;
  while (millis() - start < 2000) {
    ledIdle();
    if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
      cancelled = true;
      break;
    }
    delay(10);
  }

  if (cancelled) {
    Serial.println("[AUTO] Auto-update cancelled by BOOT");
    oledPrintCentered("Update cancelled", "Use BOOT to update");
    ledSuccess();
    return;
  }

  // First attempt
  Serial.println("[AUTO] Starting OTA attempt #1");
  bool ok = performOTA(ota_url);
  if (ok) {
    // performOTA() will reboot on success
    return;
  }

  // Retry once after 3 seconds
  Serial.println("[AUTO] OTA failed, retrying in 3s");
  oledPrintCentered("Update failed", "Retrying in 3s...");
  unsigned long waitStart = millis();
  while (millis() - waitStart < 3000) {
    ledIdle();
    delay(10);
  }

  Serial.println("[AUTO] Starting OTA attempt #2");
  ok = performOTA(ota_url);
  if (ok) {
    return;
  }

  // Both attempts failed
  Serial.println("[AUTO] OTA failed twice, waiting for user");
  autoUpdateFailed = true;
  oledPrintCentered("Update failed", "Press BOOT to retry");
  ledError();
}

// ---------- Web handlers ----------

void handleRoot() {
  String html =
    "<!DOCTYPE html><html><head><title>HBG3 Loader</title>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<style>"
    "body{font-family:sans-serif;background:#111;color:#eee;margin:0;padding:20px;text-align:center;}"
    "h1{margin-top:10px;}"
    "button{padding:10px 20px;margin:10px;font-size:16px;border:none;border-radius:4px;}"
    ".primary{background:#2196F3;color:#fff;}"
    "</style></head><body>"
    "<h1>HBG3 Loader</h1>"
    "<p>WiFi status: " + String(WiFi.isConnected() ? "Connected" : "Not connected") + "</p>"
    "<p>IP: " + (WiFi.isConnected() ? WiFi.localIP().toString() : String("N/A")) + "</p>"
    "<p>Firmware update runs automatically once WiFi is connected.</p>"
    "<button class='primary' onclick=\"fetch('/ota').then(()=>alert('OTA started')).catch(()=>{})\">Start OTA Update</button>"
    "</body></html>";

  server.send(200, "text/html", html);
}

void handleOTA() {
  server.send(200, "text/plain", "OTA starting");
  performOTA(ota_url);
}

// ---------- WiFi setup with WiFiManager ----------

void setupWiFi() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char suffix[7];
  snprintf(suffix, sizeof(suffix), "%02X%02X%02X", mac[3], mac[4], mac[5]);
  ap_ssid = String(ap_prefix) + suffix;

  WiFi.mode(WIFI_STA);
  WiFiManager wm;

  Serial.println("[WiFi] Starting WiFiManager config portal if needed");
  oledPrintCentered("WiFi setup", ap_ssid);

  bool res = wm.autoConnect(ap_ssid.c_str()); // no password

  if (!res) {
    Serial.println("[WiFi] Failed to connect, rebooting");
    oledPrintCentered("WiFi failed", "Rebooting...");
    delay(2000);
    ESP.restart();
  } else {
    Serial.println("[WiFi] Connected to WiFi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    oledPrintCentered("WiFi connected", WiFi.localIP().toString());
    ledSuccess();
  }
}

// ---------- Setup / Loop ----------

void setup() {
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("HBG3 Foot-in-the-door Loader (auto OTA)");

  oledInitAuto();
  oledPrintCentered("HBG3 Loader", "Booting...");

  setupWiFi();  // get onto real WiFi (or config portal)

  server.on("/", handleRoot);
  server.on("/ota", handleOTA);
  server.begin();
  Serial.println("Web server started");

  // Run auto-update once after WiFi is connected
  runAutoUpdateOnce();
}

void loop() {
  server.handleClient();

  // LED idle pattern when not updating
  if (!autoUpdateFailed) {
    ledIdle();
  } else {
    // After two failed attempts, keep a slow blink to show "error but alive"
    ledIdle();
  }

  // BOOT button pressed? (manual OTA)
  static bool lastState = HIGH;
  bool state = digitalRead(BOOT_BUTTON_PIN);
  if (lastState == HIGH && state == LOW) {
    Serial.println("[BTN] BOOT pressed, starting OTA");
    oledPrintCentered("OTA via button", "Updating...");
    performOTA(ota_url);
  }
  lastState = state;
}
