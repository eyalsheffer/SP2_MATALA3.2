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

// void Game::add_player(Player* p){
//     _players.push_back(p);
// }

void Game::make_action(){
    _turn++;
    _turn %= _players.size();
}