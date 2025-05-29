#include "Player.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Baron.hpp"
#include "Governor.hpp"
#include "../Game.hpp"
#include <iostream>
#include <stdexcept>
    Player::Player(Game& game,const std::string& name)
           : _game(game), _name(name), _coins(0),
      _is_sanction(false), _is_active(true),
      _can_arrest(true), _last_arrested(false)
    {}

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

/**
 * @brief Performs gather action: adds 1 coin if valid and not sanctioned, then advances turn.
 * @throws std::runtime_error if move invalid or player sanctioned.
 */
    void Player::gather(){
        _game.check_valid_move(*this);
        if(_is_sanction){
            throw std::runtime_error("Player is sanctioned");
        }
        _coins++;
        _last_action = GameAction::GATHER;
        _game.turn_manager();
    }
    /**
 * @brief Performs tax action: adds coins (3 if Governor, else 2) if valid and not sanctioned, then advances turn.
 * @throws std::runtime_error if move invalid or player sanctioned.
 */
    void Player::tax(){
        _game.check_valid_move(*this);
         if(_is_sanction){
            throw std::runtime_error("Player is sanctioned");
        }
        if(dynamic_cast<Governor*>(this)){
            _coins+= 3;
        }
        else{
            _coins+= 2;
        }

        _last_action = GameAction::TAX;
        _game.turn_manager();
    }
/**
 * @brief Performs bribe action by spending 4 coins and setting bribe state.
 * @throws std::runtime_error if move invalid or insufficient coins.
 */
    void Player::bribe(){
        _game.check_valid_move(*this);
        if(_coins < 4){
            throw std::runtime_error("Not enough money!");
            
        }
        _coins-=4;
        _last_action = GameAction::BRIBE;
        _game.set_isBribe(true);
        //_game.turn_manager();
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
    /**
 * @brief Performs arrest on another player, transferring coins with role-specific rules.
 * @param other Player to arrest.
 * @throws std::runtime_error if move invalid or target lacks required coins.
 */
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
    /**
 * @brief Performs a coup on another player by spending 7 coins, deactivates target, and handles General block and win check.
 * @param other Player to coup.
 * @throws std::runtime_error if move invalid or insufficient coins.
 */
    void Player::coup(Player& other){
        _last_action = GameAction::COUP;
        _game.check_valid_move(*this);
        if(_coins <= 6){
            _last_action = GameAction::NONE;
            throw std::runtime_error("Not enough money!"); 
        }
        _coins-=7;
        other._is_active = false;
          
        if(dynamic_cast<General*>(&other)){
             try {
                other.uniqe(*this, other);
            } catch (const std::exception& e) {
                std::cerr << "General failed to block: " << e.what() << std::endl;
    }
        }
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
