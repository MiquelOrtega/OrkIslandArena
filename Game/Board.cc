#include "Board.hh"
#include "Action.hh"


const char Board::uNDEF = gRASS;
const char Board::rIVER = 'R';


void Board::generate_units () {
  orks_ = vector< vector<int> >(nb_players(), vector<int>(nb_orks()));
  unit_ = vector<Unit>(nb_players() * nb_orks());
  for (int id = 0, pl = 0; pl < nb_players(); ++pl) {
    for (int u = 0; u < nb_orks(); ++u, ++id) {
      orks_[pl][u] = id;
      unit_[id].id = id;
      unit_[id].player = pl;
      unit_[id].health = initial_health();
    }
    spawn(orks_[pl]);
  }
}


Board::Board (istream& is, int seed) {
  set_random_seed(seed);
  *static_cast<Settings*>(this) = Settings::read_settings(is);
  names_ = vector<string>(nb_players());
  read_generator_and_grid(is);

  round_ = 0;
  total_score_ = vector<int>   (nb_players(), 0);
  cpu_status_  = vector<double>(nb_players(), 0);

  city_owner_ = vector<int>(city_.size(), -1);
  path_owner_ = vector<int>(path_.size(), -1);
  generate_units();
  _my_assert(ok(), "Invariants are not satisfied.");
}


void Board::print_settings (ostream& os) const {
  // Should match the format of *.cnf files, except for the last line of board generation.
  os << version() << endl;
  os << "nb_players                  " << nb_players()                 << endl;
  os << "rows                        " << rows()                       << endl;
  os << "cols                        " << cols()                       << endl;
  os << "nb_rounds                   " << nb_rounds()                  << endl;
  os << "initial_health              " << initial_health()             << endl;
  os << "nb_orks                     " << nb_orks()                    << endl;
  os << "cost_grass                  " << cost_grass()                 << endl;
  os << "cost_forest                 " << cost_forest()                << endl;
  os << "cost_sand                   " << cost_sand()                  << endl;
  os << "cost_city                   " << cost_city()                  << endl;
  os << "cost_path                   " << cost_path()                  << endl;
  os << "bonus_per_city_cell         " << bonus_per_city_cell()        << endl;
  os << "bonus_per_path_cell         " << bonus_per_path_cell()        << endl;
  os << "factor_connected_component  " << factor_connected_component() << endl;
}


void Board::print_names (ostream& os) const {
  os << "names         ";
  for (int pl = 0; pl < nb_players(); ++pl) os << ' ' << name(pl);
  os << endl;
}


void Board::print_state (ostream& os) const {

  // Should start with the same format of Info::read_grid.
  // Then other data describing the state.

  os << endl << endl;

  os << "   ";
  for (int j = 0; j < cols(); ++j)
    os << j / 10;
  os << endl;

  os << "   ";
  for (int j = 0; j < cols(); ++j)
    os << j % 10;
  os << endl;

  for (int i = 0; i < rows(); ++i) {
    os << i / 10 << i % 10 << " ";
    for (int j = 0; j < cols(); ++j) {
      const Cell& c = grid_[i][j];
      os << CellType2char(c.type);
    }
    os << endl;
  }

  os << endl;
  os << "cities " << city_.size() << endl;
  for (int k = 0; k < int(city_.size()); ++k) {
    os << endl << city_[k].size() << endl;
    for (auto x: city_[k]) {
      os << x.i << " " << x.j << endl;
    }
  }

  os << endl;
  os << "paths " << path_.size() << endl;
  for (int k = 0; k < int(path_.size()); ++k) {
    os << endl
       << path_[k].first.first << " " << path_[k].first.second << " "
       << path_[k].second.size() << endl;
    for (auto x: path_[k].second) {
      os << x.i << " " << x.j << endl;
    }
  }

  os << endl;
  os << "round " << round() << endl;

  os << "total_score";
  for (auto ts : total_score_) os << " " << ts;
  os << endl;

  os << "status";
  for (auto st : cpu_status_) os << " " << st;
  os << endl;

  os << endl;
  os << "city_owners" << endl;
  for (int owner : city_owner_) os << " " << owner;
  os << endl;

  os << endl;
  os << "path_owners" << endl;
  for (int owner : path_owner_) os << " " << owner;
  os << endl;

  os << endl;
  os << "units" << endl;
  for (int id = 0; id < nb_units(); ++id) {
    print_unit(unit(id), os);
    os << endl;
  }
  os << endl;
}


