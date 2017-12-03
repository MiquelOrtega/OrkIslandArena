#ifndef Utils_hh
#define Utils_hh

#include <cassert>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <getopt.h>
#include <string.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#include "Defs.hh"

using namespace std;


/** \file
 * Contains several useful includes, plus defines for errors and other
 * utilities.
 */


/**
 * Assert with message.
 */
#define _my_assert(b, s) { if (not (b)) { cerr << "error: " << s << endl; assert(b); } }


/**
 * Macro to specifically indicate when some code is unreachable,
 * so that the compiler doesn't cry when using NDEBUG.
 */
#define _unreachable() { _my_assert(false, "Unreachable code reached."); }


/**
 * C++11 to_string gives problems with Cygwin, so this is a replacement.
 */
inline string int_to_string (int i) {
    ostringstream oss;
    oss << i;
    return oss.str();
}

/**
 * C++11 stoi gives problems with Cygwin, so this is a replacement.
 */
inline int string_to_int (const string& s) {
    istringstream iss(s);
    int i;
    iss >> i;
    return i;
}


#endif
