#pragma GCC optimize "O3"
#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;

string to_string(string s) { return '"' + s + '"'; }

string to_string(const char* s) { return to_string((string)s); }

string to_string(char c) {
  string res = "";
  res += c;
  return res;
}

string to_string(bool b) { return (b ? "true" : "false"); }

template <typename A, typename B>
string to_string(pair<A, B> p) {
  return "(" + to_string(p.first) + ", " + to_string(p.second) + ")";
}

template <typename A>
string to_string(A v) {
  bool first = true;
  string res = "{";
  for (const auto& x : v) {
    if (!first) {
      res += ", ";
    }
    first = false;
    res += to_string(x);
  }
  res += "}";
  return res;
}

string to_string(uint8_t v) { return to_string((int)v); }
string to_string(uint16_t v) { return to_string((int)v); }

void debug_out() { cerr << endl; }

template <typename Head, typename... Tail>
void debug_out(Head H, Tail... T) {
  cerr << " " << to_string(H);
  debug_out(T...);
}

#define debug(...) cerr << "[" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)

class Timer {
 public:
  Timer() { restart(); }

  inline void restart() {
    start_time = getTimeOfDay();
    start_clock = getCycle();
  }
  inline double getElapsed() {
    if (rdtsc_per_sec_inv != -1)
      return (double)(getCycle() - start_clock) * rdtsc_per_sec_inv;

    const double RDTSC_MEASUREMENT_INTERVAL = 0.1;
    double res = getTimeOfDay() - start_time;
    if (res <= RDTSC_MEASUREMENT_INTERVAL) return res;

    rdtsc_per_sec_inv = 1.0 / (getCycle() - start_clock);
    rdtsc_per_sec_inv *= getTimeOfDay() - start_time;
    return getElapsed();
  }

 private:
  static double rdtsc_per_sec_inv;
  double start_time;
  unsigned long long int start_clock;

  inline double getTimeOfDay() {
    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec * 0.000001;
  }
  inline unsigned long long int getCycle() {
    unsigned int low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
  }
};
double Timer::rdtsc_per_sec_inv = -1;

inline unsigned get_random() {
  static unsigned y = 2463534242;
  return y ^= (y ^= (y ^= y << 13) >> 17) << 5;
}
inline unsigned get_random(unsigned v) { return get_random() % v; }
inline double get_random_double() { return (double)get_random() / UINT32_MAX; }
inline double get_random_log() { return log(get_random_double()); }

using ll = long long;
#if defined(LOCAL)
constexpr double TIME_LIMIT = 1.0;
#else
constexpr double TIME_LIMIT = 9.8;
#endif
constexpr int SIZE = 55;
constexpr int SS = SIZE * SIZE;
constexpr char EMP = 0x1f;
constexpr int scores[13] = {
    0,   // 0
    1,   // 1
    1,   // 2
    2,   // 3
    3,   // 4
    5,   // 5
    8,   // 6
    13,  // 7
    21,  // 8
    34,  // 9
    55,  // 10
    89,  // 11
    144  // 12
};
int W, H, S;
int WS[SS];
char WC[SS][12];
ll HASH[SS];
int bestScore = 0;
char result[SIZE][SIZE];

string toBit(ll x) {
  string s(64, '0');
  for (int i = 0; i < 64; ++i) s[i] = (x >> i) & 1;
  return s;
}

char toChar(char c) { return c ? c + 'A' - 1 : ' '; }

struct Word {
  int id, h, w, d;
  bool used = false;

  Word(int id) : id(id) {}
};

struct State {
  int score = 0;
  char X[SIZE][SIZE];
  uint8_t C[SIZE][SIZE];
  vector<Word> words;

  inline void init() {
    score = 0;
    memset(X, 0, sizeof(X));
    memset(C, 0, sizeof(C));
    for (auto& t : words) t.used = false;
  }

  inline void reverse() {
    char T[SIZE][SIZE];
    for (int i = 0; i < H; ++i) {
      for (int j = 0; j < W; ++j) {
        T[j][i] = result[i][j];
      }
    }
    memcpy(result, T, sizeof(T));
    swap(H, W);
  }

  inline bool in(int i, int j) {
    if (i < 0 || H <= i) return false;
    if (j < 0 || W <= j) return false;
    return true;
  }

  inline void put(int i, int j, char c) {
    if (in(i, j)) {
      C[i][j]++;
      X[i][j] = c;
    }
  }

  inline void remove(int i, int j) {
    if (in(i, j) && --C[i][j] == 0) {
      X[i][j] = 0;
    }
    assert(C[i][j] >= 0);
  }

  inline char getC(int i, int j) { return in(i, j) ? X[i][j] : 0; }

  inline bool IN(int i, int j) {
    char c = getC(i, j);
    return 0 < c && c < EMP;
  }

