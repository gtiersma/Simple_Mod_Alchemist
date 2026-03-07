#include <Game.h>

Game::Game(u64 titleId_, std::string path_): titleId(titleId_), path(path_) {
    // Set the name after the construction.
    // By default, the name is just the folder name if never changed.
    name = path_;
}