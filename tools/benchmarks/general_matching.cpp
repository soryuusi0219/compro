#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "library/graph/general_matching.hpp"
#include "library/graph/general_weighted_matching.hpp"

#ifdef LC_GENERAL_MATCHING_REFERENCE
#define main lc_general_matching_main
#include LC_GENERAL_MATCHING_REFERENCE
#undef main
#endif

#ifdef LC_WEIGHTED_MATCHING_REFERENCE
#define main lc_weighted_matching_main
#include LC_WEIGHTED_MATCHING_REFERENCE
#undef main
#endif

namespace {

using Clock = std::chrono::steady_clock;
using Time = std::chrono::duration<double,std::milli>;
using Edge = soryuusi::GeneralWeightedMatching<long long>::Edge;

struct Input {
  int n;
  std::vector<Edge> edges;
};

Input read(const std::string& path, bool weighted) {
  std::ifstream in(path);
  int n, m;
  if(!(in >> n >> m)) throw std::runtime_error("could not read "+path);
  Input input{n,std::vector<Edge>(m)};
  for(auto& [u,v,w] : input.edges) {
    in >> u >> v;
    w = 1;
    if(weighted) in >> w;
  }
  if(!in) throw std::runtime_error("invalid input "+path);
  return input;
}

Input generate(const std::string& name, int n) {
  if(n <= 0) throw std::runtime_error("invalid size");
  Input input{n,{}};
  std::mt19937 rng(618347);
  auto add = [&](int u, int v, long long w) { input.edges.push_back({u,v,w}); };

  if(name == "path") {
    for(int v = 2; v < n; v += 2) add(v-1,v,10'000-v%1000);
    for(int v = 1; v < n; v += 2) add(v-1,v,1+v%97);
  } else if(name == "barrier") {
    for(int v = 2; v+2 < n; v += 3) {
      add(v,v+1,100000+v);
      add(v+1,v+2,60000+v);
      add(v+2,v,20000+v);
      add(0,v,50);
      add(1,v+1,70);
    }
  } else if(name == "triangles" || name == "signed") {
    const bool use_signed = name == "signed";
    for(int v = 1; v+1 < n; v += 2) {
      add(v-1,v,90'000+v);
      add(v,v+1,15'000+v);
      add(v+1,v-1,70'000+v);
      if(use_signed) add(v-1,v,-90'000-v);
    }
    if(n%2 == 0) add(n-1,0,use_signed ? 0 : 1);
  } else if(name == "clique" || name == "tied") {
    for(int u = 0; u < n; ++u) for(int v = u+1; v < n; ++v) {
      add(u,v,name == "tied" ? 17 : 1+rng()%1000);
    }
    std::shuffle(input.edges.begin(),input.edges.end(),rng);
  } else if(name == "components") {
    for(int x = 0; x+12 <= n; x += 12) for(int u = x; u < x+12; ++u) for(int v = u+1; v < x+12; ++v) {
      add(u,v,1+rng()%1000);
    }
  } else {
    throw std::runtime_error("unknown scenario "+name);
  }
  return input;
}

struct Run {
  Time build{}, solve{}, restore{};
  int size = 0;
  std::uint64_t value = 0;
};

Run solve(const Input& input, const std::string& mode) {
  const auto start = Clock::now();
  Run result;
  if(mode == "plain") {
    soryuusi::GeneralMatching graph(input.n);
    graph.reserve_edges(input.edges.size());
    for(auto [u,v,w] : input.edges) graph.add_edge(u,v);
    const auto ready = Clock::now();
    result.size = graph.solve();
    const auto solved = Clock::now();
    const auto edges = graph.matching_edges();
    if(edges.size() != std::size_t(result.size)) throw std::runtime_error("cardinality mismatch");
    result.value = edges.size();
    result.build = ready-start;
    result.solve = solved-ready;
    result.restore = Clock::now()-solved;
  } else if(mode == "weighted" || mode == "cardinality") {
    soryuusi::GeneralWeightedMatching<long long> graph(input.n);
    graph.reserve_edges(input.edges.size());
    for(auto [u,v,w] : input.edges) graph.add_edge(u,v,w);
    const auto ready = Clock::now();
    const auto goal = mode == "weighted" ? soryuusi::WeightedMatchingObjective::Weight : soryuusi::WeightedMatchingObjective::CardinalityThenWeight;
    const auto answer = graph.solve(goal);
    const auto solved = Clock::now();
    const auto edges = graph.matching_edges();
    result.size = answer->cardinality;
    result.value = static_cast<std::uint64_t>(answer->weight);
    if(edges.size() != std::size_t(result.size)) throw std::runtime_error("cardinality mismatch");
    result.build = ready-start;
    result.solve = solved-ready;
    result.restore = Clock::now()-solved;
#ifdef LC_GENERAL_MATCHING_REFERENCE
  } else if(mode == "plain-gabow") {
    struct To { int to; };
    std::vector<std::vector<To>> graph(input.n);
    for(auto [u,v,w] : input.edges) if(u != v) {
      graph[u].push_back({v});
      graph[v].push_back({u});
    }
    const auto ready = Clock::now();
    const MaxMatching<To> answer(graph);
    const auto solved = Clock::now();
    for(int u = 0; u < input.n; ++u) result.size += answer.mt[u] > u;
    result.value = result.size;
    result.build = ready-start;
    result.solve = solved-ready;
    result.restore = Clock::now()-solved;
#endif
#ifdef LC_WEIGHTED_MATCHING_REFERENCE
  } else if(mode == "weighted-gabow") {
    using Solver = MaximumWeightedMatching<long long,long long>;
    std::vector<Solver::InputEdge> edges;
    edges.reserve(input.edges.size());
    for(auto [u,v,w] : input.edges) if(u != v && w > 0) edges.push_back({u+1,v+1,w});
    Solver graph(input.n,edges);
    const auto ready = Clock::now();
    std::vector<std::pair<int,int>> matching;
    result.value = graph.maximum_weighted_matching(matching);
    result.solve = Clock::now()-ready;
    result.build = ready-start;
    result.size = static_cast<int>(matching.size());
#endif
  } else {
    throw std::runtime_error("unknown mode "+mode);
  }
  return result;
}

}  // namespace

int main(int argc, char** argv) {
  if(argc != 4) throw std::runtime_error("usage: general_matching MODE file INPUT | MODE SCENARIO N");
  const std::string mode = argv[1], scenario = argv[2];
  const Input input = scenario == "file" ? read(argv[3],mode.find("plain") != 0) : generate(scenario,std::stoi(argv[3]));
  const auto result = solve(input,mode);
  std::cout << std::fixed << std::setprecision(5) << result.build.count() << ' ' << result.solve.count() << ' ' << result.restore.count()
            << ' ' << result.size << ' ' << result.value << ' ' << input.n << ' ' << input.edges.size() << '\n';
}
