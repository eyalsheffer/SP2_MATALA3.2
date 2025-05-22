#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>

class Player{
    protected:
    std::string _name;
    //std::string role;
    int _coins;
    bool _is_sanction;
    Player* _last_arrested;

    public:
    Player(std::string name);
    Player(Player& other);
    virtual ~Player();

    Player& operator=(Player& other);

    void gather();
    virtual void tax();
    void bribe();
    void arrest(Player& other);
    void sanction(Player& other);
    void coup(Player& other);

};
#endif