#include "Merchant.hpp"
#include <stdexcept>

void Merchant::uniqe(){
    if(_coins > 2){
        _coins++;
    }
}