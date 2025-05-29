# Turn-Based Role Strategy Game

A C++ turn-based strategy game where each player assumes a unique role (Governor, Spy, Baron, General, Judge, Merchant), each with special abilities that influence the game's progression. The game includes strict turn management and rule enforcement.

## Features

- Role-based player actions with unique abilities
- Strict turn order validation
- Action system: `gather`, `tax`, `bribe`, `arrest`, `sanction`, `coup`
- Singleton `Game` class to manage state
- Exception handling for invalid actions
- Thorough unit testing with [doctest](https://github.com/doctest/doctest)



## How to Run

Clone the project and navigate to the project directory.

### Build & Run

- **Run demo:**
  ```bash
  make Main
  ./Main

- **Run unit tests:**
    ```bash 
    make test
    ./test 
- **make valgrind :**
  ```bash 
    make valgrind
- **Clean temporary files :**
  ```bash 
    make clean