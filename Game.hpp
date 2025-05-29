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
          void clear_players();
        std::vector<Player*>& get_players();
        int get_turn();
        void set_turn(const int turn);
         bool get_isBribe() const;


        void set_isBribe(const bool isBribe);
        std::string winner();
        bool is_current( Player& p) const;
        void check_valid_move( Player& p) const;
        void turn_manager();
        bool have_arrests_options(Player& p) const;
        bool can_take_action( Player& p) const;
        //void add_player(Player* p);
        //void make_action();


};
#endif
