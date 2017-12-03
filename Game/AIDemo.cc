#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Demo


// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.

struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  typedef vector<int> VI;
  typedef vector<VI>  VVI;

  // Stores the location of orks.
  VVI ork_at;
  
  // Returns true if winning.
  bool winning() {
    for (int pl = 0; pl < nb_players(); ++pl)
      if (pl != me() and total_score(me()) <= total_score(pl))
        return false;
    return true;
  }

  // Moves ork with identifier id.
  void move(int id) {
    Unit u = unit(id);
    Pos pos = u.pos;
    // Try to move to a position within the board.
    for (int d = 0; d != DIR_SIZE; ++d) {
      Dir dir = Dir(d);
      Pos npos = pos + dir;
      if (pos_ok(npos)) {
        execute(Command(id, dir));
        return;
      }
    }
  }
  
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    
    // If more than halfway through, do nothing.
    if (round() > nb_rounds()/2) return;
    
    // If winning, do nothing.
    if (winning()) return;

    // If nearly out of time, do nothing.
    double st = status(me());
    if (st >= 0.9) return;

    if (round() == 0) {
      ork_at = VVI(rows(), VI(cols(), -1));
    }
    else {
      // Compare previous round with current one.
      for (int i = 0; i < rows(); ++i)
        for (int j = 0; j < cols(); ++j) {
          Cell c = cell(i, j);
          if (c.type == CITY                     and
              ork_at[i][j] != c.unit_id          and
              c.unit_id != -1                    and
              unit(c.unit_id).player != me()) {
            cerr << "Enemy ork " << c.unit_id << " moved to CITY position " << Pos(i, j) << " of city " << c.city_id << endl;
          }
        }
      for (int i = 0; i < rows(); ++i)
        for (int j = 0; j < cols(); ++j)
          ork_at[i][j] = cell(i, j).unit_id; // Update for the next round.
    }
    
    if (random(0, 3)) { // Do the following with probability 0.75

      VI my_orks = orks(me()); // Get the id's of my orks.

      // Process orks in random order.
      VI perm = random_permutation(my_orks.size());
      for (int k = 0; k < int(perm.size()); ++k) 
        move(my_orks[perm[k]]);                                           
    }
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
