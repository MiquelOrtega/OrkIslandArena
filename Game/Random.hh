#ifndef Random_hh
#define Random_hh


#include "Utils.hh"


/**
 * Defines a random generator.
 */
class Random_generator {

public:

  /**
   * Returns a random integer in [l..u]. u - l + 1 must be between 1 and 10^6.
   */
  int random (int l, int u);

  /**
   * Returns a random permutation of [0..n-1]. n must be between 0 and 10^6.
   */
  vector<int> random_permutation (int n);


  //////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////
  

private:

  friend class Board;
  friend class Game;
  friend class SecGame;

  static const long long RANDOM_MOD = ((long long)1)<<31;
  static const long long RANDOM_MASK = RANDOM_MOD - 1;

  long long rnd_seed;

  /**
   * Sets random seed.
   */
  void set_random_seed (int s) {
    if (s < 0) s = -s;
    rnd_seed = s & RANDOM_MASK;
  }

  /**
   * Computes next seed from current seed.
   */
  void next_rnd () {
    rnd_seed = (843314861*rnd_seed + 453816693) & RANDOM_MASK;
  }

  /**
   * Returns a real random number in [0, 1).
   */
  double uniform() {
    next_rnd();
    return double(rnd_seed)/RANDOM_MOD;
  }
  
};


inline int Random_generator::random (int l, int u) {
    if (l > u) return l; // wrong interval

    long long m = (long long)u - (long long)l + 1;
    if (m > 1e6) return l; // interval too long

    next_rnd();
    return l + int((u - l + 1)*uniform()); //...
  }

  /**
   * Returns a random permutation of [0..n-1]. n must be between 0 and 10^6.
   */
inline vector<int> Random_generator::random_permutation (int n) {
    if (n < 0 or n > 1e6) return vector<int>(0); // wrong n

    vector<int> v(n);
    for (int i = 0; i < n; ++i) v[i] = i;
    for (int i = 0; i < n; ++i) swap(v[i], v[random(i, n  - 1)]);
    return v;
  }

#endif
