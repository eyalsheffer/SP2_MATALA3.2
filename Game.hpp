#ifndef Game_HPP
#define Game_HPP

#include <iostream>
#include <vector>
#include "Players/Player.hpp"
class Game{
    private:
        std::vector<Player*> _players;
        int _turn;
        bool _is_bribe;
        Game();
        ~Game();
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;
    public:
        static Game& instance();

        std::vector<Player*>& get_players();
        int get_turn();
        void set_turn(const int turn);
        std::string winner();
        bool is_current(const Player& p) const;
        void check_valid_move(const Player& p) const;
        void turn_manager();
        bool can_take_action(const Player& p) const
        //void add_player(Player* p);
        //void make_action();


};
#endif
