#include "ofApp.h"

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

//--------------------------------------------------------------
void ofApp::setup(){

    float w = ofGetWidth();
    float h = ofGetHeight();

    // Initialize all of the sliders.
    gui.setup();


    gui.add(pool_y.setup("pool_y", 400, 0, h/2));
    gui.add(pool_y1.setup("pool_y1", 500, 0, h));
    gui.add(pool_y2.setup("pool_y2", 600, 0, h));
    gui.add(scroll_y_start.setup("scroll_y_start", 800, h / 2, h));
    gui.add(scroll_y_end.setup("scroll_y_end", 850, h / 2, h));
    gui.add(sentances_per_second.setup("sentances_per_second", .4555, .1, 1));
    gui.add(gravity.setup("gravity", 5.455, 5, 100)); // accelartion
    gui.add(terminal_velocity.setup("terminal_velocity", 82.65, 70, 300)); // max speed

    gui.add(meanderingDamping_letters.setup("curl_letters", .009, 0, .1)); // rate of which they wurrle. 0= linear
    gui.add(meanderingSpeed_letters.setup("m_speed_letters", 100, 0, 200));
    gui.add(meanderingDamping_words.setup("curl_words", .009, 0, .1));
    gui.add(meanderingSpeed_words.setup("m_speed_words", 100, 0, 200));
    gui.add(meanderingDamping_sentances.setup("curl_sentances", .009, 0, .1));
    gui.add(meanderingSpeed_sentances.setup("m_speed_sentances", 100, 0, 200));
    gui.add(wind_factor.setup("wind_factor", 0.01, 0, 1));
    gui.add(scrollSpeed.setup("scrollSpeed", 40, 20, 100));
    gui.add(poolTurnSpeed.setup("poolTurnSpeed", 0.015708, 0, PI/10));

    gui.add(combine_delay_letters.setup("c_delay_letters", 1, 0.01, 10)); // how long to wait until going to next zone
    gui.add(combine_delay_words.setup("c_delay_words", 1, 0.01, 10)); //how long do partial words wait before looking for neighbours
    gui.add(combine_delay_sentances.setup("c_delay_sentances", 1, 0.01, 10));
    gui.add(max_scroll_delay.setup("max_scroll_delay", 5, 1, 10));

 gui.loadFromFile("gui.xml");
    
    frame = 0;

    //string unicode_test = "Testing «ταБЬℓσ» : 1<2 & 4 + 1>3, now 20 % off!";

    /*
    int frame_rate = 30;
    this -> seconds_per_frame = 1.0 / frame_rate
    */


    loadFonts();
    loadGridAndObstacles();
    loadInputText();

    // NOTE: We make the time between scolls less than the input rate of sentances, because 
    // we don't want an unstable queueing scenario that grows unboundedly.
    letter_manager = new LetterManager(grid,
        &sentances_per_second,
        &pool_y,
        &pool_y1,
        &pool_y2,
        &scroll_y_start,
        &scroll_y_end,
        &meanderingDamping_letters,
        &meanderingSpeed_letters,
        &meanderingDamping_words,
        &meanderingSpeed_words,
        &meanderingDamping_sentances,
        &meanderingSpeed_sentances,
        &scrollSpeed,
        &wind_factor,
        &poolTurnSpeed,
        &gravity,
        &terminal_velocity,
        &combine_delay_letters,
        &combine_delay_words,
        &combine_delay_sentances,
        &max_scroll_delay
   );

}

void ofApp::loadFonts()
{
    ofBackground(54, 54, 54, 255);


    //old OF default is 96 - but this results in fonts looking larger than in other programs. 
    ofTrueTypeFont::setGlobalDpi(72);

    font.load("verdana.ttf", 14, true, true);
    font.setLineHeight(18.0f);
    font.setLetterSpacing(1.037);

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
    }
}

void ofApp::loadGridAndObstacles()
{
    // A grid spaced out over the window width in 100 by 100 equal locations.
    grid = new Grid(20, 20, ofGetWidth(), ofGetHeight());

    // We will focus on getting the other stages in working order first.
    ofPolyline p;
    for (int i = 0; i < 20; i++)
    {
        float angle = i*PI*2/20;
        float x = 200 + 50*cos(angle);
        float y = 200 + 50*sin(angle);
        p.addVertex(ofPoint(x, y));
    }

    // Create an obstacle that automatically adds itself to the grid.
    Obstacle * obj = new Obstacle(p, grid);
    obstacles.push_back(obj);

    ofPolyline p2;
    p2.addVertex(ofPoint(525, 200));
    p2.addVertex(ofPoint(550, 225));
    p2.addVertex(ofPoint(550, 260));
    p2.addVertex(ofPoint(500, 260));
    p2.addVertex(ofPoint(500, 225));
    obj = new Obstacle(p2, grid);
    obstacles.push_back(obj);

    ofPolyline p3;
    p3.addVertex(ofPoint(725, 210));
    p3.addVertex(ofPoint(750, 225));
    p3.addVertex(ofPoint(750, 260));
    p3.addVertex(ofPoint(700, 260));
    p3.addVertex(ofPoint(700, 225));
    obj = new Obstacle(p3, grid);
    obstacles.push_back(obj);
    //*/
    
    // Construct some bounds for the sides of the view screen.
    /*
    ofPolyline left;
    left.addVertex(1, -100);
    left.addVertex(1, ofGetWindowHeight());
    left.addVertex(-50, ofGetWindowHeight() / 2);

    ofPolyline right;
    right.addVertex(ofGetWindowWidth() - 1, -100);
    right.addVertex(ofGetWindowWidth() - 1, ofGetWindowHeight());
    right.addVertex(ofGetWindowWidth() + 50, ofGetWindowHeight()/2);

    Obstacle * o_left  = new Obstacle(left, grid);
    Obstacle * o_right = new Obstacle(right, grid);

    obstacles.push_back(o_left);
    obstacles.push_back(o_right);
    */
    
}