  void put(int i, int j, bool h, Word& w) {
    assert(!w.used);
    w.used = true;
    int id = w.id;
    int s = WS[id];
    w.h = i;
    w.w = j;
    w.d = h;
    score += scores[s];
    if (h) {
      put(i, j - 1, EMP);
      put(i, j + s, EMP);
      for (int k = 0; k < s; ++k) {
        put(i, j + k, WC[id][k]);
      }
    } else {
      put(i - 1, j, EMP);
      put(i + s, j, EMP);
      for (int k = 0; k < s; ++k) {
        put(i + k, j, WC[id][k]);
      }
    }
  }

  void remove(Word& w) {
    assert(w.used);
    w.used = false;
    int id = w.id;
    int s = WS[id];
    int i = w.h;
    int j = w.w;
    score -= scores[s];
    if (w.d) {
      remove(i, j - 1);
      remove(i, j + s);
      for (int k = 0; k < s; ++k) {
        remove(i, j + k);
      }
    } else {
      remove(i - 1, j);
      remove(i + s, j);
      for (int k = 0; k < s; ++k) {
        remove(i + k, j);
      }
    }
  }

  bool can(int i, int j, bool h, Word& w) {
    if (w.used) return false;
    int id = w.id;
    int s = WS[id];
    assert(s > 2);
    if (h) {
      if (j + s > W) return false;
      if (IN(i, j - 1)) return false;
      if (IN(i, j + s)) return false;
      for (int k = 0; k < s; ++k) {
        char c = X[i][j + k];
        if (c && c != WC[id][k]) return false;
        if (c == 0 && (IN(i - 1, j + k) || IN(i + 1, j + k))) return false;
      }
    } else {
      if (i + s > H) return false;
      if (IN(i - 1, j)) return false;
      if (IN(i + s, j)) return false;
      for (int k = 0; k < s; ++k) {
        char c = X[i + k][j];
        if (c && c != WC[id][k]) return false;
        if (c == 0 && (IN(i + k, j - 1) || IN(i + k, j + 1))) return false;
      }
    }
    return true;
  }

  inline bool can(ll h, ll m, Word& w) {
    int id = w.id;
    int s = WS[id];
    ll x = h ^ (HASH[id] & m);
    if (s < 12) x &= (1LL << ((s + 1) * 5)) - 1;
    return x == 0;
  }

  void solve(bool rev) {
    if (rev) reverse();
    int v[SS];
    State tmp;
    for (int t = 0; t < 50; ++t) {
      init();
      for (int i = 0; i < S; ++i) {
        v[i] = WS[i] * 100 + get_random(100);
      }
      sort(words.begin(), words.end(),
           [&](const Word& a, const Word& b) { return v[a.id] > v[b.id]; });
      for (Word& t : words) {
        [&]() {
          for (int i = 0; i < H; ++i) {
            for (int j = 0; j < W; ++j) {
              if (can(i, j, false, t)) {
                put(i, j, false, t);
                return;
              }
            }
          }
          for (int i = 0; i < H; ++i) {
            for (int j = 0; j < W; ++j) {
              if (can(i, j, true, t)) {
                put(i, j, true, t);
                return;
              }
            }
          }
        }();
      }
      if (bestScore < score) {
        bestScore = score;
        memcpy(result, X, sizeof(X));
      }
    }
    if (false) toDisplay();
    if (rev) reverse();
  }

  inline void toDisplay() {
    for (int i = 0; i < H; ++i) {
      for (int j = 0; j < W; ++j) {
        cerr << toChar(X[i][j]);
      }
      cerr << endl;
    }
    debug(score);
    // for (Word w : words) {
    //   if (w.used) continue;
    //   for (int i = 0; i < w.s; ++i) {
    //     cerr << toChar(w.c[i]);
    //   }
    //   cerr << endl;
    // }
  }
};
State cur;

class CrosswordPuzzler {
 public:
  vector<string> createPuzzle(int W, int H, vector<string> dict) {
    Timer timer;
    {  // input
      ::W = W;
      ::H = H;
      ::S = dict.size();
      for (int i = 0; i < S; ++i) {
        cur.words.emplace_back(Word(i));
        WS[i] = dict[i].size();
        for (int j = 0; j < WS[i]; ++j) {
          WC[i][j] = dict[i][j] - 'A' + 1;
          HASH[i] |= ll(WC[i][j]) << (j * 5);
        }
      }
    }
    {  // solve
      cur.solve(false);
      cur.solve(true);
    }
    {  // output
      vector<string> ret;
      for (int i = 0; i < H; i++) {
        ret.emplace_back(string(W, '-'));
        for (int j = 0; j < W; ++j) {
          char c = result[i][j];
          if (c && c != EMP) ret[i][j] = c + 'A' - 1;
        }
        // debug(ret[i]);
      }
      return ret;
    }
  }
};

#if defined(LOCAL)
int main(int argc, char* argv[]) {
  int seed = argc > 1 ? atoi(argv[1]) : -1;
  if (seed == -1) {
    cin >> W >> H >> S;
    vector<string> dict;
    for (int i = 0; i < S; i++) {
      string s;
      cin >> s;
      dict.emplace_back(s);
    }
    CrosswordPuzzler sol;
    vector<string> result = sol.createPuzzle(W, H, dict);
    cout << result.size() << endl;
    for (string s : result) cout << s << endl;
  } else {
  }
}
#endif