#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../Players/Player.hpp"
#include "../Players/General.hpp"
#include "../Players/Judge.hpp"
#include "../Players/Merchant.hpp"
#include "../Players/Baron.hpp"
#include "../Players/Governor.hpp"
#include "../Players/Spy.hpp"
#include "../Players/PlayerFactory.hpp"
#include "../Game.hpp"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

TEST_CASE("Game Singleton Pattern") {
    Game& game1 = Game::instance();
    Game& game2 = Game::instance();
    
    CHECK(&game1 == &game2);
}

TEST_CASE("Player Basic Functionality") {
    Game& game = Game::instance();
    
    SUBCASE("Player Constructor and Getters") {
        Player player(game, "TestPlayer");
        
        CHECK(player.get_name() == "TestPlayer");
        CHECK(player.get_coins() == 0);
        CHECK(player.get_isActive() == true);
        CHECK(player.get_isSanction() == false);
        CHECK(player.get_canArrest() == true);
        CHECK(player.get_lastArrested() == false);
    }
    
    SUBCASE("Player Setters") {
        Player player(game, "TestPlayer");
        
        player.set_coins(5);
        CHECK(player.get_coins() == 5);
        
        player.set_isActive(false);
        CHECK(player.get_isActive() == false);
        
        player.set_isSanction(true);
        CHECK(player.get_isSanction() == true);
        
        player.set_canArrest(false);
        CHECK(player.get_canArrest() == false);
        
        player.set_lastArrested(true);
        CHECK(player.get_lastArrested() == true);
        
        player.set_lastAction(GameAction::GATHER);
        CHECK(player.get_lastAction() == GameAction::GATHER);
    }
}

TEST_CASE("Player Actions - Basic Actions") {
    Game& game = Game::instance();
    game.get_players().clear(); // Clear previous players
    
    SUBCASE("Gather Action") {
        Player* player = new Player(game, "GatherPlayer");
        game.get_players().push_back(player);
        game.set_turn(0);
        
        int initial_coins = player->get_coins();
        player->gather();
        
        CHECK(player->get_coins() == initial_coins + 1);
        CHECK(player->get_lastAction() == GameAction::GATHER);
        delete player;
    }
    
    SUBCASE("Gather Action - Sanctioned Player") {
        Player* player = new Player(game, "SanctionedPlayer");
        game.get_players().push_back(player);
        game.set_turn(0);
        
        player->set_isSanction(true);
        
        CHECK_THROWS_AS(player->gather(), std::runtime_error);
        delete player;
    }
    
    SUBCASE("Tax Action") {
        Player* player = new Player(game, "TaxPlayer");
        game.get_players().push_back(player);
        game.set_turn(0);
        
        int initial_coins = player->get_coins();
        player->tax();
        
        CHECK(player->get_coins() == initial_coins + 2);
        CHECK(player->get_lastAction() == GameAction::TAX);
        delete player;
    }
    
    SUBCASE("Tax Action - Sanctioned Player") {
        Player* player = new Player(game, "SanctionedTaxPlayer");
        game.get_players().push_back(player);
        game.set_turn(0);
        
        player->set_isSanction(true);
        
        CHECK_THROWS_AS(player->tax(), std::runtime_error);
        delete player;
    }
}

