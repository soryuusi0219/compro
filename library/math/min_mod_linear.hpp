// https://github.com/soryuusi0219/compro/blob/main/library/math/min_mod_linear.hpp
#pragma once
#include <numeric>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

/**
 * @brief Staircase Convex Hull of (x, (Ax+B mod M)) for (x = 0,...)
 */
template<class INT = long long>
pair<vector<INT>,vector<INT>> ModLinearPrefixMinimum(INT a, INT b, INT m){
  vector<INT> vx = {0}, vdx;
  INT g = gcd(a, m);
  a /= g, b /= g, m /= g;
  INT p = 0, q = 1, r = 1, s = 1;
  INT dl = m - a, dr = a;
  INT x = 0, y = b;
  while(y){
    INT k = dr / dl;
    dr %= dl;
    if(!dr) --k, dr = dl;
    r += k*p, s += k*q;
    while(1){
      INT k = (dl-y > -dr) ? (dl-y+dr-1) / dr : 0;
      if(dl - k*dr <= 0) break;
      dl -= k*dr, p += k*r, q += k*s;
      k = y/dl;
      y -= k*dl;
      x += q*k;
      vx.push_back(x);
      vdx.push_back(q);
    }
    k = dl / dr;
    dl -= k * dr;
    p += k*r;
    q += k*s;
  }
  return {vx, vdx};
}

/**
 * @brief Min of (Ax+B mod M) for (L <= x < R)
 */
template<class INT = long long>
pair<INT,INT> MinModLinear(INT a, INT b, INT m, INT l, INT r){
  if((a %= m) < 0) a += m;
  if(((b += a*l) %= m) < 0) b += m;
  r -= l;
  auto [vx,vdx] = ModLinearPrefixMinimum(a,b,m);
  INT x = 0;
  for(int i = 1; i < (int)vx.size(); ++i){
    if(vx[i] >= r){
      x += (r-x-1) / vdx[i-1] * vdx[i-1];
      break;
    }
    x = vx[i];
  }
  return {x+l, (a*x+b)%m};
}

}  // namespace soryuusi
