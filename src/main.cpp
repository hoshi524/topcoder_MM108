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
int W, H, S;
int scores[13] = {
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
uint8_t C[SIZE][SIZE];
char X[SIZE][SIZE];
char result[SIZE][SIZE];

int score = 0;
int bestScore = 0;
int WS[SS], WH[SS], WW[SS];
char WC[SS][12];
ll HASH[SS];
bool WD[SS];

struct Word {
  int id;
  bool used = false;

  Word(int id) : id(id) {}
};
vector<Word> words;

void reverse() {
  char T[SIZE][SIZE];
  for (int i = 0; i < H; ++i) {
    for (int j = 0; j < W; ++j) {
      T[j][i] = X[i][j];
    }
  }
  memcpy(X, T, sizeof(X));
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
}

inline char get(int i, int j) { return in(i, j) ? X[i][j] : 0; }

inline bool IN(int i, int j) {
  char c = get(i, j);
  return 0 < c && c < EMP;
}

void put(int i, int j, bool h, Word& w) {
  assert(!w.used);
  w.used = true;
  int id = w.id;
  int s = WS[id];
  WH[id] = i;
  WW[id] = j;
  WD[id] = h;
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
  int i = WH[id];
  int j = WW[id];
  score -= scores[s];
  if (WD[id]) {
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

string toBit(ll x) {
  string s(64, '0');
  for (int i = 0; i < 64; ++i) s[i] = (x >> i) & 1;
  return s;
}

char toChar(char c) { return c ? c + 'A' - 1 : ' '; }

void solve(int minh, int maxh) {
  for (Word& t : words) {
    int id = t.id;
    if (t.used && minh <= WH[id] && WH[id] < maxh) remove(t);
  }
  if (minh + 6 < maxh) {
    for (int w = 0; w < W; ++w) {
      for (int i = 0; i < S; ++i) {
        Word& t = words[i];
        int s = minh + WS[t.id];
        if (s <= maxh && can(minh, w, false, t)) {
          put(minh, w, false, t);
          break;
        }
      }
    }
  }
  using t4 = tuple<int, int, int, int>;
  vector<t4> e;
  for (int h = minh; h < maxh - (maxh < H ? 1 : 0); ++h) {
    for (int w = 0; w + 2 < W; ++w) {
      if (IN(h, w - 1)) continue;
      ll q = -1;
      ll m = 0;
      for (int i = 0; i < 12; ++i) {
        if (w + i > W) continue;
        int i5 = i * 5;
        q = (q & ~(((1LL << 5) - 1) << i5)) | (ll(X[h][w + i]) << i5);
        if (X[h][w + i]) m |= ((1LL << 5) - 1) << i5;
      }
      for (int i = 0; i < S; ++i) {
        Word& t = words[i];
        if (t.used) continue;
        if (w + WS[t.id] <= W && can(q, m, t)) {
          e.emplace_back(WS[t.id], h, w, i);
        }
      }
    }
  }
  sort(e.begin(), e.end(),
       [](const t4& a, const t4& b) { return get<0>(a) > get<0>(b); });
  for (auto x : e) {
    int h = get<1>(x);
    int w = get<2>(x);
    int i = get<3>(x);
    if (can(h, w, true, words[i])) {
      put(h, w, true, words[i]);
    }
  }
}

void solve(bool rev) {
  if (rev) reverse();
  score = 0;
  memset(X, 0, sizeof(X));
  memset(C, 0, sizeof(C));
  for (auto& t : words) t.used = false;

  for (int h = 0; h < H;) {
    for (int i = 0; i < S; ++i) {
      Word& t = words[i];
      if (!t.used) {
        solve(h, min(h + WS[t.id], H));
        h += WS[t.id];
        break;
      }
    }
  }
  if (false) {
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

  if (rev) reverse();
  if (bestScore < score) {
    bestScore = score;
    memcpy(result, X, sizeof(X));
  }
}

class CrosswordPuzzler {
 public:
  vector<string> createPuzzle(int W, int H, vector<string> dict) {
    Timer timer;
    {  // input
      ::W = W;
      ::H = H;
      ::S = dict.size();
      for (int i = 0; i < S; ++i) {
        words.emplace_back(Word(i));
        WS[i] = dict[i].size();
        for (int j = 0; j < WS[i]; ++j) {
          WC[i][j] = dict[i][j] - 'A' + 1;
          HASH[i] |= ll(WC[i][j]) << (j * 5);
        }
      }
      sort(words.begin(), words.end(),
           [](const Word& a, const Word& b) { return WS[a.id] > WS[b.id]; });
    }
    {  // solve
      solve(false);
      solve(true);
      assert(score > 0);
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