TEST_CASE("Player Actions - Advanced Actions") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Bribe Action - Success") {
        Player* player = new Player(game, "BribePlayer");
        game.get_players().push_back(player);
        game.set_turn(0);
        
        player->set_coins(5);
        int initial_coins = player->get_coins();
        
        player->bribe();
        
        CHECK(player->get_coins() == initial_coins - 4);
        CHECK(player->get_lastAction() == GameAction::BRIBE);
        delete player;
    }
    
    SUBCASE("Bribe Action - Insufficient Funds") {
        Player* player = new Player(game, "PoorBribePlayer");
        game.get_players().push_back(player);
        game.set_turn(0);
        
        player->set_coins(3);
        
        CHECK_THROWS_AS(player->bribe(), std::runtime_error);
        delete player;
    }
    
    SUBCASE("Arrest Action - Basic") {
        Player* arrester = new Player(game, "Arrester");
        Player* target = new Player(game, "Target");
        
        game.get_players().push_back(arrester);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        arrester->set_coins(2);
        target->set_coins(3);
        
        int arrester_initial = arrester->get_coins();
        int target_initial = target->get_coins();
        
        arrester->arrest(*target);
        
        CHECK(arrester->get_coins() == arrester_initial + 1);
        CHECK(target->get_coins() == target_initial - 1);
        CHECK(arrester->get_lastAction() == GameAction::ARREST);
        delete arrester;
        delete target;
    }
    
    SUBCASE("Arrest Action - Target Has No Money") {
        Player* arrester = new Player(game, "Arrester");
        Player* target = new Player(game, "PoorTarget");
        
        game.get_players().push_back(arrester);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        target->set_coins(0);
        
        CHECK_THROWS_AS(arrester->arrest(*target), std::runtime_error);
        delete arrester;
        delete target;
    }
}

TEST_CASE("Player Actions - Sanction and Coup") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Sanction Action - Success") {
        Player* sanctioner = new Player(game, "Sanctioner");
        Player* target = new Player(game, "SanctionTarget");
        
        game.get_players().push_back(sanctioner);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        sanctioner->set_coins(5);
        int initial_coins = sanctioner->get_coins();
        
        sanctioner->sanction(*target);
        
        CHECK(sanctioner->get_coins() == initial_coins - 3);
        CHECK(target->get_isSanction() == true);
        CHECK(sanctioner->get_lastAction() == GameAction::SANCTION);
        delete sanctioner;
        delete target;
    }
    
    SUBCASE("Sanction Action - Insufficient Funds") {
        Player* sanctioner = new Player(game, "PoorSanctioner");
        Player* target = new Player(game, "SanctionTarget");
        
        game.get_players().push_back(sanctioner);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        sanctioner->set_coins(2);
        
        CHECK_THROWS_AS(sanctioner->sanction(*target), std::runtime_error);
         delete sanctioner;
        delete target;
    }
    
    SUBCASE("Coup Action - Success") {
        Player* couper = new Player(game, "Couper");
        Player* target = new Player(game, "CoupTarget");
        
        game.get_players().push_back(couper);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        couper->set_coins(8);
        int initial_coins = couper->get_coins();
        
        couper->coup(*target);
        
        CHECK(couper->get_coins() == initial_coins - 7);
        CHECK(target->get_isActive() == false);
        CHECK(couper->get_lastAction() == GameAction::COUP);
         delete couper;
        delete target;
    }
    
    SUBCASE("Coup Action - Insufficient Funds") {
        Player* couper = new Player(game, "PoorCouper");
        Player* target = new Player(game, "CoupTarget");
        
        game.get_players().push_back(couper);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        couper->set_coins(6);
        
        CHECK_THROWS_AS(couper->coup(*target), std::runtime_error);
        delete couper;
        delete target;
        
    }
}

TEST_CASE("Governor Special Abilities") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Governor Tax Bonus") {
        Governor* governor = new Governor(game, "GovernorPlayer");
        game.get_players().push_back(governor);
        game.set_turn(0);
        
        int initial_coins = governor->get_coins();
        governor->tax();
        
        CHECK(governor->get_coins() == initial_coins + 3); // Governor gets 3 instead of 2
        delete governor;
        
    }
}

