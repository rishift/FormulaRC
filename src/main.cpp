#include <Arduino.h>

#include <WiFi.h>
#include <WebSocketsServer.h>
#include <WebServer.h>

#include "page.h"

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

byte el = 15;
byte l1 = 2;
byte l2 = 4;

byte er = 5;
byte r1 = 18;
byte r2 = 19;

int speed = 0;

bool fb = 0;

void setFw()
{
    digitalWrite(l1, 1);
    digitalWrite(l2, 0);
    digitalWrite(r1, 1);
    digitalWrite(r2, 0);
}

void setBw()
{
    digitalWrite(l1, 0);
    digitalWrite(l2, 1);
    digitalWrite(r1, 0);
    digitalWrite(r2, 1);
}

void move()
{
    analogWrite(el, speed);
    analogWrite(er, speed);
}

void right()
{
    digitalWrite(el, 0);
    analogWrite(er, speed);
}

void left()
{
    digitalWrite(er, 0);
    analogWrite(el, speed);
}

void stop()
{
    digitalWrite(el, 0);
    digitalWrite(er, 0);
}

void handler(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_BIN:
    {
        byte r = payload[0];

        byte s = r % 10;
        signed char t = r / 10 - 10;


        speed = 25.5 * t;
        move();

        if (s == 2) right();
        else if (s == 0) left();

        if (t > 0)
        {
            if (fb)
            {
                fb = 0;
                setFw();
            }
        }
        else
        {
            if (!fb)
            {
                fb = 1;
                setBw();
            }
        }

        // Serial.print(s);
        // Serial.print(" ");
        // Serial.println(t);
        break;
    }
    case WStype_DISCONNECTED:
        stop();
        break;
    }
}

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("FormulaRC", "aloolelo", 1, false, 2);
    delay(2000);

    Serial.println(WiFi.softAPIP());

    pinMode(el, OUTPUT);
    pinMode(l1, OUTPUT);
    pinMode(l2, OUTPUT);
    pinMode(er, OUTPUT);
    pinMode(r1, OUTPUT);
    pinMode(r2, OUTPUT);

    stop();
    setFw();

    server.on("/", []()
              { server.send(200, "text/html", page); });
    server.begin();

    webSocket.begin();
    webSocket.onEvent(handler);
}

void loop()
{
    server.handleClient();
    webSocket.loop();
}