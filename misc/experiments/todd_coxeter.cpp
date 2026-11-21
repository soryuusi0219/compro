#ifdef _DEBUG
#define _GLIBCXX_DEBUG
#endif
#include <bits/stdc++.h>
// #include <atcoder/all>
using namespace std;

namespace atcoder {

namespace internal {

constexpr long long safe_mod(long long x, long long m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

struct barrett {
    unsigned int _m;
    unsigned long long im;

    explicit barrett(unsigned int m) : _m(m), im((unsigned long long)(-1) / m + 1) {}

    unsigned int umod() const { return _m; }

    unsigned int mul(unsigned int a, unsigned int b) const {

        unsigned long long z = a;
        z *= b;
#ifdef _MSC_VER
        unsigned long long x;
        _umul128(z, im, &x);
#else
        unsigned long long x =
            (unsigned long long)(((unsigned __int128)(z)*im) >> 64);
#endif
        unsigned long long y = x * _m;
        return (unsigned int)(z - y + (z < y ? _m : 0));
    }
};

constexpr long long pow_mod_constexpr(long long x, long long n, int m) {
    if (m == 1) return 0;
    unsigned int _m = (unsigned int)(m);
    unsigned long long r = 1;
    unsigned long long y = safe_mod(x, m);
    while (n) {
        if (n & 1) r = (r * y) % _m;
        y = (y * y) % _m;
        n >>= 1;
    }
    return r;
}

constexpr bool is_prime_constexpr(int n) {
    if (n <= 1) return false;
    if (n == 2 || n == 7 || n == 61) return true;
    if (n % 2 == 0) return false;
    long long d = n - 1;
    while (d % 2 == 0) d /= 2;
    constexpr long long bases[3] = {2, 7, 61};
    for (long long a : bases) {
        long long t = d;
        long long y = pow_mod_constexpr(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = y * y % n;
            t <<= 1;
        }
        if (y != n - 1 && t % 2 == 0) {
            return false;
        }
    }
    return true;
}
template <int n> constexpr bool is_prime = is_prime_constexpr(n);

constexpr std::pair<long long, long long> inv_gcd(long long a, long long b) {
    a = safe_mod(a, b);
    if (a == 0) return {b, 0};

    long long s = b, t = a;
    long long m0 = 0, m1 = 1;

    while (t) {
        long long u = s / t;
        s -= t * u;
        m0 -= m1 * u;  // |m1 * u| <= |m1| * s <= b


        auto tmp = s;
        s = t;
        t = tmp;
        tmp = m0;
        m0 = m1;
        m1 = tmp;
    }
    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

constexpr int primitive_root_constexpr(int m) {
    if (m == 2) return 1;
    if (m == 167772161) return 3;
    if (m == 469762049) return 3;
    if (m == 754974721) return 11;
    if (m == 998244353) return 3;
    int divs[20] = {};
    divs[0] = 2;
    int cnt = 1;
    int x = (m - 1) / 2;
    while (x % 2 == 0) x /= 2;
    for (int i = 3; (long long)(i)*i <= x; i += 2) {
        if (x % i == 0) {
            divs[cnt++] = i;
            while (x % i == 0) {
                x /= i;
            }
        }
    }
    if (x > 1) {
        divs[cnt++] = x;
    }
    for (int g = 2;; g++) {
        bool ok = true;
        for (int i = 0; i < cnt; i++) {
            if (pow_mod_constexpr(g, (m - 1) / divs[i], m) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
}
template <int m> constexpr int primitive_root = primitive_root_constexpr(m);

unsigned long long floor_sum_unsigned(unsigned long long n,
                                      unsigned long long m,
                                      unsigned long long a,
                                      unsigned long long b) {
    unsigned long long ans = 0;
    while (true) {
        if (a >= m) {
            ans += n * (n - 1) / 2 * (a / m);
            a %= m;
        }
        if (b >= m) {
            ans += n * (b / m);
            b %= m;
        }

        unsigned long long y_max = a * n + b;
        if (y_max < m) break;
        n = (unsigned long long)(y_max / m);
        b = (unsigned long long)(y_max % m);
        std::swap(m, a);
    }
    return ans;
}

}  // namespace internal

}  // namespace atcoder


#include <cassert>
#include <numeric>
#include <type_traits>

namespace atcoder {

namespace internal {

#ifndef _MSC_VER
template <class T>
using is_signed_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value ||
                                  std::is_same<T, __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int128 =
    typename std::conditional<std::is_same<T, __uint128_t>::value ||
                                  std::is_same<T, unsigned __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using make_unsigned_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value,
                              __uint128_t,
                              unsigned __int128>;

template <class T>
using is_integral = typename std::conditional<std::is_integral<T>::value ||
                                                  is_signed_int128<T>::value ||
                                                  is_unsigned_int128<T>::value,
                                              std::true_type,
                                              std::false_type>::type;

template <class T>
using is_signed_int = typename std::conditional<(is_integral<T>::value &&
                                                 std::is_signed<T>::value) ||
                                                    is_signed_int128<T>::value,
                                                std::true_type,
                                                std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<(is_integral<T>::value &&
                               std::is_unsigned<T>::value) ||
                                  is_unsigned_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<
    is_signed_int128<T>::value,
    make_unsigned_int128<T>,
    typename std::conditional<std::is_signed<T>::value,
                              std::make_unsigned<T>,
                              std::common_type<T>>::type>::type;

#else

template <class T> using is_integral = typename std::is_integral<T>;

template <class T>
using is_signed_int =
    typename std::conditional<is_integral<T>::value && std::is_signed<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<is_integral<T>::value &&
                                  std::is_unsigned<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<is_signed_int<T>::value,
                                              std::make_unsigned<T>,
                                              std::common_type<T>>::type;

#endif

template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T> using to_unsigned_t = typename to_unsigned<T>::type;

}  // namespace internal

}  // namespace atcoder


namespace atcoder {

namespace internal {

struct modint_base {};
struct static_modint_base : modint_base {};

template <class T> using is_modint = std::is_base_of<modint_base, T>;
template <class T> using is_modint_t = std::enable_if_t<is_modint<T>::value>;

}  // namespace internal

template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : internal::static_modint_base {
    using mint = static_modint;

  public:
    static constexpr int mod() { return m; }
    static mint raw(int v) {
        mint x;
        x._v = v;
        return x;
    }

    static_modint() : _v(0) {}
    template <class T, internal::is_signed_int_t<T>* = nullptr>
    static_modint(T v) {
        long long x = (long long)(v % (long long)(umod()));
        if (x < 0) x += umod();
        _v = (unsigned int)(x);
    }
    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    static_modint(T v) {
        _v = (unsigned int)(v % umod());
    }

    unsigned int val() const { return _v; }

    mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        _v -= rhs._v;
        if (_v >= umod()) _v += umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        unsigned long long z = _v;
        z *= rhs._v;
        _v = (unsigned int)(z % umod());
        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        if (prime) {
            assert(_v);
            return pow(umod() - 2);
        } else {
            auto eg = internal::inv_gcd(_v, m);
            assert(eg.first == 1);
            return eg.second;
        }
    }

    friend mint operator+(const mint& lhs, const mint& rhs) {
        return mint(lhs) += rhs;
    }
    friend mint operator-(const mint& lhs, const mint& rhs) {
        return mint(lhs) -= rhs;
    }
    friend mint operator*(const mint& lhs, const mint& rhs) {
        return mint(lhs) *= rhs;
    }
    friend mint operator/(const mint& lhs, const mint& rhs) {
        return mint(lhs) /= rhs;
    }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return lhs._v != rhs._v;
    }

  private:
    unsigned int _v;
    static constexpr unsigned int umod() { return m; }
    static constexpr bool prime = internal::is_prime<m>;
};

template <int id> struct dynamic_modint : internal::modint_base {
    using mint = dynamic_modint;

  public:
    static int mod() { return (int)(bt.umod()); }
    static void set_mod(int m) {
        assert(1 <= m);
        bt = internal::barrett(m);
    }
    static mint raw(int v) {
        mint x;
        x._v = v;
        return x;
    }

    dynamic_modint() : _v(0) {}
    template <class T, internal::is_signed_int_t<T>* = nullptr>
    dynamic_modint(T v) {
        long long x = (long long)(v % (long long)(mod()));
        if (x < 0) x += mod();
        _v = (unsigned int)(x);
    }
    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    dynamic_modint(T v) {
        _v = (unsigned int)(v % mod());
    }

    unsigned int val() const { return _v; }

    mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        _v += mod() - rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        _v = bt.mul(_v, rhs._v);
        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        auto eg = internal::inv_gcd(_v, mod());
        assert(eg.first == 1);
        return eg.second;
    }

    friend mint operator+(const mint& lhs, const mint& rhs) {
        return mint(lhs) += rhs;
    }
    friend mint operator-(const mint& lhs, const mint& rhs) {
        return mint(lhs) -= rhs;
    }
    friend mint operator*(const mint& lhs, const mint& rhs) {
        return mint(lhs) *= rhs;
    }
    friend mint operator/(const mint& lhs, const mint& rhs) {
        return mint(lhs) /= rhs;
    }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return lhs._v != rhs._v;
    }

  private:
    unsigned int _v;
    static internal::barrett bt;
    static unsigned int umod() { return bt.umod(); }
};
template <int id> internal::barrett dynamic_modint<id>::bt(998244353);

using modint998244353 = static_modint<998244353>;
using modint1000000007 = static_modint<1000000007>;
using modint = dynamic_modint<-1>;

namespace internal {

template <class T>
using is_static_modint = std::is_base_of<internal::static_modint_base, T>;

template <class T>
using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;

template <class> struct is_dynamic_modint : public std::false_type {};
template <int id>
struct is_dynamic_modint<dynamic_modint<id>> : public std::true_type {};

template <class T>
using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;

}  // namespace internal

}  // namespace atcoder

namespace atcoder {

struct dsu {
  public:
    dsu() : _n(0) {}
    explicit dsu(int n) : _n(n), parent_or_size(n, -1) {}

