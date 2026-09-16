#include "Arduino.h"
#include "ButtonMatrix.h"
#include <array>
#include <vector>

ButtonMatrix::ButtonMatrix(const u_int rowPins[], const u_int colPins[], u_int rows, u_int cols, bool rowsAreInputs, bool inputsNeedPullups = false) {
    if (rows == 0 || cols == 0) {return;} // TODO: out of range exception
    _rows = rows;
    _cols = cols;
    _bcount = _rows * _cols;
    _rowPins = rowPins;
    _colPins = colPins;
    _rowsAreInputs = rowsAreInputs;
    _pressedCoordinates.resize(_rows, std::vector<bool>(_cols, false)); // Make our coordinate system that tracks whether buttons are triggered the same size as the inputted grid.
    _callbackPressFunctions.resize(_rows, std::vector<CallbackFunction>(_cols, nullptr));
    _callbackReleaseFunctions.resize(_rows, std::vector<CallbackFunction>(_cols, nullptr));
    _callbackHeldFunctions.resize(_rows, std::vector<CallbackFunction>(_cols, nullptr));
    _debounceCounts.resize(_rows, std::vector<u_short>(_cols, 0));

    for (int i = 0; i < rows; i++) {
        if (rowsAreInputs) {if (inputsNeedPullups) {pinMode(rowPins[i], INPUT_PULLUP);} else {pinMode(rowPins[i], INPUT);}} else {pinMode(rowPins[i], OUTPUT); digitalWrite(rowPins[i], HIGH);}
    }
    for (int i = 0; i < cols; i++) {
        if (!rowsAreInputs) {if (inputsNeedPullups) {pinMode(colPins[i], INPUT_PULLUP);} else {pinMode(colPins[i], INPUT);}} else {pinMode(colPins[i], OUTPUT); digitalWrite(colPins[i], HIGH);}
    }
}

