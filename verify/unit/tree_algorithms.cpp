#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "library/tree/heavy_light_decomposition_balanced.hpp"
#include "library/tree/permutation_tree.hpp"
#include "library/tree/rerooting.hpp"
#include "library/tree/simple_euler_tour.hpp"
#include "library/tree/tree_distance_frequency.hpp"
#include "library/tree/tree_isom.hpp"

namespace {

std::string concat(std::string a, std::string b){return a+b;}
std::string empty_string(){return {};}
std::string label(char f, std::string value){return f+("["+value+"]");}
long long sum(long long a, long long b){return a+b;}
long long zero(){return 0;}
long long identity(int, long long x){return x;}

void test_rmq(){
  std::mt19937 gen(481);
  for(int n : {0,1,31,32,33,63,64,65,100,259}){
    std::vector<int> a(n);
    for(auto& x : a) x = int(gen()%17)-8;
    const soryuusi::PermutationTree::StaticRMQ rmq(a);
    for(int l = 0; l <= n; ++l){
      int minimum = std::numeric_limits<int>::max();
      assert(rmq.prod(l,l) == minimum);
      for(int r = l; r < n; ++r){
        minimum = std::min(minimum,a[r]);
        assert(rmq.prod(l,r+1) == minimum);
      }
    }
  }
}

void test_noncommutative(){
  soryuusi::Rerooting<std::string,concat,empty_string,char,label> dp(4,{"A","B","C","D"});
  dp.add_edge(0,1,'a','b');
  dp.add_edge(0,2,'c','d');
  dp.add_edge(1,3,'e','f');
  dp.build();
  const std::vector<std::string> expected{"b[Bf[D]]d[C]A","f[D]a[Ad[C]]B","c[Ab[Bf[D]]]C","e[a[Ad[C]]B]D"};
  assert(dp.answers() == expected);
  assert(dp.subanswers(0) == std::vector<std::string>({"d[C]","b[Bf[D]]"}));
  assert(dp.subanswer(5) == "e[a[Ad[C]]B]");
  dp.build();
  assert(dp.answers() == expected);

  using HLD = soryuusi::HLdecomposition_Balanced<std::string,concat,empty_string>;
  std::mt19937 gen(379);
  for(int n = 1; n <= 70; ++n){
    HLD hld(n);
    std::vector<std::vector<int>> adjacent(n);
    std::vector<std::string> value(n);
    for(int v = 0; v < n; ++v){
      value[v] = char('A'+v%26);
      if(v){
        int p = gen()%v;
        hld.add_edge(p,v);
        adjacent[v].push_back(p);
        adjacent[p].push_back(v);
      }
    }
    hld.build(gen()%n,value);
    for(int i = 0; i < 100; ++i){
      const int u = gen()%n, v = gen()%n;
      if(i%3 == 0){
        value[u] = char('a'+i%26);
        hld.set(u,value[u]);
      }
      std::string path;
      auto find_path = [&](auto&& self, int w, int p) -> bool {
        if(w == v){path = value[w]; return true;}
        for(int to : adjacent[w]) if(to != p && self(self,to,w)){
          path = value[w]+path;
          return true;
        }
        return false;
      };
      assert(find_path(find_path,u,-1));
      assert(hld.prod(u,v) == path);
    }
  }
}

void test_deep_tree(){
  const int n = 200000;
  soryuusi::SimpleEularTour tour(n);
  soryuusi::Rerooting<long long,sum,zero,int,identity> dp(n,std::vector<long long>(n,1));
  std::vector<std::vector<long long>> graph(n);
  for(int i = 1; i < n; ++i){
    tour.add_edge(i-1,i);
    dp.add_edge(i-1,i,0);
    graph[i-1].push_back(i);
    graph[i].push_back(i-1);
  }
  tour.build(n-1);
  assert(tour.lca(0,n/2) == n/2 && tour.dist(0,n-1) == n-1);
  dp.build();
  assert(dp.answer(0) == n && dp.answer(n-1) == n);
  const auto ids = soryuusi::tree_hash::get_rooted_hashs(graph);
  assert(ids[0] != ids[n-1] && ids[n-1] == 0);
  assert(soryuusi::tree_hash::tree_get_centroids(graph) == std::make_pair(n/2-1LL,n/2+0LL));
}

void test_distance_crt(){
  const int path = 71, leaves = 33000;
  soryuusi::TreeDistanceFrequency tree(path+2*leaves);
  for(int i = 1; i < path; ++i) tree.add_edge(i-1,i);
  for(int i = 0; i < leaves; ++i){
    tree.add_edge(0,path+i);
    tree.add_edge(path-1,path+leaves+i);
  }
  std::vector<long long> expected(path+2*leaves);
  for(int d = 1; d < path; ++d) expected[d] += path-d;
  for(int d = 1; d <= path; ++d) expected[d] += 2*leaves;
  expected[2] += 1LL*leaves*(leaves-1);
  expected[path+1] += 1LL*leaves*leaves;
  assert(tree.solve() == expected);
  assert(tree.solve() == expected);
}

}  // namespace

int main(){
  test_rmq();
  test_noncommutative();
  test_deep_tree();
  test_distance_crt();
}