void ofApp::loadInputText()
{
    line_index = 0;

    ifstream fin; //declare a file stream  

    //some path, may be absolute or relative to bin/data
    string path = "./inputs/";
    ofDirectory dir(path);
    //only show png files
    dir.allowExt("txt");
    //populate the directory object
    dir.listDir();

    //go through and print out all the paths
    for (int i = 0; i < dir.size(); i++)
    {
        ofLogNotice(dir.getPath(i));
        fin.open(ofToDataPath(dir.getPath(i)).c_str()); //open your text file  

        string str; //declare a string for storage  
        while(getline(fin, str))
        {
            input.push_back(str); //push the string onto a vector of strings  
        }
        fin.close();
    }

}

size_t ofApp::stringLength(string & str)
{
    // We need this to well form the string for length computation.
    str.push_back('\n');
    // str.length();// number of characters.
    size_t length = font.stringWidth(str);// Width of string displayed on screen.
    // Remove the linebreak in preparation for adding more characters.
    str.pop_back();

    return length;
}

//--------------------------------------------------------------
void ofApp::update(){

    timing++;
    if (timing > 15)
    {
        timing = 0;
    }

    auto t1 = Clock::now();

    // Handle time and differences.
    float new_time = ofGetElapsedTimef(); // Time in seconds.
    float dt = new_time - time;

// Debugging works better with fixed time steps.
#ifdef FIXED_TIME
    dt = .1;
#endif // FIXED_TIME
    
    time = new_time;

    // Used to trigger the creation of new sentances.
    time_accum += dt;

    // Test String.
    if (time_accum > 1.0 / this -> sentances_per_second)
    {
        time_accum = 0;//-= this -> seconds_per_frame;


        // Remove me.
        if(bogus)
        {
            return;
        }
        
        // Cause this to generate only the first sentance for debugging purposes.
        //bogus = true;
    
        // Create a sentance of letters.
        //int letters_per_sentance = 5;
        Letter * previous_letter = NULL;
        //for(int i = 0; i < letters_per_sentance; i++)
        string line = input[line_index++];

        // Cycle back to the first file and first line if the text runs out.
        if (line_index >= input.size())
        {
            line_index = 0;
        }

        bool last_was_a_space = false;
        int len = line.length();
        char last_char = ' ';
        const int margin = 20;       

        float y0_height = margin * 10 / (this->sentances_per_second / .4);

        string accum;

        for(int char_index = 0; char_index < len; char_index++)
        {
            char c = line[char_index];
            if(c == '\n')
            {
                continue;
            }

            // Don't create animated invisible letters for spaces, but do record the gap.
            if(c == ' ')
            {
                last_was_a_space = true;
                continue;
            }

            // Blank space on the left and right sides of the screen where letters will not form.
            int x = margin + ofRandom(ofGetWidth() - margin*2);

            // Height scaled to create the illusion of a continuous stream of letters.
            int y = -20 - ofRandom(y0_height);

            // figure out the gap between this letter and the next one.
            int previous_to_this_distance = 0;
            
            // If we are past the first character, we compute the correct spacings of the letters.
            if(char_index > 0)
            {

                // length of the image rendered for this string on a screen.
                int len_old = stringLength(accum);

                // The trick is to put the space first,
                // since trailing spaces are ignored in the font getStringWidth function.
                if (last_was_a_space)
                
                {
                    accum.push_back(' ');
                }

                accum.push_back(c);
                int len_new = stringLength(accum);

                string singleton;
                singleton.push_back(c);
                int len_char = stringLength(singleton);
                singleton.pop_back();

                singleton.push_back(last_char);
                int len_last_char = stringLength(singleton);
             
                // TODO: Try end of new string - end of string with last character + len(last_char) - len(c)
                // We need to account for all of the spaces.
   
                // Distance with proper kerning.
                previous_to_this_distance = len_new - len_old - len_char + len_last_char;
            }
            else
            {
                accum.push_back(c);
            }

            
            /*
            int x = 200; // A test to see if the letters collide with the circle.
            int y = 200;
            */

            Letter * l = new Letter(
                x, y,
                previous_letter,
                c,
                &verdana14,
                letter_manager,
                previous_to_this_distance,
                last_was_a_space,
                line_index - 1); // last_was_a_space used to delliminate words.
            
            // Try 100 locations for a collision free.
            int tries = 10;
            while (grid -> detect_collision(l) && tries-- > 0)
            {
                int x = margin + ofRandom(ofGetWidth() - margin * 2);
                int y = -20 - ofRandom(y0_height);
                l -> setPosition(x, y);
            }

            letters.push_back(l);
            if(previous_letter != NULL)
            {
                previous_letter -> setRightLetter(l);
            }
            

            // Update trailing data for next loop.
            previous_letter = l;
            last_char = c;
            last_was_a_space = false;
        }
        
    }

    auto t2 = Clock::now();
    // Letter Creation.
    if(timing == 0)
    std::cout << "Letter Creation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        << " milliseconds" << std::endl;


    letter_manager -> update(dt);

    // We use a set to determine dead letters without duplication.
    vector<list<Letter *>::iterator> dead_letters;

    // First pass, update dynamics.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter) -> update(dt);
    }

    auto t3 = Clock::now();
    if (timing == 0)
        std::cout << "letter updates: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()
        << " milliseconds" << std::endl;

    // Second pass, update position.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter) -> move(dt);
    }

    auto t4 = Clock::now();
    if (timing == 0)
        std::cout << "letter moves: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count()
        << " milliseconds" << std::endl;

    // Second pass, check for and resolve collisions.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter) -> resolve_collision(dt);
    }

    auto t5 = Clock::now();
    if (timing == 0)
        std::cout << "Resolving Collisions. "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count()
        << " milliseconds" << std::endl;

    // Dead collection pass.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        if ((*iter) -> isDead())
        {
            dead_letters.push_back(iter);
            continue;
        }
    }

    // Remove all dead letters from the update list and deallocate them.
    for (auto iter = dead_letters.begin(); iter != dead_letters.end(); ++iter)
    {

        Letter * l = **iter;

        // remove letter from collisions.
        grid -> remove_from_collision_grid(l);

        // Remove the now defunct pointer from the letter list.
        letters.erase(*iter);

        delete l;



        // FIXME: Deposit this letter into a standbye repository of letters to save time allocating.
    }

    // End of update.
    auto t6 = Clock::now();
    if (timing == 0)
    std::cout << "Update: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5).count()
        << " milliseconds" << std::endl;
}