void ButtonMatrix::poll() {
    if (!_enabled) {return;}

    if (_rowsAreInputs) { // If rows are the inputs, we're driving the columns on and off. Else, the rows are being driven and the columns are inputs.
        for (int iter = 0; iter < _cols; iter++) {
            digitalWrite(_colPins[iter], LOW); // Write the column pin low
            delayMicroseconds(_pollOutputSwitchDelay);
            for (int i = 0; i < _rows; i++) { // Read all the row pins one by one
                if (!digitalRead(_rowPins[i])) { // If the pin is LOW, the button is pressed.
                    // _pressedCoordinates[i][iter] = true; // A button is pressed, add it to the list of pressed buttons.

                    if (_debounceCounts[i][iter] < _debounceSamples) {
                        _debounceCounts[i][iter] += u_short(1);
                    }

                    if (_debounceCounts[i][iter] >= _debounceSamples/2) {
                        if (!_pressedCoordinates[i][iter]) { // Runs once when first triggered.
                            if (_gCallbackPressFunction != nullptr) {_gCallbackPressFunction();}
                            if (_callbackPressFunctions[i][iter] != nullptr) {_callbackPressFunctions[i][iter]();}    
                        }
                        _pressedCoordinates[i][iter] = true;
                        if (_gCallbackHeldFunction != nullptr) {_gCallbackHeldFunction();}
                        if (_callbackHeldFunctions[i][iter] != nullptr) {_callbackHeldFunctions[i][iter]();} // Held functions get run every loop.
                    }
                } else {
                    if (_debounceCounts[i][iter] > 0) {
                        _debounceCounts[i][iter] -= u_short(1);
                    }

                    if (_debounceCounts[i][iter] < _debounceSamples/2) {
                        if (_pressedCoordinates[i][iter]) { // Runs once when first triggered.
                            if (_gCallbackReleaseFunction != nullptr) {_gCallbackReleaseFunction();}
                            if (_callbackReleaseFunctions[i][iter] != nullptr) {_callbackReleaseFunctions[i][iter]();}    
                        }
                        _pressedCoordinates[i][iter] = false;
                    }
                }
            }
            digitalWrite(_colPins[iter], HIGH); // Reset the column pin back to high
            delayMicroseconds(_pollOutputSwitchDelay);
        }
    } else { // columns are inputs
        for (int iter = 0; iter < _rows; iter++) {
            digitalWrite(_rowPins[iter], LOW); // Write the row pin low
            delayMicroseconds(_pollOutputSwitchDelay);
            for (int i = 0; i < _cols; i++) { // Read all the row pins one by one
                if (!digitalRead(_colPins[i])) {
                    // _pressedCoordinates[iter][i] = true; // A button is pressed, add it to the list of pressed buttons.

                    if (_debounceCounts[iter][i] < _debounceSamples) {
                        _debounceCounts[iter][i] += u_short(1);
                    }

                    if (_debounceCounts[iter][i] >= _debounceSamples/2) {
                        if (!_pressedCoordinates[iter][i]) { // Runs once when first triggered.
                            if (_gCallbackPressFunction != nullptr) {_gCallbackPressFunction();}
                            if (_callbackPressFunctions[iter][i] != nullptr) {_callbackPressFunctions[iter][i]();}    
                        }

                        _pressedCoordinates[iter][i] = true;
                        if (_gCallbackHeldFunction != nullptr) {_gCallbackHeldFunction();}
                        if (_callbackHeldFunctions[iter][i] != nullptr) {_callbackHeldFunctions[iter][i]();} // Held functions get run every loop whilst the button is held.
                    }
                } else {
                    if (_debounceCounts[iter][i] > 0) {
                        _debounceCounts[iter][i] -= u_short(1);
                    }

                    if (_debounceCounts[iter][i] < _debounceSamples/2) {
                        if (_pressedCoordinates[iter][i]) { // Runs once when first triggered.
                            if (_gCallbackReleaseFunction != nullptr) {_gCallbackReleaseFunction();}
                            if (_callbackReleaseFunctions[iter][i] != nullptr) {_callbackReleaseFunctions[iter][i]();}    
                        }
                        _pressedCoordinates[iter][i] = false;
                    }
                }
            }
            digitalWrite(_rowPins[iter], HIGH); // Reset the row pin back to high
            delayMicroseconds(_pollOutputSwitchDelay);
        }
    }
}

bool ButtonMatrix::getButtonPressed(uint row, uint col) {
    if (!_enabled) {return false;}
    if (row >= _rows || col >= _cols) {return false;}
    return _pressedCoordinates[row][col];
}

std::vector<std::vector<bool>> ButtonMatrix::getButtonValues() {
    if (!_enabled) {return {};}
    return _pressedCoordinates;
}

void ButtonMatrix::attachPressCallbackEvent(uint row, uint col, CallbackFunction func) {
    if (row >= _rows || col >= _cols) {return;}
    _callbackPressFunctions[row][col] = func;
}

void ButtonMatrix::attachHeldCallbackEvent(uint row, uint col, CallbackFunction func = nullptr) {
    if (row >= _rows || col >= _cols) {return;}
    _callbackHeldFunctions[row][col] = func;
}

void ButtonMatrix::attachReleaseCallbackEvent(uint row, uint col, CallbackFunction func = nullptr) {
    if (row >= _rows || col >= _cols) {return;}
    _callbackReleaseFunctions[row][col] = func;
}

void ButtonMatrix::attachGlobalPressCallbackEvent(CallbackFunction func) {
    _gCallbackPressFunction = func;
}
void ButtonMatrix::attachGlobalHeldCallbackEvent(CallbackFunction func) {
    _gCallbackHeldFunction = func;
}
void ButtonMatrix::attachGlobalReleaseCallbackEvent(CallbackFunction func) {
    _gCallbackReleaseFunction = func;
}

void ButtonMatrix::setEnabled(bool t) {
    _enabled = t;
}

bool ButtonMatrix::getEnabled() {
    return _enabled;
}

std::array<uint, 2> ButtonMatrix::getSize() {
    return { _rows, _cols };
}