TEST_CASE("Merchant Special Abilities") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Merchant Arrest Defense") {
        Player* arrester = new Player(game, "Arrester");
        Merchant* merchant = new Merchant(game, "MerchantPlayer");
        
        game.get_players().push_back(arrester);
        game.get_players().push_back(merchant);
        game.set_turn(0);
        
        merchant->set_coins(3);
        int merchant_initial = merchant->get_coins();
        int arrester_initial = arrester->get_coins();
        
        arrester->arrest(*merchant);
        
        // Merchant loses 2 coins (1 normal + 1 penalty), arrester gains 1 but loses 1 (net 0)
        CHECK(merchant->get_coins() == merchant_initial - 2);
        CHECK(arrester->get_coins() == arrester_initial);
        delete arrester;
        delete merchant;
    }
    
    SUBCASE("Merchant Insufficient Funds for Arrest") {
        Player* arrester = new Player(game, "Arrester");
        Merchant* merchant = new Merchant(game, "PoorMerchant");
        
        game.get_players().push_back(arrester);
        game.get_players().push_back(merchant);
        game.set_turn(0);
        
        merchant->set_coins(1); // Merchant needs at least 2 coins
        
        CHECK_THROWS_AS(arrester->arrest(*merchant), std::runtime_error);
        delete arrester;
        delete merchant;
    }
}

TEST_CASE("Baron Special Abilities") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Baron Sanction Defense") {
        Player* sanctioner = new Player(game, "Sanctioner");
        Baron* baron = new Baron(game, "BaronPlayer");
        
        game.get_players().push_back(sanctioner);
        game.get_players().push_back(baron);
        game.set_turn(0);
        
        sanctioner->set_coins(5);
        baron->set_coins(2);
        
        int baron_initial = baron->get_coins();
        
        sanctioner->sanction(*baron);
        
        CHECK(baron->get_coins() == baron_initial + 1); // Baron gets compensation
        CHECK(baron->get_isSanction() == true);
        delete sanctioner;
        delete baron;
    }
}

TEST_CASE("Judge Special Abilities") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Judge Sanction Cost Increase") {
        Player* sanctioner = new Player(game, "Sanctioner");
        Judge* judge = new Judge(game, "JudgePlayer");
        
        game.get_players().push_back(sanctioner);
        game.get_players().push_back(judge);
        game.set_turn(0);
        
        sanctioner->set_coins(5);
        int initial_coins = sanctioner->get_coins();
        
        sanctioner->sanction(*judge);
        
        // Sanctioner pays 3 + 1 extra for Judge
        CHECK(sanctioner->get_coins() == initial_coins - 4);
        CHECK(judge->get_isSanction() == true);
        delete sanctioner;
        delete judge;
    }
}

TEST_CASE("General Special Abilities") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("General Arrest Defense") {
        Player* arrester = new Player(game, "Arrester");
        General* general = new General(game, "GeneralPlayer");
        
        game.get_players().push_back(arrester);
        game.get_players().push_back(general);
        game.set_turn(0);
        
        general->set_coins(3);
        int general_initial = general->get_coins();
        int arrester_initial = arrester->get_coins();
        
        arrester->arrest(*general);
        
        // General gets back the coin that was taken
        CHECK(general->get_coins() == general_initial);
        CHECK(arrester->get_coins() == arrester_initial);
        delete arrester;
        delete general;
    }
}

TEST_CASE("Game Turn Management") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Current Player Check") {
        Player* player1 = new Player(game, "Player1");
        Player* player2 = new Player(game, "Player2");
        
        game.get_players().push_back(player1);
        game.get_players().push_back(player2);
        game.set_turn(0);
        
        CHECK(game.is_current(*player1) == true);
        CHECK(game.is_current(*player2) == false);
        
        game.set_turn(1);
        CHECK(game.is_current(*player1) == false);
        CHECK(game.is_current(*player2) == true);
        delete player1;
        delete player2;
    }
    
    SUBCASE("Turn Manager Progression") {
        Player* player1 = new Player(game, "Player1");
        Player* player2 = new Player(game, "Player2");
        Player* player3 = new Player(game, "Player3");
        
        game.get_players().push_back(player1);
        game.get_players().push_back(player2);
        game.get_players().push_back(player3);
        game.set_turn(0);
        
        CHECK(game.get_turn() == 0);
        
        player1->gather(); // This should advance the turn
        CHECK(game.get_turn() == 1);
        
        player2->gather(); // This should advance the turn
        CHECK(game.get_turn() == 2);
        
        player3->gather(); // This should wrap around to 0
        CHECK(game.get_turn() == 0);
        delete player1;
        delete player2;
        delete player3;
    }
}