void Board::print_results () const {
  int max_score = 0;
  vector<int> v;
  for (int pl = 0; pl < nb_players(); ++pl) {
    cerr << "info: player " << name(pl)
         << " got score " << total_score(pl) << endl;
    if (total_score(pl) > max_score) {
      max_score = total_score(pl);
      v = vector<int>(1, pl);
    }
    else if (total_score(pl) == max_score) v.push_back(pl);
  }

  cerr << "info: player(s)";
  for (int pl : v) cerr << " " << name(pl);
  cerr << " got top score" << endl;
}


void Board::next(const vector<Action>& act, ostream& os) {

  _my_assert(ok(), "Invariants are not satisfied.");

  ++round_;

  int np = nb_players();
  int nu = nb_units();

  // Chooses (at most) one command per unit.
  vector<bool> seen(nu, false);
  vector<Command> v;
  for (int pl = 0; pl < np; ++pl)
    for (const Command& m : act[pl].v_) {
      int id = m.id;
      Dir dir = m.dir;
      if (not unit_ok(id))
        cerr << "warning: id out of range : " << id << endl;
      else if (unit(id).player != pl)
        cerr << "warning: unit " << id << " of player " << unit(id).player
             << " not owned by " << pl << endl;
      else {
        // Here it is an assert because repetitions should have already been filtered out.
        _my_assert(not seen[id], "More than one command for the same unit.");
        seen[id] = true;
        if (not dir_ok(dir))
          cerr << "warning: direction not valid: " << dir << endl;
        else if (dir != NONE)
          v.push_back(Command(id, dir));
      }
    }

  // Executes commands using a random order.
  int num = v.size();
  vector<int> perm = random_permutation(num);
  vector<bool> killed(nu, false);
  vector<Command> commands_done;
  for (int i = 0; i < num; ++i) {
    Command m = v[perm[i]];
    if (not killed[m.id] and move(m.id, m.dir, killed))
      commands_done.push_back(m);
  }
  os << "commands" << endl;
  Action::print(commands_done, os);

  // To ensure that executions of Game and SecGame are the same.
  for (int pl = 0; pl < np; ++pl)
    sort(orks_[pl].begin(), orks_[pl].end());

  vector<int> dead;
  for (int id = 0; id < nu; ++id)
    if (killed[id]) dead.push_back(id);

  spawn(dead);

  compute_total_scores();

  _my_assert(ok(), "Invariants are not satisfied.");
}


void Board::spawn(const vector<int>& gen) {

  // Generate set of candidate positions for generation.
  set<Pos> cands;
  for (int i = 0; i < rows(); ++i) {
    int j;
    for (j = 0;    j < cols() and cell(i, j).type == WATER; ++j) ;
    if (j < cols()) cands.insert(Pos(i, j));

    for (j = cols()-1; j >= 0 and cell(i, j).type == WATER; --j) ;
    if (j >= 0)     cands.insert(Pos(i, j));
  }

  // Regenerate killed units using valid candidate positions.
  for (int id : gen) {
    Pos pos = Pos(-1, -1);
    while (pos == Pos(-1, -1) and not cands.empty()) {
      int k = random(0, cands.size()-1);
      auto it = cands.begin();
      advance(it, k);
      if (valid_to_spawn(*it)) pos = *it;
      cands.erase(it);
    }
    if (pos == Pos(-1, -1)) // This should very very rarely happen.
      for (int i = 0; i < rows() and pos == Pos(-1, -1); ++i)
        for (int j = 0; j < cols() and pos == Pos(-1, -1); ++j)
          if (valid_to_spawn(Pos(i, j)))
            pos = Pos(i, j);
    _my_assert(pos != Pos(-1, -1), "Cannot find a cell to regenerate units");
    place(id, pos);
  }
}


