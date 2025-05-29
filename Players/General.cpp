#include "General.hpp"
#include <stdexcept>
/**
 * @brief Blocks a coup if active, has 5+ coins, and action is coup; else throws.
 * @param action Player performing the coup.
 * @param target Player targeted by the coup.
 */
void General::uniqe(Player& action,Player& target ){
    if(!_is_active && target.get_name() != _name){
        throw std::runtime_error("Player is not active!");
    }
    if(_coins < 5){
        throw std::runtime_error("Doesn't have 5 coins");
    }
    if(action.get_lastAction() != GameAction::COUP){
        throw std::runtime_error("Can only block coup!");
    }
    action.set_lastAction(GameAction::NONE);
    _coins -= 5;
    target.set_isActive(true);
}