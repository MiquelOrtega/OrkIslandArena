#include "Info.hh"

bool Info::ok() {

  // Borders should be water.
  for (int k = 0; k < rows(); ++k) {
    if (grid_[k][0].type != WATER) {
      cerr << "error: cell at position " << Pos(k, 0)
           << " does not contain water" << endl;
      return false;
    }
    if (grid_[k][cols()-1].type != WATER) {
      cerr << "error: cell at position " << Pos(k, cols()-1)
           << " does not contain water" << endl;
      return false;
    }
  }

  for (int k = 0; k < cols(); ++k) {
    if (grid_[0][k].type != WATER) {
      cerr << "error: cell at position " << Pos(0, k)
           << " does not contain water" << endl;
      return false;
    }
    if (grid_[rows()-1][k].type != WATER) {
      cerr << "error: cell at position " << Pos(rows()-1, k)
           << " does not contain water" << endl;
      return false;
    }
  }

  // grid_[i][j].type != CELL_TYPE_SIZE
  for (int i = 0; i < rows(); ++i)
    for (int j = 0; j < cols(); ++j)
      if (grid_[i][j].type == CELL_TYPE_SIZE) {
        cerr << "error: cell at position " << Pos(i, j)
             << " contains invalid cell type" << endl;
        return false;
      }
  
  // grid_[i][j].type == CITY iff grid_[i][j].city_id != -1
  map<int, int> cnt_cities;
  for (int i = 0; i < rows(); ++i)
    for (int j = 0; j < cols(); ++j) {
      CellType t = grid_[i][j].type;
      int     id = grid_[i][j].city_id;
      if (t == CITY and id == -1) {
        cerr << "error: CITY cell at position " << Pos(i, j)
             << "has invalid city identifier" << endl;
        return false;
      }
      if (t != CITY and id != -1) {
        cerr << "error: non-CITY cell at position " << Pos(i, j)
             << "has valid city identifier" << endl;
        return false;
      }
      if (id != -1) ++cnt_cities[id];
    }

  // (i, j) in city_[k] iff grid_[i][j].city_id == k.
  for (int k = 0; k < int(city_.size()); ++k) {
    if (cnt_cities[k] != int(city_[k].size())) {
      cerr << "error: mismatch in the number of cells of city " << k << endl;
      return false;
    }
    for (auto x: city_[k])
      if (grid_[x.i][x.j].city_id != k) {
        cerr << "error: CITY cell at position " << x
             << "has a mismatched city identifier" << endl;
        return false;
      }
  }

  // grid_[i][j].type == PATH iff grid_[i][j].path_id != -1
  map<int,int> cnt_paths;
  for (int i = 0; i < rows(); ++i)
    for (int j = 0; j < cols(); ++j) {
      CellType t = grid_[i][j].type;
      int     id = grid_[i][j].path_id;
      if (t == PATH and id == -1) {
        cerr << "error: PATH cell at position " << Pos(i, j)
             << "has invalid path identifier" << endl;
        return false;
      }
      if (t != PATH and id != -1) {
        cerr << "error: non-PATH cell at position " << Pos(i, j)
             << "has valid path identifier" << endl;
        return false;
      }
      if (id != -1) ++cnt_paths[id];
    }

  // (i, j) in path_[k] iff grid_[i][j].path_id == k.
  for (int k = 0; k < int(path_.size()); ++k) {
    if (cnt_paths[k] != int(path_[k].second.size())) {
      cerr << "error: mismatch in the number of cells of path " << k << endl;
      return false;
    }
    for (auto x: path_[k].second)
      if (grid_[x.i][x.j].path_id != k) {
        cerr << "error: PATH cell at position " << x
             << "has a mismatched path identifier" << endl;
        return false;
      }
  }

  // Paths connect legal cities.
  for (int k = 0; k < int(path_.size()); ++k) {
    if (not city_ok(path_[k].first.first )) {
      cerr << "error: path " << k << " has invalid city identifiers (1)" << endl;
      return false;
    }
    if (not city_ok(path_[k].first.second)) {
      cerr << "error: path " << k << " has invalid city identifiers (2)" << endl;
      return false;
    }
  }

  // If grid_[i][j].unit_id != -1 then grid_[i][j].type != WATER
  // Each unit occurs on the board exactly once.
  vector<bool> mkd(nb_units(), false);
  int cnt = 0;
  for (int i = 0; i < rows(); ++i)
    for (int j = 0; j < cols(); ++j) {
      CellType t = grid_[i][j].type;
      int     id = grid_[i][j].unit_id;
      if (id != -1) {
        if (t == WATER) {
          cerr << "error: WATER cells cannot have units" << endl;
          return false;
        }
        if (mkd[id]) {
          cerr << "error: unit " << id << " appears twice" << endl;
          return false;
        }
        mkd[id] = true;
        ++cnt;
      }
    }
  if (cnt != nb_units()) {
    cerr << "error: mismatch with units" << endl;
    return false;
  }

  // Unit info is valid and consistent with the linked data structures.
  for (int id = 0; id < nb_units(); ++id) {
    Unit u = unit_[id];
    if (u.id != id) {
      cerr << "error: mismatch with unit identifiers (1)" << endl;
      return false;
    }
    if (not player_ok(u.player)) {
      cerr << "error: player of unit is not valid" << endl;
      return false;
    }
    Pos p = u.pos;
    if (grid_[p.i][p.j].unit_id != id) {
      cerr << "error: mismatch with unit identifiers (2)" << endl;
      return false;
    }
    if (u.health < 0) {
      cerr << "error: health cannot be negative" << endl;
      return false;
    }
  }

  set<int> all;
  for (int pl = 0; pl < nb_players(); ++pl)
    for (int id : orks_[pl]) {
      all.insert(id);
      if (not unit_ok(id)) {
        cerr << "error: mismatch with players (1)" << endl;
        return false;
      }
      if (unit_[id].player != pl) {
        cerr << "error: mismatch with players (2)" << endl;
        return false;
      }
    }
  if (int(all.size ()) != nb_units()) {
    cerr << "error: number of units does not match" << endl;
    return false;
  }
  return true;
}
