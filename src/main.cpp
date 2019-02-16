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
constexpr char EMP = 0x1f;
int W, H, S;
int score = 0;
int bestScore = 0;
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
char X[SIZE][SIZE];
char result[SIZE][SIZE];

struct Word {
  int s;
  char c[12];
  ll hash = 0;
  bool used = false;

  Word(string x) {
    s = x.size();
    for (int i = 0; i < s; ++i) {
      c[i] = x[i] - 'A' + 1;
      hash |= ll(c[i]) << (i * 5);
    }
  }
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
  if (in(i, j)) X[i][j] = c;
}

inline char get(int i, int j) { return in(i, j) ? X[i][j] : 0; }

inline bool IN(int i, int j) {
  char c = get(i, j);
  return 0 < c && c < EMP;
}

void put(int i, int j, bool h, Word& w) {
  int s = w.s;
  assert(s > 2);
  w.used = true;
  score += scores[s];
  if (h) {
    put(i, j - 1, EMP);
    put(i, j + s, EMP);
    for (int k = 0; k < s; ++k) {
      put(i, j + k, w.c[k]);
    }
  } else {
    put(i - 1, j, EMP);
    put(i + s, j, EMP);
    for (int k = 0; k < s; ++k) {
      put(i + k, j, w.c[k]);
    }
  }
}

bool can(int i, int j, bool h, Word& w) {
  if (w.used) return false;
  int s = w.s;
  assert(s > 2);
  if (h) {
    if (j + s > W) return false;
    if (IN(i, j - 1)) return false;
    if (IN(i, j + s)) return false;
    for (int k = 0; k < s; ++k) {
      char c = X[i][j + k];
      if (c && c != w.c[k]) return false;
      if (k && IN(i - 1, j + k) && IN(i - 1, j + k - 1)) return false;
      if (k && IN(i + 1, j + k) && IN(i + 1, j + k - 1)) return false;
    }
  } else {
    if (j + s > H) return false;
    if (IN(i - 1, j)) return false;
    if (IN(i + s, j)) return false;
    for (int k = 0; k < s; ++k) {
      char c = X[i + k][j];
      if (c && c != w.c[k]) return false;
      if (k && IN(i + k, j - 1) && IN(i + k - 1, j - 1)) return false;
      if (k && IN(i + k, j + 1) && IN(i + k - 1, j + 1)) return false;
    }
  }
  return true;
}

inline bool can(ll h, ll m, Word& w) {
  ll x = h ^ (w.hash & m);
  if (w.s < 12) x &= (1LL << ((w.s + 1) * 5)) - 1;
  return x == 0;
}

string toBit(ll x) {
  string s(64, '0');
  for (int i = 0; i < 64; ++i) s[i] = (x >> i) & 1;
  return s;
}

char toChar(char c) { return c ? c + 'A' - 1 : ' '; }

void solve(bool rev) {
  if (rev) reverse();
  memset(X, 0, sizeof(X));
  score = 0;
  for (auto& t : words) t.used = false;

  int under[SIZE];
  memset(under, 0, sizeof(under));
  for (int i = 0, ok = 1; ok; ++i) {
    ok = 0;
    auto f = [&](int w) {
      int h = max(under[w], under[w + 2]);
      if (h == H) return;
      for (Word& t : words) {
        int s = h + t.s;
        if (t.used || H < s) continue;
        ok = 1;
        put(h, w, false, t);
        under[w + 1] = s;
        break;
      }
    };
    if (i & 1) {
      for (int w = W - 1 - (W & 1); w >= 0; w -= 2) f(w);
    } else {
      for (int w = 0; w < W; w += 2) f(w);
    }
  }
  using t4 = tuple<int, int, int, int>;
  vector<t4> e;
  for (int h = 0; h < H; ++h) {
    for (int w = 0; w + 2 < W; ++w) {
      if (w > 0 && X[h][w - 1]) continue;
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
        if (w + t.s <= W && can(q, m, t)) {
          e.emplace_back(t.s, h, w, i);
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
        words.emplace_back(Word(dict[i]));
      }
      sort(words.begin(), words.end(),
           [](const Word& a, const Word& b) { return a.s > b.s; });
    }
    {  // solve
      solve(false);
      solve(true);
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