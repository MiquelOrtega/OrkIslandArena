#include "Registry.hh"


typedef map<string, Registry::Factory> dict_;


dict_* reg_ = 0;


int Registry::Register (const char* name, Factory factory) {
  if (reg_ == 0) reg_ = new dict_();
  (*reg_)[name] = factory;
  return 999;
}


Player* Registry::new_player (string name) {
  auto it = reg_->find(name);
  _my_assert(it != reg_->end(), "Player " + name + " not registered.");
  return (it->second)();
}


void Registry::print_players (ostream& os) {
  for (const auto& it : *reg_) cout << it.first << endl;
}
