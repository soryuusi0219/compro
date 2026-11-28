#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "library/data_structure/lazy_segtree.hpp"
#include "library/graph/dynamic_star_minimum_cut.hpp"
#include "library/graph/extreme_vertex_set.hpp"
#include "library/graph/global_minimum_cut.hpp"
#include "library/graph/gomory_hu_tree.hpp"
#include "library/graph/max_flow.hpp"
#include "library/tree/heavy_light_decomposition.hpp"

namespace {

using namespace soryuusi;
using Cap = long long;
using Edge = UndirectedCutEdge<Cap>;
using Clock = std::chrono::steady_clock;
using Time = std::chrono::duration<double,std::milli>;

struct Input {
  std::vector<Edge> edges;
  std::vector<Cap> weights;
  std::vector<std::pair<int,Cap>> queries;
};

Input read_case(const std::string& path) {
  std::ifstream in(path);
  if(!in) throw std::runtime_error("could not open "+path);
  int n,m,q;
  in >> n >> m >> q;
  Input input;
  input.weights.resize(n);
  input.edges.resize(m);
  input.queries.resize(q);
  for(auto& w : input.weights) in >> w;
  for(auto& [u,v,w] : input.edges) in >> u >> v >> w;
  for(auto& [v,w] : input.queries) in >> v >> w;
  if(!in) throw std::runtime_error("could not read "+path);
  return input;
}

Input synthetic(const std::string& name, int n) {
  std::mt19937 rng(51821);
  Input input;
  auto add = [&](int u, int v, Cap w) { input.edges.push_back({u,v,w}); };
  if(name == "clique" || name == "density10" || name == "density25" || name == "density50") {
    const int density = name == "clique" ? 100 : (name == "density50" ? 50 : (name == "density25" ? 25 : 10));
    for(int v = 0; v < n; ++v) for(int u = 0; u < v; ++u) if(u == v-1 || int(rng()%100) < density) add(u,v,1+rng()%1009);
  } else if(name == "lollipop") {
    const int k = std::min(96,n);
    for(int v = 0; v < k; ++v) for(int u = 0; u < v; ++u) add(u,v,1000+rng()%19);
    for(int v = k; v < n; ++v) add(v-1,v,30'000+v);
  } else if(name == "parallel") {
    for(int v = 0; v < n; ++v) {
      for(int r = 0; r < 12; ++r) add(v,(v+1)%n,1+(v%16));
      add(v,v,200'000);
      add(v,(v+2)%n,0);
    }
  } else if(name == "disconnected") {
    for(int v = 1; v < n; ++v) if(v%8) add(v-1,v,1);
    for(int v = 2; v < n; ++v) if(v%8 == 2) add(v-2,v,1);
  } else if(name == "chain" || name == "star") {
    for(int v = 1; v < n; ++v) add(name == "star" ? 0 : v-1,v,1+v%211);
  } else if(name != "empty") {
    throw std::runtime_error("unknown scenario "+name);
  }
  input.weights.resize(n);
  for(auto& w : input.weights) w = rng()%10007;
  for(int i = 0; i < 200000; ++i) input.queries.emplace_back(rng()%n,rng()%10007);
  return input;
}

struct HldStar {
  static Cap op(Cap a, Cap b) { return std::min(a,b); }
  static Cap inf() { return std::numeric_limits<Cap>::max(); }
  static Cap add(Cap a, Cap b) { return a+b; }
  static Cap zero() { return 0; }
  int root;
  HLdecomposition hld;
  LazySegtree<Cap,op,inf,Cap,add,add,zero> seg;
  std::vector<Cap> weights;

  HldStar(const Input& input, internal::ExtremeSetCandidates<Cap> tree)
    : root(static_cast<int>(tree.parent.size())-1), hld(root+1), weights(input.weights) {
    std::vector<Cap> sum(root+1), data(root+1);
    for(int v = 0; v < int(weights.size()); ++v) sum[v] = weights[v];
    for(int v = 0; v < root; ++v) {
      sum[tree.parent[v]] += sum[v];
      hld.add_edge(v,tree.parent[v]);
    }
    hld.build(root);
    for(int v = 0; v <= root; ++v) data[hld.inid(v)] = tree.cut[v]+sum[v];
    seg = decltype(seg)(data);
  }

  Cap set(int v, Cap w) {
    const Cap delta = w-weights[v];
    weights[v] = w;
    hld.for_each(root,v,[&](int l, int r) { seg.apply(l,r,delta); });
    return seg.all_prod();
  }
};

Cap gusfield(const Input& input, bool reuse) {
  const int n = static_cast<int>(input.weights.size());
  std::vector<int> parent(n,0);
  std::vector<Cap> cut(n);
  auto build = [&] {
    MaxFlow<Cap> flow(n);
    flow.reserve_edges(2*static_cast<int>(input.edges.size()));
    for(auto [u,v,w] : input.edges) if(u != v && w) {
      flow.add_edge(u,v,w);
      flow.add_edge(v,u,w);
    }
    return flow;
  };
  MaxFlow<Cap> reused = reuse ? build() : MaxFlow<Cap>(0);
  for(int s = 1; s < n; ++s) {
    MaxFlow<Cap> fresh = reuse ? MaxFlow<Cap>(0) : build();
    auto& flow = reuse ? reused : fresh;
    if(reuse && s > 1) flow.reset();
    const int t = parent[s];
    cut[s] = flow.flow(s,t);
    const auto side = flow.min_cut(s);
    for(int v = 1; v < n; ++v) if(v != s && parent[v] == t && side[v]) parent[v] = s;
    if(t != 0 && side[parent[t]]) {
      parent[s] = parent[t];
      parent[t] = s;
      std::swap(cut[s],cut[t]);
    }
  }
  Cap checksum = 0;
  for(Cap w : cut) checksum += w;
  return checksum;
}

}  // namespace

int main(int argc, char** argv) {
  if(argc < 4) throw std::runtime_error("usage: undirected_cuts MODE file INPUT | MODE SCENARIO N");
  const std::string mode = argv[1], scenario = argv[2];
  const auto input = scenario == "file" ? read_case(argv[3]) : synthetic(scenario,std::stoi(argv[3]));
  const int n = static_cast<int>(input.weights.size());
  Time build{}, update{};
  std::uint64_t checksum = 0;
  const auto start = Clock::now();
  auto measure_star = [&](auto& cut) {
    const auto ready = Clock::now();
    build = ready-start;
    for(auto [v,w] : input.queries) checksum = checksum*1000000007 + cut.set(v,w);
    update = Clock::now()-ready;
  };

  if(mode == "star-top") {
    DynamicStarMinimumCut<Cap> cut(input.weights,input.edges);
    measure_star(cut);
  } else if(mode == "star-hld") {
    HldStar cut(input,internal::extreme_set_candidates(n,input.edges));
    measure_star(cut);
  } else if(mode == "evs") {
    const auto candidate = internal::extreme_set_candidates(n,input.edges);
    const auto ready = Clock::now();
    build = ready-start;
    const auto tree = internal::filter_extreme_set_candidates(n,candidate);
    update = Clock::now()-ready;
    for(Cap w : tree.cut) checksum += w;
  } else if(mode == "candidates") {
    auto tree = internal::extreme_set_candidates(n,input.edges);
    build = Clock::now()-start;
    for(Cap w : tree.cut) checksum += w;
  } else if(mode == "gh-reuse" || mode == "gh-rebuild") {
    checksum = gusfield(input,mode == "gh-reuse");
    build = Clock::now()-start;
  } else if(mode == "gh-dinic" || mode == "gh-push") {
    const auto method = mode == "gh-dinic" ? GomoryHuFlow::Dinic : GomoryHuFlow::PushRelabel;
    GomoryHuTree<Cap> tree(n,input.edges,method);
    build = Clock::now()-start;
    for(int v = 1; v < n; ++v) checksum += tree.capacity(v);
  } else if(mode == "cut-dense" || mode == "cut-sparse" || mode == "cut-auto") {
    const auto method = mode == "cut-dense" ? GlobalMinCutMethod::Dense : (mode == "cut-sparse" ? GlobalMinCutMethod::Sparse : GlobalMinCutMethod::Auto);
    auto result = global_minimum_cut(n,input.edges,method);
    build = Clock::now()-start;
    if(result) {
      Cap restored = 0;
      for(auto [u,v,w] : input.edges) if(result->side[u] != result->side[v]) restored += w;
      if(restored != result->value) throw std::runtime_error("cut witness mismatch");
      checksum = result->value;
    }
  } else {
    throw std::runtime_error("unknown mode "+mode);
  }

  std::cout << std::fixed << std::setprecision(4) << build.count() << ' ' << update.count() << ' ' << checksum << '\n';
}
