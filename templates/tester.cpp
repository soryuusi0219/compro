// Template codes are in https://github.com/soryuusi0219/compro
#ifdef _DEBUG
#define _GLIBCXX_DEBUG
#endif
#include <bits/stdc++.h>
#include <atcoder/all>
#define COMPRO_USE_ACL_MODINT 1
// #include "library/math/modint.hpp"
// #define SORYUUSI_MODINT_HPP
using namespace std;
namespace atcoder {}
using namespace atcoder;
#define GET4(_1,_2,_3,NAME,...) NAME
#define GET5(_1,_2,_3,_4,NAME,...) NAME
#define GET6(_1,_2,_3,_4,_5,NAME,...) NAME
#define GET11(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,NAME,...) NAME
#define rep1(i, n)  for(long long i=0;i<(long long)(n);i++)
#define rep2(i,k,n) for(long long i=k;i<(long long)(n);i++)
#define rep3(i,k,n,s) for(long long i=k;i<(long long)(n);i+=(s))
#define rep(...) GET5(__VA_ARGS__, rep3, rep2, rep1)(__VA_ARGS__)
#define per1(i,n) for(long long i=(n)-1ll;i>-1ll;i--)
#define per2(i,k,n) for(long long i=(n)-1ll;i>(long long)(k)-1ll;i--)
#define per3(i,k,n,s) for(long long i=(n)-1ll;i>(long long)(k)-1ll;i-=(s))
#define per(...) GET5(__VA_ARGS__, per3, per2, per1)(__VA_ARGS__)
#define perm(c) sort(all(c));for(bool c##p=1;c##p;c##p=next_permutation(all(c)))
#define pb emplace_back
#define lb(v,k) (lower_bound(all(v),(k))-v.begin())
#define ub(v,k) (upper_bound(all(v),(k))-v.begin())
#define fi first
#define se second
#define dame(...) {out(__VA_ARGS__);return 0;}
#define decimal cout<<fixed<<setprecision(15);
#define all(a) a.begin(),a.end()
#define rall(a) a.rbegin(),a.rend()
#define sortv(a) (sort(all(a)))
#define rsortv(a) (sort(rall(a)))
#define dupli(a) {sort(all(a));a.erase(unique(all(a)),a.end());}
#define maxv(a) (*max_element(all(a)))
#define minv(a) (*min_element(all(a)))
#define reducev(a) (reduce(all(a)))
#define readi1(n0) ll n0; cin >> (n0);
#define readi2(n0,n1) ll n0,n1; cin >> (n0) >> (n1);
#define readi3(n0,n1,n2) ll n0,n1,n2; cin >> (n0) >> (n1) >> (n2);
#define readi4(n0,n1,n2,n3) ll n0,n1,n2,n3; cin >> (n0) >> (n1) >> (n2) >> (n3);
#define readi5(n0,n1,n2,n3,n4) ll n0,n1,n2,n3,n4; cin >> (n0) >> (n1) >> (n2) >> (n3) >> (n4);
#define readi6(n0,n1,n2,n3,n4,n5) ll n0,n1,n2,n3,n4,n5; cin >> (n0) >> (n1) >> (n2) >> (n3) >> (n4) >> (n5);
#define readi7(n0,n1,n2,n3,n4,n5,n6) ll n0,n1,n2,n3,n4,n5,n6; cin >> (n0) >> (n1) >> (n2) >> (n3) >> (n4) >> (n5) >> (n6);
#define readi8(n0,n1,n2,n3,n4,n5,n6,n7) ll n0,n1,n2,n3,n4,n5,n6,n7; cin >> (n0) >> (n1) >> (n2) >> (n3) >> (n4) >> (n5) >> (n6) >> (n7);
#define readi9(n0,n1,n2,n3,n4,n5,n6,n7,n8) ll n0,n1,n2,n3,n4,n5,n6,n7,n8; cin >> (n0) >> (n1) >> (n2) >> (n3) >> (n4) >> (n5) >> (n6) >> (n7) >> (n8);
#define readi10(n0,n1,n2,n3,n4,n5,n6,n7,n8,n9) ll n0,n1,n2,n3,n4,n5,n6,n7,n8,n9; cin >> (n0) >> (n1) >> (n2) >> (n3) >> (n4) >> (n5) >> (n6) >> (n7) >> (n8) >> (n9);
#define readi(...) GET11(__VA_ARGS__, readi10, readi9, readi8, readi7, readi6, readi5, readi4, readi3, readi2, readi1)(__VA_ARGS__)
#define readll(...) readi(__VA_ARGS__)
#define readvi1(v,n) vi v(n); rep(i,(n)) cin >> (v)[i];
#define readvi2(v,w,n) vi v(n),w(n); rep(i,(n)) cin >> (v)[i] >> (w)[i];
#define readvi3(v,w,x,n) vi v(n),w(n),x(n); rep(i,(n)) cin >> (v)[i] >> (w)[i] >> (x)[i];
#define readvi4(v,w,x,y,n) vi v(n),w(n),x(n),y(n); rep(i,(n)) cin >> (v)[i] >> (w)[i] >> (x)[i] >> (y)[i];
#define readvi(...) GET6(__VA_ARGS__, readvi4, readvi3, readvi2, readvi1)(__VA_ARGS__)
#define readvll(...) readvll(__VA_ARGS__)
#define reads1(n) string n; cin >> (n);
#define reads2(n,m) string n,m; cin >> (n) >> (m);
#define reads3(n,m,l) string n,m,l; cin >> (n) >> (m) >> (l);
#define reads4(n,m,l,k) string n,m,l,k; cin >> (n) >> (m) >> (l) >> (k);
#define reads(...) GET5(__VA_ARGS__, reads4, reads3, reads2, reads1)(__VA_ARGS__)
#define readvs(v,n) vs v(n); rep(i,(n)) cin >> (v)[i];
#define readd1(n) ld n; cin >> (n);
#define readd2(n,m) ld n,m; cin >> (n) >> (m);
#define readd3(n,m,l) ld n,m,l; cin >> (n) >> (m) >> (l);
#define readd4(n,m,l,k) ld n,m,l,k; cin >> (n) >> (m) >> (l) >> (k);
#define readd(...) GET5(__VA_ARGS__, readd4, readd3, readd2, readd1)(__VA_ARGS__)
#define readvvi(v,n,m) vvi v((n),vi(m)); rep(i,(n))rep(j,(m)) cin >> (v)[i][j];
#define readvvs(v,n,m) vvs v((n),vs(m)); rep(i,(n))rep(j,(m)) cin >> (v)[i][j];
#define readvpi(v,n) vpi v(n); rep(i,(n)) cin >> (v)[i].fi >> (v)[i].se;
#define readg(G,n,m) vvi G(n);rep(i,m){readi(a,b);a--;b--;G[a].push_back(b);G[b].push_back(a);}
#define readgd(G,n,m) vvi G(n);rep(i,m){readi(a,b);a--;b--;G[a].push_back(b);}
#define readgdrev(G,n,m) vvi G(n);rep(i,m){readi(a,b);a--;b--;G[b].push_back(a);}
#define readt(G,n) vvi G(n);rep(i,n-1){readi(a,b);a--;b--;G[a].push_back(b);G[b].push_back(a);}
using ll = long long;
using ull = unsigned long long;
using ld = long double;
template<class T> using P = pair<T,T>;
template<class T> using PP = tuple<T,T,T>;
template<class T> using PPP = tuple<T,T,T,T>;
using pi = P<ll>;
using ppi = PP<ll>;
using pppi = PPP<ll>;
using pll = P<ll>;
using ppll = PP<ll>;
using pppll = PPP<ll>;
template<class T> using V = vector<T>;
template<class T> using VV = vector<vector<T>>;
template<class T> using VVV = vector<vector<vector<T>>>;
template<class T> using VVVV = vector<vector<vector<vector<T>>>>;
using vi=V<ll>;
using vvi=VV<ll>;
using vvvi=VVV<ll>;
using vvvvi=VVVV<ll>;
using vll=V<ll>;
using vvll=VV<ll>;
using vvvll=VVV<ll>;
using vvvvll=VVVV<ll>;
using vpi=V<pi>;
using vvpi=VV<pi>;
using vppi=V<ppi>;
using vvppi=VV<ppi>;
using vpppi=V<pppi>;
using vvpppi=VV<pppi>;
using vpll=V<pll>;
using vvpll=VV<pll>;
using vppll=V<ppll>;
using vvppll=VV<ppll>;
using vpppll=V<pppll>;
using vvpppll=VV<pppll>;
using vb=V<bool>;
using vvb=VV<bool>;
using vs=V<string>;
using vvs=VV<string>;
const ll inf=(1 << 30);
const ll Inf=((ll)1 << 40);
const ll INf=((ll)1 << 50);
const ll INF=((ll)1 << 60);
const double eps=1e-10;
template<class T> using PQ = priority_queue<T>;
template<class T> using SPQ = priority_queue<T,vector<T>,greater<T>>;
/////////////////////////////////////////////////////////
using lll = __int128_t;
using ulll = __uint128_t;
const lll infinf=((lll)1 << 65);
const lll Infinf=((lll)1 << 75);
const lll INfinf=((lll)1 << 85);
const lll INFinf=((lll)1 << 95);
const lll INFInf=((lll)1 << 105);
const lll INFINf=((lll)1 << 115);
const lll INFINF=((lll)1 << 125);
istream &operator>>(istream &is, lll& v) {
  string s; is >> s;
  v = 0;
  for(char c:s){if(isdigit(c))v=v*10+(c-'0');}
  if(s[0]=='-') v*=-1;
  return is;
}
ostream &operator<<(ostream &os, lll v) {
  if(!ostream::sentry(os)) return os;
  char buf[64];
  char *d=end(buf);
  ulll tmp = (v<0?-v:v);
  do{d--;*d=char(tmp%10+'0');tmp/=10;}while(tmp);
  if(v<0) {d--;*d = '-';}
  int len=end(buf)-d;
  if(os.rdbuf()->sputn(d, len)!=len){os.setstate(ios_base::badbit);}
  return os;
}
istream &operator>>(istream &is, ulll& v) {
  string s; is >> s;
  v = 0;
  for(char c:s){if(isdigit(c))v=v*10+(c-'0');}
  return is;
}
ostream &operator<<(ostream &os, ulll v) {
  if(!ostream::sentry(os)) return os;
  char buf[64];
  char *d=end(buf);
  do{d--;*d=char(v%10+'0');v/=10;}while(v);
  int len=end(buf)-d;
  if(os.rdbuf()->sputn(d, len)!=len){os.setstate(ios_base::badbit);}
  return os;
}
/////////////////////////////////////////////////////////
#ifdef ATCODER_MODINT_HPP
using mint = modint998244353;
const ll mod = 998244353;
//using mint = modint1000000007;
//const ll mod = 1000000007;
using vm=V<mint>;
using vvm=VV<mint>;
using vvvm=VVV<mint>;
using vvvvm=VVVV<mint>;
using pm = P<mint>;
using ppm = PP<mint>;
using pppm = PPP<mint>;
using vpm=V<pm>;
using vvpm=VV<pm>;
using vppm=V<ppm>;
using vvppm=VV<ppm>;
using vpppm=V<pppm>;
using vvpppm=VV<pppm>;
template<int M> istream &operator>>(istream &is, static_modint<M>& m){ll x; is >> x; m = x; return is;}
template<int M> istream &operator>>(istream &is, dynamic_modint<M>&  m){ll x; is >> x; m = x; return is;}
template<int M> ostream &operator<<(ostream &os, static_modint<M> m){os << m.val(); return os;}
template<int M> ostream &operator<<(ostream &os, dynamic_modint<M> m){os << m.val(); return os;}
#define readm1(n) mint n; cin >> (n);
#define readm2(n,m) mint n,m; cin >> (n) >> (m);
#define readm3(n,m,l) mint n,m,l; cin >> (n) >> (m) >> (l);
#define readm4(n,m,l,k) mint n,m,l,k; cin >> (n) >> (m) >> (l) >> (k);
#define readm(...) GET5(__VA_ARGS__, readm4, readm3, readm2, readm1)(__VA_ARGS__)
#define readvm1(v,n) vm v(n); rep(i,(n)) cin >> (v)[i];
#define readvm2(v,w,n) vm v(n),w(n); rep(i,(n)) cin >> (v)[i] >> (w)[i];
#define readvm3(v,w,x,n) vm v(n),w(n),x(n); rep(i,(n)) cin >> (v)[i] >> (w)[i] >> (x)[i];
#define readvm4(v,w,x,y,n) vm v(n),w(n),x(n); rep(i,(n)) cin >> (v)[i] >> (w)[i] >> (x)[i] >> (y)[i];
#define readvm(...) GET6(__VA_ARGS__, readvm4, readvm3, readvm2, readvm1)(__VA_ARGS__)
#endif
//////////////////////////////////////////////////////////  
template<class T> ostream &operator<<(ostream &os, vector<vector<vector<T>>> v);
template<class T> ostream &operator<<(ostream &os, vector<vector<T>> v);
template<class T> istream &operator>>(istream &is, vector<T>& v);
template<class T> ostream &operator<<(ostream &os, vector<T> v);
template<class S, class T> istream &operator>>(istream &is, pair<S,T>& p);
template<class S, class T> ostream &operator<<(ostream &os, pair<S,T> p);
template<class... T> istream &operator>> (istream &is, tuple<T...>& p);
template<class... T> ostream &operator<< (ostream &os, tuple<T...> p);
template<class T> ostream &operator<<(ostream &os, set<T> s);
template<class T> ostream &operator<<(ostream &os, multiset<T> s);
template<class S, class T> ostream &operator<<(ostream &os, map<S,T> m);
template<class S, class T> ostream &operator<<(ostream &os, multimap<S,T> m);
template<class T> ostream &operator<<(ostream &os, queue<T> qu);
template<class T> ostream &operator<<(ostream &os, stack<T> st);
template<class T, class Container, class Compare> ostream &operator<<(ostream &os, priority_queue<T, Container, Compare> qu);
//////////////////////////////////////////////////////////
ostream &operator<<(ostream &os, vector<bool> v){for(bool x : v) os << x; return os;}
ostream &operator<<(ostream &os, vector<vector<bool>> v){for(vector<bool>&x : v) os << x << "\n"; return os;}
template<class T> ostream &operator<<(ostream &os, vector<vector<vector<T>>> v){for(vector<vector<T>> &w : v)for(vector<T> &x : w) os << x << "\n"; return os;}
template<class T> ostream &operator<<(ostream &os, vector<vector<T>> v){for(vector<T> &x : v) os << x << "\n"; return os;}
template<class T> istream &operator>>(istream &is, vector<T>& v){for(T &x : v) is >> x; return is;}
template<class T> ostream &operator<<(ostream &os, vector<T> v){for(T &x : v) os << x << " "; return os;}
template<class S, class T> istream &operator>>(istream &is, pair<S,T>& p){is >> p.fi >> p.se; return is;}
template<class S, class T> ostream &operator<<(ostream &os, pair<S,T> p){if(&os == &cerr) return os << "(" << p.fi << "," << p.se << ")"; else return os << p.fi << " " << p.se;}
template<class Tuple, size_t ...I> array<int,sizeof...(I)>tuple_cin(istream &is, Tuple&& t, index_sequence<I...>){return {{ (void(is >> get<I>(t)), 0)... }};}
template<class... T> istream &operator>> (istream &is, tuple<T...>& p){tuple_cin(is, p, make_index_sequence<tuple_size<decay_t<tuple<T...>>>::value>{}); return is;}
template<class Tuple, size_t ...I> array<int,sizeof...(I)>tuple_cout(ostream &os, Tuple&& t, index_sequence<I...>){if(&os == &cerr) return {{ (void(os << get<I>(t) << ","), 0)... }};return {{ (void(os << get<I>(t) << " "), 0)... }};}
template<class... T> ostream &operator<< (ostream &os, tuple<T...> p){if(&os == &cerr) os << "("; tuple_cout(os, p, make_index_sequence<tuple_size<decay_t<tuple<T...>>>::value>{}); if(&os == &cerr) os << ")"; return os;}
template<class T> ostream &operator<<(ostream &os, set<T> s){if(&os == &cerr) os << "set["; for(T x : s) os << x << " "; if(&os == &cerr) os << "]"; return os;}
template<class T> ostream &operator<<(ostream &os, multiset<T> s){if(&os == &cerr) os << "multiset["; for(T x : s) os << x << " "; if(&os == &cerr) os << "]"; return os;}
template<class T> ostream &operator<<(ostream &os, unordered_set<T> s){if(&os == &cerr) os << "unordered_set["; for(T x : s) os << x << " "; if(&os == &cerr) os << "]"; return os;}
template<class T> ostream &operator<<(ostream &os, unordered_multiset<T> s){if(&os == &cerr) os << "unordered_multiset["; for(T x : s) os << x << " "; if(&os == &cerr) os << "]"; return os;}
template<class S, class T> ostream &operator<<(ostream &os, map<S,T> m){if(&os == &cerr) os << "map["; for(auto [k, v] : m) os << "(" << k << "," << v << ") "; if(&os == &cerr) os << "]"; return os;}
template<class S, class T> ostream &operator<<(ostream &os, multimap<S,T> m){if(&os == &cerr) os << "multimap["; for(auto [k, v] : m) os << "(" << k << "," << v << ") "; if(&os == &cerr) os << "]"; return os;}
template<class S, class T> ostream &operator<<(ostream &os, unordered_map<S,T> m){if(&os == &cerr) os << "unordered_map["; for(auto [k, v] : m) os << "(" << k << "," << v << ") "; if(&os == &cerr) os << "]"; return os;}
template<class S, class T> ostream &operator<<(ostream &os, unordered_multimap<S,T> m){if(&os == &cerr) os << "unordered_multimap["; for(auto [k, v] : m) os << "(" << k << "," << v << ") "; if(&os == &cerr) os << "]"; return os;}
template<class T> ostream &operator<<(ostream &os, queue<T> qu){if(&os == &cerr) os << "queue["; while(!qu.empty()){os << qu.front() << " ";qu.pop();} if(&os == &cerr) os << "]"; return os;}
template<class T> ostream &operator<<(ostream &os, stack<T> st){if(&os == &cerr) os << "stack["; while(!st.empty()){os << st.top() << " ";st.pop();} if(&os == &cerr) os << "]"; return os;}
template<class T, class Container, class Compare> ostream &operator<<(ostream &os, priority_queue<T, Container, Compare> qu){if(&os == &cerr) os << "prioroty_queue["; while (!qu.empty()){os << qu.top() << " ";qu.pop();} if(&os == &cerr) os << "]"; return os;}
//////////////////////////////////////////////////////////
template<class... Ts> void read(Ts&... a) {(cin >> ... >> a);}
void out0(float a){cout<<fixed<<setprecision(15)<<a;}
void out0(double a){cout<<fixed<<setprecision(15)<<a;}
void out0(ld a){cout<<fixed<<setprecision(15)<<a;}
template<class T> void out0(T a){cout<<a;}
template<class T>void out(T a) {out0(a); cout << "\n";}
template<class T, class... Ts>void out(T a, Ts... b) {out0(a); cout << " "; out(b...);}
template<class... Ts>void outf(Ts... a) {out(a...); cout << flush;}

