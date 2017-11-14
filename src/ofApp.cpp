#include "ofApp.h"

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

//--------------------------------------------------------------
void ofApp::setup(){

    float w = ofGetWidth();
    float h = ofGetHeight();

    //w = 1080;
    //h = 1920;

    //int bottom_y = 1920 + 50;// ofGetHeight() + 50;
    int bottom_y = ofGetHeight() + 50;

    // Initialize all of the sliders.
    gui.setup();

    int n = 9;
    gui.add(divide_y1.setup("divide_y1", h * 1 / n, 0, h));
    gui.add(divide_y2.setup("divide_y2", h * 2 / n, 0, h));
    /*
    gui.add(divide_y3.setup("divide_y3", h * 3 / n, 0, h));
    gui.add(divide_y4.setup("divide_y4", h * 4 / n, 0, h));
    gui.add(divide_y5.setup("divide_y5", h * 5 / n, 0, h));
    gui.add(divide_y6.setup("divide_y6", h * 6 / n, 0, h));
    gui.add(divide_y7.setup("divide_y7", h * 7 / n, 0, h));
    gui.add(divide_y8.setup("divide_y8", h * 8 / n, 0, h));
    */

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

    gui.add(dead_zone_height.setup("dead_zone_height", 5, 5, 100));
    gui.add(wordToSentancePoolDelay.setup("w_2_s_delay", 0, 1, 10));// Time in seconds that it a complete word will wait in the word pool.

    gui.add(bounce_energy.setup("bounce_energy", .25, 0, 1)); // What percentage of a letter's energy is conserved upon bouncing. Over 1 creates more energy.

    gui.add(show_grid.setup(false));

    gui.add(viscocity.setup("viscocity", 4.5, 0, 100)); // Percentage of the screen the velocity travels to each frame.

    gui.add(fontSize1.setup("fSize1", 14, 14, 100));
    gui.add(fontSize2.setup("fSize2", 20, 14, 100));
    gui.add(fontSize3.setup("fSize3", 32, 14, 100));
    gui.add(fontSize4.setup("fSize4", 48, 14, 100));
    gui.add(fontSize5.setup("fSize5", 64, 14, 100));
    /*
    gui.add(fontSize5.setup("fSize6", 64, 14, 100));
    gui.add(fontSize5.setup("fSize7", 64, 14, 100));
    gui.add(fontSize5.setup("fSize8", 64, 14, 100));
    gui.add(fontSize5.setup("fSize9", 64, 14 100));
    */

    gui.add(spaceSize1.setup("space1", 1, 1, 5));
    gui.add(spaceSize2.setup("space2", 1, 1, 5));
    gui.add(spaceSize3.setup("space3", 1, 1, 5));
    gui.add(spaceSize4.setup("space4", 1, 1, 5));
    gui.add(spaceSize5.setup("space5", 1, 1, 5));
    /*
    gui.add(spaceSize6.setup("space6", 1, 1, 50));
    gui.add(spaceSize7.setup("space7", 1, 1, 50));
    gui.add(spaceSize8.setup("space8", 1, 1, 50));
    gui.add(spaceSize9.setup("space9", 1, 1, 50));
    */

    gui.loadFromFile("gui.xml"); // Normal settings.
    //gui.loadFromFile("gui_stephan.xml"); // Stephan's settings.
    
    frame = 0;

    //string unicode_test = "Testing «ταБЬℓσ» : 1<2 & 4 + 1>3, now 20 % off!";

    /*
    int frame_rate = 30;
    this -> seconds_per_frame = 1.0 / frame_rate
    */

    fontSliders.push_back(&fontSize1);
    fontSliders.push_back(&fontSize2);
    fontSliders.push_back(&fontSize3);
    fontSliders.push_back(&fontSize4);
    fontSliders.push_back(&fontSize5);
    /*
    fontSliders.push_back(fontSize6);
    fontSliders.push_back(fontSize7);
    fontSliders.push_back(fontSize8);
    fontSliders.push_back(fontSize9);
    */

    spaceSliders.push_back(&spaceSize1);
    spaceSliders.push_back(&spaceSize2);
    spaceSliders.push_back(&spaceSize3);
    spaceSliders.push_back(&spaceSize4);
    spaceSliders.push_back(&spaceSize5);
    /*
    spaceSliders.push_back(&spaceSize6);
    spaceSliders.push_back(&spaceSize7);
    spaceSliders.push_back(&spaceSize8);
    spaceSliders.push_back(&spaceSize9);
    */

    init_fontAndSpaceSizeVectors(); // Inits sizes from gui.
    fontManager = new FontManager(&fontSizes, &spaceSizes);

    loadGridAndObstacles();
    loadInputText();

    y_dividers.push_back(&divide_y1);
    y_dividers.push_back(&divide_y2);
    /*
    y_dividers.push_back(&divide_y3);
    y_dividers.push_back(&divide_y4);
    y_dividers.push_back(&divide_y5);
    y_dividers.push_back(&divide_y6);
    y_dividers.push_back(&divide_y7);
    y_dividers.push_back(&divide_y8);
    */

    // NOTE: We make the time between scolls less than the input rate of sentances, because 
    // we don't want an unstable queueing scenario that grows unboundedly.
    letter_manager = new LetterManager(collision_detection_grid,
        &sentances_per_second,

        &y_dividers,

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
        &max_scroll_delay,
        &dead_zone_height,
        &wordToSentancePoolDelay,
        &bounce_energy,
        input.size(),
        fluid_dynamics_grid,
        bottom_y        
   );

}

