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


template <class S, S (*op)(S, S), S (*e)()>
struct TreeRangeContour {
  struct Ancestor { int g, comp_id, dist, seg_leaf_idx; };

  int n;
  vector<S> v;

  // 内部グラフ用 (最大頂点数は 2N になるため 1次元配列によるCSRライクなリスト形式)
  vector<int> h, nxt, to, weight;

  // 頂点ごとの属する成分情報 (CSR形式)
  vector<int> anc_head, anc_nxt;
  vector<Ancestor> anc_list;

  // 各重心の持つ成分ID (CSR形式)
  vector<int> g_comp_head, g_comp_nxt, g_comp_list;

  // 各成分(Segment Tree)ごとの情報
  vector<int> comp_dist_offset, comp_size, comp_seg_offset;
  vector<int> dists_flat;
  vector<S> seg_flat;

  vector<int> sz;
  vector<bool> used;

  TreeRangeContour() {}

  TreeRangeContour(int n_) : n(n_), v(n, e()), h(n){
    nxt.reserve(2*n-2);
    to.reserve(2*n-2);
  }

  TreeRangeContour(const vector<S>& v_) : n(a.size()), v(v_), h(n){
    nxt.reserve(2*n-2);
    to.reserve(2*n-2);
  }

  void add_edge(int u, int v) {
    nxt.push_back(h[u]); h[u] = to.size(); to.push_back(v);
    nxt.push_back(h[v]); h[v] = to.size(); to.push_back(u);
  }

  void add_anc(int v, int g, int comp_id, int dist, int seg_leaf_idx) {
    anc_nxt.push_back(anc_head[v]);
    anc_list.push_back({g, comp_id, dist, seg_leaf_idx});
    anc_head[v] = anc_nxt.size() - 1;
  }

  void add_g_comp(int g, int comp_id) {
    g_comp_nxt.push_back(g_comp_head[g]);
    g_comp_list.push_back(comp_id);
    g_comp_head[g] = g_comp_nxt.size() - 1;
  }

  void build() {
    int max_v = 2 * n;
    head.assign(max_v, -1);
    nxt.clear(); to.clear(); weight.clear();
    anc_head.assign(n, -1);
    g_comp_head.assign(max_v, -1);
    sz.assign(max_v, 0);
    used.assign(max_v, false);

    vector<int> orig_head(n, -1), orig_nxt, orig_to;
    for (auto& edge : temp_edges) {
      orig_nxt.push_back(orig_head[edge.first]); orig_to.push_back(edge.second); orig_head[edge.first] = orig_nxt.size() - 1;
      orig_nxt.push_back(orig_head[edge.second]); orig_to.push_back(edge.first); orig_head[edge.second] = orig_nxt.size() - 1;
    }

    auto build_tree = [&](auto& self, int u, int p) -> void {
      vector<int> children;
      for (int e_idx = orig_head[u]; ~e_idx; e_idx = orig_nxt[e_idx]) {
        if (orig_to[e_idx] != p) children.push_back(orig_to[e_idx]);
      }
      int cur = u;
      for (size_t i = 0; i < children.size(); ++i) {
        int v = children[i];
        if (i < children.size() - 2 && children.size() > 2) {
          int nxt_node = v_new++;
          add_new_edge(cur, nxt_node, 0);
          add_new_edge(cur, v, 1);
          cur = nxt_node;
        } else {
          add_new_edge(cur, v, 1);
        }
      }
      for (int v : children) self(self, v, u);
    };
    build_tree(build_tree, 0, -1);

    int comp_count = 0;
    auto get_centroid = [&](int root) -> int {
      auto get_sz = [&](auto& self, int v, int p) -> void {
        sz[v] = 1;
        for (int e_idx = head[v]; ~e_idx; e_idx = nxt[e_idx]) {
          int nxt_v = to[e_idx];
          if (nxt_v != p && !used[nxt_v]) {
            self(self, nxt_v, v);
            sz[v] += sz[nxt_v];
          }
        }
      };
      get_sz(get_sz, root, -1);
      int total = sz[root];
      auto dfs = [&](auto& self, int v, int p) -> int {
        for (int e_idx = head[v]; ~e_idx; e_idx = nxt[e_idx]) {
          int nxt_v = to[e_idx];
          if (nxt_v != p && !used[nxt_v] && sz[nxt_v] * 2 > total) {
            return self(self, nxt_v, v);
          }
        }
        return v;
      };
      return dfs(dfs, root, -1);
    };

    auto build_cd = [&](auto& self, int root) -> void {
      int g = get_centroid(root);
      used[g] = true;
      // 【修正】自分自身に対する祖先情報を登録 (comp_id を -1 にしておく)
      if (g < n) {
        add_anc(g, g, -1, 0, -1);
      }
      for (int e_idx = head[g]; ~e_idx; e_idx = nxt[e_idx]) {
        int nxt_v = to[e_idx];
        int weight_e = weight[e_idx];
        if (!used[nxt_v]) {
          vector<pair<int, int>> nodes;
          auto dfs = [&](auto& self_dfs, int u, int p, int d) -> void {
            if (u < n) nodes.push_back({d, u});
            for (int ie = head[u]; ~ie; ie = nxt[ie]) {
              int iv = to[ie];
              if (iv != p && !used[iv]) {
                self_dfs(self_dfs, iv, u, d + weight[ie]);
              }
            }
          };
          dfs(dfs, nxt_v, g, weight_e);
          sort(nodes.begin(), nodes.end());
          int c_id = comp_count++;
          add_g_comp(g, c_id);
          int k = nodes.size();
          comp_size.push_back(k);
          comp_dist_offset.push_back(dists_flat.size());
          for (int i = 0; i < k; ++i) dists_flat.push_back(nodes[i].first);
          comp_seg_offset.push_back(seg_flat.size());
          int seg_st = seg_flat.size();
          seg_flat.resize(seg_st + 2 * k, e());
          for (int i = 0; i < k; ++i) {
            int v = nodes[i].second;
            int d = nodes[i].first;
            add_anc(v, g, c_id, d, i);
            seg_flat[seg_st + k + i] = val[v];
          }
          for (int i = k - 1; i > 0; --i) {
            seg_flat[seg_st + i] = op(seg_flat[seg_st + 2 * i], seg_flat[seg_st + 2 * i + 1]);
          }
        }
      }

      for (int e_idx = head[g]; ~e_idx; e_idx = nxt[e_idx]) {
        if (!used[to[e_idx]]) self(self, to[e_idx]);
      }
    };
    build_cd(build_cd, 0);
    temp_edges.clear();
    temp_edges.shrink_to_fit();
  }

