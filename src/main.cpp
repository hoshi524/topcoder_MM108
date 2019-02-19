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
constexpr double TIME_LIMIT = 3;
#else
constexpr double TIME_LIMIT = 9.5;
#endif
constexpr int SIZE = 50;
constexpr int SS = SIZE * SIZE;
constexpr char EMP = 0x1f;
constexpr char JK = EMP - 1;
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
int bestScore = 0;
char result[SIZE][SIZE];

char toChar(char c) { return c ? c + 'A' - 1 : ' '; }

vector<int> ids;
vector<int> sids[13];
using t4 = tuple<int, int, bool, int>;
vector<t4> temp;

struct State {
  int score = 0;
  char X[SIZE][SIZE];
  uint8_t WH[SS];
  uint8_t WW[SS];
  bool WD[SS];
  bool used[SS];

  inline void setTemp() {
    memset(X, 0, sizeof(X));
    memset(used, 0, sizeof(used));
    temp.clear();
    for (int id : ids) {
      if (used[id]) continue;
      int s = WS[id];
      if (s < 8) break;
      [&]() {
        for (int i = 0; i < H; ++i) {
          for (int j = 0; j < W; ++j) {
            if (can_(i, j, false, id)) {
              put(i, j, false, id);
              temp.emplace_back(i, j, false, s);
              return;
            }
          }
        }
        for (int j = 0; j < W; ++j) {
          for (int i = 0; i < H; ++i) {
            if (can_(i, j, true, id)) {
              put(i, j, true, id);
              temp.emplace_back(i, j, true, s);
              return;
            }
          }
        }
      }();
    }
    score = 0;
    memset(used, 0, sizeof(used));
    for (int i = 0; i < H; ++i) {
      for (int j = 0; j < W; ++j) {
        if (X[i][j] == 0) continue;
        if (X[i][j] < EMP) X[i][j] = JK;
      }
    }
  }

  inline bool applyTemp() {
    for (t4& t : temp) {
      int i, j, s;
      bool h;
      tie(i, j, h, s) = t;
      if ([&]() {
            for (int id : sids[s]) {
              if (can(i, j, h, id)) {
                put(i, j, h, id);
                return false;
              }
            }
            return true;
          }())
        return false;
    }
    update();
    return true;
  }

  inline bool in(int i, int j) {
    if (i < 0 || H <= i) return false;
    if (j < 0 || W <= j) return false;
    return true;
  }

  inline void put(int i, int j, char c) {
    if (in(i, j)) X[i][j] = c;
  }

  inline char getC(int i, int j) { return in(i, j) ? X[i][j] : 0; }

  inline bool IN(int i, int j) {
    char c = getC(i, j);
    return 0 < c && c < EMP;
  }

  void put(int i, int j, bool h, int id) {
    assert(!used[id]);
    int s = WS[id];
    WH[id] = i;
    WW[id] = j;
    WD[id] = h;
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
    score += scores[s];
    used[id] = true;
  }

  bool can(int i, int j, bool h, int id) {
    if (used[id]) return false;
    int s = WS[id];
    if (h) {
      if (j + s > W) return false;
      if (IN(i, j - 1)) return false;
      if (IN(i, j + s)) return false;
      for (int k = 0; k < s; ++k) {
        char c = X[i][j + k];
        if (c && c != JK && c != WC[id][k]) return false;
        if (c == 0 && (IN(i - 1, j + k) || IN(i + 1, j + k))) return false;
      }
    } else {
      if (i + s > H) return false;
      if (IN(i - 1, j)) return false;
      if (IN(i + s, j)) return false;
      for (int k = 0; k < s; ++k) {
        char c = X[i + k][j];
        if (c && c != JK && c != WC[id][k]) return false;
        if (c == 0 && (IN(i + k, j - 1) || IN(i + k, j + 1))) return false;
      }
    }
    return true;
  }

  bool can_(int i, int j, bool h, int id) {
    if (used[id]) return false;
    int s = WS[id];
    if (h) {
      if (j + s > W) return false;
      if (IN(i, j - 1)) return false;
      if (IN(i, j + s)) return false;
      for (int k = 0; k < s; ++k) {
        char c = X[i][j + k];
        if (c) return false;
        if (c == 0 && (IN(i - 1, j + k) || IN(i + 1, j + k))) return false;
      }
    } else {
      if (i + s > H) return false;
      if (IN(i - 1, j)) return false;
      if (IN(i + s, j)) return false;
      for (int k = 0; k < s; ++k) {
        char c = X[i + k][j];
        if (c) return false;
        if (c == 0 && (IN(i + k, j - 1) || IN(i + k, j + 1))) return false;
      }
    }
    return true;
  }

  inline void update() {
    if (bestScore < score) {
      bestScore = score;
      memcpy(result, X, sizeof(X));
    }
  }
};
State cur;

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

Timer timer;

void solve(bool rev) {
  {
    int v[SS];
    for (int id : ids) {
      v[id] = WS[id] * 100 + get_random(100);
    }
    sort(ids.begin(), ids.end(), [&](int a, int b) { return v[a] > v[b]; });
  }
  if (rev) reverse();
  cur.setTemp();
  int count[13];
  for (int i = 0; i < 13; ++i) count[i] = sids[i].size();
  for (t4 t : temp) count[get<3>(t)]--;
  for (int id : ids) {
    if (count[WS[id]] == 0) continue;
    if (timer.getElapsed() > TIME_LIMIT) break;
    [&]() {
      for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
          if (cur.can(i, j, true, id)) {
            State tmp = cur;
            tmp.put(i, j, true, id);
            if (tmp.applyTemp()) {
              cur.put(i, j, true, id);
              count[WS[id]]--;
              return;
            }
          }
          if (cur.can(i, j, false, id)) {
            State tmp = cur;
            tmp.put(i, j, false, id);
            if (tmp.applyTemp()) {
              cur.put(i, j, false, id);
              count[WS[id]]--;
              return;
            }
          }
        }
      }
    }();
  }
  if (rev) reverse();
}

class CrosswordPuzzler {
 public:
  vector<string> createPuzzle(int W, int H, vector<string> dict) {
    {  // input
      ::W = W;
      ::H = H;
      ::S = dict.size();
      for (int i = 0; i < S; ++i) {
        WS[i] = dict[i].size();
        ids.emplace_back(i);
        sids[WS[i]].emplace_back(i);
        for (int j = 0; j < WS[i]; ++j) {
          WC[i][j] = dict[i][j] - 'A' + 1;
        }
      }
    }
    {  // solve
      for (int i = 0; timer.getElapsed() < TIME_LIMIT; i = 1 - i) {
        solve(i);
      }
    }
    {  // output
      vector<string> ret;
      for (int i = 0; i < H; i++) {
        ret.emplace_back(string(W, '-'));
        for (int j = 0; j < W; ++j) {
          char c = result[i][j];
          if (c && c < JK) ret[i][j] = c + 'A' - 1;
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