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
#define dame(a) {out(a);return 0;}
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
#define readvi1(v,n) vi v(n); rep(i,(n)) cin >> (v).at(i);
#define readvi2(v,w,n) vi v(n),w(n); rep(i,(n)) cin >> (v).at(i) >> (w).at(i);
#define readvi3(v,w,x,n) vi v(n),w(n),x(n); rep(i,(n)) cin >> (v).at(i) >> (w).at(i) >> (x).at(i);
#define readvi4(v,w,x,y,n) vi v(n),w(n),x(n); rep(i,(n)) cin >> (v).at(i) >> (w).at(i) >> (x).at(i) >> (y).at(i);
#define readvi(...) GET5(__VA_ARGS__, readvi3, readvi2, readvi1)(__VA_ARGS__)
#define reads1(n) string n; cin >> (n);
#define reads2(n,m) string n,m; cin >> (n) >> (m);
#define reads3(n,m,l) string n,m,l; cin >> (n) >> (m) >> (l);
#define reads4(n,m,l,k) string n,m,l,k; cin >> (n) >> (m) >> (l) >> (k);
#define reads(...) GET5(__VA_ARGS__, reads4, reads3, reads2, reads1)(__VA_ARGS__)
#define readvs(v,n) vs v(n); rep(i,(n)) cin >> (v).at(i);
#define readd1(n) ld n; cin >> (n);
#define readd2(n,m) ld n,m; cin >> (n) >> (m);
#define readd3(n,m,l) ld n,m,l; cin >> (n) >> (m) >> (l);
#define readd4(n,m,l,k) ld n,m,l; cin >> (n) >> (m) >> (l) >> (k);
#define readd(...) GET5(__VA_ARGS__, readd4, readd3, readd2, readd1)(__VA_ARGS__)
#define readvvi(v,n,m) vvi v((n),vi(m)); rep(i,(n))rep(j,(m)) cin >> (v).at(i).at(j);
#define readvvs(v,n,m) vvs v((n),vs(m)); rep(i,(n))rep(j,(m)) cin >> (v).at(i).at(j);
#define readvpi(v,n) vpi v(n); rep(i,(n)) cin >> (v).at(i).fi >> (v).at(i).se;
#define readg(G,n,m) vvi G(n);rep(i,m){readi(a,b);a--;b--;G.at(a).push_back(b);G.at(b).push_back(a);}
#define readgd(G,n,m) vvi G(n);rep(i,m){readi(a,b);a--;b--;G.at(a).push_back(b);}
#define readgdrev(G,n,m) vvi G(n);rep(i,m){readi(a,b);a--;b--;G.at(b).push_back(a);}
#define readt(G,n) vvi G(n);rep(i,n-1){readi(a,b);a--;b--;G.at(a).push_back(b);G.at(b).push_back(a);}
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
//////////////////////////////////////////////////////////
template<class T> using PQ = priority_queue<T>;
template<class T> using SPQ = priority_queue<T,vector<T>,greater<T>>;
template<class T> bool chmin(T&a,T b){if(a>b){a=b;return true;}return false;}
template<class T> bool chmax(T&a,T b){if(a<b){a=b;return true;}return false;}
void out0(ll a){cout<<a;}
void out0(unsigned int a){cout<<a;}
void out0(int a){cout<<a;}
void out0(long int a){cout<<a;}
void out0(long unsigned int a){cout<<a;}
void out0(unsigned long long a){cout<<a;}
void out0(char a){cout<<a;}
void out0(char* a){cout<<a;}
void out0(string a){cout<<a;}
void out0(float a){cout<<fixed<<setprecision(15)<<a;}
void out0(double a){cout<<fixed<<setprecision(15)<<a;}
void out0(ld a){cout<<fixed<<setprecision(15)<<a;}
template<class T> void out0s(T a);
template<class T1, class T2> void out0(pair<T1,T2> p){out0s(p.fi); out0s(p.se);}
template < typename Tuple, size_t ...I >
array<int,sizeof...(I)>
tuple_print(Tuple&& t, index_sequence<I...>){
  return {{ (void( out0s(get<I>(t))), 0)... }};
}
template<class... T> void out0(tuple<T...> p){
  tuple_print(p,
  make_index_sequence<tuple_size<decay_t<tuple<T...>>>::value>{});
}
template<class T> void out0(vector<T> v){for(T c : v) out0s(c);}
template<class T> void out0(set<T> v){for(T c : v) out0s(c);}
template<class T> void out0s(T a){out0(a); cout << " ";}
template<class T> void out(VV<T> v){for(auto c : v) {out0(c); cout << endl;}}
template<class S, class T> void out(map<S,T> v){for(auto c : v) {out0(c); cout << endl;}}
template<class T>void out(T a) {out0(a); cout << endl;}
template<class T, class... Ts>void out(T a, Ts... b) {out0(a); cout << " "; out(b...);}
void out0d(ll a){cout<<a;}
void out0d(int a){cout<<a;}
void out0d(unsigned int a){cout<<a;}
void out0d(long unsigned int a){cout<<a;}
void out0d(long int a){cout<<a;}
void out0d(unsigned long long a){cout<<a;}
void out0d(char a){cout<<a;}
void out0d(char* a){cout<<a;}
void out0d(string a){cout<<a;}
void out0d(float a){cout<<fixed<<setprecision(15)<<a;}
void out0d(double a){cout<<fixed<<setprecision(15)<<a;}
void out0d(ld a){cout<<fixed<<setprecision(15)<<a;}
template<class T> void out0sd(T a);
template<class T1, class T2> void out0d(pair<T1,T2> p){cout << "("; out0sd(p.fi); out0d(p.se); cout << ")";}
template < typename Tuple, size_t ...I >
array<int,sizeof...(I)>
tuple_printd(Tuple&& t, index_sequence<I...>){
  return {{ (void( out0s(get<I>(t))), 0)... }};
}
template<class... T> void out0d(tuple<T...> p){
  cout << "(";
  tuple_printd(p,
  make_index_sequence<tuple_size<decay_t<tuple<T...>>>::value>{});
  cout << ")";
}
template<class T> void out0d(vector<T> v){for(T c : v) out0sd(c);}
template<class T> void out0d(set<T> v){for(T c : v) out0sd(c);}
template<class T> void out0sd(T a){out0d(a); cout << " ";}
template<class T> void outd(VV<T> v){for(auto c : v) {out0d(c); cout << endl;}}
template<class S, class T> void outd(map<S,T> v){for(auto c : v) {out0d(c); cout << endl;}}
template<class T>void outd(T a) {out0d(a); cout << endl;}
template<class T, class... Ts>void outd(T a, Ts... b) {out0d(a); cout << " "; outd(b...);}
template<class T> void outm(T a){cout<<a.val()<<'\n';}
template<class T> void outvm(T v){rep(i,v.size()){if(i)cout<<' ';cout<<v[i].val();}cout<<'\n';}
template<class T> void outvvm(T v){rep(i,v.size())outvm(v[i]);}
template<class T> bool isin(T x,T l,T r){return (l)<=(x)&&(x)<=(r);}
template<class T> void yesno(T b){if(b)out("yes");else out("no");}
template<class T> void YesNo(T b){if(b)out("Yes");else out("No");}
template<class T> void YESNO(T b){if(b)out("YES");else out("NO");}
template<class T> void posimp(T b){if(b)out("possible");else out("impossible");}
template<class T> void PosImp(T b){if(b)out("Possible");else out("Impossible");}
template<class T> void POSIMP(T b){if(b)out("POSSIBLE");else out("IMPOSSIBLE");}
void outs(ll a,ll b,ll i){if(abs(a)>=i-100)out(b);else out(a);}
ll gcd(ll a,ll b){if(b==0)return a;return gcd(b,a%b);}
ll lcm(ll a,ll b){return (a / gcd(a,b)) * b;}
ll powll(ll a,ll b){ll res=1;while(b){if(b&1)res=res*a;a=a*a;b>>=1;}return res;}
ll modpow(ll a,ll b,ll modd){ll res=1;a%=modd;while(b){if(b&1)res=res*a%modd;a=a*a%modd;b>>=1;}return res;}
ll sqrtll(ll a){assert(a >= 0); ll r = (ll)sqrtl((ld)a)-1; while(r < 0 || (r+1)*(r+1) <= a) r++; return r;}
ll cbrtll(ll a){assert(a >= 0); ll r = (ll)cbrtl((ld)a)-1; while(r < 0 || (r+1)*(r+1)*(r+1) <= a) r++; return r;}
ll modinv(ll a, ll b){assert(a); if(a == 1) return 1; if(b == 1) return 0; ll ret = (1ll-b*modinv(b%a, a))/a; ret %= b; if(ret < 0) ret += b; return ret;}

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
template<int M> void out0(static_modint<M> a){cout<<a.val();}
template<int M> void out0(dynamic_modint<M> a){cout<<a.val();}
template<int M> void out0d(static_modint<M> a){cout<<a.val();}
template<int M> void out0d(dynamic_modint<M> a){cout<<a.val();}
/////////////////////////////////////////////////////////

