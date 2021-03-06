#include "FontManager.h"

// Must at the minnimum contains at least 1 normal font.
FontManager::FontManager(vector<int>   * fontSizes,
                         vector<float> * spaceSizes)
{
    this -> fontSizes = fontSizes;
    this -> spaceSizes = spaceSizes;

    loadFonts();
}


FontManager::~FontManager()
{
}

void FontManager::loadFonts()
{
    loadFonts(&this -> normal_fonts, &italic_fonts);
}

void FontManager::reloadFonts()
{
    vector<ofTrueTypeFont *> new_normal_fonts;
    vector<ofTrueTypeFont *> new_italic_fonts;
    loadFonts(&new_normal_fonts, &new_italic_fonts);

    for (int i = 0; i < fontSizes -> size(); i++)
    {
        ofTrueTypeFont * old1 = normal_fonts[i];
        normal_fonts[i] = new_normal_fonts[i];
        delete old1;

        ofTrueTypeFont * old2 = italic_fonts[i];
        italic_fonts[i] = new_italic_fonts[i];
        delete old2;
    }
}

void FontManager::loadFonts(
    vector<ofTrueTypeFont *> * normal_fonts,
    vector<ofTrueTypeFont *> * italic_fonts)
{
    //old OF default is 96 - but this results in fonts looking larger than in other programs. 
    ofTrueTypeFont::setGlobalDpi(72);

    for (auto iter = fontSizes->begin(); iter != fontSizes->end(); iter++)
    {
        int size = *iter;

        ofTrueTypeFont * normal = new ofTrueTypeFont();
        normal->load("verdana.ttf", size, true, true);

        ofTrueTypeFont * italic = new ofTrueTypeFont();
        italic->load("verdanai.ttf", size, true, true);


        /* settings for 14 pt font.
        normal -> setLineHeight(18.0f);
        normal -> setLetterSpacing(1.037);
        */

        // Generalize line spacings for any size.
        normal->setLineHeight(1.2857142857142858*size);
        //normal -> setLetterSpacing(0.07407142857142857*size);
        italic->setLineHeight(1.2857142857142858*size);
        //italic -> setLetterSpacing(0.07407142857142857*size);

        normal->setLetterSpacing(1.037);
        italic->setLetterSpacing(1.037);

        normal_fonts -> push_back(normal);
        italic_fonts -> push_back(italic);
    }

    /*
    verdana14.load("verdana.ttf", 14, true, true);
    verdana14.setLineHeight(18.0f);
    verdana14.setLetterSpacing(1.037);

    verdana30.load("verdana.ttf", 30, true, true);
    verdana30.setLineHeight(34.0f);
    verdana30.setLetterSpacing(1.035);

    verdana14A.load("verdana.ttf", 14, false);
    verdana14A.setLineHeight(18.0f);
    verdana14A.setLetterSpacing(1.037);

    franklinBook14.load("frabk.ttf", 14);
    franklinBook14.setLineHeight(18.0f);
    franklinBook14.setLetterSpacing(1.037);

    franklinBook14A.load("frabk.ttf", 14, false);
    franklinBook14A.setLineHeight(18.0f);
    franklinBook14A.setLetterSpacing(1.037);
    */

    /*
    bFirst = true;
    typeStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789,:&!?";

    string str = "From fairest creatures we desire increase, ";

    string accum;

    int len = str.length();
    for (int i = 0; i < len + 1; i++)
    {
    int length = font.stringWidth(accum);
    //cout << length << endl; // Number of letters.
    char c = str[i];
    accum.push_back(c);
    }*/
}

// Returns the font of size_index [1...N], and the italic version if italic is true.
ofTrueTypeFont * FontManager::getFont(int size_index, bool italic)
{   
    if(!italic)
    {
        return normal_fonts[size_index - 1];
    }
    else
    {
        return italic_fonts[size_index - 1];
    }
}


// FIXME: Convert this into an finite state machine that gets italics and sizings.
void FontManager::spawnSentance(
    string line,
    list<Letter *> * output,
    LetterManager * letterManager,
    int sentance_index)
{
    
    initial_state(output, letterManager, sentance_index);

    int len = line.length();
    for (int char_index = 0; char_index < len; char_index++)
    {
        char c = line[char_index];

        // Handle State changes.
        if (c == '\\')
        {
            char_index += 1;
            c = line[char_index];

            changeState(c);
            continue;
        }
    
        // Handle spaces.
        if (c == ' ')
        {
            addSpace();
            continue;
        }
    
        // Otherwise output a letter.
        outputLetter(c);
    }

    conclude();
}

void FontManager::initial_state(
    list<Letter *> * output,
    LetterManager * letterManager,
    int sentance_index)
{
    this -> first_letter = true;

    // Variable State.
    this -> size_index = 1;
    this -> italics = false;
    this -> gap_accum = 0;
    this -> previous_letter = NULL;
    this -> space_after_previous_letter = false;

    update_font();

    this -> width_of_previous_letter = 0;
    this -> previous_character = '\null';

    // Constant State over the span of a sentance.
    this -> output         = output;
    this -> letterManager  = letterManager;
    this -> sentance_index = sentance_index;
}

