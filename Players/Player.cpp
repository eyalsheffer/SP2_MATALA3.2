#include "Player.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Baron.hpp"
#include <iostream>
    Player::Player(std::string name){
        _coins = 0;
        _name = name;
        _is_sanction = false;
        //_last_arrested = nullptr;
        _is_active = true;
        _can_arrest = true;
    }
    Player::Player(Player& other){
        _coins = other._coins;
        _name = other._name;
        _is_sanction = other._is_sanction;
        _is_active = other._is_active;
        _can_arrest = other._can_arrest;
        std::cout << "Player '" << _name << "' created" << std::endl;
        //_last_arrested = other._last_arrested;
        //_last_arrested = other._last_arrested ? new Player(*other._last_arrested) : nullptr;
    }
    Player::~Player(){
         std::cout << "Player '" << _name << "' destroyed" << std::endl;
         //delete _last_arrested ;
    }

    Player& Player::operator=(Player& other) {
        if (this != &other) {
            _coins = other._coins;
            _name = other._name;
            _is_sanction = other._is_sanction;
            _is_active = other._is_active;
            _can_arrest = other._can_arrest;
            //delete _last_arrested;
            //_last_arrested = other._last_arrested ? new Player(*other._last_arrested) : nullptr;
        }
        return *this;
    }
    
    std::string Player::get_name(){
        return _name;
    }
    int Player::get_coins(){
        return _coins;
    }
    bool Player::get_isSanction(){
        return _is_sanction;
    }
    bool Player::get_isActive(){
        return _is_active;
    }
    bool Player::get_canArrest(){
        return _can_arrest;
    }

    void Player::set_name(const std::string& name){
        _name = name;
    }
    void Player::set_coins(const int coins){
        _coins = coins;
    }
     void Player::set_isActive(const bool isActive){
        _is_active = isActive;
    }
     void Player::set_isSanction(const bool isSanction){
        _is_sanction = isSanction;
    }
     void Player::set_canArrest(const bool canArrest){
        _can_arrest = canArrest;
    }

    void Player::gather(){
        if(!_is_sanction){
            _coins++;
        }
    }
    void Player::tax(){
        if(!_is_sanction){
            _coins+= 2;
        }
    }

    void Player::bribe(){
        if(_coins > 3){
            _coins-=4;
        }

    }
    void Player::arrest(Player& other){
        _coins++;
        other._coins--;   
        //other._last_arrested = this;
        if(dynamic_cast<General*>(&other)){
            _coins--;
            other._coins++;
        }
        if(dynamic_cast<Merchant*>(&other)){
            _coins--;
            other._coins--;
        }
    }
    void Player::sanction(Player& other){
        if(_coins > 2){
            _coins -=3;
            if(dynamic_cast<Judge*>(&other)){
                _coins--;
            }
            else if(dynamic_cast<Baron*>(&other)){
                other._coins++;
            }
            other._is_sanction = true;
            
        }
    }
    void Player::coup(Player& other){
        if(_coins > 6){
            _coins-=7;
            other._is_active = false;
        }
    }
