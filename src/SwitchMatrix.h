#ifndef SwitchMatrix_h
#define SwitchMatrix_h

#include "Arduino.h"
#include <vector>
#include <array>

using CallbackFunction = void (*)();

class SwitchMatrix
{
    public:
        SwitchMatrix(const u_int rowPins[], const u_int colPins[], u_int rows, u_int cols, bool rowsAreInputs, bool inputsNeedPullups);
        void poll();
        void attachPressCallbackEvent(uint row, uint col, CallbackFunction func);
        void attachHeldCallbackEvent(uint row, uint col, CallbackFunction func);
        void attachReleaseCallbackEvent(uint row, uint col, CallbackFunction func);
        void attachGlobalPressCallbackEvent(CallbackFunction func);
        void attachGlobalHeldCallbackEvent(CallbackFunction func);
        void attachGlobalReleaseCallbackEvent(CallbackFunction func);
        void setEnabled(bool t);
        bool getEnabled();
        std::array<uint, 2> getSize();
        std::vector<std::vector<bool>> getButtonValues();
        bool getButtonPressed(uint row, uint col);
        
    private:
        uint _rows;
        uint _cols;
        uint _bcount;
        int _pollIterCount = 0;
        bool _rowsAreInputs;
        uint8_t _pollOutputSwitchDelay = 3;
        const u_int* _rowPins;
        const u_int* _colPins;
        std::vector<std::vector<bool>> _pressedCoordinates;
        std::vector<std::vector<u_short>> _debounceCounts;
        short _debounceSamples = 10;
        std::vector<std::vector<CallbackFunction>> _callbackPressFunctions;
        std::vector<std::vector<CallbackFunction>> _callbackReleaseFunctions;
        std::vector<std::vector<CallbackFunction>> _callbackHeldFunctions;
        CallbackFunction _gCallbackPressFunction = nullptr;
        CallbackFunction _gCallbackHeldFunction = nullptr;
        CallbackFunction _gCallbackReleaseFunction = nullptr;
        bool _enabled = true;
};

#endif