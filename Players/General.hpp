#ifndef General_HPP
#define General_HPP

#include <iostream>
#include "Player.hpp"

class General : public Player{
    private:

    public:
    General (Game& game,const std::string& name):Player(game ,name){};
    void uniqe(Player& action,Player& target) override;
    

};
#endif