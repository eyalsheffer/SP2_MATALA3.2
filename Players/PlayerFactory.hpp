#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
//#include "Player.hpp"

class PlayerFactory {
public:
    static Player* createPlayer(const std::string& role,Game& game, const std::string& name) {
        if (role == "Governor") return new Governor(game,name);
        if (role == "Spy")      return new Spy(game,name);
        if (role == "Baron")    return new Baron(game,name);
        if (role == "General")  return new General(game,name);
        if (role == "Judge")    return new Judge(game,name);
        if (role == "Merchant") return new Merchant(game,name);
        return new Player(game,name); 
    }
};
