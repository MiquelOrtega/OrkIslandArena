#include "Player.hh"

void Player::reset (ifstream& is) {

  // Should read what Board::print_state() prints.
  // Should fill the same data structures as
  // Board::Board (istream& is, int seed), except for settings and names.
  
  *(Action*)this = Action();

  read_grid(is);

  string s;
  is >> s >> round_;
  _my_assert(s == "round", "Expected 'round' while parsing.");
  _my_assert(round_ >= 0 and round_ < nb_rounds(), "Round is not ok.");

  is >> s;
  _my_assert(s == "total_score", "Expected 'total_score' while parsing.");
  total_score_ = vector<int>(nb_players());
  for (auto& ts : total_score_) {
    is >> ts;
    _my_assert(ts >= 0, "Total score cannot be negative.");
  }

  is >> s;
  _my_assert(s == "status", "Expected 'status' while parsing.");
  cpu_status_ = vector<double>(nb_players());
  for (auto& st : cpu_status_) {
    is >> st;
    _my_assert(st == -1 or (st >= 0 and st <= 1), "Status is not ok.");
  }

  is >> s;
  _my_assert(s == "city_owners", "Expected 'city_owners' while parsing.");
  city_owner_ = vector<int>(nb_cities());
  for (int& co : city_owner_) {
    is >> co;
    _my_assert(co == -1 or (co >= 0 and co <= nb_players()), "City owner is not ok.");
  }

  is >> s;
  _my_assert(s == "path_owners", "Expected 'path_owners' while parsing.");
  path_owner_ = vector<int>(nb_paths());
  for (int& po : path_owner_) {
    is >> po;
    _my_assert(po == -1 or (po >= 0 and po <= nb_players()), "Path owner is not ok.");
  }

  is >> s;
  _my_assert(s == "units", "Expected 'units' while parsing.");

  unit_ = vector<Unit>( nb_players() * nb_orks() );
  orks_= vector< vector<int> >(nb_players());

  for (int id = 0; id < nb_units(); ++id) {
    int pl, i, j, h;
    bool read(is >> pl >> i >> j >> h);
    _my_assert(read, "Could not read info for unit " + int_to_string(id) + ".");
    _my_assert(pos_ok(i, j), "Position is not ok.");
    _my_assert(cell(i, j).type != WATER, "Cell should be water.");
    _my_assert(cell(i, j).unit_id == -1, "Cell should not have any unit.");
    _my_assert(h >= 0, "Health should be non-negative");
    grid_[i][j].unit_id = id;
    unit_[id] = Unit(id, pl, Pos(i, j), h);
    orks_[pl].push_back(id);
  }

  _my_assert(ok(), "Invariants are not satisfied.");
}
