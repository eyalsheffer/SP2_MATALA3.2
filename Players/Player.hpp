#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>
#include "../GameAction.hpp"
//#include "../Game.hpp"
class Game;

class Player{
    protected:
    Game& _game;
    std::string _name;
    //std::string role;
    int _coins;
    bool _is_sanction;
    bool _is_active;
    bool _can_arrest;
    bool _last_arrested;
    GameAction _last_action = GameAction::NONE;
    //Player* _last_arrested;

    public:
    Player(Game& game,const std::string& name);
    Player(Player& other) = delete;
    virtual ~Player() = default;

    Player& operator=(const Player& other) = delete;

    std::string get_name();
    int get_coins();
    bool get_isSanction();
    bool get_isActive();
    bool get_canArrest();
    bool get_lastArrested();
    GameAction get_lastAction();

    void set_name(const std::string& name);
    void set_coins(const int coins);
    void set_isActive(const bool isActive);
    void set_isSanction(const bool sanctioned);
    void set_canArrest(const bool canArrest);
    void set_lastArrested(const bool lastArrest);
    void set_lastAction(GameAction act);


    void gather();
    virtual void tax();
    void bribe();
    void arrest(Player& other);
    void sanction(Player& other);
    void coup(Player& other);
    virtual void uniqe(){}
    virtual void uniqe(Player& other){(void)other;}
    virtual void uniqe(Player& action, Player& target){(void)action; (void)target;}

};
#endif