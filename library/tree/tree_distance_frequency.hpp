// https://github.com/soryuusi0219/compro/blob/main/library/tree/tree_distance_frequency.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <vector>

#include "../math/convolution.hpp"
#include "../math/internal_math.hpp"
#include "../math/modint.hpp"
#include "centroid_decomposition.hpp"

namespace soryuusi {

class TreeDistanceFrequency {
  static constexpr long long mod1 = 998244353, mod2 = 469762049;
  static constexpr long long inv = internal::inv_gcd(mod1,mod2).second;
  using Mint1 = static_modint<mod1>;
  using Mint2 = static_modint<mod2>;
  CentroidDecomposition tree;

public:
  explicit TreeDistanceFrequency(int n) : tree(n){assert(n > 0);}
  void add_edge(int u, int v){tree.addEdge(u,v);}
  std::vector<long long> solve(){
    const int n = tree.n;
    if(tree.dfs_order.empty()) tree.build();
    std::vector<long long> result(n,0);
    std::vector<Mint1> sum1(n,0);
    std::vector<Mint2> sum2(n,0);
    std::vector<bool> seen(n,false);
    std::vector<int> whole, child;
    struct Visit {int vertex, parent, depth;};
    std::vector<Visit> stack;
    auto visit_depths = [&](int source, int parent, int depth, auto&& visit){
      stack.push_back({source,parent,depth});
      while(!stack.empty()){
        auto [v,p,d] = stack.back();
        stack.pop_back();
        visit(d);
        for(int to : tree.G.vertices(v)) if(to != p && !seen[to]) stack.push_back({to,v,d+1});
      }
    };
    auto add_square = [&](const std::vector<int>& histogram, int offset, int sign){
      const int size = histogram.size();
      if(size <= 32){
        for(int i = 0; i < size; ++i) for(int j = 0; j < size && i+j+offset < n; ++j){
          result[i+j+offset] += (long long)sign * histogram[i] * histogram[j];
        }
        return;
      }
      auto a = convolution_square<mod1>(histogram);
      auto b = convolution_square<mod2>(histogram);
      const int end = std::min<int>(a.size(),n-offset);
      for(int i = 0; i < end; ++i){
        sum1[i+offset] += sign*a[i];
        sum2[i+offset] += sign*b[i];
      }
    };
    for(int k = 0; k < n; ++k){
      int v = tree.dfs_order[k], size = tree.sz[v];
      if(size < 30){
        for(int j = 0; j < size; ++j) visit_depths(tree.dfs_order[k+j],-1,0,[&](int d){++result[d];});
        k += size-1;
        continue;
      }
      seen[v] = true;
      whole.assign(1,1);
      for(int to : tree.G.vertices(v)) if(!seen[to]){
        child.clear();
        visit_depths(to,v,0,[&](int d){
          if(d >= (int)child.size()) child.resize(d+1);
          ++child[d];
        });
        if(whole.size() <= child.size()) whole.resize(child.size()+1);
        for(int i = 0; i < (int)child.size(); ++i) whole[i+1] += child[i];
        add_square(child,2,-1);
      }
      add_square(whole,0,1);
    }
    for(int d = 1; d < n; ++d){
      long long a = sum1[d].val();
      long long t = (Mint2(sum2[d]-a)*inv).val();
      result[d] = (result[d] + a + mod1*t)/2;
    }
    result[0] = 0;
    return result;
  }
};

}  // namespace soryuusi
