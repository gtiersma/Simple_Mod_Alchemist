#include <Game.h>

#include <StateAlchemist/constants.h>


Game::Game(u64 titleId_, std::string name_): titleId(titleId_), name(name_) {
    path = ALCHEMIST_PATH + "/" + name_;
}