#include "helpergpio.h"

static int buttonState;

int debounce(int buttonValue)
{
    int button;
    // Code abgeändert von https://elektro.turanis.de/html/prj059/index.html
    if (buttonState == 0 && buttonValue == 48)
    {
        // Taster wird gedrückt (steigende Flanke)
        buttonState = 1;
        button = 1;
    }
    else if (buttonState == 1 && buttonValue == 48)
    {
        // Taster wird gehalten
        buttonState = 2;
        button = 0;
    }
    else if (buttonState == 2 && buttonValue == 49)
    {
        // Taster wird losgelassen (fallende Flanke)
        buttonState = 3;
        button = 0;
    }
    else if (buttonState == 3 && buttonValue == 49)
    {
        // Taster losgelassen
        buttonState = 0;
        button = 0;
    }
    return button;
}

int main()
{
    unsigned int counter = 0;
    SetVal("/sys/class/gpio/export", "17", 2); /* export GPIO17 */
    SetVal("/sys/class/gpio/gpio17/direction", "in", 2); /* set direction of GPIO17 to "in" */
    buttonState = 0;
    while (1) {
        if (debounce(ReadVal("/sys/class/gpio/gpio17/value", 1)) == 1) {
            printf("Button pressed %u times!\n", ++counter);
        }
    }
    SetVal("/sys/class/gpio/unexport", "17", 2);
}