TEST_CASE("Game Valid Move Checks") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Inactive Player Cannot Move") {
        Player* player = new Player(game, "InactivePlayer");
        game.get_players().push_back(player);
        game.set_turn(0);
        
        player->set_isActive(false);
        
        CHECK_THROWS_AS(player->gather(), std::runtime_error);
        delete player;
    }
    
    SUBCASE("Out of Turn Player Cannot Move") {
        Player* player1 = new Player(game, "Player1");
        Player* player2 = new Player(game, "Player2");
        
        game.get_players().push_back(player1);
        game.get_players().push_back(player2);
        game.set_turn(0); // It's player1's turn
        
        CHECK_THROWS_AS(player2->gather(), std::runtime_error);
           delete player1;
        delete player2;
    }
    
    SUBCASE("Player with 10+ Coins Must Coup") {
        Player* rich_player = new Player(game, "RichPlayer");
        Player* target = new Player(game, "Target");
        
        game.get_players().push_back(rich_player);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        rich_player->set_coins(10);
        
        CHECK_THROWS_AS(rich_player->gather(), std::runtime_error);
        CHECK_THROWS_AS(rich_player->tax(), std::runtime_error);
        
        // But coup should work
        CHECK_NOTHROW(rich_player->coup(*target));
           delete rich_player;
        delete target;
    }
}

TEST_CASE("Game Winner Detection") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("No Winner - Multiple Active Players") {
        Player* player1 = new Player(game, "Player1");
        Player* player2 = new Player(game, "Player2");
        
        game.get_players().push_back(player1);
        game.get_players().push_back(player2);
        
        CHECK_THROWS_AS(game.winner(), std::runtime_error);
        delete player1;
        delete player2;
    }
    
    SUBCASE("Winner - Single Active Player") {
        Player* player1 = new Player(game, "Winner");
        Player* player2 = new Player(game, "Loser");
        
        game.get_players().push_back(player1);
        game.get_players().push_back(player2);
        
        player2->set_isActive(false);
        
        CHECK(game.winner() == "Winner");
        delete player1;
        delete player2;
    }
    
    SUBCASE("No Winner - No Players") {
        game.get_players().clear();
        
        CHECK_THROWS_AS(game.winner(), std::runtime_error);
    }
}

TEST_CASE("Merchant Turn Bonus") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Merchant Gets Bonus at Turn Start") {
        Merchant* merchant = new Merchant(game, "MerchantPlayer");
        game.get_players().push_back(merchant);
        game.set_turn(0);
        
        merchant->set_coins(3);
        int initial_coins = merchant->get_coins();
        
        // Simulate turn manager being called (this happens in turn_manager())
        game.turn_manager();
        
        // After turn manager, if merchant had 3+ coins, should get +1
        CHECK(merchant->get_coins() == initial_coins + 1);
        delete merchant;
        
    }
}

TEST_CASE("Edge Cases and Error Handling") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Empty Game Turn Management") {
        CHECK_THROWS_AS(game.turn_manager(), std::runtime_error);
        Player* temp = new Player(game, "TestPlayer");
        CHECK_THROWS_AS(game.is_current(*temp), std::runtime_error);
        delete temp;
    }
    
    SUBCASE("Bribe Turn Handling") {
        Player* player = new Player(game, "BribePlayer");
        game.get_players().push_back(player);
        game.set_turn(0);
        
        player->set_coins(5);
        int current_turn = game.get_turn();
        
        player->bribe();
        
        // After bribe, turn should not advance (handled by _is_bribe flag)
        CHECK(game.get_turn() == current_turn);
        delete player;
        
    }
}

