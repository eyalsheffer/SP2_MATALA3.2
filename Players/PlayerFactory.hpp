#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
//#include "Player.hpp"

class PlayerFactory {
public:
    static Player* createPlayer(const std::string& role, const std::string& name) {
        if (role == "Governor") return new Governor(name);
        if (role == "Spy")      return new Spy(name);
        if (role == "Baron")    return new Baron(name);
        if (role == "General")  return new General(name);
        if (role == "Judge")    return new Judge(name);
        if (role == "Merchant") return new Merchant(name);
        return new Player(name); 
    }
};
