#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <random>
#include <set>
#include <utility>
#include <vector>

#include "library/math/permutation_subgroup.hpp"

namespace {

using Permutation = std::vector<int>;

Permutation identity(int n){
  Permutation p(n);
  std::iota(p.begin(),p.end(),0);
  return p;
}

void compare_with_enumeration(const soryuusi::permutation_subgroup& group, int n, const std::vector<Permutation>& generators){
  auto p = identity(n);
  std::set<Permutation> expected{p};
  std::vector<Permutation> queue{p};
  for(std::size_t i = 0; i < queue.size(); ++i){
    for(const auto& generator : generators){
      for(int j = 0; j < n; ++j) p[j] = generator[queue[i][j]];
      if(expected.insert(p).second) queue.push_back(p);
    }
  }

  std::vector<std::vector<bool>> orbits(n,std::vector<bool>(n));
  for(const auto& q : expected){
    for(int k = n-1; k >= 0; --k){
      orbits[k][q[k]] = true;
      if(q[k] != k) break;
    }
  }
  std::vector<int> indices(n);
  for(int k = 0; k < n; ++k) indices[k] = std::count(orbits[k].begin(),orbits[k].end(),true);
  assert(group.indices() == indices);

  p = identity(n);
  do {
    assert(group.is_in(p) == expected.contains(p));
  }while(std::next_permutation(p.begin(),p.end()));
}

}  // namespace

int main(){
  std::mt19937 random(528);
  for(int n = 0; n <= 6; ++n){
    for(int trial = 0; trial < 24; ++trial){
      std::vector<Permutation> generators;
      soryuusi::permutation_subgroup group(n);
      for(int stage = 0; stage <= 4; ++stage){
        group.build();
        group.build();
        compare_with_enumeration(group,n,generators);
        auto p = identity(n);
        std::shuffle(p.begin(),p.end(),random);
        generators.push_back(p);
        group.add(p);
        group.add(p);
      }
      group.build();
      compare_with_enumeration(group,n,generators);
    }
  }

  constexpr int n = 50;
  auto cycle = identity(n), swap = cycle;
  std::rotate(cycle.begin(),cycle.begin()+1,cycle.end());
  std::swap(swap[0],swap[1]);
  soryuusi::permutation_subgroup symmetric(n,{cycle,swap});
  symmetric.build();
  auto indices = identity(n);
  for(int& factor : indices) ++factor;
  assert(symmetric.indices() == indices);

  auto odd = identity(n-1), three_cycle = odd;
  std::swap(odd[0],odd[1]);
  std::rotate(three_cycle.begin(),three_cycle.begin()+1,three_cycle.begin()+3);
  auto long_cycle = identity(n-1);
  std::rotate(long_cycle.begin(),long_cycle.begin()+1,long_cycle.end());
  soryuusi::permutation_subgroup alternating(n-1,{long_cycle,three_cycle});
  alternating.build();
  indices.pop_back();
  indices[1] = 1;
  assert(alternating.indices() == indices && alternating.is_in(long_cycle) && !alternating.is_in(odd));
  alternating.add(odd);
  alternating.build();
  indices[1] = 2;
  assert(alternating.indices() == indices && alternating.is_in(odd));

  auto rotation = identity(7), reflection = rotation;
  std::rotate(rotation.begin(),rotation.begin()+1,rotation.end());
  std::reverse(reflection.begin(),reflection.end());
  soryuusi::permutation_subgroup dihedral(7,{rotation,reflection});
  dihedral.build();
  compare_with_enumeration(dihedral,7,{rotation,reflection});
}
