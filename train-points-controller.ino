#include <Arduino.h>
#include <Wire.h>
#define DELAY_INTERVAL 500

uint8_t pins[14] = {
    A0,
    A1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13
};

uint16_t state = 0;
uint16_t newState = 0;

void setup() {
  for(int i = 0; i < 14; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
  
  Wire.begin(0x40);
  Wire.onReceive(setState);
  # ifdef _DEBUG
  Serial.begin(9600);
  Serial.println("Listening on 0x40");
  # endif
}

void loop() {
  uint16_t diff;
  uint8_t pinIndex = 0;
  if(state == newState) {
    return;
  }

  # ifdef DEBUG
  Serial.print("New state received: ");
  Serial.println(newState, HEX);
  # endif

  diff = (state ^ newState) & newState;
  do {
    if ((diff & 0x1) != 0x0) {
      # ifdef _DEBUG
      Serial.print("Sending pulse to pin: ");
      Serial.println(pinIndex, HEX);
      # endif

      digitalWrite(pins[pinIndex], HIGH);
      delay(DELAY_INTERVAL);
      digitalWrite(pins[pinIndex], LOW);
      delay(DELAY_INTERVAL);

      # ifdef _DEBUG
      Serial.println("End of pulse");
      # endif
    }

    diff = diff >> 1;
    pinIndex += 1;
  } while(diff != 0);

  state = newState;
}

void setState(int available) {
  uint8_t *newStatePtr = (uint8_t*)&newState;

  # ifdef _DEBUG
  Serial.print("Received: ");
  Serial.print(available, HEX);
  Serial.println(" bytes");
  # endif

  for(int i = 0; i < 2; i++) {
    if(i < available) {
      newStatePtr[i] = (uint8_t)Wire.read();
    } else {
      newStatePtr[i] = 0;
    }
  }
}
