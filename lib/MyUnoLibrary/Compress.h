#include <Arduino.h>

#define RX
#define TX
#define D0
#define D1
#define D2
#define SET
#define READ
#define PUSH

uint8_t packBools8(bool *b) {
	uint8_t package = 0;
	for (uint8_t i = 0; i < 8; i++) {
		package = package | b[i];
		if (i != 7) package <<= 1;
	}
	return package;
}

template <class T>
void pushToReg(uint8_t rx, uint8_t push, uint8_t set, T data){
  uint8_t n = sizeof(T) * 8;
  for(uint8_t i = 0; i < n; i++){
    (data & 0x01 == 0x01) ? digitalWrite(rx, HIGH) : digitalWrite(rx, LOW);

    digitalWrite(push, HIGH);
    delay(2);
    digitalWrite(push, LOW);

    if (i != n - 1) data >>= 1;
  }
  digitalWrite(set, HIGH);
  delay(2);
  digitalWrite(set, LOW);
}

uint8_t readReg(uint8_t tx, uint8_t read, uint8_t d0, uint8_t d1, uint8_t d2){
  uint8_t data = 0;
  digitalWrite(read, HIGH);

  for(uint8_t i = 0; i < 8; i++){

    digitalWrite(d0, (i & 0x01)     );
    digitalWrite(d1, (i & 0x02) >> 1);
    digitalWrite(d2, (i & 0x04) >> 2);

    if (analogRead(tx) >= 1000) data |= 0x01;
    if(i != 7) data <<= 1;
  }

  digitalWrite(read, LOW);
  return data;
}

template <class T>
T readReg_generic(uint8_t tx, uint8_t read, uint8_t *d){
  T data = 0;
  uint8_t n = sizeof(T) * 8;
  uint8_t t;
  switch (n){
  case 8: t = 3;
      break;
  case 16: t = 4;
      break;
  case 32: t = 5;
      break;
  case 64: t = 6;
      break;
  default:
      break;
  }
  digitalWrite(read, HIGH);
  for(uint8_t i = 0; i < n; i++){    

    for(uint8_t j = 0; j < t; j++){
        digitalWrite(d[j], (i & (uint8_t)pow(2, j)) >> j);
    }

    if (analogRead(tx) >= 1000) data |= 0x01;
    if(i != n - 1) data <<= 1;
  }
  digitalWrite(read, LOW);
  return data;
}
/* Beispiel
uint8_t output[6];
uint16_t d = readReg_generic<uint16_t>(1, 2, output);
*/


void unpackBools8(uint8_t package, bool *b) {
	for (int8_t i = 7; i > -1; i--) {
		b[i] = package & 0x01;
		package >>= 1;
	}
}