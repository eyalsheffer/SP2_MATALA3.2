#ifndef JUDGE_HPP
#define JUDGE_HPP

#include <iostream>
#include "Player.hpp"

class Judge : public Player{
    private:

    public:
    Judge (const std::string& name):Player(name){}
    void uniqe(Player& other) override;
};
#endif;