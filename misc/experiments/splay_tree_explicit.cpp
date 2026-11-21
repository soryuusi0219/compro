#ifdef _DEBUG
#define _GLIBCXX_DEBUG
#endif
#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
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
#define rsort(a) {sort(all(a));reverse(all(a));}
#define dupli(a) {sort(all(a));a.erase(unique(all(a)),a.end());}
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
#define readvi1(v,n) vi v(n); rep(i,(n)) cin >> (v)[i];
#define readvi2(v,w,n) vi v(n),w(n); rep(i,(n)) cin >> (v)[i] >> (w)[i];
#define readvi3(v,w,x,n) vi v(n),w(n),x(n); rep(i,(n)) cin >> (v)[i] >> (w)[i] >> (x)[i];
#define readvi4(v,w,x,y,n) vi v(n),w(n),x(n); rep(i,(n)) cin >> (v)[i] >> (w)[i] >> (x)[i] >> (y)[i];
#define readvi(...) GET6(__VA_ARGS__, readvi4, readvi3, readvi2, readvi1)(__VA_ARGS__)
#define reads1(n) string n; cin >> (n);
#define reads2(n,m) string n,m; cin >> (n) >> (m);
#define reads3(n,m,l) string n,m,l; cin >> (n) >> (m) >> (l);
#define reads4(n,m,l,k) string n,m,l,k; cin >> (n) >> (m) >> (l) >> (k);
#define reads(...) GET5(__VA_ARGS__, reads4, reads3, reads2, reads1)(__VA_ARGS__)
#define readvs(v,n) vs v(n); rep(i,(n)) cin >> (v)[i];
#define readd1(n) ld n; cin >> (n);
#define readd2(n,m) ld n,m; cin >> (n) >> (m);
#define readd3(n,m,l) ld n,m,l; cin >> (n) >> (m) >> (l);
#define readd4(n,m,l,k) ld n,m,l; cin >> (n) >> (m) >> (l) >> (k);
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
template<class T> using V = vector<T>;
template<class T> using VV = vector<vector<T>>;
template<class T> using VVV = vector<vector<vector<T>>>;
template<class T> using VVVV = vector<vector<vector<vector<T>>>>;
using vi=V<ll>;
using vvi=VV<ll>;
using vvvi=VVV<ll>;
using vvvvi=VVVV<ll>;
using vpi=V<pi>;
using vvpi=VV<pi>;
using vppi=V<ppi>;
using vvppi=VV<ppi>;
using vpppi=V<pppi>;
using vvpppi=VV<pppi>;
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
  string s; cin >> s;
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
  string s; cin >> s;
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
template<class T> ostream &operator<<(ostream &os, vector<vector<vector<T>>> v){for(vector<vector<T>> &w : v)for(vector<T> &x : w) os << x << "\n"; return os;}
template<class T> ostream &operator<<(ostream &os, vector<vector<T>> v){for(vector<T> &x : v) os << x << "\n"; return os;}
template<class T> istream &operator>>(istream &is, vector<T>& v){for(T &x : v) is >> x; return is;}
template<class T> ostream &operator<<(ostream &os, vector<T> v){for(T &x : v) os << x << " "; return os;}
template<class S, class T> istream &operator>>(istream &is, pair<S,T>& p){is >> p.fi >> p.se; return is;}
template<class S, class T> ostream &operator<<(ostream &os, pair<S,T> p){os << p.fi << " " << p.se; return os;}
template<class Tuple, size_t ...I> array<int,sizeof...(I)>tuple_cin(istream &is, Tuple&& t, index_sequence<I...>){return {{ (void(is >> get<I>(t)), 0)... }};}
template<class... T> istream &operator>> (istream &is, tuple<T...>& p){tuple_cin(is, p, make_index_sequence<tuple_size<decay_t<tuple<T...>>>::value>{}); return is;}
template<class Tuple, size_t ...I> array<int,sizeof...(I)>tuple_cout(ostream &os, Tuple&& t, index_sequence<I...>){return {{ (void(os << get<I>(t) << " "), 0)... }};}
template<class... T> ostream &operator<< (ostream &os, tuple<T...> p){tuple_cout(os, p, make_index_sequence<tuple_size<decay_t<tuple<T...>>>::value>{}); return os;}
template<class T> ostream &operator<<(ostream &os, set<T> s){for(T x : s) os << x << " "; return os;}
template<class T> ostream &operator<<(ostream &os, multiset<T> s){for(T x : s) os << x << " "; return os;}
template<class S, class T> ostream &operator<<(ostream &os, map<S,T> m){for(auto [k, v] : m) os << "(" << k << "," << v << ") "; return os;}
template<class S, class T> ostream &operator<<(ostream &os, multimap<S,T> m){for(auto [k, v] : m) os << "(" << k << "," << v << ") "; return os;}
template<class T> ostream &operator<<(ostream &os, queue<T> qu){while (!qu.empty()){os << qu.front() << " ";qu.pop();}return os;}
template<class T> ostream &operator<<(ostream &os, stack<T> st){while (!st.empty()){os << st.top() << " ";st.pop();}return os;}
template<class T, class Container, class Compare> ostream &operator<<(ostream &os, priority_queue<T, Container, Compare> qu){while (!qu.empty()){os << qu.top() << " ";qu.pop();}return os;}
template<class... Ts> void read(Ts&... a) {(cin >> ... >> a);}
//////////////////////////////////////////////////////////
void out0(float a){cout<<fixed<<setprecision(15)<<a;}
void out0(double a){cout<<fixed<<setprecision(15)<<a;}
void out0(ld a){cout<<fixed<<setprecision(15)<<a;}
template<class T> void out0(T a){cout<<a;}
template<class T>void out(T a) {out0(a); cout << "\n";}
template<class T, class... Ts>void out(T a, Ts... b) {out0(a); cout << " "; out(b...);}
template<class... Ts>void outf(Ts... a) {out(a...); cout << flush;}
void out0d(ll a){cout<<a;}
void out0d(int a){cout<<a;}
void out0d(unsigned int a){cout<<a;}
void out0d(long unsigned int a){cout<<a;}
void out0d(long int a){cout<<a;}
void out0d(unsigned long long a){cout<<a;}
void out0d(lll a){cout<<a;}
void out0d(ulll a){cout<<a;}
void out0d(char a){cout<<a;}
void out0d(char* a){cout<<a;}
void out0d(string a){cout<<a;}
void out0d(float a){cout<<fixed<<setprecision(15)<<a;}
void out0d(double a){cout<<fixed<<setprecision(15)<<a;}
void out0d(ld a){cout<<fixed<<setprecision(15)<<a;}
template<int M> void out0d(static_modint<M> a){cout<<a.val();}
template<int M> void out0d(dynamic_modint<M> a){cout<<a.val();}
template<class T> void out0d(vector<T> v);
template<class T> void out0d(set<T> v);
template<class S, class T> void out0d(map<S,T> v);
template<class T> void out0d(multiset<T> v);
template<class T> void out0d(queue<T> qu);
template<class T> void out0d(stack<T> st);
template<class T> void out0sd(T a);
template<class T, class Container, class Compare> void out0d(priority_queue<T, Container, Compare> qu);
template<class S, class T> void out0d(pair<S,T> p){cout << "("; out0d(p.fi); cout << ", "; out0d(p.se); cout << ")";}
template < typename Tuple, size_t ...I >
array<int,sizeof...(I)>
tuple_printd(Tuple&& t, index_sequence<I...>){return {{ (void( out0sd(get<I>(t))), 0)... }};}
template<class... T> void out0d(tuple<T...> p){cout << "(";tuple_printd(p,make_index_sequence<tuple_size<decay_t<tuple<T...>>>::value>{});cout << ")";}
template<class T> void out0d(vector<T> v){for(T& c : v) out0sd<T>(c);}
template<class T> void out0d(set<T> v){cout << "set["; for(T c : v) out0sd(c); cout << "]";}
template<class T> void out0d(multiset<T> v){cout << "multiset["; for(T c : v) out0sd(c); cout << "]";}
template<class S, class T> void out0d(map<S,T> v){cout << "map["; for(auto [k, x] : v) { out0d(k); cout << ":" ; out0d(x); cout << " ";}cout << "]";}
template<class S, class T> void out0d(multimap<S,T> v){cout << "multimap["; for(auto [k,x] : v) { out0d(k); cout << ":" ; out0d(x); cout << ", ";}cout << "]";}
template<class T> void out0d(queue<T> qu){cout << "queue["; while (!qu.empty()){out0sd(qu.front());qu.pop();}cout << "]";}
template<class T> void out0d(stack<T> st){cout << "stack["; while (!st.empty()){out0sd(st.top());st.pop();}cout << "]";}
template<class T, class Container, class Compare> void out0d(priority_queue<T, Container, Compare> qu){cout << "priority_queue["; while (!qu.empty()){out0sd(qu.top()); qu.pop();}cout << "]";}
template<class T> void out0sd(T a){out0d(a); cout << " ";}
template<class T> void outd(VV<T> v){for(auto c : v) {out0d(c); cout << endl;}}
template<class T> void outd(T a) {out0d(a); cout << endl;}
template<class T, class... Ts>void outd(T a, Ts... b) {out0d(a); cout << " "; outd(b...);}
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
template<class T> bool isin(T x,T l,T r){return (l)<=(x)&&(x)<=(r);}
template<class T> void yesno(T b){if(b)out("yes");else out("no");}
template<class T> void YesNo(T b){if(b)out("Yes");else out("No");}
template<class T> void YESNO(T b){if(b)out("YES");else out("NO");}
template<class T> void posimp(T b){if(b)out("possible");else out("impossible");}
template<class T> void PosImp(T b){if(b)out("Possible");else out("Impossible");}
template<class T> void POSIMP(T b){if(b)out("POSSIBLE");else out("IMPOSSIBLE");}
template<class T> bool chmin(T&a,T b){if(a>b){a=b;return true;}return false;}
template<class T> bool chmax(T&a,T b){if(a<b){a=b;return true;}return false;}
template<class T> T dist_sq(P<T> x, P<T> y){return (x.fi-y.fi)*(x.fi-y.fi)+(x.se-y.se)*(x.se-y.se);}
template<class T> T cross_product(P<T> x, P<T> y){return x.fi * y.se - x.se * y.fi;}
template<class T> bool compare_by_arg(P<T> x, P<T> y){if(x.fi == 0 && x.se == 0) return true; if(y.fi == 0 && y.se == 0) return false; if(x.se >= 0 && y.se < 0) return true; if(x.se < 0 && y.se >= 0) return false; if(x.se == 0 && y.se == 0) return x.fi > y.fi; return x.fi * y.se > x.se * y.fi;}
template<class T> void argsort(V<P<T>>& a){sort(all(a),compare_by_arg<T>);} 
const vpi adj = {{1,0},{0,1},{-1,0},{0,-1}};
const vpi king = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
const vpi knight = {{2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2},{1,-2},{2,-1}};
void outs(ll a,ll b,ll i){if(abs(a)>=i-100)out(b);else out(a);}
ll gcd(ll a,ll b){if(b==0)return abs(a);return gcd(abs(b),abs(a)%abs(b));}
ll lcm(ll a,ll b){return abs((a / gcd(a,b)) * b);}
template<class T> T POW(T a, ll b){T res=1;while(b){if(b&1)res=res*a;a=a*a;b>>=1;}return res;}
ll divfloor(ll a, ll b){return (a >= 0 ? a/b : (a-b+1)/b);}
ll divceil(ll a, ll b){return (a >= 0 ? (a+b-1)/b : a/b);}
ll modpow(ll a,ll b,ll modd){ll res=1;a%=modd;while(b){if(b&1)res=res*a%modd;a=a*a%modd;b>>=1;}return res;}
ll sqrtll(ll a){assert(a >= 0); ll r = (ll)sqrtl((ld)a)-1; while(r < 0 || (r+1)*(r+1) <= a) r++; return r;}
ll cbrtll(ll a){assert(a >= 0); ll r = (ll)cbrtl((ld)a)-1; while(r < 0 || (r+1)*(r+1)*(r+1) <= a) r++; return r;}
ll modinv(ll a, ll b){assert(a); if(a == 1) return 1; if(b == 1) return 0; ll ret = (1ll-b*modinv(b%a, a))/a; ret %= b; if(ret < 0) ret += b; return ret;}

