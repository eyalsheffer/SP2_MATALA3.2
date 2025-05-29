#include "Judge.hpp"
#include <stdexcept>
#include "../Game.hpp"
/**
 * @brief Blocks a bribe action and advances the turn.
 * @param other Player performing the bribe.
 * @throws std::runtime_error if inactive or last action not BRIBE.
 */
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