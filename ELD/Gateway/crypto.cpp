#include <Arduino.h>
#include <Wire.h>

#include "crypto.h"

#include "cryptoauthlib.h"

static ATCAIfaceCfg cfg =
{
    .iface_type             = ATCA_I2C_IFACE,
    .devtype                = ATECC608A,
    .atcai2c.slave_address  = 0xC0,
    .atcai2c.bus            = 0,
    .atcai2c.baud           = 400000,
    .wake_delay             = 1500,
    .rx_retries             = 20
};

bool initCrypto(void)
{
    Wire.begin();
    Wire.setClock(400000);

    ATCA_STATUS status = atcab_init(&cfg);

    if (status != ATCA_SUCCESS)
    {
        Serial.println("ATECC608 initialization failed.");
        return false;
    }

    uint8_t serial[9];

    status = atcab_read_serial_number(serial);

    if (status != ATCA_SUCCESS)
    {
        Serial.println("Unable to read serial number.");
        return false;
    }

    Serial.print("ATECC608 Serial: ");

    for (int i = 0; i < 9; i++)
    {
        Serial.printf("%02X", serial[i]);
    }

    Serial.println();

    atcab_sleep();

    return true;
}

bool signHash(const uint8_t hash[32],
              uint8_t signature[64])
{
    ATCA_STATUS status;

    status = atcab_wakeup();

    if(status != ATCA_SUCCESS)
        return false;

    status = atcab_sign(0, hash, signature);

    atcab_sleep();

    return status == ATCA_SUCCESS;
}

bool getPublicKey(uint8_t publicKey[64])
{
    ATCA_STATUS status;

    status = atcab_wakeup();

    if(status != ATCA_SUCCESS)
        return false;

    status = atcab_get_pubkey(0, publicKey);

    atcab_sleep();

    return status == ATCA_SUCCESS;
}