template<class Key, class T, T (*op)(T,T), T (*e)(), class F, T (*mapping)(F, T), F (*composition)(F, F), F (*id)(), class Compare = less<Key>>
struct splay_tree_explicit {
  struct node{
    node* pp = nullptr;
    node* lp = nullptr;
    node* rp = nullptr;
    Key key;
    T value = e();
    T sum = e();
    int index = -1;
    int z = 0;
    int sumz = 0;
    F lazy = id();
    node(int i, Key k, T v) : key(k), value(v), sum(v), index(i), z(1), sumz(1){}
    void cul() {
      sum = value;
      sumz = z;
      if(rp) {
        sumz += rp->sumz;
        sum = op(sum,rp->sum); 
      }
      if(lp) {
        sumz += lp->sumz;
        sum = op(lp->sum,sum);
      }
    }
    void rotr(){
      node* q = pp;
      node* r = q->pp;
      q->lp=rp;
      if(rp){
        rp->pp = q;
      }
      rp = q;
      q->pp = this;
      pp = r;
      if(r){
        if(r->lp == q) r->lp = this;
        if(r->rp == q) r->rp = this;
      }
      if(q) q->cul();
      cul();
      if(r) r->cul();
    }
    void rotl(){
      node* q = pp;
      node* r = q->pp;
      q->rp=lp;
      if(lp){
        lp->pp = q;
      }
      lp = q;
      q->pp = this;
      pp = r;
      if(r){
        if(r->lp == q) r->lp = this;
        if(r->rp == q) r->rp = this;
      }
      if(q) q->cul();
      cul();
      if(r) r->cul();
    }
    //propagate
    void propagate(F x){
      lazy = composition(x, lazy);
      value = mapping(x, value);
      sum = mapping(x, sum);
    }
    //toglle
    void toggle() {
      swap(lp, rp);
      swap(sum,sum);
    }
    //push
    void push() {
      if(lazy != id()) {
        if(lp) lp->propagate(lazy);
        if(rp) rp->propagate(lazy);
        lazy = id();
      }
    }
    void splay(){
      push();
      while(pp){
        node* q = pp;
        if(!(q->pp)){
          q->push(), push();
          if(q->lp == this) rotr();
          else rotl();
        }else{
          node* r = q->pp;
          r->push(); q->push(); push();
          if(r->lp == q){
            if(q->lp == this) {q->rotr(); rotr();}
            else {rotl(); rotr();}
          }else{
            if(q->rp == this) {q->rotl(); rotl();}
            else {rotr(); rotl();}
          }
        }
      }
    }
  };
  using pnode = unique_ptr<node>;
  vector<pnode> nodes;
  node* root = nullptr;
  node* splay(node* c){
    if(c){
      c -> splay();
      root = c;
    }
    return c;
  }

