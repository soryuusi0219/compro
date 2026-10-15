// Compile with -DTEMPLATE_KIND=0 (main), 1 (main_noACL), or 2 (tester).
#ifndef TEMPLATE_KIND
#define TEMPLATE_KIND 0
#endif

// Including a template under a different entry-point name lets the tests run
// without starting tester's infinite loop. Renaming main removes its implicit
// return 0, so suppress that warning only while parsing the included template.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main template_entry_point
#if TEMPLATE_KIND == 0
#include "templates/main.cpp"
#elif TEMPLATE_KIND == 1
#include "templates/main_noACL.cpp"
#elif TEMPLATE_KIND == 2
#include "templates/tester.cpp"
#else
#error Invalid TEMPLATE_KIND
#endif
#undef main
#pragma GCC diagnostic pop

struct TestKey {
  int value;
  bool operator==(const TestKey&) const = default;
};
template<> struct std::hash<TestKey> {
  size_t operator()(const TestKey& x) const { return std::hash<int>{}(x.value); }
};

template<class T>
void check_hash(const T& x, const T& y) {
  unordered_set<T,Hash<T>> st;
  st.insert(x);
  st.insert(x);
  st.insert(y);
  assert(st.count(x) == 1 && st.count(y) == 1);
  assert(st.size() == (x == y ? 1U : 2U));
  unordered_map<T,int,Hash<T>> counts;
  ++counts[x];
  ++counts[x];
  assert(counts.at(x) == 2);
  assert(Hash<T>{}(x) == Hash<T>{}(T(x)));
}

void aggregates_and_sorting() {
  vi v{3,1,2};
  ll maximum = maxv(v), minimum = minv(v), sum = reducev(v);
  assert(maximum == 3 && minimum == 1 && sum == 6);
  assert(maxv(v)+minv(v) == 4);
  assert(maxv(v) == 3 && minv(v) == 1 && reducev(v) == 6);
  vi wide{2000000000LL,2000000000LL};
  assert(reducev(wide) == 4000000000LL);
  vi empty;
  assert(reducev(empty) == 0);
  sortv(empty);
  rsortv(empty);
  // The sorting macros must also form a single expression in if/else.
  if(true) sortv(v); else rsortv(v);
  assert((v == vi{1,2,3}));
  if(false) sortv(v); else rsortv(v);
  assert((v == vi{3,2,1}));
  assert((vi(rall(v)) == vi{1,2,3}));
  assert((idxsort(v) == vector<int>{2,1,0}));
  assert((idxsort(v,greater<>{}) == vector<int>{0,1,2}));
  assert(idxsort(empty).empty());
  const vector<ll> indices{2,0,1};
  sort_by_idx(v,indices);
  assert((v == vi{1,3,2}));
  vector<bool> bits{true,false,true};
  sort_by_idx(bits,indices);
  assert((bits == vector<bool>{true,true,false}));
  vector<int> first{2,1,1,2}, second{4,3,2,1};
  sortpair(first,second);
  assert((first == vector<int>{1,1,2,2}));
  assert((second == vector<int>{2,3,1,4}));
  sortpair(first,second,greater<>{},greater<>{});
  assert((first == vector<int>{2,2,1,1}));
  assert((second == vector<int>{4,1,3,2}));
  // Different values may be equivalent under a custom first comparator.
  auto by_abs = [](int x, int y) { return abs(x) < abs(y); };
  first = {-1,1,-1,2}; second = {30,10,20,0};
  sortpair(first,second,by_abs);
  assert((second == vector<int>{10,20,30,0}));
  vector<int> e1, e2;
  sortpair(e1,e2);
  mt19937 gen(20261015);
  for(int n = 0; n < 64; ++n) {
    vector<int> a(n), b(n);
    for(int i = 0; i < n; ++i) a[i] = int(gen()%11)-5, b[i] = int(gen()%11)-5;
    const auto original = a;
    auto order = idxsort(a);
    sort_by_idx(a,order);
    assert(is_sorted(all(a)) && original.size() == a.size());
    vector<pair<int,int>> expected(n);
    for(int i = 0; i < n; ++i) expected[i] = {a[i],b[i]};
    sort(all(expected));
    sortpair(a,b);
    for(int i = 0; i < n; ++i) assert((expected[i] == pair<int,int>{a[i],b[i]}));
  }
}

void hash_keys() {
  check_hash(1,2);
  check_hash(string("a"),string("b"));
  check_hash(pair<int,int>{1,2},pair<int,int>{2,1});
  check_hash(vector<int>{1,2},vector<int>{2,1});
  check_hash(vector<int>{},vector<int>{1});
  check_hash(vector<bool>{true,false},vector<bool>{false,true});
  check_hash(tuple<int,string>{1,"a"},tuple<int,string>{2,"a"});
  check_hash(tuple<>{},tuple<>{});
  check_hash(TestKey{1},TestKey{2});
  check_hash(vector<TestKey>{{1},{2}},vector<TestKey>{{2},{1}});
  using Nested = tuple<vector<pair<int,int>>,pair<vector<bool>,tuple<int,string>>>;
  check_hash(Nested{{{1,2},{3,4}},{{true,false},{5,"x"}}},
             Nested{{{1,2},{3,4}},{{false,true},{5,"x"}}});
  check_hash(vector<vector<int>>{{1,2},{3}},vector<vector<int>>{{1},{2,3}});
}

void predicates_and_output() {
  assert(isin(0,0,3) && isin(2,0,3) && !isin(3,0,3) && !isin(-1,0,3));
  assert(!isin(0,0,0));
  for(int x = -1; x <= 3; ++x) for(int y = -1; y <= 4; ++y) {
    assert(isinbox(x,y,2,3) == (0 <= x && x < 2 && 0 <= y && y < 3));
    assert(isinbox(x,y,1,3,2,4) == (1 <= x && x < 3 && 2 <= y && y < 4));
  }
  ll x = 4;
  assert(chmin(x,3) && x == 3);
  assert(!chmin(x,3) && !chmin(x,5));
  assert(chmax(x,5) && x == 5);
  assert(!chmax(x,5) && !chmax(x,3));
  ostringstream capture;
  auto previous = cout.rdbuf(capture.rdbuf());
  assert(yesno(2) == 2 && yesno(0) == 0);
  assert(YesNo(true) && !YesNo(false));
  assert(YESNO(2) == 2 && YESNO(0) == 0);
  assert(posimp(2) == 2 && posimp(0) == 0);
  assert(PosImp(2) == 2 && PosImp(0) == 0);
  assert(POSIMP(2) == 2 && POSIMP(0) == 0);
  assert(AliceBob(2) == 2 && AliceBob(0) == 0);
  cout.rdbuf(previous);
  assert(capture.str() == "yes\nno\nYes\nNo\nYES\nNO\npossible\nimpossible\nPossible\nImpossible\nPOSSIBLE\nIMPOSSIBLE\nAlice\nBob\n");
}

int main() {
  aggregates_and_sorting();
  hash_keys();
  predicates_and_output();
  cout << "templates: all tests passed (" << TEMPLATE_KIND << ")\n";
}
