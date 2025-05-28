#include "Judge.hpp"
#include <stdexcept>
#include "../Game.hpp"

void Judge::uniqe(Player& other){
      if(!_is_active){
        throw std::runtime_error("Player is not active!");
    }
    if(other.get_lastAction() != GameAction::BRIBE){
        throw std::runtime_error("Can only block bribe!");
    }
    _game.set_isBribe(false);
    _game.turn_manager();
}