TEST_CASE("Action Chain Tests") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Multiple Actions in Sequence") {
        Player* player = new Player(game, "ActionPlayer");
        game.get_players().push_back(player);
        game.set_turn(0);
        
        // Start with some coins
        player->set_coins(2);
        
        // Gather
        player->gather();
        CHECK(player->get_coins() == 3);
        CHECK(player->get_lastAction() == GameAction::GATHER);
        
        // Tax (after turn advances and comes back)
        game.set_turn(0); // Reset for next action
        player->tax();
        CHECK(player->get_coins() == 5);
        CHECK(player->get_lastAction() == GameAction::TAX);
        delete player;
    }
}
TEST_CASE("Baron Special Abilities - Complete") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Baron Unique Action - Success") {
        Baron* baron = new Baron(game, "BaronPlayer");
        game.get_players().push_back(baron);
        game.set_turn(0);
        
        baron->set_coins(5);
        int initial_coins = baron->get_coins();
        
        baron->uniqe();
        
        CHECK(baron->get_coins() == initial_coins + 3);
        delete baron;
    }
    
    SUBCASE("Baron Unique Action - Insufficient Coins") {
        Baron* baron = new Baron(game, "PoorBaron");
        game.get_players().push_back(baron);
        game.set_turn(0);
        
        baron->set_coins(2);
        
        CHECK_THROWS_AS(baron->uniqe(), std::runtime_error);
        delete baron;
    }
    
    SUBCASE("Baron Unique Action - Too Many Coins") {
        Baron* baron = new Baron(game, "RichBaron");
        game.get_players().push_back(baron);
        game.set_turn(0);
        
        baron->set_coins(10);
        
        CHECK_THROWS_AS(baron->uniqe(), std::runtime_error);
        delete baron;
    }
    
    SUBCASE("Baron Unique Action - Inactive Player") {
        Baron* baron = new Baron(game, "InactiveBaron");
        game.get_players().push_back(baron);
        game.set_turn(0);
        
        baron->set_coins(5);
        baron->set_isActive(false);
        
        CHECK_THROWS_AS(baron->uniqe(), std::runtime_error);
        delete baron;
    }
    
    SUBCASE("Baron Sanction Defense - Existing Test Enhanced") {
        Player* sanctioner = new Player(game, "Sanctioner");
        Baron* baron = new Baron(game, "BaronPlayer");
        
        game.get_players().push_back(sanctioner);
        game.get_players().push_back(baron);
        game.set_turn(0);
        
        sanctioner->set_coins(5);
        baron->set_coins(2);
        
        int baron_initial = baron->get_coins();
        int sanctioner_initial = sanctioner->get_coins();
        
        sanctioner->sanction(*baron);
        
        CHECK(baron->get_coins() == baron_initial + 1); // Baron gets compensation
        CHECK(baron->get_isSanction() == true);
        CHECK(sanctioner->get_coins() == sanctioner_initial - 3); // Normal sanction cost
        delete sanctioner;
        delete baron;
    }
}

void reset_game_state(Game& game) {
    game.get_players().clear();
    game.set_turn(0);
}

    TEST_CASE("General Special Abilities - Complete") {
        Game& game = Game::instance();

        SUBCASE("General Coup Block - Success") {
        reset_game_state(game);

        Player* couper = new Player(game, "Couper");
        General* general = new General(game, "GeneralPlayer");

        game.get_players().push_back(couper);
        game.get_players().push_back(general);
        game.set_turn(0);

        couper->set_coins(8);
        general->set_coins(6);  // Ensure general has enough BEFORE coup

        int general_initial = general->get_coins();  // Save BEFORE coup

        couper->coup(*general);  // Marks general inactive

        //general->uniqe(*couper, *general);  // General blocks it

        CHECK(general->get_coins() == general_initial - 5); // Paid 5 to block
        CHECK(general->get_isActive() == true);             // Restored to active
        CHECK(couper->get_lastAction() == GameAction::NONE); // Action nullified
        delete couper;
        delete general;
    }
    SUBCASE("General Block - Wrong Action") {
        reset_game_state(game);

        Player* player = new Player(game, "Player");
        General* general = new General(game, "GeneralPlayer");

        game.get_players().push_back(player);
        game.get_players().push_back(general);
        game.set_turn(0);

        general->set_coins(6);
        player->gather(); // Not a coup

        CHECK_THROWS_AS(general->uniqe(*player, *general), std::runtime_error);
         delete player;
        delete general;
    }



   

    SUBCASE("General Arrest Defense - Existing Test Enhanced") {
        reset_game_state(game);

        Player* arrester = new Player(game, "Arrester");
        General* general = new General(game, "GeneralPlayer");

        game.get_players().push_back(arrester);
        game.get_players().push_back(general);
        game.set_turn(0);

        general->set_coins(3);
        arrester->set_coins(2);

        int general_initial = general->get_coins();
        int arrester_initial = arrester->get_coins();

        arrester->arrest(*general);

        // General's passive defense should work
        CHECK(general->get_coins() == general_initial);
        CHECK(arrester->get_coins() == arrester_initial);
         delete arrester;
        delete general;
    }
}

