#include "Game.hpp"
#include "Players/Baron.hpp"
#include "Players/Merchant.hpp"
Game::Game(){
    _turn = 0;
    _is_bribe = false;
}
Game::~Game(){
    for(Player* p : _players){
          delete p;
    }
}
Game& Game::instance(){
    static Game instance;
    return instance;
}

std::vector<Player*>& Game::get_players(){
    return _players;
}
int Game::get_turn(){
    return _turn;
}
void Game::set_turn(const int turn){
     _turn  = turn;
}

std::string Game::winner(){
    std::string won;
    int count  = 0;
    for(Player* p : _players){
        if(p->get_isActive()){
            count++;
            won = p->get_name();
        }
    }
    if(count == 1){
        return won;
    }
    throw std::runtime_error("No winner yet or multiple players still active");
}

bool Game::is_current(const Player& p) const{
    if(_players.empty()){
        throw std::runtime_error("No players");
    }
    return _players[_turn] == p;
    
}

void Game::check_valid_move(const Player& p) const{
    if(!p.get_isActive()){
        throw std::runtime_error("players not active");
    }
    if(!is_current(p)){
        throw std::runtime_error("player is out of turn");
    }

}  

bool Game::can_take_action(const Player& p) const{
    bool res = false;
    if(!p.get_isSanction){
        res = true;
    }
    if(p.get_coins > 2){
        res = true;
    }
    if(dynamic_cast<Baron*>(p) && p.get_coins > 2){
        res true;
    }
    if(p._can_arrest){
        for(Player* player : _players){
            if(i != _turn && player->get_isActive() && !player->get_lastArrested()){
                if(player->get_coins() > 0 || (dynamic_cast<Merchant*>(player) && player->get_coins() > 1))
                res = true;
                break;
            }
        }
    }
    return res;
}
void Game::turn_manager(){
    if(_players.empty()){
        throw std::runtime_error("No players");
    }
    Player* currentPlayer = _players[_turn];
    if(currentPlayer->get_isSanction()){
        currentPlayer->set_isSanction(false);
    }
    if(!currentPlayer->get_canArrest()){
        currentPlayer->set_canArrest(true);
    }
    while (!_players[_turn]->get_isActive)
    {
        if(_is_bribe){
            _is_bribe = false;
            break;
        }
        _turn = (turn + 1) % (static_cast<int>(_players.size));
    }
    if(dynamic_cast<Merchant*>(_players[_turn]) && _players[_turn]->get_coins() > 2){
        _players[_turn]->set_coins(_players[_turn]->get_coins() + 1);
    }
    if(!can_take_action(_players[_turn]))[
        turn_manager();
    ]
    
}
