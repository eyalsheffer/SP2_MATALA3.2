#include "Game.hpp"
Game::Game(){
    _turn = 0;
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
