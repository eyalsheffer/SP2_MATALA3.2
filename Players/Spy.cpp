#include "Spy.hpp"
#include "Player.hpp"

void Spy::uniqe(Player& other){
    if(!_is_active){
        throw std::runtime_error("Player is not active!");
    }
    if(!other.get_isActive){
        throw std::runtime_error("Target player is not active");
    }
    other.set_canArrest = false;

}