#include "Spy.hpp"
#include "Player.hpp"

void Spy::reveal(Player& other){
    other.set_canArrest(false);

}