  node* most_left(node* c){
    while(c -> lp) c = c -> lp;
    return splay(c);
  }
  node* most_right(node* c){
    while(c -> rp) c = c -> rp;
    return splay(c);
  }
  node* kth_node(node* c, int k){
    if(!c) return nullptr;
    while(true){
      int lsumz = 0;
      if(c->lp) lsumz = c->lp->sumz;
      if(lsumz == k) break;
      if(lsumz > k){
        c = c->lp;
        continue;
      }
      if(!c->rp) break;
      k--;
      if(c->lp) k -= c->lp->sumz;
      c = c->rp;
    }
    return splay(c);
  }
  node* next(node* c){
    if(!c) return {nullptr};
    c->splay();
    if(!c -> rp) return nullptr;
    return most_left(c->rp);
  }
  node* prev(node* c){
    if(!c) return {nullptr};
    c->splay();
    if(!c -> lp) return nullptr;
    return most_right(c->lp);
  }
  node* make_node(Key k, T v){
    int index = nodes.size();
    nodes.push_back(make_unique<node>(node(index,k,v)));
    return nodes.back().get();
  }
  void delete_node(node* c){
    if(!c) return;
    int i = c->index;
    if(i < (int)nodes.size()-1){
      node* d = nodes.back().get();
      swap(c->index,d->index);
      swap(nodes[i],nodes.back());
    }
    nodes.pop_back();
  }
  node* insert_left(node* c, Key k, T v){
    node* d = make_node(k, v);
    c->lp = d;
    d->pp = c;
    return splay(d);
  }
  node* insert_right(node* c, Key k, T v){
    node* d = make_node(k, v);
    c->rp = d;
    d->pp = c;
    return splay(d);
  }
  node* erase(node* c){
    if(!c) return nullptr;
    c->splay();
    if(c->lp){
      node* d = prev(c);
      d->splay();
      d->rp = c->rp;
      if(d->rp) d->rp->pp = d;
      delete_node(c);
      return root = d;
    }
    if(c->rp){
      node* d = next(c);
      d->splay();
      d->lp = c->lp;
      if(d->lp) d->lp->pp = d;
      delete_node(c);
      return root = d;
    }
    delete_node(c);
    return root = nullptr;
  }
  node* find(node* c, Key k){
    while(c){
      if(c->key == k) return splay(c);
      if(Compare()(k,c->key)) c = c->lp;
      else c = c->rp;
    }
    return nullptr;
  }
  node* lower_bound(node* c, Key k){
    node* res = nullptr;
    node* prev = nullptr;
    while(c){
      prev = c;
      if(!Compare()(c->key,k)) {
        res = c;
        c = c->lp;
      }
      else c = c->rp;
    }
    splay(prev);
    return res;
  }
  node* upper_bound(node* c, Key k){
    node* res = nullptr;
    node* prev = nullptr;
    while(c){
      prev = c;
      if(Compare()(k,c->key)) {
        res = c;
        c = c->lp;
      }
      else c = c->rp;
    }
    splay(prev);
    return res;
  }

