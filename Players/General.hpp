#ifndef General_HPP
#define General_HPP

#include <iostream>
#include "Player.hpp"

class General : public Player{
    private:

    public:
    General (const std::string& name):Player(name){};
    void uniqe(Player& other) override;
    

};
#endif