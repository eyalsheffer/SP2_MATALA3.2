#ifndef BARON_HPP
#define BARON_HPP

#include <iostream>
#include "Player.hpp"

class Baron : public Player{
    private:

    public:
    Baron (Game& game,const std::string& name):Player(game, name){};
    void uniqe() override;

};
#endif