  node* lower_bound_rev(node* c, Key k){
    node* res = nullptr;
    node* prev = nullptr;
    while(c){
      prev = c;
      if(!Compare()(k,c->key)) {
        res = c;
        c = c->rp;
      }
      else c = c->lp;
    }
    splay(prev);
    return res;
  }
  node* upper_bound_rev(node* c, Key k){
    node* res = nullptr;
    node* prev = nullptr;
    while(c){
      prev = c;
      if(Compare()(c->key,k)) {
        res = c;
        c = c->rp;
      }
      else c = c->lp;
    }
    splay(prev);
    return res;
  }

  pair<Key,T> get_val(node* c){
    assert(c);
    return {c->key, c->value};
  }

  int size(){return nodes.size();}
  void insert(Key x, T v){
    if(!root){
      root = make_node(x,v);
      return;
    }
    node* c = root;
    while(c){
      if(Compare()(x,c->key)) {
        if(!c->lp) {
          insert_left(c,x,v);
          return;
        }
        c = c->lp;
      }
      else {
        if(!c->rp) {
          insert_right(c,x,v);
          return;
        }
        c = c->rp;
      }
    }
  }
  void erase(Key x){
    node* c = find(root,x);
    if(c) erase(c);
  }
  pair<Key,T> get_kth(int k){
    node* c = kth_node(root,k);
    return {root-> key, root->value};
  }
  T operator[](Key x){
    node* c = find(root, x);
    if(!c) return e();
    return c->value;
  }
  T at(Key x){
    node* c = find(root, x);
    if(!c) return e();
    return c->value;
  }
  bool contains(Key x){
    return find(root, x);
  }
  void set(Key x, T v){
    node* c = find(root, x);
    assert(c);
    if(c) c->value = v;
  }
  pair<Key,T> lower_bound(Key k){
    return get_val(lower_bound(root, k));
  }
  pair<Key,T> upper_bound(Key k){
    return get_val(upper_bound(root, k));
  }
  pair<Key,T> lower_bound_rev(Key k){
    return get_val(lower_bound_rev(root, k));
  }
  pair<Key,T> upper_bound_rev(Key k){
    return get_val(upper_bound_rev(root, k));
  }
  T prod(Key l, Key r, bool left_inclusive = true, bool right_inclusive = false){
    if(!Compare()(l,r)) return e();
    node* c = left_inclusive
    ? lower_bound(root, l)
    : upper_bound(root, l);
    if(!c) return e();
    splay(c);
    node* cl = c->lp;
    c->lp = nullptr;
    node* d = right_inclusive
    ? lower_bound_rev(root, r)
    : upper_bound_rev(root, r);
    if(!d){
      c->splay();
      c->lp = cl;
      c->cul();
      root = c;
      return e();
    }
    splay(d);
    node* dr = d->rp;
    d->rp = nullptr;
    d->cul();

    T ret = d->sum;

    d->rp = dr;
    d->cul();
    c->splay();
    c->lp = cl;
    c->cul();
    root = c;

    return ret;
  }
  void apply(Key l, Key r, F f, bool left_inclusive = true, bool right_inclusive = false){
    if(!Compare()(l,r)) return e();
    node* c = left_inclusive
    ? lower_bound(root, l)
    : upper_bound(root, l);
    if(!c) return;
    node* cl = c->lp;
    c->lp = nullptr;
    node* d = right_inclusive
    ? lower_bound_rev(root, r)
    : upper_bound_rev(root, r);
    if(!d){
      c->splay();
      c->lp = cl;
      c->cul();
      return;
    }
    node* dr = d->rp;
    d->rp = nullptr;
    d->cul();

    d->value = mapping(f, d->value); 
    d->sum = mapping(f, d->sum);
    d->lazy = composition(f,d->lazy);

    d->rp = dr;
    d->cul();
    c->splay();
    c->lp = cl;
    c->cul();
    root = c;
  }
  T prod_all(){
    if(!root) return e();
    return root->sum;
  }
  void apply_all(F f){
    if(!root) return;
    root->value = mapping(f, root->value); 
    root->sum = mapping(f, root->sum);
    root->lazy = composition(f,root->lazy);
  }
  splay_tree_explicit(){}
  /*
  splay_tree(vector<T>& v){
    int n = v.size();
    for(int i = 0; i < n; i++) make_node(v[i]);
    function<node*(int, int)> dfs = [&](int l, int r){
      if(l >= r) return (node*)nullptr;
      if(l == r-1) return nodes[l].get();
      int mid = (r+l)/2;
      node* c = nodes[mid].get();
      node* d1 = dfs(l,mid);
      node* d2 = dfs(mid+1,r);
      if(d1) {d1->pp = c;c->lp = d1;}
      if(d2) {d2->pp = c;c->rp = d2;}
      c->cul();
      return c;
    };
    root = dfs(0,n);
  }
  */
  