void ofApp::init_fontAndSpaceSizeVectors()
{
    for (int i = 0; i < sizesInUse; i++)
    {
        fontSizes.push_back((int)*fontSliders[i]);
        spaceSizes.push_back(*spaceSliders[i]);
    }
}

void ofApp::refreshFontAndSpaceSizes()
{
    for (int i = 0; i < sizesInUse; i++)
    {
        fontSizes[i] = (int)*fontSliders[i];
        spaceSizes[i] = (float)*spaceSliders[i];
    }
}

void ofApp::loadGridAndObstacles()
{

    // A grid spaced out over the window width in 100 by 100 equal locations.
    int N = 10;
    collision_detection_grid = new Grid(N, N, ofGetWidth(), ofGetHeight());
    int F_rows = 40;
    int F_columns =  40;
    fluid_dynamics_grid = new Grid(F_rows, F_columns, ofGetWidth(), ofGetHeight());

    // We will focus on getting the other stages in working order first.
    /*
    ofPolyline p;
    for (int i = 0; i < 20; i++)
    {
        float angle = i*PI*2/20;
        float x = 200 + 50*cos(angle);
        float y = 100 + 50*sin(angle);
        p.addVertex(ofPoint(x, y));
    }

    // Create an obstacle that automatically adds itself to the grid.
    Obstacle * obj = new Obstacle(p, collision_detection_grid);
    obstacles.push_back(obj);

    ofPolyline p2;
    p2.addVertex(ofPoint(525, 200));
    p2.addVertex(ofPoint(550, 225));
    p2.addVertex(ofPoint(550, 260));
    p2.addVertex(ofPoint(500, 260));
    p2.addVertex(ofPoint(500, 225));
    obj = new Obstacle(p2, collision_detection_grid);
    obstacles.push_back(obj);

    ofPolyline p3;
    p3.addVertex(ofPoint(725, 210));
    p3.addVertex(ofPoint(750, 225));
    p3.addVertex(ofPoint(750, 260));
    p3.addVertex(ofPoint(700, 260));
    p3.addVertex(ofPoint(700, 225));
    obj = new Obstacle(p3, collision_detection_grid);
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
            // Only push the non empty strings.
            if(str.length() > 0)
                input.push_back(str); //push the string onto a vector of strings  
        }
        fin.close();
    }

}

//--------------------------------------------------------------
void ofApp::update(){

    synchFontSizes();

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

        spawnSentance();
        
    }

    auto t2 = Clock::now();
    // Letter Creation.
    /*
    if(timing == 0)
    std::cout << "Letter Creation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        << " milliseconds" << std::endl;
    */


    letter_manager -> update(dt);

    // We use a set to determine dead letters without duplication.
    vector<list<Letter *>::iterator> dead_letters;

    // First pass, update dynamics.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter) -> update(dt);
    }

    auto t3 = Clock::now();
    /*
    if (timing == 0)
        std::cout << "letter updates: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()
        << " milliseconds" << std::endl;
    */

    
    /*
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter) -> avoidOthers(dt);
    }*/

    // Second pass, update position.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter) -> move(dt);
    }

    auto t4 = Clock::now();
    /*
    if (timing == 0)
        std::cout << "letter moves: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count()
        << " milliseconds" << std::endl;
    */

    // Second pass, check for and resolve collisions.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter) -> resolve_collision(dt);
    }

    auto t5 = Clock::now();
    /*
    if (timing == 0)
        std::cout << "Resolving Collisions. "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count()
        << " milliseconds" << std::endl;
    */

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
        collision_detection_grid -> remove_from_collision_grid(l);

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

    fluid_dynamics_grid -> step_meander_velocities(dt, viscocity);

}

