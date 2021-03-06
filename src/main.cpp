#include <M5Stack.h>
#include <WiFi.h>
#include "EL.h"
#include <Adafruit_BMP280.h>
#include <Adafruit_SHT31.h>

#define WIFI_SSID "ssid"         // !!!! change
#define WIFI_PASS "pass" // !!!! change

WiFiClient client;
WiFiUDP elUDP;
// EL echo(elUDP, 0x02, 0x90, 0x01);
byte eojs[][3] = {
    {0x00, 0x11, 0x01}, // 温度センサ
    {0x00, 0x12, 0x01}, // 湿度センサ
    {0x00, 0x2d, 0x01}, // 気圧センサ
};
EL echo(elUDP, eojs, sizeof(eojs) / sizeof(byte[3]));

Adafruit_BMP280 bmp;
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void printNetData()
{
    Serial.println("---");

    // IP
    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP  Address: ");
    Serial.println(ip);

    IPAddress dgwip = WiFi.gatewayIP();
    Serial.print("DGW Address: ");
    Serial.println(dgwip);

    IPAddress smip = WiFi.subnetMask();
    Serial.print("SM  Address: ");
    Serial.println(smip);

    Serial.println("---");
}

void setup() {
    M5.begin();
    M5.Lcd.setTextSize(2);

    Serial.begin(115200);
    Serial.println(F("BMP280 Sensor event test"));

    if (!bmp.begin(0x76)) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while (1) delay(10);
    }

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
        Serial.println("Couldn't find SHT31");
        while (1) delay(1);
    }

    M5.Lcd.println("wifi connect start");

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("wait...");
        delay(1000);
    }
    M5.Lcd.println("wifi connect ok");
    M5.update();

    printNetData();
    echo.begin(); // EL 起動シーケンス

    echo.devices[0][0x80] = new byte[2] {0x01, 0x30};                                                                                                   // power
	echo.devices[0][0x81] = new byte[2] {0x01, 0x40};                                                                                                   // position
	echo.devices[0][0x82] = new byte[5] {0x04, 0x00, 0x00, 0x4b, 0x00};                                                                                 // release K
	echo.devices[0][0x83] = new byte[19]{0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};   // identification number
	echo.devices[0][0x88] = new byte[4] {0x01, 0x42};                                                                                                   // error status
	echo.devices[0][0x8a] = new byte[4] {0x03, 0x00, 0x00, 0x00};                                                                                       // maker unknown
	echo.devices[0][0xe0] = new byte[3] {0x02, 0x00, 0x00};                                                                                             // temperature
    echo.devices[0][0x9D] = new byte[3] {0x02, 0x01, 0x80};                                                                                             // 状変アナウンスプロパティ
    echo.devices[0][0x9E] = new byte[3] {0x02, 0x01, 0x81};                                                                                             // Setプロパティマップ
    echo.devices[0][0x9f] = new byte[12]{11, 10, 0x80, 0x81, 0x82, 0x83, 0x88, 0x8a, 0xe0, 0x9d, 0x9e, 0x9f};                                           // Getプロパティマップ

    echo.devices[1][0x80] = new byte[2] {0x01, 0x30};                                                                                                   // power
	echo.devices[1][0x81] = new byte[2] {0x01, 0x40};                                                                                                   // position
	echo.devices[1][0x82] = new byte[5] {0x04, 0x00, 0x00, 0x4b, 0x00};                                                                                 // release K
	echo.devices[1][0x83] = new byte[19]{0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};   // identification number
	echo.devices[1][0x88] = new byte[4] {0x01, 0x42};                                                                                                   // error status
	echo.devices[1][0x8a] = new byte[4] {0x03, 0x00, 0x00, 0x00};                                                                                       // maker unknown
	echo.devices[1][0xe0] = new byte[2] {0x01, 0x00};                                                                                                   // humidity
    echo.devices[1][0x9D] = new byte[3] {0x02, 0x01, 0x80};                                                                                             // 状変アナウンスプロパティ
    echo.devices[1][0x9E] = new byte[3] {0x02, 0x01, 0x81};                                                                                             // Setプロパティマップ
    echo.devices[1][0x9f] = new byte[12]{11, 10, 0x80, 0x81, 0x82, 0x83, 0x88, 0x8a, 0xe0, 0x9d, 0x9e, 0x9f};                                           // Getプロパティマップ

    echo.devices[2][0x80] = new byte[2] {0x01, 0x30};                                                                                                   // power
	echo.devices[2][0x81] = new byte[2] {0x01, 0x40};                                                                                                   // position
	echo.devices[2][0x82] = new byte[5] {0x04, 0x00, 0x00, 0x4b, 0x00};                                                                                 // release K
	echo.devices[2][0x83] = new byte[19]{0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};   // identification number
	echo.devices[2][0x88] = new byte[4] {0x01, 0x42};                                                                                                   // error status
	echo.devices[2][0x8a] = new byte[4] {0x03, 0x00, 0x00, 0x00};                                                                                       // maker unknown
	echo.devices[2][0xe0] = new byte[3] {0x02, 0x00, 0x00};                                                                                             // pressure
    echo.devices[2][0x9D] = new byte[3] {0x02, 0x01, 0x80};                                                                                             // 状変アナウンスプロパティ
    echo.devices[2][0x9E] = new byte[3] {0x02, 0x01, 0x81};                                                                                             // Setプロパティマップ
    echo.devices[2][0x9f] = new byte[12]{11, 10, 0x80, 0x81, 0x82, 0x83, 0x88, 0x8a, 0xe0, 0x9d, 0x9e, 0x9f};                                           // Getプロパティマップ

    for (int i = 0; i < 3; i++){
        echo.devices[i].printAll();
    }

    const byte seoj[] = {0x0e, 0xf0, 0x01};
    const byte deoj[] = {0x0e, 0xf0, 0x01};
    echo.sendMultiOPC1(seoj, deoj, EL_INF, 0xd5, echo.profile[0xd5]);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextSize(2);
}

