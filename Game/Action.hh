#ifndef Action_hh
#define Action_hh


#include "Structs.hh"

/**
 * A command is defined by the id of a unit (= ork) and a direction.
 */
struct Command {

  int id;
  Dir dir;

  /**
   * Constructor, given a unit identifier and a movement direction.
   */
  Command (int id, Dir dir) : id(id), dir(dir) { }

};


/**
 * Class that stores the commands requested by a player in a round.
 */
class Action {

public:

  /**
   * Adds a command to the action (= list of commands).
   * Fails if a command is already present for this unit.
   */
  void execute(Command m);

  //////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////

  /**
   * Empty constructor.
   */
  Action () : q_(0) { }


private:

  friend class Game;
  friend class SecGame;
  friend class Board;

  /**
   * Maximum number of commands allowed for a player during one round.
   */
  static const int MAX_COMMANDS = 1000;

  /**
   * Number of commands tried so far.
   */
  int q_;

  /**
   * Set of units that have already performed a command.
   */
  set<int> u_;

  /**
   * List of commands to be performed during this round.
   */
  vector<Command> v_;

  /**
   * Read/write commands to/from a stream.
   */
  Action (istream& is);
  static void print (const vector<Command>& commands, ostream& os);

  /**
   * Conversion from char to Dir.
   */
  static inline Dir c2d (char c) {
    switch (c) {
    case 'b': return BOTTOM;
    case 'r': return RIGHT;
    case 't': return TOP;
    case 'l': return LEFT;
    case 'n': return NONE;
    }
    // _unreachable(); 
    return DIR_SIZE; // Can't abort: if data is corrupted, master will fail.
  }

  /**
   * Conversion from Dir to char.
   */
  static inline char d2c (Dir d) {
    switch (d) {
    case BOTTOM: return 'b';
    case RIGHT:  return 'r';
    case TOP:    return 't';
    case LEFT:   return 'l';
    case NONE:   return 'n';
    default:     _unreachable();
    }
  }

};

inline void Action::execute(Command m) {
  ++q_;
  _my_assert(q_ <= MAX_COMMANDS, "Too many commands.");

  if (u_.find(m.id) != u_.end()) {
    cerr << "warning: command already requested for unit " << m.id << endl;
    return;
  }

  u_.insert(m.id);
  v_.push_back(m);
}

#endif
