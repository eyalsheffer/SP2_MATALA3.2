#ifndef Governor_HPP
#define Governor_HPP

#include <iostream>
#include "Player.hpp"

class Governor : public Player{
    private:

    public:
    Governor (const std::string& name):Player(name){};
    void tax() override;

};
#endif