#include "Player.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include <iostream>
    Player::Player(std::string name){
        _coins = 0;
        _name = name;
        _is_sanction = false;
        _last_arrested = nullptr;
    }
    Player::Player(Player& other){
        _coins = other._coins;
        _name = other._name;
        _is_sanction = other._is_sanction;
        //_last_arrested = other._last_arrested;
        _last_arrested = other._last_arrested ? new Player(*other._last_arrested) : nullptr;
    }
    Player::~Player(){
         delete _last_arrested ;
    }

    Player& Player::operator=(Player& other) {
        if (this != &other) {
            _coins = other._coins;
            _name = other._name;
            _is_sanction = other._is_sanction;
            //delete _last_arrested;
            _last_arrested = other._last_arrested ? new Player(*other._last_arrested) : nullptr;
        }
        return *this;
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
        other._last_arrested = this;
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
            else{
                other._is_sanction = true;
            }
        }
    }
    void Player::coup(Player& other){
        if(_coins > 6){
            other.~Player();//////
        }
    }
