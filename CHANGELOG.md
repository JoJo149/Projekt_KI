
# Change Log
All notable changes to this project will be documented in this file.
 
## [Meilenstein 1](https://git.tu-berlin.de/hallo123/projekt-ki/-/tree/0f3e85bac9a3bbec9c1aa49b01b48de32397f11d) - 2025-05-02

Basic implementation of the game logic for Guards and Towers.\
As well as basic Tests and Benchmarks.
 
### Added
- [Projekt KI-1.0.1](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/1)
  MINOR FEN to bitboard and vice versa.
- [Projekt KI-1.0.2](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/2)
  MINOR Print game nice in Console
- [Projekt KI-1.1.0](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/3)
    MAJOR Game Logic und Zuggenerator
- [Projekt KI-1.2.0](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/4)
    MAJOR added tests and benchmarks
 
## [Meilenstein 2](https://git.tu-berlin.de/hallo123/projekt-ki/-/tree/a1b2b29389f5fec5252930ce7629a859de3cc9df) - 2025-05-20
  
To start the basic implementation of an AI we implemented Min Max and Alpha Beta Search.
We implemented a basic client to communicate with the game server, which was provided.\
Lastly we added multithreading to our AI.

### Added
- [Projekt KI-2.1.0](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/5)
  MAJOR minmax and alpha beta search 
- [Projekt KI-2.2.0](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/6)
  MAJOR Server Implementation

### Changed
- [Projekt KI-2.2.1](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/7)
  PATCH Bewertungsfunkton verbessern
 
## [Meilenstein 3]() - 2025-06-20
 
### Added
- [Projekt KI-3.1.0](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/11)
  MAJOR transposition table
- [Projekt KI-3.1.2](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/14)
  MINOR Docker für Discord server
- [Projekt KI-3.2.0](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/15)
  MAJOR sorting moves

### Changed
- [Projekt KI-3.0.1](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/9)
  PATCH optimize eval function
- [Projekt KI-3.0.2](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/10)
  PATCH Move und Movelist optimieren, nicht immer neue Liste erstellen

### Fixed
- [Projekt KI-3.1.1](https://git.tu-berlin.de/hallo123/projekt-ki/-/issues/12)
  MAJOR build an eval function similar to: [stockfish](https://hxim.github.io/Stockfish-Evaluation-Guide/)

### STATES FOR BENCHMARKS
- [BASE VERSION](https://git.tu-berlin.de/hallo123/projekt-ki/-/tree/a1b2b29389f5fec5252930ce7629a859de3cc9df)
- [REFACTORED VERSION](https://git.tu-berlin.de/hallo123/projekt-ki/-/tree/11951e2ba4720dd0b14a6d94922382bd6e8295ff)
- [TRANSPOSITION TABLE](https://git.tu-berlin.de/hallo123/projekt-ki/-/tree/4ede579d6f408243b7f44184791e9f078349c08a)
- [optimized undo move](https://git.tu-berlin.de/hallo123/projekt-ki/-/tree/1dce06493aebd1dbfd85b93656c57572ed811e44)
- [sorting moves](https://git.tu-berlin.de/hallo123/projekt-ki/-/tree/6ef6a0cb5ba57057026871aa871ea52d171a0562)

