#ifndef Merchant_HPP
#define Merchant_HPP

#include <iostream>
#include "Player.hpp"

class Merchant : public Player{
    private:

    public:
    Merchant (const std::string& name):Player(name){};
    void uniqe() override;
};
#endif