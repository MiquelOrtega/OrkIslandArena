#ifndef Game_hh
#define Game_hh


#include "Player.hh"
#include "Board.hh"


/**
 * Game class.
 */
class Game {

public:

  static void run (vector<string> names, istream& is, ostream& os, int seed);

};


#endif
