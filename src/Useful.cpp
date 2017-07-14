#include "Useful.h"


// Converts the given character into a single length string.
string charToString(char character)
{
    string s;
    s.push_back(character);
    s.push_back('\0');
    return s;
}