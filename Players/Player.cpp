#include "Player.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Baron.hpp"
#include "../Game.hpp"
#include <iostream>
#include <stdexcept>
    Player::Player(Game& game,const std::string& name)
           : _game(game), _name(name), _coins(0),
      _is_sanction(false), _is_active(true),
      _can_arrest(true), _last_arrested(false)
    {}
    
    // Player::Player(Player& other)
    //     : _game(other._game),       // must be in initializer list
    //     _name(other._name),
    //     _coins(other._coins),
    //     _is_sanction(other._is_sanction),
    //     _is_active(other._is_active),
    //     _can_arrest(other._can_arrest),
    //     _last_arrested(other._last_arrested),
    //     _last_action(other._last_action)
    // {
    //     std::cout << "Player '" << _name << "' created" << std::endl;
    // }
            
    // Player::~Player(){
    //      std::cout << "Player '" << _name << "' destroyed" << std::endl;
    // }

    // Player& Player::operator=(const Player& other) {
    //     if (this != &other) {
    //         _coins = other._coins;
    //         _name = other._name;
    //         _game = other._game;
    //         _is_sanction = other._is_sanction;
    //         _is_active = other._is_active;
    //         _can_arrest = other._can_arrest;
    //         _last_arrested = other._last_arrested;
    //         _last_action = other._last_action;
    //     }
    //     return *this;
    // }
    
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
    bool Player::get_lastArrested(){
        return _last_arrested;
    }
    GameAction Player::get_lastAction(){
        return _last_action;
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
    void Player::set_lastArrested(const bool lastArrest){
        _last_arrested = lastArrest;
    }
    void Player::set_lastAction(GameAction act){
        _last_action = act;
    }


    void Player::gather(){
        _game.check_valid_move(*this);
        if(_is_sanction){
            throw std::runtime_error("Player is sanctioned");
        }
        _coins++;
        _last_action = GameAction::GATHER;
        _game.turn_manager();
    }
    void Player::tax(){
        _game.check_valid_move(*this);
         if(_is_sanction){
            throw std::runtime_error("Player is sanctioned");
        }
        _coins+= 2;
        _last_action = GameAction::TAX;
        _game.turn_manager();
    }

    void Player::bribe(){
        _game.check_valid_move(*this);
        if(_coins < 4){
            throw std::runtime_error("Not enough money!");
            
        }
        _coins-=4;
        _last_action = GameAction::BRIBE;
        _game.set_isBribe(false);
        _game.turn_manager();
    }
    void Player::arrest(Player& other){

        _game.check_valid_move(*this);
        if(other.get_coins() < 1){
            throw std::runtime_error("target player has no money");
        }
        if(dynamic_cast<Merchant*>(&other) && other.get_coins() < 2){
            throw std::runtime_error("Merchant doesnt have 2 coins");
        }
        _coins++;
        other._coins--;   
        if(dynamic_cast<General*>(&other)){
            _coins--;
            other._coins++;
        }
        if(dynamic_cast<Merchant*>(&other)){
            _coins--;
            other._coins--;
        }
        _last_action = GameAction::ARREST;
        _game.turn_manager();
    }
    void Player::sanction(Player& other){

        _game.check_valid_move(*this);
        if(_coins <= 2){
             throw std::runtime_error("Not enough money!");  
        }
        _coins -=3;
        if(dynamic_cast<Judge*>(&other)){
            _coins--;
        }
        else if(dynamic_cast<Baron*>(&other)){
            other._coins++;
        }
        other._is_sanction = true;
        _last_action = GameAction::SANCTION;
        _game.turn_manager();
    }
    void Player::coup(Player& other){
        _game.check_valid_move(*this);
        if(_coins <= 6){
            throw std::runtime_error("Not enough money!"); 
        }
        _coins-=7;
        other._is_active = false;
          // Check for a winner after the coup
    try {
        std::string win = _game.winner();
        std::cout << "Game over! Winner: " << win << std::endl;
        return;  
    } catch (const std::runtime_error& e) {
        // No winner yet — continue game
    }
        _game.turn_manager();
    }
