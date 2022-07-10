// OPENAUTO PRO resistive buttons
// You need Arduino Leonardo or Micro to use this code
#include <Keyboard.h>

// *CONFIGURATION*
#define BUTTONS_TOLERANCE 15
#define LONG_PRESS_TIME 800
#define DOUBLE_CLICK_INTERVAL 80
#define READ_DELAY 20
#define VERBOSE false

const int BUTTONS[5][4] = {
    //{analogValue, single click function, double click function, long click function}
    {872, 2, 12, 20}, // CH+
    {825, 3, 13, 17}, // CH-
    {767, 7, 18, 10}, // VOL+
    {697, 6, 16, 10}, // VOL-
    {980, 4, 11, 4}   // MODE
};
const int FUNCTIONS[21][2] = {
    {218},      // 0  arrow up
    {217},      // 1  arrow down
    {215},      // 2  arrow right
    {216},      // 3  arrow left
    {176},      // 4  select
    {177},      // 5  back
    {200},      // 6  volume down
    {201},      // 7  volume up
    {202},      // 8  brightness down
    {203},      // 9  brightness up
    {128, 204}, // 10 toggle mute
    {'h'},      // 11 home
    {'p'},      // 12 phone, answer call
    {'o'},      // 13 hang up call
    {'x'},      // 14 play
    {'c'},      // 15 pause
    {'v'},      // 16 previous track
    {'b'},      // 17 toggle play
    {'n'},      // 18 next track
    {'j'},      // 19 media menu
    {'m'}       // 20 voice assistant
};

// variables
int analogReadValue = 0;
int pressDuration = 0;

// functions

void printLog(char *message, int parameter)
{
    if (VERBOSE)
    {
        Serial.print(String(message));
        Serial.print(": ");
        Serial.println(parameter);
    }
}

int readPressedButton(int analogValue)
{
    int button = -1;
    for (int i = 0; i < 5; i++)
    {
        button = BUTTONS[i][0];
        if (analogValue > button - BUTTONS_TOLERANCE && analogValue < button + BUTTONS_TOLERANCE)
        {
            return i; // return button number
        }
    }
    return -1;
}

unsigned long pressTime(int pressedButton)
{
    unsigned long pressedTimeStart = millis();
    int currentPressedButton = pressedButton;
    int analogReadValue = 0;
    while (currentPressedButton == pressedButton)
    {
        delay(READ_DELAY);
        analogReadValue = analogRead(A0);
        currentPressedButton = readPressedButton(analogReadValue);
    }
    unsigned long pressedTime = millis() - pressedTimeStart;
    printLog("Pressed button", pressedButton);
    return pressedTime; // return pressed time
}

int detectAction(int analogReadValue)
{
    int pressedButton = readPressedButton(analogReadValue);
    int currentPressedButton = pressedButton;
    int action = -1;
    int pressTimeValue = pressTime(pressedButton);
    bool doubleClicked = false;
    printLog("pressTimeValue", pressTimeValue);
    unsigned long pressTimeEnd = millis();
    if (pressedButton == -1)
    {
        return -1;
    }
    if (pressTimeValue >= LONG_PRESS_TIME)
    {
        action = BUTTONS[pressedButton][3];
        printLog("Long press", action);
        return action;
    }
    else
    {
        while (millis() - pressTimeEnd < DOUBLE_CLICK_INTERVAL)
        {
            delay(READ_DELAY);
            currentPressedButton = readPressedButton(analogRead(A0));
            if (currentPressedButton == pressedButton)
            {
                action = BUTTONS[pressedButton][2];
                doubleClicked = true;
                printLog("Double click", action);
                while (currentPressedButton == pressedButton)
                {
                    currentPressedButton = readPressedButton(analogRead(A0));
                }
                return action;
            }
        }
    }
    if (!doubleClicked)
    {
        action = BUTTONS[pressedButton][1];
        printLog("Single click", action);
    }
    return action;
}

// main
void setup()
{
    if (VERBOSE)
    {
        Serial.begin(9600);
    }
    Keyboard.begin();
}

void loop()
{
    if (analogRead(A0) > BUTTONS_TOLERANCE)
    {
        delay(READ_DELAY);
        analogReadValue = analogRead(A0);
        int action = detectAction(analogReadValue);
        printLog("Action", action);
        if (action >= 0)
        {
            Keyboard.press(FUNCTIONS[action][0]);
            if (FUNCTIONS[action][1])
            {
                Keyboard.press(FUNCTIONS[action][1]);
            }
            Keyboard.releaseAll();
        }
    }
}