int getDeviceId(byte eoj0, byte eoj1, byte eoj2) {
    const int count = sizeof(eojs) / sizeof(byte[3]);
    int id = -1;
    for (int i = 0;i < count; i++) {
        if (eoj0 == eojs[i][0] && eoj1 == eojs[i][1] && eoj2 == eojs[i][2]) {
            id = i;
            break;
        }
    }

    Serial.printf("devId:%d, eoj:%02x%02x%02x", id, eoj0, eoj1, eoj2);
    Serial.println();
    return id;
}

int count = 0;
byte *buf_p = new byte[3]{2, 0, 0,};
byte *buf_t = new byte[3]{2, 0, 0,};
byte *buf_h = new byte[2]{1, 0,};
void loop() {
    if (count == 0) {
        float p = bmp.readPressure() / 100.0;
        float t = sht31.readTemperature();
        float h = sht31.readHumidity();

        if (! isnan(p)) {  // check if 'is not a number'
            Serial.print("Pressure hPa= "); Serial.print(p); Serial.print("\t\t");
        } else { 
            Serial.println("Failed to read pressure");
        }
        if (! isnan(t)) {  // check if 'is not a number'
            Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
        } else { 
            Serial.println("Failed to read temperature");
        }
        if (! isnan(h)) {  // check if 'is not a number'
            Serial.print("Hum. % = "); Serial.println(h);
        } else { 
            Serial.println("Failed to read humidity");
        }

        M5.Lcd.setCursor(0, 10);
        int height = M5.Lcd.fontHeight(1);
        M5.Lcd.fillRect(0,10,320,height,BLACK);
        M5.Lcd.printf("%5.2fC, %5.2f%%, %7.2fhPa", t, h, p);

        uint16_t s_t = 0x7fff;
        if (t < -273.2)
            s_t = 0x8000;
        else if (t <= 3276.6)
            s_t = (uint16_t)(t * 10);
        buf_t[1] = s_t >> 8;
        buf_t[2] = s_t & 0xff;
        echo.update(0, 0xe0, buf_t);                       // ECHONET Liteの状態を変更
        buf_h[1] = (byte)h;
        echo.update(1, 0xe0, buf_h);                       // ECHONET Liteの状態を変更
        uint16_t s_p = 0xffff;
        if (p < 0)
            s_p = 0xfffe;
        else if (p <= 6553.3)
            s_p = (uint16_t)(p * 10);
        buf_p[1] = s_p >> 8;
        buf_p[2] = s_p & 0xff;
        echo.update(2, 0xe0, buf_p);                       // ECHONET Liteの状態を変更

        Serial.printf("t:%04x, h:%02x, p:%04x", s_t, buf_h[1], s_p);
        Serial.println();
    }

    // パケット貰ったらやる
    int packetSize = 0;     // 受信データ量

    if (0 != (packetSize = echo.read())) // 0!=はなくてもよいが，Warning出るのでつけとく
    {                                    // 受け取った内容読み取り，あったら中へ
        // ESVがSETとかGETとかで動作をかえる
        switch (echo._rBuffer[EL_ESV])
        {
        case EL_SETI:
        case EL_SETC:
            {
                const int devId = getDeviceId(echo._rBuffer[EL_DEOJ + 0], echo._rBuffer[EL_DEOJ + 1], echo._rBuffer[EL_DEOJ + 2]);
                if (devId >= 0) {
                    switch (echo._rBuffer[EL_EPC])
                    {
                    case 0x81:
                        byte *pdcedt = new byte[2]{0x01, echo._rBuffer[EL_EDT + 0]};           // ECHONET Liteの状態を変更
                        echo.update(devId, echo._rBuffer[EL_EPC], pdcedt); // ECHONET Liteの状態を変更
                        delete[] pdcedt;
                        break;
                    }
                }

                if (echo._rBuffer[EL_ESV] == EL_SETC)
                { // SETCなら返信必要
                    echo.returner();
                }
            }
            break;
        case EL_GET:
            echo.returner();
            break; // GetとINF_REQここまで
        }
    }
    delay(200);

    count = (count + 1) % 10;
    M5.update();
}