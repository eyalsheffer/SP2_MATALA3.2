#ifndef Governor_HPP
#define Governor_HPP

#include <iostream>
#include "Player.hpp"

class Governor : public Player{
    private:

    public:
    Governor (Game& game,const std::string& name):Player(game ,name){}
    void uniqe(Player& other) override;

};
#endif