#include "Baron.hpp"
#include "stdexcept"



void Baron::uniqe(){
    if(!_is_active){
        throw std::runtime_error("Player is not active!");
    }
    if(_coins < 3){
        throw std::runtime_error("Doesn't have 3 coins");
    }
    if(_coins > 9 ){
        throw std::runtime_error("Player has 10 coins, must coup");
    }
    else{
        _coins += 3;
    }
}