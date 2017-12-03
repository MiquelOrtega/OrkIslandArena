#ifndef Info_hh
#define Info_hh


#include "Settings.hh"
#include "State.hh"


/*! \file
 * Contains a class to store most of the information of the game.
 */


/**
 * Stores all the information of the game,
 * except the names of the vectors and the random generator of the board.
 */
class Info : public Settings, public State {

  friend class Game;
  friend class SecGame;

public:

  /**
   * Reads the grid of the board.
   * Should fill the same data structures as a board generator.
   */
  void read_grid (istream& is) {
    string l;
    is >> l; // Read 1st line of column labels.
    is >> l; // Read 2nd line of column labels.
    grid_ = vector< vector<Cell> >(rows(), vector<Cell>(cols()));
    for (int i = 0; i < rows(); ++i) {
      string s;
      is >> l >> s;
      // Read row label in l and row in s.
      _my_assert((int)s.size() == cols(),
                 "The read map has a line with incorrect lenght.");
      for (int j = 0; j < cols(); ++j)
        grid_[i][j].type = char2CellType(s[j]);
    }

    int nb_cities_;
    is >> l >> nb_cities_;
    _my_assert(l == "cities", "Expected 'cities'.");
    city_ = vector<City>(nb_cities_);
    for (auto& x : city_) {
      int sz;
      is >> sz;
      x = City(sz);
      for (Pos& p : x) {
        is >> p.i >> p.j;
        _my_assert(pos_ok(p), "Position of city is not ok.");
      }
    }
    int nb_paths_;
    is >> l >> nb_paths_;
    _my_assert(l == "paths", "Expected 'paths'.");
    path_ = vector<Path>(nb_paths_);
    for (auto& x : path_) {
      int a, b, sz;
      is >> a >> b >> sz;
      x = {{a, b}, vector<Pos>(sz)};
      for (Pos& p : x.second) {
        is >> p.i >> p.j;
        _my_assert(pos_ok(p), "Position of path is not ok.");
      }
    }

    for (int k = 0; k < int(city_.size()); ++k)
      for (auto x: city_[k]) {
        _my_assert(grid_[x.i][x.j].type == CITY, "Should be city.");
        grid_[x.i][x.j].city_id = k;
      }

    for (int k = 0; k < int(path_.size()); ++k)
      for (auto x: path_[k].second) {
        _my_assert(grid_[x.i][x.j].type == PATH, "Should be path.");
        grid_[x.i][x.j].path_id = k;
      }
  }

  /**
   * Checks invariants are preserved.
   */
  bool ok();
};

#endif
