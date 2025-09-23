/*
 * =====================================================================================
 *
 *       Filename:  music.cpp
 *
 *    Description:  An example program demonstrating the use of music.h library.
 *
 *        Version:  1.0
 *        Created:  2023/10/27 19:20:00
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  NorMir
 *
 * =====================================================================================
 */
#include "music.h"
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>

// A simple function to demonstrate BGM playback
void some_other_functions(){
	puts("Playing BGM. Press any key to exit.");
	getch();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <music_file.txt>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    try {
        // --- Example 1: Direct Playback ---
        std::cout << "--- Direct Playback Example ---" << std::endl;
        std::cout << "Loading music from " << filename << " and playing once." << std::endl;

        MusicPlayer player;
        MusicList music;
        music.readFile(filename);
        player.playList(music);

        std::cout << "Playback finished." << std::endl << std::endl;

        // --- Example 2: Background Music (BGM) ---
        std::cout << "--- Background Music (BGM) Example ---" << std::endl;
        std::cout << "Loading music from " << filename << " to play in the background." << std::endl;

        BGM bgm(filename);
        bgm.play();

        // The main program can continue to do other things
        // while the music plays in the background.
        some_other_functions();

        // Stop the background music gracefully
        std::cout << "Stopping BGM..." << std::endl;
        bgm.stop();
        std::cout << "BGM stopped." << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