TEST_CASE("Governor Special Abilities - Complete") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Governor Tax Block - Success") {
        Governor* blocker = new Governor(game, "BlockerGovernor");
        Player* player = new Player(game, "TaxPlayer");
        
        game.get_players().push_back(blocker);
        game.get_players().push_back(player);
        game.set_turn(1); // Player's turn
        
        player->set_coins(3);
        int initial_coins = player->get_coins();
        
        player->tax();
        
        
        // Now Governor blocks
        game.set_turn(0); // Governor's turn
        blocker->uniqe(*player);
        
        CHECK(player->get_coins() == initial_coins); // Tax reversed
         delete blocker;
        delete player;
    }
    
    SUBCASE("Governor Tax Block - Against Another Governor") {
        Governor* blocker = new Governor(game, "BlockerGovernor");
        Governor* target = new Governor(game, "TargetGovernor");
        
        game.get_players().push_back(blocker);
        game.get_players().push_back(target);
        game.set_turn(1); // Target's turn
        
        target->set_coins(3);
        int initial_coins = target->get_coins();
        
        target->tax();
        
        // Now blocker Governor blocks
        game.set_turn(0); // Blocker's turn
        blocker->uniqe(*target);
        
        CHECK(target->get_coins() == initial_coins); // 3 coin tax reversed
         delete blocker;
        delete target;
    }
    
    SUBCASE("Governor Block - Wrong Action") {
        Governor* governor = new Governor(game, "Governor");
        Player* player = new Player(game, "Player");
        
        game.get_players().push_back(governor);
        game.get_players().push_back(player);
        game.set_turn(1);
        
        player->gather(); // Not tax
        
        game.set_turn(0);
        CHECK_THROWS_AS(governor->uniqe(*player), std::runtime_error);
        delete governor;
        delete player;
    }
    
    SUBCASE("Governor Block - Inactive") {
        Governor* governor = new Governor(game, "Governor");
        Player* player = new Player(game, "Player");
        
        game.get_players().push_back(governor);
        game.get_players().push_back(player);
        game.set_turn(1);
        
        player->tax();
        
        governor->set_isActive(false);
        game.set_turn(0);
        CHECK_THROWS_AS(governor->uniqe(*player), std::runtime_error);
        delete governor;
        delete player;
    }
}

