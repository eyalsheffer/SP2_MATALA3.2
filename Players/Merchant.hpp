#ifndef Merchant_HPP
#define Merchant_HPP

#include <iostream>
#include "Player.hpp"

class Merchant : public Player{
    private:

    public:
    Merchant (Game& game,const std::string& name):Player(game ,name){}
    void uniqe() override;
};
#endif