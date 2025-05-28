#include "Judge.hpp"
#include <stdexcept>

void Judge::uniqe(Player& other){
      if(!_is_active){
        throw std::runtime_error("Player is not active!");
    }
    if(other.get_lastAction() != GameAction::BRIBE){
        throw std::runtime_error("Can only block bribe!");
    }
    other.set_coins(other.get_coins() - 3);
    other.set_coins(other.get_coins() - 2);
}