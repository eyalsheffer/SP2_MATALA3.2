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
    _players.clear();
    // clear_players();
}
Game& Game::instance(){
    static Game instance;
    return instance;
}
/**
 * @brief Deletes all players, clears the player list, resets turn and bribe state.
 */
void Game::clear_players() {
    for (Player* p : _players) {
        delete p;
    }
    _players.clear();
    _turn = 0;
    _is_bribe = false;
}
std::vector<Player*>& Game::get_players(){
    return _players;
}
int Game::get_turn(){
   
    return _turn;
}
bool Game::get_isBribe() const {
    return _is_bribe;
}
void Game::set_turn(const int turn){
     _turn  = turn;
}
void Game::set_isBribe(const bool isBribe){
     _is_bribe  = isBribe;
}
/**
 * @brief Returns the name of the single active player (the winner).
 * @return Winner's name.
 * @throws std::runtime_error if no single winner yet.
 */
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

bool Game::is_current( Player& p) const{
    if(_players.empty()){
        throw std::runtime_error("No players");
    }
    return _players[_turn]->get_name() ==  p.get_name();
    
}
/**
 * @brief Validates if a player can make a move; checks active status, turn order, and forced coup.
 * @param p Player to validate.
 * @throws std::runtime_error if invalid.
 */
void Game::check_valid_move( Player& p) const{
    if(!p.get_isActive()){
        throw std::runtime_error("players not active");
    }
    if(!is_current(p)){
        throw std::runtime_error("player is out of turn");
    }
    if(p.get_coins() > 9 && p.get_lastAction() != GameAction::COUP){
        throw std::runtime_error("player has 10 coins, must coup");
    }

}  
/**
 * @brief Checks if the player can arrest any other active players with coins.
 * @param p Player checking arrest options.
 * @return true if arrest options exist, false otherwise.
 */
bool Game::have_arrests_options(Player& p) const{
    if (!p.get_canArrest()) {
        return false;
    }
    for (int i = 0; i < static_cast<int>(_players.size()); ++i) {
        Player* player = _players[i];
        if (i != _turn && player->get_isActive() && !player->get_lastArrested()) {
            int coins = player->get_coins();
            if (coins > 0 || (dynamic_cast<Merchant*>(player) && coins > 1)) {
                return true;
            }
        }
    }
    return false;
}
/**
 * @brief Determines if a player can take an action based on sanction status and coins.
 * @param p Player to check.
 * @return true if action allowed, false otherwise.
 */
bool Game::can_take_action(Player& p) const{
    if(!p.get_isSanction()){
        return true;
    }
    if(p.get_coins() > 2){
        return true;
    }
    if(dynamic_cast<Baron*>(&p) && p.get_coins() > 2){
        return true;
    }
    return this->have_arrests_options(p);
}
/**
 * @brief Advances to the next active player’s turn, handles sanctions, arrests, bribes, and Merchant bonus.
 * @throws std::runtime_error if no players.
 */
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
    if (_is_bribe) {
        _is_bribe = false;
        return;  
    }   

    do {
        _turn = (_turn + 1) % static_cast<int>(_players.size());
    } while (!_players[_turn]->get_isActive());

    if(dynamic_cast<Merchant*>(_players[_turn]) && _players[_turn]->get_coins() > 2){
        _players[_turn]->set_coins(_players[_turn]->get_coins() + 1);
    }
    if(!can_take_action(*_players[_turn])){
        this->turn_manager();
    }
    
}
