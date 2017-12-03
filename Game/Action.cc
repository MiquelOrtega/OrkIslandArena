#include "Action.hh"

Action::Action (istream& is) {
  u_.clear();
  v_.clear();

  // Warning: all read operations must be checked for SecGame.
  int i;
  while (is >> i and i != -1) {
    char d;
    if (is >> d) {
      u_.insert(i);
      v_.push_back(Command(i, c2d(d)));
    }
    else {
      cerr << "warning: only half an operation given for unit " << i << endl;
      return;
    }
  }
}

void Action::print (const vector<Command>& commands, ostream& os) {
  for (Command a : commands) os << a.id << ' ' << d2c(a.dir) << endl;
  os << -1 << endl;
}