    int merge(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        int x = leader(a), y = leader(b);
        if (x == y) return x;
        if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);
        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        return x;
    }

    bool same(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        return leader(a) == leader(b);
    }

    int leader(int a) {
        assert(0 <= a && a < _n);
        if (parent_or_size[a] < 0) return a;
        return parent_or_size[a] = leader(parent_or_size[a]);
    }

    int size(int a) {
        assert(0 <= a && a < _n);
        return -parent_or_size[leader(a)];
    }

    std::vector<std::vector<int>> groups() {
        std::vector<int> leader_buf(_n), group_size(_n);
        for (int i = 0; i < _n; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(_n);
        for (int i = 0; i < _n; i++) {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < _n; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [&](const std::vector<int>& v) { return v.empty(); }),
            result.end());
        return result;
    }

  private:
    int _n;
    std::vector<int> parent_or_size;
};

}  // namespace atcoder

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

tuple<ll, vi,vi> todd_coxeter(const vector<string>& s, const ll M = 1000){
  vi nxta(M,-1);
  vi nxtb(M,-1);
  vi prva(M,-1);
  vi prvb(M,-1);
  vi data(M,0);
  dsu d(M);
  ll id = 1;
  auto get_nxta = [&](ll x)->ll {
    x = d.leader(x);
    if(nxta[x] != -1) return d.leader(nxta[x]);
    data[id] = 1;
    prva[id] = x;
    nxta[x] = id++;
    return nxta[x];
  };
  auto get_nxtb = [&](ll x)->ll {
    x = d.leader(x);
    if(nxtb[x] != -1) return d.leader(nxtb[x]);
    data[id] = 1;
    prvb[id] = x;
    nxtb[x] = id++;
    return nxtb[x];
  };
  function<ll(ll,ll)> merge = [&](ll i, ll j)->ll {
    if(i < 0) return j;
    if(j < 0) return i;
    i = d.leader(i);
    j = d.leader(j);
    if(i == j) return i;
    ll k = d.merge(i,j);
    data[k] = max(data[i],data[j]);
    if(~nxta[i] && ~nxta[j]) nxta[k] = merge(nxta[i],nxta[j]);
    if(~nxtb[i] && ~nxtb[j]) nxtb[k] = merge(nxtb[i],nxtb[j]);
    if(~prva[i] && ~prva[j]) prva[k] = merge(prva[i],prva[j]);
    if(~prvb[i] && ~prvb[j]) prvb[k] = merge(prvb[i],prvb[j]);
    return k;
  };
  auto reduction = [&](ll i, const string& t) -> void {
    // outf(i,t);
    i = d.leader(i);
    ll j = i;
    for(auto c : t){
      if(c == 'A') j = get_nxta(j);
      else j = get_nxtb(j);
    }
    merge(i,j);
  };
  data[0] = 1;
  rep(i,M) if(data[d.leader(i)] == 1){
    data[d.leader(i)] = 2;
    for(auto& s0 : s) reduction(i,s0);
  }
  vi ids(M,-1);
  vi valid;
  rep(i,M) if(d.leader(i) == i && data[i]){
    ids[i] = valid.size();
    valid.push_back(i);
  }
  // out(ids);
  // out(valid);
  ll m = valid.size();
  vi perma(m), permb(m);
  rep(i,m){
    perma[i] = ids[get_nxta(valid[i])];
    permb[i] = ids[get_nxtb(valid[i])];
  }

  return {m,perma,permb};
}

int main(){
  std::cin.tie(nullptr), std::ios_base::sync_with_stdio(false);
  readi(n);
  readvs(s,n);
  out(todd_coxeter(s,1000000));
}