#include "Settings.hh"

Settings Settings::read_settings (istream& is) {
  // Should match the format of *.cnf files, except for the last line of board generation.
  Settings r;
  string s, v;

  // Version, compared part by part.
  istringstream vs(version());
  while (!vs.eof()) {
    is >> s;
    vs >> v;
    _my_assert(s == v, "Problems when reading.");
  };

  is >> s >> r.nb_players_;
  _my_assert(s == "nb_players", "Expected 'nb_players' while parsing.");
  _my_assert(r.nb_players_ == 4, "Wrong number of players.");

  is >> s >> r.rows_;
  _my_assert(s == "rows", "Expected 'rows' while parsing.");
  _my_assert(r.rows_ >= 20, "Wrong number of rows.");

  is >> s >> r.cols_;
  _my_assert(s == "cols", "Expected 'cols' while parsing.");
  _my_assert(r.cols_ >= 20, "Wrong number of columns.");

  is >> s >> r.nb_rounds_;
  _my_assert(s == "nb_rounds", "Expected 'nb_rounds' while parsing.");
  _my_assert(r.nb_rounds_ >= 1, "Wrong number of rounds.");

  is >> s >> r.initial_health_;
  _my_assert(s == "initial_health", "Expected 'initial_health' while parsing.");
  _my_assert(r.initial_health_ > 0, "Wrong initial health.");

  is >> s >> r.nb_orks_;
  _my_assert(s == "nb_orks", "Expected 'nb_orks' while parsing.");
  _my_assert(r.nb_orks_ >= 1, "Wrong number of orks.");
  _my_assert(r.rows_ * r.cols_ >= 25 * r.nb_players_ * r.nb_orks_, "Wrong parameters.");

  is >> s >> r.cost_[GRASS];
  _my_assert(s == "cost_grass", "Expected 'cost_grass' while parsing.");
  _my_assert(r.cost_[GRASS] >= 0, "Cost of grass should be non-negative");

  is >> s >> r.cost_[FOREST];
  _my_assert(s == "cost_forest", "Expected 'cost_forest' while parsing.");
  _my_assert(r.cost_[FOREST] >= 0, "Cost of forest should be non-negative");

  is >> s >> r.cost_[SAND];
  _my_assert(s == "cost_sand", "Expected 'cost_sand' while parsing.");
  _my_assert(r.cost_[SAND] >= 0, "Cost of sand should be non-negative");

  is >> s >> r.cost_[CITY];
  _my_assert(s == "cost_city", "Expected 'cost_city' while parsing.");
  _my_assert(r.cost_[CITY] >= 0, "Cost of city should be non-negative");

  is >> s >> r.cost_[PATH];
  _my_assert(s == "cost_path", "Expected 'cost_path' while parsing.");
  _my_assert(r.cost_[PATH] >= 0, "Cost of path should be non-negative");

  is >> s >> r.bonus_per_city_cell_;
  _my_assert(s == "bonus_per_city_cell", "Expected 'bonus_per_city_cell' while parsing.");
  _my_assert(r.bonus_per_city_cell_ >= 1, "Wrong bonus per city cell.");

  is >> s >> r.bonus_per_path_cell_;
  _my_assert(s == "bonus_per_path_cell", "Expected 'bonus_per_path_cell' while parsing.");
  _my_assert(r.bonus_per_path_cell_ >= 1, "Wrong bonus per path cell.");

  is >> s >> r.factor_connected_component_;
  _my_assert(s == "factor_connected_component", "Expected 'factor_connected_component' while parsing.");
  _my_assert(r.factor_connected_component_ >= 1, "Wrong factor for connected components.");
  
  _my_assert(r.rows_ == r.cols_, "Board should be square.");

  return r;
}
