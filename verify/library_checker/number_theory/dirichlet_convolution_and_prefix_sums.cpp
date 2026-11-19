#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include "library/math/dirichlet_sum.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

using Mint = soryuusi::modint998244353;
using Dirichlet = soryuusi::Dirichlet_Sum<Mint>;

Dirichlet read_function(){
  const int boundary = Dirichlet::eval_points.back();
  std::vector<Mint> lower(Dirichlet::K), upper(Dirichlet::L);
  for(int i = 0; i < boundary-1; ++i) soryuusi::cin >> lower[i];
  for(int i = (int)upper.size()-1; i >= 0; --i) soryuusi::cin >> upper[i];
  lower[boundary-1] = upper.back();
  // Extend the lower table through K without consuming another input value.
  const auto& points = Dirichlet::eval_points;
  for(int i = boundary; i < (int)lower.size(); ++i){
    auto pos = std::lower_bound(points.rbegin(),points.rend(),i+1);
    lower[i] = pos != points.rend() && *pos == i+1 ? upper[points.rend()-pos-1] : lower[i-1];
  }
  for(int i = (int)lower.size()-1; i > 0; --i) lower[i] -= lower[i-1];
  return Dirichlet(std::move(lower),std::move(upper));
}

int main(){
  int t;
  soryuusi::cin >> t;
  while(t--){
    long long n;
    soryuusi::cin >> n;
    Dirichlet::init(n,(long long)std::sqrt((long double)n));
    auto f = read_function();
    auto h = f * read_function();
    Mint prefix = 0;
    int boundary = Dirichlet::eval_points.back();
    for(int i = 0; i < boundary-1; ++i){
      prefix += h.al[i];
      soryuusi::cout << prefix.val() << ' ';
    }
    for(int i = (int)h.au.size()-1; i >= 0; --i) soryuusi::cout << h.au[i].val() << " \n"[i == 0];
  }
}
