#include <Game.h>

Game::Game(u64 titleId_, std::string folderName_): titleId(titleId_), folderName(folderName_) {
    // Set the name after the construction.
    // By default, the name is just the folder name if never changed.
    name = folderName_;
}