  void set(int v, S x) {
    val[v] = x;
    for (int ae = anc_head[v]; ~ae; ae = anc_nxt[ae]) {
      auto& anc = anc_list[ae];
      // 【修正】自身のレコード (comp_id == -1) はセグメント木を持たないのでスキップ
      if (anc.comp_id == -1) continue;
      
      int c_id = anc.comp_id;
      int p = anc.seg_leaf_idx;

      int s_off = comp_seg_offset[c_id];
      int k = comp_size[c_id];
      p += k;
      seg_flat[s_off + p] = x;
      while (p > 1) {
        p >>= 1;
        seg_flat[s_off + p] = op(seg_flat[s_off + 2 * p], seg_flat[s_off + 2 * p + 1]);
      }
    }
  }

  S prod(int v, int a, int b) {
    if (a >= b) return e();
    S res = e();
    
    // 【修正】自己レコードから距離0として val[v] が加算されるため、手動加算を削除

    for (int ae = anc_head[v]; ~ae; ae = anc_nxt[ae]) {
      auto& anc = anc_list[ae];
      int g = anc.g;
      int v_comp_id = anc.comp_id; // g == v の時は -1 になる
      int d_g = anc.dist;

      if (g < n && a <= d_g && d_g < b) {
        res = op(res, val[g]);
      }

      for (int ce = g_comp_head[g]; ~ce; ce = g_comp_nxt[ce]) {
        int c_id = g_comp_list[ce];
        // v_comp_id が -1 の場合はどのコンポーネントもスキップされないため、すべての枝を検索可能
        if (c_id == v_comp_id) continue;

        int l_dist = a - d_g;
        int r_dist = b - d_g;
        if (l_dist >= r_dist) continue;

        int offset = comp_dist_offset[c_id];
        int k = comp_size[c_id];

        auto st = dists_flat.begin() + offset;
        int l_idx = lower_bound(st, st + k, l_dist) - st;
        int r_idx = lower_bound(st, st + k, r_dist) - st;

        if (l_idx < r_idx) {
          int s_off = comp_seg_offset[c_id];
          S sml = e(), smr = e();
          int l = l_idx + k;
          int r = r_idx + k;
          while (l < r) {
            if (l & 1) sml = op(sml, seg_flat[s_off + (l++)]);
            if (r & 1) smr = op(seg_flat[s_off + (--r)], smr);
            l >>= 1; r >>= 1;
          }
          res = op(res, op(sml, smr));
        }
      }
    }
    return res;
  }
};

ll op(ll x, ll y){return x+y;}
ll e(){return 0;}


int main(){
  std::cin.tie(nullptr), std::ios_base::sync_with_stdio(false);
  readi(n,q);
  readvi(a,n);
  TreeRangeContour<ll,op,e> g(a);
  rep(i,n-1){
    readi(u,v);
    g.add_edge(u,v);
  }
  g.build();
  while(q--){
    readi(t);
    if(t == 0){
      readi(p,x)
      g.set(p,a[p]+=x);
    }else{
      readi(p,a,b);
      out(g.prod(p,a,b));
    }
  }
}