TEST_CASE("Judge Special Abilities - Complete") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Judge Bribe Block - Success") {
        Judge* judge = new Judge(game, "JudgePlayer");
        Player* briber = new Player(game, "BriberPlayer");
        
        game.get_players().push_back(judge);
        game.get_players().push_back(briber);
        game.set_turn(1); // Briber's turn
        
        briber->set_coins(5);
        
        briber->bribe();
        CHECK(game.get_isBribe() == true); // Bribe should set this flag
        
        // Judge blocks the bribe
        game.set_turn(0);
        judge->uniqe(*briber);
        
        CHECK(game.get_isBribe() == false); // Bribe blocked
        delete judge;
        delete briber;
        
    }
     
    SUBCASE("Judge Block - Wrong Action") {
        Judge* judge = new Judge(game, "JudgePlayer");
        Player* player = new Player(game, "Player");
        
        game.get_players().push_back(judge);
        game.get_players().push_back(player);
        game.set_turn(1);
        
        player->gather(); // Not bribe
        
        game.set_turn(0);
        CHECK_THROWS_AS(judge->uniqe(*player), std::runtime_error);
        delete judge;
        delete player;
    }
    
    SUBCASE("Judge Block - Inactive") {
        Judge* judge = new Judge(game, "JudgePlayer");
        Player* briber = new Player(game, "BriberPlayer");
        
        game.get_players().push_back(judge);
        game.get_players().push_back(briber);
        game.set_turn(1);
        
        briber->set_coins(5);
        briber->bribe();
        
        judge->set_isActive(false);
        game.set_turn(0);
        CHECK_THROWS_AS(judge->uniqe(*briber), std::runtime_error);
        delete judge;
        delete briber;
    }
    
    SUBCASE("Judge Sanction Cost Increase - Existing Test Enhanced") {
        Player* sanctioner = new Player(game, "Sanctioner");
        Judge* judge = new Judge(game, "JudgePlayer");
        
        game.get_players().push_back(sanctioner);
        game.get_players().push_back(judge);
        game.set_turn(0);
        
        sanctioner->set_coins(5);
        int initial_coins = sanctioner->get_coins();
        
        sanctioner->sanction(*judge);
        
        CHECK(sanctioner->get_coins() == initial_coins - 4); // 3 + 1 extra for Judge
        CHECK(judge->get_isSanction() == true);
        delete judge;
        delete sanctioner;
    }
}

TEST_CASE("Merchant Special Abilities - Complete") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Merchant Turn Bonus - Success") {
        Merchant* merchant = new Merchant(game, "MerchantPlayer");
        game.get_players().push_back(merchant);
        game.set_turn(0);
        
        merchant->set_coins(3);
        int initial_coins = merchant->get_coins();
        
        merchant->uniqe(); // Should be called at turn start
        
        CHECK(merchant->get_coins() == initial_coins + 1);
        delete merchant;
    }
    
    SUBCASE("Merchant Turn Bonus - Insufficient Coins") {
        Merchant* merchant = new Merchant(game, "PoorMerchant");
        game.get_players().push_back(merchant);
        game.set_turn(0);
        
        merchant->set_coins(2);
        int initial_coins = merchant->get_coins();
        
        merchant->uniqe();
        
        CHECK(merchant->get_coins() == initial_coins); // No bonus
        delete merchant;
    }
    
    SUBCASE("Merchant Arrest Defense - Existing Test Enhanced") {
        Player* arrester = new Player(game, "Arrester");
        Merchant* merchant = new Merchant(game, "MerchantPlayer");
        
        game.get_players().push_back(arrester);
        game.get_players().push_back(merchant);
        game.set_turn(0);
        
        merchant->set_coins(3);
        arrester->set_coins(2);
        
        int merchant_initial = merchant->get_coins();
        int arrester_initial = arrester->get_coins();
        
        arrester->arrest(*merchant);
        
        // Merchant loses 2 coins (1 normal + 1 penalty), arrester gains 1 but loses 1 (net 0)
        CHECK(merchant->get_coins() == merchant_initial - 2);
        CHECK(arrester->get_coins() == arrester_initial);
        delete arrester;
        delete merchant;
    }
    
    SUBCASE("Merchant Arrest Defense - Insufficient Funds") {
        Player* arrester = new Player(game, "Arrester");
        Merchant* merchant = new Merchant(game, "PoorMerchant");
        
        game.get_players().push_back(arrester);
        game.get_players().push_back(merchant);
        game.set_turn(0);
        
        merchant->set_coins(1); // Merchant needs at least 2 coins
        
        CHECK_THROWS_AS(arrester->arrest(*merchant), std::runtime_error);
         delete arrester;
        delete merchant;
    }
}