void out0d(float a){cerr<<fixed<<setprecision(15)<<a;}
void out0d(double a){cerr<<fixed<<setprecision(15)<<a;}
void out0d(ld a){cerr<<fixed<<setprecision(15)<<a;}
template<class T> void out0d(T a){cerr<<a;}
template<class T>void outd(T a) {out0d(a); cerr << endl;}
template<class T, class... Ts>void outd(T a, Ts... b) {out0d(a); cerr << " "; outd(b...);}
//////////////////////////////////////////////////////////
template<class T> vector<T> &operator++(vector<T>& v){for(T& x : v) x++; return v;}
template<class T> vector<T> &operator--(vector<T>& v){for(T& x : v) x--; return v;}
template<class T> vector<T> operator++(vector<T>& v, signed){auto res = v; for(T& x : v) x++; return res;}
template<class T> vector<T> operator--(vector<T>& v, signed){auto res = v; for(T& x : v) x--; return res;}
template<class T> vector<T> operator+=(vector<T>& v, const vector<T>& w){if(v.size() < w.size()) v.resize(w.size()); for(int i = 0; i < (int)w.size(); i++) v[i] += w[i]; return v;}
template<class T> vector<T> operator-=(vector<T>& v, const vector<T>& w){if(v.size() < w.size()) v.resize(w.size()); for(int i = 0; i < (int)w.size(); i++) v[i] -= w[i]; return v;}
template<class T> vector<T> operator*=(vector<T>& v, const vector<T>& w){if(v.size() < w.size()) v.resize(w.size()); for(int i = 0; i < (int)w.size(); i++) v[i] *= w[i]; return v;}
template<class T> vector<T> operator/=(vector<T>& v, const vector<T>& w){if(v.size() < w.size()) v.resize(w.size()); for(int i = 0; i < (int)w.size(); i++) v[i] /= w[i]; return v;}
template<class T> vector<T> operator+(vector<T> v, const vector<T>& w){return (v += w);}
template<class T> vector<T> operator-(vector<T> v, const vector<T>& w){return (v -= w);}
template<class T> vector<T> operator*(vector<T> v, const vector<T>& w){return (v *= w);}
template<class T> vector<T> operator/(vector<T> v, const vector<T>& w){return (v /= w);}
template<class T> vector<T> operator*=(vector<T>& v, T w){for(T& x : v) x*=w; return v;}
template<class T> vector<T> operator/=(vector<T>& v, T w){for(T& x : v) x/=w; return v;}
template<class T> vector<T> operator*(vector<T> v, T x){return (v *= x);}
template<class T> vector<T> operator/(vector<T> v, T x){return (v /= x);}
template<class S, class T> pair<S,T> operator+=(pair<S,T>& p, const pair<S,T>& q){p.fi+=q.fi, p.se+=q.se; return p;}
template<class S, class T> pair<S,T> operator+(pair<S,T> p, const pair<S,T>& q){return (p += q);}
template<class S, class T> pair<S,T> operator-=(pair<S,T>& p, const pair<S,T>& q){p.fi-=q.fi, p.se-=q.se; return p;}
template<class S, class T> pair<S,T> operator-(pair<S,T> p, const pair<S,T>& q){return (p -= q);}
//////////////////////////////////////////////////////////
// Example: unordered_set<pi, Hash<pi>> st;
template<class T> struct Hash : std::hash<T> {};
template<class T> size_t HashCombine(size_t seed,const T& v){return seed^(Hash<T>{}(v)+0x9e3779b9+(seed<<6)+(seed>>2));}
template<class T,class U> struct Hash<pair<T,U>>{
  size_t operator()(const pair<T,U>& v) const {return HashCombine(Hash<T>{}(v.first),v.second);}
};
template<class T,class Alloc> struct Hash<vector<T,Alloc>>{
  size_t operator()(const vector<T,Alloc>& v) const {size_t seed=0; for(const T& x:v) seed=HashCombine(seed,x); return seed;}
};
template<class... Ts> struct Hash<tuple<Ts...>>{
  size_t operator()(const tuple<Ts...>& v) const {size_t seed=0; std::apply([&](const auto&... x){((seed=HashCombine(seed,x)),...);},v); return seed;}
};
//////////////////////////////////////////////////////////
template<class T> bool isin(T x,T l,T r){return l<=x&&x<r;}
template<class T> bool isinbox(T x,T y,T h,T w){return 0<=x&&x<h&&0<=y&&y<w;}
template<class T> bool isinbox(T x,T y,T l,T r,T u,T b){return l<=x&&x<r&&u<=y&&y<b;}
template<class T> T yesno(T b){if(b)out("yes");else out("no");return b;}
template<class T> T YesNo(T b){if(b)out("Yes");else out("No");return b;}
template<class T> T YESNO(T b){if(b)out("YES");else out("NO");return b;}
template<class T> T posimp(T b){if(b)out("possible");else out("impossible");return b;}
template<class T> T PosImp(T b){if(b)out("Possible");else out("Impossible");return b;}
template<class T> T POSIMP(T b){if(b)out("POSSIBLE");else out("IMPOSSIBLE");return b;}
template<class T> T AliceBob(T b){if(b)out("Alice");else out("Bob");return b;}
template<class T, class U> bool chmin(T&a,U b){return a>b ? a=b, true : false;}
template<class T, class U> bool chmax(T&a,U b){return a<b ? a=b, true : false;}
template<class T> T dist_sq(P<T> x, P<T> y){return (x.fi-y.fi)*(x.fi-y.fi)+(x.se-y.se)*(x.se-y.se);}
template<class T> T cross_product(P<T> x, P<T> y){return x.fi * y.se - x.se * y.fi;}
template<class T> bool compare_by_arg(P<T> x, P<T> y){if(x == y) return false; if(x.fi == 0 && x.se == 0) return true; if(y.fi == 0 && y.se == 0) return false; if(x.se >= 0 && y.se < 0) return true; if(x.se < 0 && y.se >= 0) return false; if(x.se == 0 && y.se == 0) return x.fi > y.fi; return x.fi * y.se > x.se * y.fi;}
template<class T> void argsort(V<P<T>>& a){sort(all(a),compare_by_arg<T>);}
template<class T, class Comp = less<>> inline vector<int> idxsort(const T& v, Comp comp = Comp()){vector<int> res(v.size()); iota(res.begin(), res.end(), 0); sort(res.begin(), res.end(), [&](int x, int y){return comp(v[x],v[y]);}); return res;}
template<class T, class U> inline void sort_by_idx(T& v, const vector<U>& I){assert(v.size() == I.size()); T tmpv = v; for(int i = 0; i < (int)I.size(); ++i){v[i] = tmpv[I[i]];}}
template<class T, class U, class Comp1 = less<T>, class Comp2 = less<U>> inline void sortpair(vector<T>& v1, vector<U>& v2, Comp1 comp1 = Comp1(), Comp2 comp2 = Comp2()){assert(v1.size() == v2.size()); vector<int> I(v1.size()); iota(I.begin(), I.end(), 0); sort(I.begin(), I.end(), [&](int x, int y){if(comp1(v1[x],v1[y])) return true; if(comp1(v1[y],v1[x])) return false; return comp2(v2[x],v2[y]);}); sort_by_idx(v1, I); sort_by_idx(v2, I);}
const vpi adj = {{1,0},{0,1},{-1,0},{0,-1}};
const vpi king = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
const vpi knight = {{2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2},{1,-2},{2,-1}};
void outs(ll a,ll b,ll i){if(abs(a)>=i-100)out(b);else out(a);}
ll gcd(ll a,ll b){if(b==0)return abs(a);return gcd(abs(b),abs(a)%abs(b));}
ll lcm(ll a,ll b){if(!a) return 0; return abs((a / gcd(a,b)) * b);}
template<class T> T POW(T a, ll b){T res=1;while(b){if(b&1)res=res*a;a=a*a;b>>=1;}return res;}
ll divfloor(ll a, ll b){return (a >= 0 ? a/b : (a-b+1)/b);}
ll divceil(ll a, ll b){return (a >= 0 ? (a+b-1)/b : a/b);}
ll modpow(ll a,ll b,ll modd){ll res=1;a%=modd;while(b){if(b&1)res=res*a%modd;a=a*a%modd;b>>=1;}return res;}
ll sqrtll(ll a){assert(a >= 0); ll r = (ll)sqrtl((ld)a)-1; while(r < 0 || (r+1)*(r+1) <= a) r++; return r;}
ll cbrtll(ll a){assert(a >= 0); ll r = (ll)cbrtl((ld)a)-1; while(r < 0 || (r+1)*(r+1)*(r+1) <= a) r++; return r;}
ll modinv(ll a, ll b){assert(a); if(a == 1) return 1; if(b == 1) return 0; ll ret = (1ll-b*modinv(b%a, a))/a; ret %= b; if(ret < 0) ret += b; return ret;}

