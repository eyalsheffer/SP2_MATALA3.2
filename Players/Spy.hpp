#ifndef SPY_HPP
#define SPY_HPP

#include <iostream>
#include "Player.hpp"

class Spy : public Player{
    private:

    public:
    Spy (Game& game,const std::string& name):Player(game ,name){}
    void uniqe(Player& other) override;

};
#endif