#ifndef Game_HPP
#define Game_HPP

#include <iostream>
#include <vector>
#include "Players/Player.hpp"
class Game{
    private:
        std::vector<Player*> _players;
        int _turn;

        Game();
        ~Game();

        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;
    public:
        static Game& instance();

        std::vector<Player*>& get_players();
        int get_turn();
        //void add_player(Player* p);
        void make_action();


};
#endif