void ofApp::spawnSentance()
{
    // Remove me.
    if (bogus)
    {
        return;
    }

    // Cause this to generate only the first sentance for debugging purposes.
    //bogus = true;
    
    //for(int i = 0; i < letters_per_sentance; i++)
    string line = input[line_index];

    fontManager -> spawnSentance(line, &letters, letter_manager, line_index);

    line_index += 1;

    // Cycle back to the first file and first line if the text runs out.
    if (line_index >= input.size())
    {
        line_index = 0;
    }

}


//--------------------------------------------------------------
void ofApp::draw()
{

    //ofScale(.5, .5);

    auto t1 = Clock::now();

    std::stringstream strm;
    strm << "fps: " << floor(ofGetFrameRate()) << ", n = " << letters.size();
    ofSetWindowTitle(strm.str());

    ofBackground(ofColor(255));

    // Hexadecimal color could be used form here maybe?
    // http://wiki.frankiezafe.org/index.php?title=Programmation:Hexadecimal_color_in_openframeworks

    int width = ofGetWidth();
    int height = ofGetHeight();

    /*
    // #ifdef DEBUG
    phase_1A = ofRectangle(0, 0, width, divide_y1 - 1);
    phase_1B = ofRectangle(0, divide_y1, width, divide_y2 - divide_y1 - 1);

    // 3 Pool Sections.
    phase_2A = ofRectangle(0, divide_y2, width, divide_y3 - divide_y2 - 1);
    phase_2B = ofRectangle(0, divide_y3, width, divide_y4 - divide_y3 - 1);
    phase_2C = ofRectangle(0, divide_y4, width, divide_y5 - divide_y4 - 1);

    phase_3A = ofRectangle(0, divide_y5, width, divide_y6 - divide_y5 - 1);
    phase_3B = ofRectangle(0, divide_y6, width, divide_y7 - divide_y6 - 1);
    phase_3C = ofRectangle(0, divide_y7, width, divide_y8 - divide_y7 - 1);
    

    ofSetColor(155, 255, 255); // blue.
    ofDrawRectangle(phase_1A);

    ofSetColor(255, 127, 129); // red.
    ofDrawRectangle(phase_1B);
    
    ofSetColor(155, 255, 255); // blue.
    ofDrawRectangle(phase_2A);

    ofSetColor(255, 127, 129); // red.
    ofDrawRectangle(phase_2B);

    ofSetColor(155, 255, 255); // blue.
    ofDrawRectangle(phase_2C);

    ofSetColor(255, 127, 129); // red.
    ofDrawRectangle(phase_3A);

    ofSetColor(155, 255, 255); // blue.
    ofDrawRectangle(phase_3B);

    ofSetColor(255, 127, 129); // red.
    ofDrawRectangle(phase_3C);
    */
    ofSetColor(0, 0, 0, 255); // Black divider lines.
    ofDrawLine(0, divide_y1, width, divide_y1);
    ofDrawLine(0, divide_y2, width, divide_y2);

    // Draw all of the letters to the screen.
    ofSetColor(0, 0, 0, 255); // Letters are black.
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

    /*
    if(ofRandom(1) < .01)
    std::cout << "Draw: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        << " milliseconds" << std::endl;
    */

    
    // I'm not showing this grid, because it is only interesting for optimization purposes.
    //collision_detection_grid -> draw();
    //if(show_grid)
    //  fluid_dynamics_grid -> draw();

    //ofScale(2, 2);

    gui.draw();

    /*
    std::string str = "üöéñ and so on";
     // Try using the following for drawing a Unicode string.
    auto substr = ofUTF8Substring(str, start, len);
    auto text32 = ofx::TextConverter::toUTF32(str);
    string letter = ofx::TextConverter::toUTF8(text32[0]);
    std::cout << ofx::TextConverter::toUTF8(text32[0]) << " letter " << letter << endl;
    ofSetColor(255);
    font.drawString(letter, 0, 0);
    */
    

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

void ofApp::synchFontSizes()
{
    bool changed = false;
    for (int i = 0; i < sizesInUse; i++)
    {
        if (fontSizes[i] != (int)*fontSliders[i])
        {
            changed = true;
            fontSizes[i] = (int)*fontSliders[i];
        }
        if (spaceSizes[i] != *spaceSliders[i])
        {
            changed = true;
            spaceSizes[i] = *spaceSliders[i];
        }
    }

    if (changed)
    {
        fontManager -> reloadFonts();
    }
}