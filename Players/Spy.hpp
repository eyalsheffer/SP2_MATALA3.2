#ifndef SPY_HPP
#define SPY_HPP

#include <iostream>
#include "Player.hpp"

class Spy : public Player{
    private:

    public:
    Spy (const std::string& name):Player(name){};
    void uniqe(Player& other) override

};
#endif