struct eular_tour_tree{
  struct splay_tree {
    struct node{  
      node* pp = nullptr;
      node* lp = nullptr;
      node* rp = nullptr;
      int index = -1;
      int z = 0;
      int sumz = 0;
      node(int i) : index(i), z(1), sumz(1){}
      void cul() {
        sumz = z;
        if(rp) sumz += rp->sumz;
        if(lp) sumz += lp->sumz;
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
      void splay(){
        while(pp){
          node* q = pp;
          if(!(q->pp)){
            if(q->lp == this) rotr();
            else rotl();
          }else{
            node* r = q->pp;
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
    node* most_left(node* c){
      while(c -> lp) {
        c = c -> lp;
      }
      c->splay();
      return c;
    }
    node* most_right(node* c){
      while(c -> rp){
        c = c -> rp;
      }
      c->splay();
      return c;
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
      c->splay();
      return c;
    }
    node* next(node* c){
      if(!c) return {nullptr};
      c->splay();
      if(!c -> rp) return nullptr;
      node* d = most_left(c->rp);
      d->splay();
      return d;
    }
    node* prev(node* c){
      if(!c) return {nullptr};
      c->splay();
      if(!c -> lp) return nullptr;
      node* d = most_right(c->lp);
      d->splay();
      return d;
    }
    pair<node*, node*> split_right(node* c){
      if(!c) return {nullptr,nullptr};
      node* d = next(c);
      if(!d) return {c,nullptr};
      c->pp = nullptr;
      d->lp = nullptr;
      d->cul();
      return {c,d};
    }
    pair<node*, node*> split_left(node* c){
      if(!c) return {nullptr,nullptr};
      node* d = prev(c);
      if(!d) return {nullptr,c};
      c->pp = nullptr;
      d->rp = nullptr;
      d->cul();
      return {d,c};
    }
    node* merge(node* c, node* d){
      if(!c) return d;
      if(!d) return c;
      c->splay();
      d->splay();
      d = most_left(d);
      d->splay();
      if(c->pp) return d;
      c->pp = d;
      d->lp = c;
      d->cul();
      return d;
    }
    node* make_node(){
      int index = nodes.size();
      nodes.push_back(make_unique<node>(node(index)));
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
    node* insert_right(node* c){
      node* d = make_node();
      auto [c1, c2] = split_right(c);
      return merge(merge(c1,d),c2);
    }
    node* insert_left(node* c){
      node* d = make_node();
      auto [c1, c2] = split_left(c);
      return merge(merge(c1,d),c2);
    }
    node* erase(node* c){
      if(!c) return nullptr;
      c->splay();
      node* d1 = c->lp;
      node* d2 = c->rp;
      if(d1) d1->pp = nullptr;
      if(d2) d2->pp = nullptr;
      delete_node(c);
      return merge(d1,d2);
    }
  };
};

int main(){
  #ifndef _DEBUG
  std::cin.tie(nullptr), std::ios_base::sync_with_stdio(false);
  #endif
}