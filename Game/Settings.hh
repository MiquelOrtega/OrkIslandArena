#ifndef Settings_hh
#define Settings_hh


#include "Structs.hh"


/** \file
 * Contains a class to store all the game settings that do not change
 * during a game, except the names of the players.
 */

/**
 * Stores most of the game settings.
 */
class Settings {

public:

  /**
   * Returns a string with the game name and version.
   */
  static string version ();

  /**
   * Returns the number of players in the game.
   */
  int nb_players () const;

  /**
   * Returns the number of rows of the board of the game.
   */
  int rows () const;

  /**
   * Returns the number of columns of the board of the game.
   */
  int cols () const;

  /**
   * Returns the number of rounds of the game.
   */
  int nb_rounds () const;

  /**
   * Returns the initial health of each unit.
   */
  int initial_health () const;

  /**
   * Returns the number of orks each player controls initially.
   */
  int nb_orks () const;

  /**
   * Returns the cost in health of moving to a cell of type GRASS.
   */
  int cost_grass () const;

  /**
   * Returns the cost in health of moving to a cell of type FOREST.
   */
  int cost_forest () const;

  /**
   * Returns the cost in health of moving to a cell of type SAND.
   */
  int cost_sand () const;

  /**
   * Returns the cost in health of moving to a cell of type CITY.
   */
  int cost_city () const;

  /**
   * Returns the cost in health of moving to a cell of type PATH.
   */
  int cost_path () const;

  /**
   * Returns the cost in health of moving to a cell of type t (t != WATER).
   */
  int cost (CellType t) const;
  
  /**
   * Returns the bonus in points for each cell in a conquered city.
   */
  int bonus_per_city_cell () const;

  /**
   * Returns the bonus in points for each cell in a conquered city.
   */
  int bonus_per_path_cell () const;

  /**
   * Returns the factor multiplying the size of connected components.
   */
  int factor_connected_component () const;

  /**
   * Returns whether pl is a valid player identifier.
   */
  bool player_ok (int pl) const;

  /**
   * Returns whether (i, j) is a position inside the board.
   */
  bool pos_ok (int i, int j) const;

  /**
   * Returns whether p is a position inside the board.
   */
  bool pos_ok (Pos p) const;


  //////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////


private:

  friend class Info;
  friend class Board;
  friend class Game;
  friend class SecGame;
  friend class Player;

  int nb_players_;
  int rows_;
  int cols_;
  int nb_rounds_;

  int initial_health_;
  int nb_orks_;

  int cost_[6];

  int bonus_per_city_cell_;
  int bonus_per_path_cell_;

  int factor_connected_component_;
  
  /**
   * Reads the settings from a stream.
   */
  static Settings read_settings (istream& is);

};


inline string Settings::version () {
  return string(GAME_NAME) + " " + string(VERSION);
}

inline int Settings::nb_players () const {
  return nb_players_;
}

inline int Settings::rows () const {
  return rows_;
}

inline int Settings::cols () const {
  return cols_;
}

inline int Settings::nb_rounds () const {
  return nb_rounds_;
}

inline int Settings::initial_health () const {
  return initial_health_;
}

inline int Settings::nb_orks () const {
  return nb_orks_;
}

inline int Settings::cost_grass () const {
  return cost_[GRASS];
}

inline int Settings::cost_forest () const {
  return cost_[FOREST];
}

inline int Settings::cost_sand () const {
  return cost_[SAND];
}

inline int Settings::cost_city () const {
  return cost_[CITY];
}

inline int Settings::cost_path () const {
  return cost_[PATH];
}

inline int Settings::cost(CellType t) const {
  if (t != WATER) return cost_[t];
  else {
    cerr << "warning: cost requested for CellType WATER" << endl;
    return INT_MAX;
  }
}

inline int Settings::bonus_per_city_cell () const {
  return bonus_per_city_cell_;
}

inline int Settings::bonus_per_path_cell () const {
  return bonus_per_path_cell_;
}

inline int Settings::factor_connected_component () const {
  return factor_connected_component_;
}

inline bool Settings::player_ok (int pl) const {
  return pl >= 0 and pl < nb_players();
}

inline bool Settings::pos_ok (int i, int j) const {
  return i >= 0 and i < rows() and j >= 0 and j < cols();
}

inline bool Settings::pos_ok (Pos p) const {
  return pos_ok(p.i, p.j);
}


#endif
