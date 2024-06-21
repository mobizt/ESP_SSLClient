/**
 * This example shows how to parse public key.
 *
 * Email: suwatchai@outlook.com
 *
 * Github: https://github.com/mobizt/ESP_SSLSClient
 *
 * Copyright (c) 2023 mobizt
 *
 */
#include <Arduino.h>

#include <ESP_SSLClient.h>

const char public_key[] PROGMEM = "-----BEGIN PUBLIC KEY-----\n"
                                  "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAnD6sFeCJf9bR7PH/YisP\n"
                                  "wCePMUltELq/hp/Dm409XU/yEbgBTI/wMUnH+2juOTYeCirOB10bc/QrFVhaLV3g\n"
                                  "HWyvSL+Wv0ufdWeZiLXAnICNddiJCQvOU+5seNUoPw4QKvFAQgZLogsjWm2uajJS\n"
                                  "nSGskVCt48gaWVrIiO4OQZVkSTxXrUNknenXJ9b2bXam/Bt1ya1E1t1/b/ITZ/03\n"
                                  "WQjfGwXgctqYytvAHC3v541s9o53J2uWn/pOBJc2058Ad5WyzqPRHzA1gbl+XinK\n"
                                  "s85X5U5PAwJiVCsSyCFkNGwJmmuCPtrHipwZi5ax5jfrb71Plilj2VOXdrR2zU7F\n"
                                  "OQIDAQAB\n"
                                  "-----END PUBLIC KEY-----\n";

ESP_SSLClient ssl_client;

void setup()
{

    Serial.begin(115200);
    delay(5000);

    PublicKey pk;
    pk.parse(public_key);

    Serial.println();

    Serial.println("Parse key...");

    if (pk.isRSA())
    {
        Serial.println("RSA key detected.");

        Serial.println("Modulus...");

        for (int i = 0; i < (int)pk.getRSA()->nlen; i++)
        {
            Serial.print(((const char *)pk.getRSA()->n)[i], HEX);
            Serial.print(" ");
        }

        Serial.println();
    }
    else if (pk.isEC())
    {
        Serial.println("ECC key detected.");
        Serial.println("Curve point...");

        for (int i = 0; i < (int)pk.getEC()->qlen; i++)
        {
            Serial.print(((const char *)pk.getEC()->q)[i], HEX);
            Serial.print(" ");
        }

        Serial.println();
    }
    else
    {
        Serial.println("Unknown key");
    }
}

void loop()
{
}