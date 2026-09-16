#include <Arduino.h>
#include <ButtonMatrix.h>
#include <vector>

const u_int rowPins[] = {0, 1, 2, 3, 4, 5, 6};
const u_int colPins[] = {7, 8, 9, 10};

ButtonMatrix matrix(rowPins, colPins, 7, 4, false, false);

void pressCallback() {
  Serial.println("Pressed!");
}

void heldCallback() {
  Serial.println("Holding....");
}

void releaseCallback() {
  Serial.println("Released!");
}

void globalPressCallback() {
  Serial.println("A button was pressed!");

  std::vector<std::vector<bool>> buttons = matrix.getButtonValues();
  for (int i = 0; i < buttons.size(); i++) {
    for (int j = 0; j < buttons[i].size(); j++) {
      if (buttons[i][j]) {
        String msg = "Button (" + String(i) + ", " + String(j) + ") is currently pressed!";
        Serial.println(msg);
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  matrix.attachPressCallbackEvent(1, 1, pressCallback);
  matrix.attachHeldCallbackEvent(1, 1, heldCallback);
  matrix.attachReleaseCallbackEvent(1, 1, releaseCallback);
  matrix.attachGlobalPressCallbackEvent(globalPressCallback);
}

void loop() {
  matrix.poll();
}