bool Board::valid_to_spawn(Pos pos) {
  CellType t = cell(pos).type;
  if (t == WATER or t == CITY or t == PATH) return false;
  for (int d = 0; d <= NONE; ++d) {
    Pos pos2 = pos + Dir(d);
    if (pos_ok(pos2) and cell(pos2).unit_id != -1)
      return false;
  }
  return true;
}


void Board::place(int id, Pos p) {
  _my_assert(unit_ok(id), "Invalid identifier.");
  _my_assert( pos_ok( p), "Invalid position.");
  unit_[id].pos = p;
  grid_[p.i][p.j].unit_id = id;
}


void Board::kill(int id, int pl, vector<bool>& killed) {
  _my_assert(   unit_ok(id), "Invalid identifier.");
  _my_assert( player_ok(pl), "Invalid player.");
  _my_assert(not killed[id], "Cannot already be dead.");
  killed[id] = true;

  Unit& u = unit_[id];
  grid_[u.pos.i][u.pos.j].unit_id = -1;

  if (pl != u.player) {
    auto& o = orks_[u.player];
    auto it = find(o.begin(), o.end(), id);
    _my_assert(it != o.end(), "Cannot find id to kill.");
    swap(*it, *o.rbegin());
    o.pop_back();
    orks_[pl].push_back(id);
    u.player = pl;
  }
  u.pos    = Pos(-1, -1);
  u.health = initial_health();
}


// id is a valid unit id, moved by its player, and d is a valid dir != NONE.
bool Board::move(int id, Dir dir, vector<bool>& killed) {
  _my_assert(unit_ok(id ), "Invalid identifier.");
  _my_assert( dir_ok(dir), "Invalid direction");
  _my_assert( dir != NONE, "Direction cannot be NONE");
  Unit& u = unit_[id];
  Pos p1 = u.pos;
  _my_assert(pos_ok(p1), "Initial position in movement is not ok.");

  Cell& c1 = grid_[p1.i][p1.j];
  _my_assert(c1.type != WATER, "Initial position cannot be water.");

  Pos p2 = p1 + dir;
  if (not pos_ok(p2)) return false;

  Cell& c2 = grid_[p2.i][p2.j];
  if (c2.type == WATER) return false;

  int id2 = c2.unit_id;
  if (id2 != -1) {
    _my_assert(unit_ok(id2), "Invalid identifier.");
    Unit& u2 = unit_[id2];
    _my_assert(u2.health >= 0, "Health cannot be negative.");
    if (u2.player == u.player) return false;
    u.health -= cost_[c2.type];
    bool u_kills_u2 =
      u.health > u2.health or (u.health == u2.health and random(0, 1));
    if (u_kills_u2) kill(id2,  u.player, killed);
    else          { kill(id,  u2.player, killed); return false; }
  }
  else u.health -= cost_[c2.type];

  if (u.health < 0) {
    kill(id, u.player, killed);
    return false;
  }
  else {
    c1.unit_id = -1;
    c2.unit_id = id;
    u.pos = p2;
    return true;
  }
}


void Board::compute_scores_city_or_path(int bonus, const vector<Pos>& v, int& owner) {

  vector<int> sc(nb_players(), 0);
  for (Pos p : v) {
    int uid = cell(p).unit_id;
    if (uid != -1) ++sc[unit(uid).player];
  }
  int max_sc = 0;
  int max_pl = -1; // *Only* player with maximum score (-1 if more than one).
  for (int pl = 0; pl < nb_players(); ++pl) {
    if (sc[pl] > max_sc) {
      max_sc = sc[pl];
      max_pl = pl;
    }
    else if (sc[pl] == max_sc) max_pl = -1;
  }
  if (max_pl != -1) {     // Change of owner.
    total_score_[max_pl] += bonus * v.size();
    owner = max_pl;
  }
  else if (owner != -1) { // The owner is the same.
    total_score_[owner] += bonus * v.size();
  }
}


