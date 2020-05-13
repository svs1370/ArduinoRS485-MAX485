/*
  This file is part of the ArduinoRS485 library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "RS485_MAX485.h"

RS485_MAX485Class::RS485_MAX485Class(HardwareSerial& hwSerial, int txPin, int dePin, int rePin) :
  _serial(&hwSerial),
  _txPin(txPin),
  _dePin(dePin),
  _rePin(rePin),
  _transmissionBegun(false),
  _transmissionDelay(650) // proper delay will be calculated based on baud rate in begin method
{
}

void RS485_MAX485Class::begin(unsigned long baudrate)
{
  begin(baudrate, SERIAL_8N1);
}

void RS485_MAX485Class::begin(unsigned long baudrate, uint16_t config)
{
  _baudrate = baudrate;
  _config = config;

  if (_dePin > -1) {
    pinMode(_dePin, OUTPUT);
    digitalWrite(_dePin, LOW);
  }

  if (_rePin > -1) {
    pinMode(_rePin, OUTPUT);
    digitalWrite(_rePin, HIGH);
  }

  _transmissionBegun = false;

  // transmisison delay in us is the amount of time required to send 3.5 chars,
  // which is equivalent to 3.5*11 bits in 8N1 mode, at the specified baud rate
  _transmissionDelay = (unsigned int)((1000000 * 3.5 * 11)/baudrate);

  _serial->begin(baudrate, config);
}

void RS485_MAX485Class::end()
{
  _serial->end();

  if (_rePin > -1) {
    digitalWrite(_rePin, LOW);
    pinMode(_dePin, INPUT);
  }
  
  if (_dePin > -1) {
    digitalWrite(_dePin, LOW);
    pinMode(_rePin, INPUT);
  }
}

int RS485_MAX485Class::available()
{
  return _serial->available();
}

int RS485_MAX485Class::peek()
{
  return _serial->peek();
}

int RS485_MAX485Class::read(void)
{
  return _serial->read();
}

void RS485_MAX485Class::flush()
{
  return _serial->flush();
}

size_t RS485_MAX485Class::write(uint8_t b)
{
  if (!_transmissionBegun) {
    setWriteError();
    return 0;
  }

  return _serial->write(b);
}

RS485_MAX485Class::operator bool()
{
  return true;
}

void RS485_MAX485Class::beginTransmission()
{
  if (_dePin > -1) {
    digitalWrite(_dePin, HIGH);
    delayMicroseconds(_transmissionDelay);
  }

  _transmissionBegun = true;
}

void RS485_MAX485Class::endTransmission()
{
  _serial->flush();

  if (_dePin > -1) {
    delayMicroseconds(50);
    digitalWrite(_dePin, LOW);
  }

  _transmissionBegun = false;
}

void RS485_MAX485Class::receive()
{
  if (_rePin > -1) {
    digitalWrite(_rePin, LOW);
  }
}

void RS485_MAX485Class::noReceive()
{
  if (_rePin > -1) {
    digitalWrite(_rePin, HIGH);
  }
}

void RS485_MAX485Class::sendBreak(unsigned int duration)
{
  _serial->flush();
  _serial->end();
  pinMode(_txPin, OUTPUT);
  digitalWrite(_txPin, LOW);
  delay(duration);
  _serial->begin(_baudrate, _config);
}

void RS485_MAX485Class::sendBreakMicroseconds(unsigned int duration)
{
  _serial->flush();
  _serial->end();
  pinMode(_txPin, OUTPUT);
  digitalWrite(_txPin, LOW);
  delayMicroseconds(duration);
  _serial->begin(_baudrate, _config);
}

void RS485_MAX485Class::setPins(int txPin, int dePin, int rePin)
{
  _txPin = txPin;
  _dePin = dePin;
  _rePin = rePin;

  if ( _dePin > -1 ) {
    pinMode( _dePin, OUTPUT);
    digitalWrite( _dePin, LOW);
  }

  if ( _rePin > -1 ) {
    pinMode( _rePin, OUTPUT);
    digitalWrite( _rePin, LOW);
  }
}

RS485_MAX485Class RS485_MAX485(SERIAL_PORT_HARDWARE, RS485_DEFAULT_TX_PIN, RS485_DEFAULT_DE_PIN, RS485_DEFAULT_RE_PIN);
