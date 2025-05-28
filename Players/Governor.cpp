#include "Governor.hpp"
#include <stdexcept>

void Governor::uniqe(Player& other){
    if(!_is_active){
        throw std::runtime_error("Player is not active!");
    }
    if(other.get_lastAction() != GameAction::TAX){
        throw std::runtime_error("Can only block tax!");
    }
    if(dynamic_cast<Governor*>(&other)){
        other.set_coins(other.get_coins() - 3);
    }
    else{
        other.set_coins(other.get_coins() - 2);
    }
    
}