TEST_CASE("Spy Special Abilities - Complete") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Spy Disable Arrest - Success") {
        Spy* spy = new Spy(game, "SpyPlayer");
        Player* target = new Player(game, "TargetPlayer");
        
        game.get_players().push_back(spy);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        CHECK(target->get_canArrest() == true); // Initially can arrest
        
        spy->uniqe(*target);
        
        CHECK(target->get_canArrest() == false); // Now cannot arrest
         delete spy;
        delete target;
    }
    
    SUBCASE("Spy Action - Target Inactive") {
        Spy* spy = new Spy(game, "SpyPlayer");
        Player* target = new Player(game, "InactiveTarget");
        
        game.get_players().push_back(spy);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        target->set_isActive(false);
        
        CHECK_THROWS_AS(spy->uniqe(*target), std::runtime_error);
        delete spy;
        delete target;
    }
    
    SUBCASE("Spy Action - Spy Inactive") {
        Spy* spy = new Spy(game, "InactiveSpy");
        Player* target = new Player(game, "TargetPlayer");
        
        game.get_players().push_back(spy);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        spy->set_isActive(false);
        
        CHECK_THROWS_AS(spy->uniqe(*target), std::runtime_error);
        delete spy;
        delete target;
    }
    
    SUBCASE("Spy Effect on Arrest Actions") {
        Spy* spy = new Spy(game, "SpyPlayer");
        Player* victim = new Player(game, "VictimPlayer");
        Player* target = new Player(game, "TargetPlayer");
        
        game.get_players().push_back(spy);
        game.get_players().push_back(victim);
        game.get_players().push_back(target);
        game.set_turn(0);
        
        // Spy disables victim's arrest ability
        spy->uniqe(*victim);
        CHECK(victim->get_canArrest() == false);
        

        game.set_turn(1); // Victim's turn
        victim->set_coins(2);
        target->set_coins(2);
        delete spy;
        delete victim;
        delete target;

    }
}

TEST_CASE("Special Abilities Integration Tests") {
    Game& game = Game::instance();
    game.get_players().clear();
    
    SUBCASE("Multiple Special Players Interaction") {
        Baron* baron = new Baron(game, "Baron");
        General* general = new General(game, "General");
        Governor* governor = new Governor(game, "Governor");
        Judge* judge = new Judge(game, "Judge");
        Merchant* merchant = new Merchant(game, "Merchant");
        Spy* spy = new Spy(game, "Spy");
        
        game.get_players().push_back(baron);
        game.get_players().push_back(general);
        game.get_players().push_back(governor);
        game.get_players().push_back(judge);
        game.get_players().push_back(merchant);
        game.get_players().push_back(spy);
        
        // Set initial states
        baron->set_coins(5);
        general->set_coins(6);
        governor->set_coins(4);
        judge->set_coins(3);
        merchant->set_coins(3);
        spy->set_coins(2);
        
        game.set_turn(0); // Baron's turn
        
        // Test Baron's unique ability
        baron->uniqe();
        CHECK(baron->get_coins() == 8);
        
        // Test Merchant's turn bonus
        game.set_turn(4); // Merchant's turn
        int merchant_coins = merchant->get_coins();
        merchant->uniqe();
        CHECK(merchant->get_coins() == merchant_coins + 1);
        
        // Test Spy's ability
        game.set_turn(5); // Spy's turn
        spy->uniqe(*general);
        CHECK(general->get_canArrest() == false);
         delete baron;
        delete general;
         delete governor;
        delete judge;
        delete merchant;
        delete spy;
    }
    
    SUBCASE("Edge Case - All Players Have Unique Abilities") {
        // Test scenario where all special abilities might be triggered
        Governor* gov1 = new Governor(game, "Gov1");
        Governor* gov2 = new Governor(game, "Gov2");
        
        game.get_players().push_back(gov1);
        game.get_players().push_back(gov2);
        
        gov1->set_coins(5);
        gov2->set_coins(3);
        
        game.set_turn(1); // Gov2's turn
        gov2->tax(); // Gets 3 coins (Governor bonus)
        CHECK(gov2->get_coins() == 6);
        
        game.set_turn(0); // Gov1's turn
        gov1->uniqe(*gov2); // Blocks the tax
        CHECK(gov2->get_coins() == 3); // Tax reversed
        delete gov1;
        delete gov2;
    }
}