struct Random {
  mt19937_64 rng;
  Random() : rng(random_device{}()) {}

  ll operator()(ll r) {
    uniform_int_distribution<ll> dist(0, r);
    return dist(rng);
  }

  ll operator()(ll l, ll r) {
    uniform_int_distribution<ll> dist(l, r);
    return dist(rng);
  }

  vi permu(ll n) {
    vi p(n);
    iota(all(p), 0);
    shuffle(all(p), rng);
    return p;
  }

  vi vect(ll n, ll r) {
    vi a(n);
    for(auto& x : a) x = (*this)(r);
    return a;
  }

  vi vect(ll n, ll l, ll r) {
    vi a(n);
    for(auto& x : a) x = (*this)(l, r);
    return a;
  }

  vpi tree(ll n) {
    if(n < 2) return {};
    vpi es;
    vi p(n - 2), d(n, 1);
    for(ll& v : p) d[v = (*this)(0, n - 1)]++;
    ll x = 0, l;
    while(d[x] != 1) x++;
    l = x;
    for(ll v : p) {
      es.push_back({l, v});
      if(--d[v] == 1 && v < x) l = v;
      else {
        while(d[++x] != 1);
        l = x;
      }
    }
    es.push_back({l, n - 1});
    return es;
  }

  vpi graph(ll n, ll m) {
    set<pi> st;
    vpi es;
    if(m < n - 1) {
      while((ll)st.size() < m) {
        ll u = (*this)(0, n - 1), v = (*this)(0, n - 1);
        if(u > v) swap(u, v);
        if(u != v) st.insert({u, v});
      }
    } else {
      es = tree(n);
      for(auto [u, v] : es) {
        if(u > v) swap(u, v);
        st.insert({u, v});
      }
      while((ll)st.size() < m) {
        ll u = (*this)(0, n - 1), v = (*this)(0, n - 1);
        if(u > v) swap(u, v);
        if(u != v && !st.count({u, v})) st.insert({u, v});
      }
    }
    return vpi(all(st));
  }
} rnd;


// #include "library/math/modint_factorials.hpp"
// using namespace soryuusi;

ll solve(ll n){
  return n ^ 1;
}

ll naive(ll n){
  return n & 1 ? n - 1 : n + 1;
}

void test(){
  ll n = rnd(100);

  auto ans = naive(n);
  auto myans = solve(n);
  if(ans != myans){
    outf("WA");
    outf(n);
    outf("expected:",ans);
    outf("found   :",myans);
    assert(false);
  }
  outf("AC");
}


int main(){
  std::cin.tie(nullptr), std::ios_base::sync_with_stdio(false);
  while(true) test();
  int TESTCASE = 1;
  // cin >> TESTCASE;
  while(TESTCASE--){
    
  }
}
