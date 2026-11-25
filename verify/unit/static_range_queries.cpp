#include <algorithm>
#include <cassert>
#include <limits>
#include <random>
#include <utility>
#include <vector>

#include "library/data_structure/static_range_prod.hpp"
#include "library/data_structure/static_rmq.hpp"

namespace {

struct Affine {
  int a, b;
  bool operator==(const Affine&) const = default;
};
Affine compose(Affine f, Affine g){return {g.a*f.a%1009,(g.a*f.b+g.b)%1009};}
Affine identity(){return {1,0};}
int minimum(int a, int b){return std::min(a,b);}
constexpr int infinity(){return std::numeric_limits<int>::max();}

template<int BlockSize>
void check_monoid(const std::vector<Affine>& values){
  const soryuusi::StaticRangeProd<Affine,compose,identity,BlockSize> range(values);
  const int n = values.size();
  for(int l = 0; l <= n; ++l){
    Affine expected = identity();
    for(int r = l; r <= n; ++r){
      assert(expected == range.prod(l,r));
      if(r < n) expected = compose(expected,values[r]);
    }
  }
  assert(range.naive_prod(0,n) == range.prod(0,n));
}

template<int BlockSize>
void check_rmq(soryuusi::StaticRMQ<int,false,BlockSize>& min_range,
    soryuusi::StaticRMQ<int,true,BlockSize>& max_range, const std::vector<int>& values){
  min_range.build(values);
  max_range.build(values);
  const int n = values.size();
  for(int l = 0; l <= n; ++l){
    int mn = std::numeric_limits<int>::max(), mx = std::numeric_limits<int>::lowest();
    for(int r = l; r <= n; ++r){
      assert(mn == min_range.prod(l,r));
      assert(mx == max_range.prod(l,r));
      if(r < n){
        mn = std::min(mn,values[r]);
        mx = std::max(mx,values[r]);
      }
    }
  }
  assert(min_range.naive_prod(0,n) == min_range.prod(0,n));
  assert(max_range.naive_prod(0,n) == max_range.prod(0,n));
}

template<int BlockSize>
void test_rebuild(std::mt19937& gen){
  soryuusi::StaticRMQ<int,false,BlockSize> min_range;
  soryuusi::StaticRMQ<int,true,BlockSize> max_range;
  for(int n : {0,1,7,8,9,15,16,17,31,32,33,63,64,65,129,259,3,0}){
    std::vector<int> values(n);
    for(int& value : values) value = int(gen()%19)-9;
    check_rmq(min_range,max_range,values);
  }
  std::vector<int> values(97,std::numeric_limits<int>::lowest());
  check_rmq(min_range,max_range,values);
  std::fill(values.begin(),values.end(),std::numeric_limits<int>::max());
  check_rmq(min_range,max_range,values);
}

}  // namespace

int main(){
  std::mt19937 gen(3614);
  for(int n : {0,1,2,3,4,5,7,8,9,15,16,17,31,32,63,64,65,127,129,259}){
    std::vector<Affine> values(n);
    for(auto& [a,b] : values) a = gen()%1009, b = gen()%1009;
    check_monoid<1>(values);
    check_monoid<2>(values);
    check_monoid<4>(values);
    check_monoid<8>(values);
    check_monoid<16>(values);
    check_monoid<32>(values);
  }
  test_rebuild<0>(gen);
  test_rebuild<1>(gen);
  test_rebuild<8>(gen);
  test_rebuild<16>(gen);
  test_rebuild<32>(gen);
  test_rebuild<64>(gen);
  const soryuusi::StaticRangeProd<int,minimum,infinity> legacy({4,5,1,3});
  assert(legacy.prod(0,4) == 1);
  const soryuusi::StaticRMQ<int> defaults({4,5,1,3});
  assert(defaults.prod(0,4) == 1);
  const double inf = std::numeric_limits<double>::infinity();
  const soryuusi::StaticRMQ<double> floating({inf,-1.,-inf});
  assert(floating.prod(0,0) == inf && floating.prod(0,3) == -inf);

  constexpr auto pair_identity = [](){return std::pair{99,-1};};
  const soryuusi::StaticRMQ<std::pair<int,int>,false,8,pair_identity> pairs({{2,4},{2,3},{4,0}});
  assert(pairs.prod(0,2) == std::make_pair(2,3));
}
