# SwitchMatrix
Library for interfacing with button/switch diode matrices.
> This documentation is incomplete, and this library is still in early development.

> Note: untested on physical hardware (for now). Just waiting on some PCB's to get here before I can test.

Matrices are created by defining pins for rows and columns, setting the matrix size, stating whether rows or column pins are inputs, and whether the inputs need internal pullup resistors.
```
const u_int rowPins[] = {0, 1, 2, 3};
const u_int colPins[] = {4, 5, 6, 7};

SwitchMatrix matrix(rowPins, colPins, 4, 4, false, false);
```
Matrices need to be polled every loop (or however often you want it to update).
```
void loop() {
    matrix.poll();
}
```

Callback functions can be assigned to press, release, and held events. Held events rerun every poll() iteration whilst the button is pressed.
A callback function is either set as Global (IE, all buttons on the matrix can trigger it), or Local (where only one button can trigger it).
```
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
  matrix.attachPressCallbackEvent(1, 1, pressCallback); // Assign callbacks to button with coordinates (1, 1)
  matrix.attachHeldCallbackEvent(1, 1, heldCallback);
  matrix.attachReleaseCallbackEvent(1, 1, releaseCallback);
  matrix.attachGlobalPressCallbackEvent(globalPressCallback); // Assign global callback to all buttons
}

void loop() {
  matrix.poll();
}
```
Made for matrices defined in this (or similar) format.
> In this image rows are outputs and columns are inputs, and the inputs have external pullup resistors attached, so when using this library you would instantiate the ButtonMatrix object as ```SwitchMatrix matrix(rowPins, colPins, 4, 4, false, false);```.

![4x4 diode button matrix with rows as outputs, columns as inputs, and pullup resisistors on the columns.](https://github.com/ve-cto/ButtonMatrixLib/matrix.png "4x4 diode button matrix with rows as outputs, columns as inputs, and pullup resisistors on the columns.")