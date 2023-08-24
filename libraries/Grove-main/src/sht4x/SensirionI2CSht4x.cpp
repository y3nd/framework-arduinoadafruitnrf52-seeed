/*
 * THIS FILE IS AUTOMATICALLY GENERATED
 *
 * I2C-Generator: 0.2.0
 * Yaml Version: 0.1.0
 * Template Version: 0.7.0-12-g6411a7e
 */
/*
 * Copyright (c) 2021, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "SensirionI2CSht4x.h"
#include "Arduino.h"
#include "SensirionCore.h"
#include <Wire.h>


SensirionI2CSht4x::SensirionI2CSht4x() {
}

void SensirionI2CSht4x::begin(TwoWire& i2cBus) {
    _i2cBus = &i2cBus;
}

uint16_t
SensirionI2CSht4x::measureHighPrecisionTicks(uint16_t& temperatureTicks,
                                             uint16_t& humidityTicks) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0xFD, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(10);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt16(temperatureTicks);
    error |= rxFrame.getUInt16(humidityTicks);
    return error;
}

uint16_t SensirionI2CSht4x::measureHighPrecision(float& temperature,
                                                 float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error = measureHighPrecisionTicks(temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = _convertTicksToCelsius(temperatureTicks);
    humidity = _convertTicksToPercentRH(humidityTicks);
    return NoError;
}

uint16_t
SensirionI2CSht4x::measureMediumPrecisionTicks(uint16_t& temperatureTicks,
                                               uint16_t& humidityTicks) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0xF6, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(5);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt16(temperatureTicks);
    error |= rxFrame.getUInt16(humidityTicks);
    return error;
}

uint16_t SensirionI2CSht4x::measureMediumPrecision(float& temperature,
                                                   float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error = measureMediumPrecisionTicks(temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = _convertTicksToCelsius(temperatureTicks);
    humidity = _convertTicksToPercentRH(humidityTicks);
    return NoError;
}

uint16_t
SensirionI2CSht4x::measureLowestPrecisionTicks(uint16_t& temperatureTicks,
                                               uint16_t& humidityTicks) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0xE0, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(2);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt16(temperatureTicks);
    error |= rxFrame.getUInt16(humidityTicks);
    return error;
}

uint16_t SensirionI2CSht4x::measureLowestPrecision(float& temperature,
                                                   float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error = measureLowestPrecisionTicks(temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = _convertTicksToCelsius(temperatureTicks);
    humidity = _convertTicksToPercentRH(humidityTicks);
    return NoError;
}

uint16_t SensirionI2CSht4x::activateHighestHeaterPowerLongTicks(
    uint16_t& temperatureTicks, uint16_t& humidityTicks) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0x39, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(1100);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt16(temperatureTicks);
    error |= rxFrame.getUInt16(humidityTicks);
    return error;
}

uint16_t SensirionI2CSht4x::activateHighestHeaterPowerLong(float& temperature,
                                                           float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error =
        activateHighestHeaterPowerLongTicks(temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = _convertTicksToCelsius(temperatureTicks);
    humidity = _convertTicksToPercentRH(humidityTicks);
    return NoError;
}

uint16_t SensirionI2CSht4x::activateHighestHeaterPowerShortTicks(
    uint16_t& temperatureTicks, uint16_t& humidityTicks) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0x32, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(110);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt16(temperatureTicks);
    error |= rxFrame.getUInt16(humidityTicks);
    return error;
}

uint16_t SensirionI2CSht4x::activateHighestHeaterPowerShort(float& temperature,
                                                            float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error =
        activateHighestHeaterPowerShortTicks(temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = _convertTicksToCelsius(temperatureTicks);
    humidity = _convertTicksToPercentRH(humidityTicks);
    return NoError;
}

uint16_t SensirionI2CSht4x::activateMediumHeaterPowerLongTicks(
    uint16_t& temperatureTicks, uint16_t& humidityTicks) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0x2F, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(1100);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt16(temperatureTicks);
    error |= rxFrame.getUInt16(humidityTicks);
    return error;
}

uint16_t SensirionI2CSht4x::activateMediumHeaterPowerLong(float& temperature,
                                                          float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error = activateMediumHeaterPowerLongTicks(temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = _convertTicksToCelsius(temperatureTicks);
    humidity = _convertTicksToPercentRH(humidityTicks);
    return NoError;
}

uint16_t SensirionI2CSht4x::activateMediumHeaterPowerShortTicks(
    uint16_t& temperatureTicks, uint16_t& humidityTicks) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0x24, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(110);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt16(temperatureTicks);
    error |= rxFrame.getUInt16(humidityTicks);
    return error;
}

uint16_t SensirionI2CSht4x::activateMediumHeaterPowerShort(float& temperature,
                                                           float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error =
        activateMediumHeaterPowerShortTicks(temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = _convertTicksToCelsius(temperatureTicks);
    humidity = _convertTicksToPercentRH(humidityTicks);
    return NoError;
}

uint16_t SensirionI2CSht4x::activateLowestHeaterPowerLongTicks(
    uint16_t& temperatureTicks, uint16_t& humidityTicks) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0x1E, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(1100);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt16(temperatureTicks);
    error |= rxFrame.getUInt16(humidityTicks);
    return error;
}

uint16_t SensirionI2CSht4x::activateLowestHeaterPowerLong(float& temperature,
                                                          float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error = activateLowestHeaterPowerLongTicks(temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = _convertTicksToCelsius(temperatureTicks);
    humidity = _convertTicksToPercentRH(humidityTicks);
    return NoError;
}

uint16_t SensirionI2CSht4x::activateLowestHeaterPowerShortTicks(
    uint16_t& temperatureTicks, uint16_t& humidityTicks) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0x15, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(110);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt16(temperatureTicks);
    error |= rxFrame.getUInt16(humidityTicks);
    return error;
}

uint16_t SensirionI2CSht4x::activateLowestHeaterPowerShort(float& temperature,
                                                           float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error =
        activateLowestHeaterPowerShortTicks(temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = _convertTicksToCelsius(temperatureTicks);
    humidity = _convertTicksToPercentRH(humidityTicks);
    return NoError;
}

uint16_t SensirionI2CSht4x::serialNumber(uint32_t& serialNumber) {
    uint16_t error;
    uint8_t buffer[6];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0x89, buffer, 6);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    if (error) {
        return error;
    }

    delay(10);

    SensirionI2CRxFrame rxFrame(buffer, 6);
    error = SensirionI2CCommunication::receiveFrame(SHT4X_I2C_ADDRESS, 6,
                                                    rxFrame, *_i2cBus);
    if (error) {
        return error;
    }

    error |= rxFrame.getUInt32(serialNumber);
    return error;
}

uint16_t SensirionI2CSht4x::softReset() {
    uint16_t error;
    uint8_t buffer[1];
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt8Command(0x94, buffer, 1);

    error = SensirionI2CCommunication::sendFrame(SHT4X_I2C_ADDRESS, txFrame,
                                                 *_i2cBus);
    delay(10);
    return error;
}

float SensirionI2CSht4x::_convertTicksToCelsius(uint16_t ticks) {
    return static_cast<float>(ticks * 175.0 / 65535.0 - 45.0);
}

float SensirionI2CSht4x::_convertTicksToPercentRH(uint16_t ticks) {
    return static_cast<float>(ticks * 125.0 / 65535.0 - 6);
}