//--------------------------------------------------------------
void ofApp::draw()
{
    auto t1 = Clock::now();

    std::stringstream strm;
    strm << "fps: " << floor(ofGetFrameRate()) << ", n = " << letters.size();
    ofSetWindowTitle(strm.str());

    ofBackground(ofColor(255));

    // Hexadecimal color could be used form here maybe?
    // http://wiki.frankiezafe.org/index.php?title=Programmation:Hexadecimal_color_in_openframeworks

    int width = ofGetWidth();
    int height = ofGetHeight();

//    #ifdef DEBUG
    phase_1 = ofRectangle(0, 0, width, pool_y - 1);

    // 3 Pool Sections.
    phase_2A = ofRectangle(0, pool_y, width, pool_y1 - pool_y - 1);
    phase_2B = ofRectangle(0, pool_y1, width, pool_y2 - pool_y1 - 1);
    phase_2C = ofRectangle(0, pool_y2, width, scroll_y_start - pool_y2 - 1);

    phase_3A = ofRectangle(0, scroll_y_start, width, scroll_y_end - scroll_y_start);
    phase_3B = ofRectangle(0, scroll_y_end, width, height - scroll_y_start);

    ofSetColor(155, 255, 255); // blue.
    ofDrawRectangle(phase_1);

    ofSetColor(255, 127, 129); // red.
    ofDrawRectangle(phase_2A);

    ofSetColor(155, 255, 255); // blue.
    ofDrawRectangle(phase_2B);

    ofSetColor(255, 127, 129); // red.
    ofDrawRectangle(phase_2C);

    ofSetColor(155, 255, 255); // blue.
    ofDrawRectangle(phase_3A);

    ofSetColor(255, 127, 129); // red.
    ofDrawRectangle(phase_3B);

//    grid -> draw();
//    #endif

    // Draw all of the letters to the screen.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (**iter).draw();
    }

    //cout << letters.size() << endl;

    for (auto iter = obstacles.begin(); iter != obstacles.end(); iter++)
    {
        (**iter).draw();
    }

    /*
    ofSetColor(0);
    font.drawString("f", 0, 400);
    font.drawString("fa", 0, 440);
    font.drawString("fai", 0, 480);
    font.drawString("fair", 0, 520);
    font.drawString("faire", 0, 560);
    font.drawString("faires", 0, 600);
    font.drawString("fairest", 0, 640);
    font.drawString("fairest c", 0, 680);
    font.drawString("fairest cr", 0, 720);
    font.drawString("fairest cre", 0, 760);

    //font.drawString("Thy self thy foe, to thy sweet self too cruel:", 0, 400);
    */

    // End of draw.
    auto t2 = Clock::now();

    if(ofRandom(1) < .01)
    std::cout << "Draw: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        << " milliseconds" << std::endl;

    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
if(key == 'g')  gui.saveToFile("gui.xml");
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
