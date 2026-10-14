// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_template.hpp
#pragma once

namespace soryuusi {

/*
segtree<S,op,e>

S e() {
  return ;
}
S op(S x, S y) {
  return ;
}


segtree<ll,op,e>

//addition
ll e() {
  return 0;
}
ll op(ll x, ll y) {
  return x+y;
}

//minimum
ll e() {
  return INF;
}
ll op(ll x, ll y) {
  return min(x,y);
}

//maximum
ll e() {
  return 0;
}
ll op(ll x, ll y) {
  return max(x,y);
}


segtree<pi,op,e>

//pair addtion
pi e() {
  return {0,0};
}
pi op(pi x, pi y) {
  return {x.fi+y.fi,x.se+y.se};
}

segtree<mint,op,e>

//mint addtion
mint e() {
  return 0;
}
mint op(mint x, mint y) {
  return x+y;
}

segtree<pm,op,e>

//mint composite
mint e() {
  return {1,0};
}
mint op(pm x, pm y) {
  return {x.fi*y.fi,x.se+x.fi+y.se};
}
*/

}  // namespace soryuusi
