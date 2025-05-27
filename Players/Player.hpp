#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>

class Player{
    protected:
    std::string _name;
    //std::string role;
    int _coins;
    bool _is_sanction;
    bool _is_active;
    bool _can_arrest;
    //Player* _last_arrested;

    public:
    Player(std::string name);
    Player(Player& other);
    virtual ~Player();

    Player& operator=(Player& other);

    std::string get_name();
    int get_coins();
    bool get_isSanction();
    bool get_isActive();
    bool get_canArrest();

    void set_name(const std::string& name);
    void set_coins(const int coins);
    void set_isActive(const bool isActive);
    void set_isSanction(bool sanctioned);
    void set_canArrest(bool sanctioned);

    void gather();
    virtual void tax();
    void bribe();
    void arrest(Player& other);
    void sanction(Player& other);
    void coup(Player& other);

};
#endif