  void print(){
    for(int i = 0; i < (int)nodes.size(); i++){
      node* c = nodes[i].get();
      cout << "i:" << c->index;
      cout << " p:"; if(c->pp) cout << c->pp->index; else cout << " ";
      cout << " r:"; if(c->rp) cout << c->rp->index; else cout << " ";
      cout << " l:"; if(c->lp) cout << c->lp->index; else cout << " ";
      cout << " key:" << c-> key;
      cout << " sumz:" << c->sumz;
      cout << " value:" << c->value.val();
      cout << " sum:" << c->sum.val();
      cout << " lazy:" << c->lazy.val();
      cout << endl;
    }
  }
};

using S = pm;
S e() {
  return {1,0};
}
S op(S x, S y) {
  return {x.fi*y.fi, y.se+y.fi*x.se};
}
using F = bool;
S mapping(F f, S x){
  return f ? x : e();
}
F composition(F f, F g){
  return f && g;
}
F id(){
  return true;
}

int main(){
  splay_tree_explicit<ll,S,op,e,F,mapping,composition,id> st;
  readi(n,q);
  while(q--){
    readi(t,a,b,c);
    if(t == 0){
      if(st.contains(a)) st.set(a,pm{b,c});
      else st.insert(a, pm{b, c});
    }else{
      auto f = st.prod(a,b);
      out(f.fi * c + f.se);
    }
  }
}