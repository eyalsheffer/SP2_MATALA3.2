#include "General.hpp"
#include <stdexcept>

void General::uniqe(Player& other){
    if(!_is_active){
        throw std::runtime_error("Player is not active!");
    }
    if(_coins < 5){
        throw std::runtime_error("Doesn't have 5 coins");
    }
    if(other.get_lastAction() != GameAction::COUP){
        throw std::runtime_error("Can only block coup!");
    }
    _coins -= 5;
    other.set_isActive(true);
}