#include "Governor.hpp"
#include <stdexcept>
/**
 * @brief Blocks tax action; reduces coins by 3 if Governor, else by 2.
 * @param other Player whose tax action is blocked.
 * @throws std::runtime_error if inactive or last action not TAX.
 */
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