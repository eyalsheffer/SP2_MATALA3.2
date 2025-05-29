#include "Merchant.hpp"
#include <stdexcept>
/**
 * @brief Adds 1 coin if player has more than 2 coins.
 */
void Merchant::uniqe(){
    if(_coins > 2){
        _coins++;
    }
}