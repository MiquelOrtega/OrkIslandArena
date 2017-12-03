#include "Game.hh"


void help (int argc, char** argv) {
  cout << "Usage: " << argv[0] << " [options] player1 player2 ... [< default.cnf] [> default.out] " << endl;
  cout << "Available options:" << endl;
  cout << "--seed=seed     -s seed     set random seed"                   << endl;
  cout << "--input=file    -i input    set input file  (default: stdin)"  << endl;
  cout << "--output=file   -o output   set output file (default: stdout)" << endl;
  cout << "--list          -l          list registered players"           << endl;
  cout << "--version       -v          print version"                     << endl;
  cout << "--help          -h          print help"                        << endl;
}


int main (int argc, char** argv) {
  if (argc == 1) {
    help(argc, argv);
    return EXIT_SUCCESS;
  }

  struct option long_options[] = {
    { "seed",    required_argument, 0, 's' },
    { "input",   required_argument, 0, 'i' },
    { "output",  required_argument, 0, 'o' },
    { "list",    no_argument,       0, 'l' },
    { "version", no_argument,       0, 'v' },
    { "help",    no_argument,       0, 'h' },
    { 0, 0, 0, 0 }
  };

  char* ifile = 0;
  char* ofile = 0;
  int seed = -1;
  vector<string> names;

  while (true) {
    int index = 0;
    int c = getopt_long(argc, argv, "s:i:o:lvh", long_options, &index);
    if (c == -1) break;

    switch (c) {
      case 's':
        seed = string_to_int(optarg);
        break;
      case 'i':
        ifile = optarg;
        break;
      case 'o':
        ofile = optarg;
        break;
      case 'l':
        Registry::print_players(cout);
        return EXIT_SUCCESS;
      case 'v':
        cout << Board::version() << endl;
        cout << "compiled " << __TIME__ << " " << __DATE__ << endl;
        return EXIT_SUCCESS;
      case 'h':
        help(argc, argv);
        return EXIT_SUCCESS;
      default:
        return EXIT_FAILURE;
    }
  }

  while (optind < argc) {
    names.push_back(argv[optind++]);
    _my_assert(names.back().size() <= 12, "Player name too long.");
  }

  _my_assert(seed >= 0, "Missing seed?");

  istream* is = ifile ? new ifstream(ifile) : &cin;
  ostream* os = ofile ? new ofstream(ofile) : &cout;

  Game::run(names, *is, *os, seed);

  if (ifile) delete is;
  if (ofile) delete os;
}