int size_of_connected_component_of(int u,
                                   const vector<vector<int>>& g,
                                   vector<bool>& mkd) {
  mkd[u] = true;
  int s = 1;
  for (int v : g[u])
    if (not mkd[v])
      s += size_of_connected_component_of(v, g, mkd);
  return s;
}


void Board::compute_scores_graph(int pl) {
  map<int, int> id;
  int sz = 0;
  for (int k = 0; k < int(city_.size()); ++k)
    if (city_owner_[k] == pl) {
      id[k] = sz;
      ++sz;
    }
  // There could be repeated edges, but there will be few.
  vector<vector<int>> g(sz);
  for (int k = 0; k < int(path_.size()); ++k) {
    int a = path_[k].first.first;
    int b = path_[k].first.second;
    if (path_owner_[k] == pl and
        city_owner_[a] == pl and
        city_owner_[b] == pl) {
      int ida = id[a];
      int idb = id[b];
      g[ida].push_back(idb);
      g[idb].push_back(ida);
    }
  }

  vector<bool> mkd(sz, false);
  for (int u = 0; u < sz; ++u)
    if (not mkd[u]) {
      int s = size_of_connected_component_of(u, g, mkd);
      _my_assert(0 <= s and s <= 25, "Unexpected size of connected component.");
      total_score_[pl] += factor_connected_component() * int(1 << s);
    }
}


void Board::compute_total_scores () {

  for (int k = 0; k < int(city_.size()); ++k)
    compute_scores_city_or_path(bonus_per_city_cell(), city_[k], city_owner_[k]);

  for (int k = 0; k < int(path_.size()); ++k)
    compute_scores_city_or_path(bonus_per_path_cell(), path_[k].second, path_owner_[k]);

  for (int pl = 0; pl < nb_players(); ++pl)
    compute_scores_graph(pl);
}


// ***************************************************************************

// S, E, N, W
const vector<int> Board::DIRI4 = { 1,  0, -1,  0};
const vector<int> Board::DIRJ4 = { 0,  1,  0, -1};

// SW, S, SE, E, NE, N, NW, W
const vector<int> Board::DIRI8 = {  1,  1,  1,  0, -1, -1, -1,  0};
const vector<int> Board::DIRJ8 = { -1,  0,  1,  1,  1,  0, -1, -1};

void Board::generator1 (const vector<int>& param) {

  _my_assert(param.empty(), "GENERATOR1 requires no parameter.");

  margin = min(rows(), cols()) / 5;

  bool found = false;
  while (not found) {

    m = vector<vector<char>>(rows(), vector<char>(cols(), uNDEF));
    city_.clear();
    path_.clear();

    fill_borders_with_water();
    place_sea();
    place_forests();
    place_deserts();
    place_rivers();
    place_cities();
    place_paths();

    recode_rivers();
    fill_gaps_of_water();

    found = valid();
  }

  grid_ = vector< vector<Cell> >(rows(), vector<Cell>(cols()));
  for (int i = 0; i < rows(); ++i)
    for (int j = 0; j < cols(); ++j)
      grid_[i][j].type = char2CellType(m[i][j]);

  for (int k = 0; k < int(city_.size()); ++k)
    for (auto x: city_[k]) {
      _my_assert(grid_[x.i][x.j].type == CITY, "Mismatch with cities.");
      grid_[x.i][x.j].city_id = k;
    }

  for (int k = 0; k < int(path_.size()); ++k)
    for (auto x: path_[k].second) {
      _my_assert(grid_[x.i][x.j].type == PATH, "Mismatch with paths.");
      grid_[x.i][x.j].path_id = k;
    }
}
