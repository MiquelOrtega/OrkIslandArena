#include "Game.hh"


void Game::run (vector<string> names, istream& is, ostream& os, int seed) {
  cerr << "info: seed " << seed << endl;

  cerr << "info: loading game" << endl;
  Board b(is, seed);
  cerr << "info: loaded game" << endl;

  int np = b.nb_players();
  int nr = b.nb_rounds();

  _my_assert(np == (int)names.size(), "Wrong number of players.");

  vector<Player*> players;
  for (int pl = 0; pl < np; ++pl) {
    string name = names[pl];
    b.names_[pl] = name;
    cerr << "info: loading player " << name << endl;
    players.push_back(Registry::new_player(name));
    players[pl]->me_ = pl;
    players[pl]->set_random_seed(seed + pl + 1);
    *static_cast<Settings*>(players[pl]) = (Settings)b;
  }
  cerr << "info: players loaded" << endl;

  os << "Game" << endl << endl;
  os << "Seed " << seed << endl << endl;
  b.print_settings(os);
  b.print_names(os);
  b.print_state(os);

  for (int round = 0; round < nr; ++round) {
    cerr << "info: start round " << round << endl;
    vector<Action> actions(np);
    for (int pl = 0; pl < np; ++pl) {
      cerr << "info:     start player " << pl << endl;
      players[pl]->reset(b);
      players[pl]->play();
      actions[pl] = *players[pl];
      cerr << "info:     end player " << pl << endl;
    }

    b.next(actions, os);
    b.print_state(os);
    cerr << "info: end round " << round << endl;
  }

  b.print_results();

  cerr << "info: game played" << endl;
}
