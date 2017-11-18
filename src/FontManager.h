#pragma once

#include "ofMain.h"
#include "Letter.h"
#include "LetterManager.h"

class Letter;

/*
 * Written by Bryce Summers on October.28th.2017
 * Encapsulates lists of fonts of a set of fixed sizes set externally.
 * Allows for a font to be queried, could also manage spacings of letters.
 */

class FontManager
{
public:
    FontManager(vector<int>   * fontSizes,
                vector<float> * spaceSizes);
    ~FontManager();

    vector<int>   * fontSizes;
    vector<float> * spaceSizes;

    vector<ofTrueTypeFont *> normal_fonts;
    vector<ofTrueTypeFont *> italic_fonts;


    // Returns the font of size_index [0...N], and the italic version if italic is true.
    ofTrueTypeFont * getFont(int size_index, bool italic);

    // Spawns a set of falling letters.
    void spawnSentance(string line,
                       list<Letter *> * output,
                       LetterManager * letterManager,
                       int sentance_index);

    void reloadFonts();
    float getSpaceSizeFactor(int sizeIndex);

    float getWordOffset(string & str, int font_size_index, bool font_italics);

private:

    void loadFonts();
    void loadFonts(vector<ofTrueTypeFont *> * normal_fonts,
                   vector<ofTrueTypeFont *> * italic_fonts);

    // -- Finite State machine for generating characters.

    // Variable state.

    bool first_letter = false;

    ofTrueTypeFont * current_font;
    int size_index; // 1 - N (Human readable starting at 1, offset by -1 on the machine.
    bool italics;
    int gap_accum; // The accumulated gap between letters.
    Letter * previous_letter;
    
    // True iff the previous letter is separated from this one by at least 1 space.
    bool space_after_previous_letter;
    int width_of_previous_letter;
    char previous_character;

    // Constant state.
    list<Letter *> * output;
    LetterManager * letterManager;
    int sentance_index;

    void initial_state(list<Letter *> * output,
        LetterManager * letterManager,
        int sentance_index);

    // Changes the state based on the single key character c.
    // 'n' --> normal
    // 'i' --> italics
    // '0' - '9' --> setSizeIndex.
    void changeState(char c);
    void change_size_index(int size_index);
    void change_italics(bool italics);
    void update_font();
    void addSpace();
    float getSpaceSize();

    // Resets the gap calculation.
    void outputLetter(char letter);

    // Returns a random initial position.
    ofVec2f getInitialPosition();

    // Returns the offset from the previous letter: "as{d    a}ksdjk" --> "as{d a}ksdjk"
    float getOffsetWithoutLargeSpaces(char c);

    // Returns the offset from the previous letter or beginining accounting for large spaces and indentation.
    float getOffsetWithLargeSpaces(char c);

    float getKernedNonspacedOffset(char c);

    // Defaults to current font.
    float FontManager::stringWidth(string & str);
    float FontManager::stringWidth(string & str, ofTrueTypeFont * font);

    // Finish by clearing away any unnecessary pointer state.
    void conclude();

};