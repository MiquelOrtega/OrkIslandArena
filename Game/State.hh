#ifndef State_hh
#define State_hh


#include "Structs.hh"

/*! \file
 * Contains a class to store the current state of a game.
 */


/**
 * Stores the game state.
 */
class State {

public:

  /**
   * Returns the current round.
   */
  int round () const;

  /**
   * Returns the total score of a player.
   */
  int total_score (int pl) const;

  /**
   * Returns the percentage of cpu time used in the last round, in the
   * range [0.0 - 1.0] or a value lesser than 0 if this player is dead.
   * Note that this only returns a sensible value in server executions.
   */
  double status (int pl) const;

  /**
   * Returns a copy of the cell at (i, j).
   */
  Cell cell (int i, int j) const;

  /**
   * Returns a copy of the cell at p.
   */
  Cell cell (Pos p) const;

  /**
   * Returns the total number of units (= orks) in the game.
   */
  int nb_units () const;

  /**
   * Returns the information of the unit with (unit) identifier id.
   * Units are identified with natural numbers from 0 to nb_units() - 1.
   */
  Unit unit (int id) const;

  /**
   * Returns the number of cities in the game.
   */
  int nb_cities () const;

  typedef vector<Pos>  City; // City type

  /**
   * Returns the information of the city with (city) identifier id.
   * Cities are identified with natural numbers from 0 to nb_cities() - 1.
   */
  City city(int id) const;

  /**
   * Returns the identifier of the player who last conquered the city
   * with identifier id, -1 if the city has never been conquered.
   */
  int city_owner(int id) const;

  /**
   * Returns the total number of paths in the game.
   */
  int nb_paths () const;

  typedef pair< pair<int,int>, vector<Pos> >  Path; // Path type

  /**
   * Returns the information of the path with (path) identifier id.
   * Paths are identified with natural numbers from 0 to nb_path() - 1.
   */
  Path path(int id) const;

  /**
   * Returns the identifier of the player who last conquered the path
   * with identifier id, -1 if the path has never been conquered.
   */
  int path_owner(int id) const;

  /**
   * Returns the ids of all the orks of a player.
   */
  vector<int> orks(int pl);


  //////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////


private:

  friend class Info;
  friend class Board;
  friend class Game;
  friend class SecGame;
  friend class Player;

  vector<City>           city_;
  vector<Path>           path_;
  vector<vector<Cell>>   grid_;
  vector<int>      city_owner_;
  vector<int>      path_owner_;
  vector<Unit>           unit_;
  vector< vector<int> >  orks_;
  
  int                   round_;
  vector<int>     total_score_;
  vector<double>   cpu_status_; // -1 -> dead, 0..1 -> % of cpu time limit

  /**
   * Returns whether id is a valid unit identifier.
   */
  inline bool unit_ok (int id) const {
    return id >= 0 and id < nb_units();
  }

  inline bool city_ok (int id) const {
    return id >= 0 and id < nb_cities();
  }

  inline bool path_ok (int id) const {
    return id >= 0 and id < nb_paths();
  }

};

inline int State::round () const {
  return round_;
}

inline Cell State::cell (int i, int j) const {
  if (i >= 0 and i < (int)grid_.size() and j >= 0 and j < (int)grid_[i].size())
    return grid_[i][j];
  else {
    cerr << "warning: cell requested for position " << Pos(i, j) << endl;
    return Cell();
  }
}

inline Cell State::cell (Pos p) const {
  return cell(p.i, p.j);
}

inline int State::nb_units () const {
  return unit_.size();
}

inline int State::nb_cities () const {
  return city_.size();
}

inline int State::nb_paths () const {
  return path_.size();
}


inline int State::total_score (int pl) const {
  if (pl >= 0 and pl < (int)total_score_.size())
    return total_score_[pl];
  else {
    cerr << "warning: total score requested for player " << pl << endl;
    return -1;
  }
}

inline double State::status (int pl) const {
  if (pl >= 0 and pl < (int)cpu_status_.size())
    return cpu_status_[pl];
  else {
    cerr << "warning: status requested for player " << pl << endl;
    return -2;
  }
}

inline Unit State::unit (int id) const {
  if (unit_ok(id))
    return unit_[id];
  else {
    cerr << "warning: unit requested for identifier " << id << endl;
    return Unit();
  }
}

inline State::City State::city(int id) const {
  if (city_ok(id))
    return city_[id];
  else {
    cerr << "warning: city requested for identifier " << id << endl;
    return City();
  }
}

inline State::Path State::path(int id) const {
  if (path_ok(id))
    return path_[id];
  else {
    cerr << "warning: path requested for identifier " << id << endl;
    return Path();
  }
}

inline int State::city_owner(int id) const {
  if (city_ok(id))
    return city_owner_[id];
  else {
    cerr << "warning: city owner requested for identifier " << id << endl;
    return -1;
  }
}

inline int State::path_owner(int id) const {
  if (path_ok(id))
    return path_owner_[id];
  else {
    cerr << "warning: path owner requested for identifier " << id << endl;
    return -1;
  }
}

inline vector<int> State::orks (int pl) {
  if (pl >= 0 and pl < (int)orks_.size()) return orks_[pl];
  else {
    cerr << "warning: orks requested for player " << pl << endl;
    return vector<int>();
  }
}

#endif
