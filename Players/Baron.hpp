#ifndef BARON_HPP
#define BARON_HPP

#include <iostream>
#include "Player.hpp"

class Baron : public Player{
    private:

    public:
    Baron (const std::string& name):Player(name){};
    void invest() ;

};
#endif