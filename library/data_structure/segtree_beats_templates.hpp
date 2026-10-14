// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_beats_templates.hpp
#pragma once

namespace soryuusi {

/*
// Range (chmax,chmin,add)
// Range (max,min,sum)

struct S{
  long long sum,len,mx1,mx2,mxc,mn1,mn2,mnc;
  bool fail;
  S() : sum(0), len(0), mx1(-INF), mx2(-INF), mxc(0), mn1(INF), mn2(INF), mnc(0), fail(false){}
  S(long long x) : sum(x), len(1), mx1(x), mx2(-INF), mxc(1), mn1(x), mn2(INF), mnc(1), fail(false){}
};
S e() {
  return S();
}
S op(S x, S y) {
  S ret;
  ret.sum = x.sum + y.sum;
  ret.len = x.len + y.len;
  if(x.mx1 == y.mx1){
    ret.mx1 = x.mx1;
    ret.mx2 = max(x.mx2,y.mx2);
    ret.mxc = x.mxc+y.mxc;
  }
  else if(x.mx1 > y.mx1){
    ret.mx1 = x.mx1;
    ret.mx2 = max(x.mx2,y.mx1);
    ret.mxc = x.mxc;
  }
  else{
    ret.mx1 = y.mx1;
    ret.mx2 = max(x.mx1,y.mx2);
    ret.mxc = y.mxc;
  }
  if(x.mn1 == y.mn1){
    ret.mn1 = x.mn1;
    ret.mn2 = min(x.mn2,y.mn2);
    ret.mnc = x.mnc+y.mnc;
  }
  else if(x.mn1 < y.mn1){
    ret.mn1 = x.mn1;
    ret.mn2 = min(x.mn2,y.mn1);
    ret.mnc = x.mnc;
  }
  else{
    ret.mn1 = y.mn1;
    ret.mn2 = min(x.mn1,y.mn2);
    ret.mnc = y.mnc;
  }
  return ret;
}
struct F{
  long long mx,mn,add;
  F() : mx(-INF), mn(INF), add(0){}
  F(long long a, long long x){
    if(a == 1) {mx = x; mn = INF; add = 0;} //mx
    else if(a == 0) {mx = -INF; mn = x; add = 0;} //mn
    else if(a == 2) {mx = -INF; mn = INF; add = x;} //add
  }
  bool operator==(F o){
    return (mx == o.mx && mn == o.mn && add == o.add);
  }
};

S mapping(F f, S x){
  S ret = x;
  if(ret.mn2 < f.mx){
    ret.fail = true;
    return ret;
  }
  else if(ret.mn1 < f.mx){
    ret.sum += ret.mnc * (f.mx-ret.mn1);
    if(ret.mn1 == ret.mx1) ret.mx1 = f.mx;
    else if(ret.mn1 == ret.mx2) ret.mx2 = f.mx;
    ret.mn1 = f.mx;
  }
  if(ret.mx2 > f.mn){
    ret.fail = true;
    return ret;
  }
  else if(ret.mx1 > f.mn){
    ret.sum += ret.mxc * (f.mn-ret.mx1);
    if(ret.mx1 == ret.mn1) ret.mn1 = f.mn;
    else if(ret.mx1 == ret.mn2) ret.mn2 = f.mn;
    ret.mx1 = f.mn;
  }
  ret.sum += f.add * ret.len;
  ret.mx1 += f.add;
  ret.mx2 += f.add;
  ret.mn1 += f.add;
  ret.mn2 += f.add;
  return ret;
}
F composition(F f, F g){
  F ret = g;
  long long k = f.mx;
  k -= ret.add;
  chmax(ret.mn,k);
  chmax(ret.mx,k);
  long long l = f.mn;
  l -= ret.add;
  chmin(ret.mn,l);
  ret.add += f.add;
  return ret;
}
F id(){
  F ret;
  return ret;
}

segtree_beats<S, op, e, F, mapping, composition, id>;
*/

}  // namespace soryuusi
