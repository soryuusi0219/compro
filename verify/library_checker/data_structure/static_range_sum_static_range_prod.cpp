#include "library/utility/fastio.hpp"

#include <utility>
#include <vector>

#include "library/data_structure/static_range_prod.hpp"

namespace {

long long sum(long long x, long long y){return x+y;}
long long zero(){return 0;}

}  // namespace

int main(){
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> values(n);
  for(auto& x : values) soryuusi::cin >> x;
  const soryuusi::StaticRangeProd<long long,sum,zero,64> range(std::move(values));
  while(q--){
    int l, r;
    soryuusi::cin >> l >> r;
    soryuusi::cout << range.prod(l,r) << '\n';
  }
}