void FontManager::changeState(char c)
{
    switch (c)
    {
        case 'n': change_italics(false); return;
        case 'i': change_italics(true);  return;
        //case '0': change_size_index(0);  return;
        case '1': change_size_index(1);  return;
        case '2': change_size_index(2);  return;
        case '3': change_size_index(3);  return;
        case '4': change_size_index(4);  return;
        case '5': change_size_index(5);  return;
        case '6': change_size_index(6);  return;
        case '7': change_size_index(7);  return;
        case '8': change_size_index(8);  return;
        case '9': change_size_index(9);  return;
    }
}

// 0 - 10.
void FontManager::change_size_index(int size_index)
{
    this -> size_index = size_index;
    update_font();
}

void FontManager::change_italics(bool italics)
{
    this -> italics = italics;
    update_font();
}

void FontManager::update_font()
{
    current_font = getFont(size_index, italics);
}

void FontManager::addSpace()
{
    gap_accum += getSpaceSize();
    space_after_previous_letter = true;
}

float FontManager::getSpaceSize()
{
    string positive = ". .";
    string negative = "..";

    float pos_width = stringWidth(positive);
    float neg_width = stringWidth(negative);

    return pos_width - neg_width;
}

// Resets the gap calculation.
void FontManager::outputLetter(char c)
{
    ofVec2f pos = getInitialPosition();

    float offset_min  = getOffsetWithoutLargeSpaces(c);
    if (first_letter) // Give the first letter, the full spacing.
    {
        offset_min = 0;
    }
    float offset_full = getOffsetWithLargeSpaces(c);

    Letter * l = new Letter(letterManager,
        pos.x, pos.y,
        previous_letter,
        c,
        offset_min,
        offset_full, // Used as offset in text scroll.
        space_after_previous_letter, // used to delliminate words.
        sentance_index, // Sentance index.
        size_index, // Everything the letter needs to dynamically query its font.
        italics,
        this
    );

    // First letter has been spawned.
    first_letter = false;

    output -> push_back(l);

    if (previous_letter != NULL)
    {
        previous_letter -> setRightLetter(l);
    }

    // Update trailing data for next loop.
    previous_letter = l;
    previous_character = c;
    string str = "";
    str.push_back(c);
    width_of_previous_letter = stringWidth(str);
    space_after_previous_letter = false;
    gap_accum = 0;
}

// Returns a random initial position.
ofVec2f FontManager::getInitialPosition()
{
    const int margin = 20;
    float y0_height = margin * 10;

    // Blank space on the left and right sides of the screen where letters will not form.
    float x = margin + ofRandom(ofGetWidth() - margin * 2);

    // Height scaled to create the illusion of a continuous stream of letters.
    float y = -20 - ofRandom(y0_height);

    return ofVec2f(x, y);
}


// Returns the offset from the previous letter: "as{d    a}ksdjk" --> "as{d a}ksdjk"
float FontManager::getOffsetWithoutLargeSpaces(char c)
{
    // Without any spacing, this is just the kerned offset from the previous letter.
    if (!space_after_previous_letter)
    {
        return getKernedNonspacedOffset(c);
    }

    return width_of_previous_letter + getSpaceSize();
}

// Returns the offset from the previous letter or beginining accounting for large spaces and indentation.
float FontManager::getOffsetWithLargeSpaces(char c)
{
    // Without any spacing, this is just the kerned offset from the previous letter.
    if (!space_after_previous_letter)
    {
        return getKernedNonspacedOffset(c);
    }

    return width_of_previous_letter + gap_accum;
}

float FontManager::getKernedNonspacedOffset(char c)
{
    string ligature = "";
    ligature.push_back(previous_character);
    ligature.push_back(c);

    float ligature_width = stringWidth(ligature);
    string character = "";
    character.push_back(c);
    float character_width = stringWidth(character);

    return ligature_width - character_width;
}

float FontManager::stringWidth(string & str)
{
    return stringWidth(str, current_font);
}

float FontManager::stringWidth(string & str, ofTrueTypeFont * font)
{
    // We need this to well form the string for length computation.
    str.push_back('\n');
    // str.length();// number of characters.
    
    // Width of string displayed on screen.
    // Remove the linebreak in preparation for adding more characters.
    size_t length = font -> stringWidth(str);
                                                  
    str.pop_back();

    return length;

    return 0;
}

// Finish by clearing away any unnecessary pointer state.
void FontManager::conclude()
{
    previous_letter = NULL;

    // Clear away contant state.
    this -> output = NULL;
    this -> letterManager = NULL;
}

float FontManager::getSpaceSizeFactor(int sizeIndex)
{
    return spaceSizes -> at(sizeIndex - 1);
}

float FontManager::getWordOffset(string & str, int font_size_index, bool font_italics)
{
    ofTrueTypeFont * font = getFont(font_size_index, font_italics);
    float offset = stringWidth(str, font);
    return offset;
}