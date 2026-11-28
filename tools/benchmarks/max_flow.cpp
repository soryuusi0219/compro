#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <atcoder/maxflow>

#include "library/graph/bounded_flow.hpp"
#include "library/graph/max_flow.hpp"

// Comparison headers are read from the local reference clones.
#include "reference_libraries/hitonanode/cplib-cpp/flow/maxflow_pushrelabel.hpp"
#include "reference_libraries/qandori/cp-library/flow/max_flow_lower_bound.hpp"

using ll = long long;
using vi = std::vector<ll>;
using std::max;
using std::min;
using std::vector;

#define pb push_back
#define SZ(a) int((a).size())
#define fore(i,a,b) for(int i = (a); i < (b); ++i)
#include "reference_libraries/IvanRenison/kactlpp/content/graph/Dinic.h"
#undef fore
#undef SZ
#undef pb

namespace {

struct InputEdge {
  int u, v;
  ll cap;
};

struct Input {
  int n;
  std::vector<InputEdge> edges;
  std::optional<ll> expected;
};

Input generate(const std::string& name, int scale, unsigned seed) {
  std::mt19937_64 rng(seed);
  Input input{};
  auto& e = input.edges;
  if(name == "chain") {
    input.n = 200000/scale;
    for(int v = 1; v < input.n; ++v) e.push_back({v-1,v,1LL<<50});
  } else if(name == "ladder") {
    // Every phase finds one unit exit, one step farther away than the previous one.
    input.n = 10000/scale;
    const int t = input.n-1;
    for(int v = 0; v < t; ++v) {
      if(v+1 != t) e.push_back({v,v+1,t});
      e.push_back({v,t,1});
    }
  } else if(name == "layered") {
    const int width = 2500/scale, layers = 12, degree = 7;
    input.n = layers*width+2;
    for(int x = 0; x < width; ++x) {
      e.push_back({0,x+1,1});
      e.push_back({(layers-1)*width+x+1,input.n-1,1});
    }
    for(int d = 0; d+1 < layers; ++d) {
      for(int x = 0; x < width; ++x) {
        for(int i = 0; i < degree; ++i) e.push_back({1+d*width+x,1+(d+1)*width+int(rng()%width),1});
      }
    }
  } else if(name == "dense" || name == "trap") {
    input.n = 850/scale;
    const int t = input.n-1;
    for(int u = 1; u < t; ++u) {
      for(int v = 1; v < t; ++v) if(u != v) e.push_back({u,v,1+ll(rng()%1000)});
      e.push_back({0,u,1LL<<35});
      if(name == "dense") e.push_back({u,t,1+ll(rng()%2000)});
    }
    if(name == "trap") {
      // A large preflow enters the dense core but only one unit reaches the sink.
      e.push_back({t-1,t,1});
    }
  } else if(name == "sparse" || name == "dead") {
    input.n = 100000/scale;
    const int t = input.n-1;
    for(int i = 0; i < 7*input.n; ++i) {
      const int u = rng()%input.n, v = rng()%input.n;
      if(u != v && (name != "dead" || v != t)) e.push_back({u,v,1+ll(rng()%(1ULL<<42))});
    }
  } else {
    throw std::runtime_error("unknown case");
  }
  return input;
}

std::vector<std::filesystem::path> input_paths(const std::string& directory) {
  std::vector<std::filesystem::path> paths;
  for(const auto& entry : std::filesystem::directory_iterator(directory)) {
    if(entry.path().extension() == ".in") paths.push_back(entry.path());
  }
  std::sort(paths.begin(),paths.end());
  if(paths.empty()) throw std::runtime_error("no .in cases");
  return paths;
}

std::vector<Input> read_cases(const std::string& directory) {
  std::vector<Input> inputs;
  for(auto path : input_paths(directory)) {
    std::ifstream in(path);
    int n,m;
    if(!(in >> n >> m) || n < 2 || m < 0) throw std::runtime_error("invalid case header");
    Input input{n,std::vector<InputEdge>(m),std::nullopt};
    for(auto& [u,v,c] : input.edges) if(!(in >> u >> v >> c)) throw std::runtime_error("invalid case edge");
    std::ifstream out(path.replace_extension(".out"));
    ll expected;
    if(!(out >> expected)) throw std::runtime_error("missing expected output");
    input.expected = expected;
    inputs.push_back(std::move(input));
  }
  return inputs;
}

struct PushRelabel : soryuusi::MaxFlow<ll> {
  using soryuusi::MaxFlow<ll>::MaxFlow;
  ll flow(int s, int t) { return flow_push_relabel(s,t); }
};

struct KactlScaling : Dinic {
  using Dinic::Dinic;
  void add_edge(int u, int v, ll cap) { if(u != v) addEdge(u,v,cap); }
  ll flow(int s, int t) { return calc(s,t); }
};

using Clock = std::chrono::steady_clock;
using Time = std::chrono::duration<double,std::milli>;

template <class Graph> void measure(const std::vector<Input>& inputs, int repeats, bool reuse) {
  Time build{}, solve{}, restore{}, reset{};
  unsigned long long flow_sum = 0, restored = 0;
  for(const auto& [n,edges,expected] : inputs) {
    auto make_graph = [&]() {
      const auto start = Clock::now();
      Graph graph(n);
      if constexpr(requires { graph.reserve_edges(0); }) graph.reserve_edges(edges.size());
      else if constexpr(requires { graph.reserve(0); }) graph.reserve(edges.size());
      for(const auto& [u,v,c] : edges) graph.add_edge(u,v,c);
      build += Clock::now()-start;
      return graph;
    };
    auto run = [&](Graph& graph) {
      const auto start = Clock::now();
      const ll flow = graph.flow(0,n-1);
      const auto solved = Clock::now();
      if constexpr(requires { graph.edges(); }) {
        for(const auto& e : graph.edges()) restored += e.flow;
      } else {
        for(auto& row : graph.adj) for(auto& arc : row) restored += arc.flow();
      }
      const auto end = Clock::now();
      solve += solved-start;
      restore += end-solved;
      if(expected && flow != *expected) throw std::runtime_error("incorrect maximum flow");
      flow_sum += flow;
    };

    if(reuse) {
      auto graph = make_graph();
      for(int rep = 0; rep < repeats; ++rep) {
        run(graph);
        const auto start = Clock::now();
        if constexpr(requires { graph.reset(); }) graph.reset();
        else if constexpr(requires { graph.reset_flow(); }) graph.reset_flow();
        else if constexpr(requires { graph.change_edge(0,ll(0),ll(0)); }) {
          for(int i = 0; i < int(edges.size()); ++i) graph.change_edge(i,graph.get_edge(i).cap,0);
        } else {
          throw std::runtime_error("reset benchmark supports dinic, push, acl and qandori");
        }
        reset += Clock::now()-start;
      }
    } else {
      for(int rep = 0; rep < repeats; ++rep) {
        auto graph = make_graph();
        run(graph);
      }
    }
  }
  std::cout << std::fixed << std::setprecision(6) << build.count()/(reuse ? 1 : repeats) << ' '
            << solve.count()/repeats << ' ' << restore.count()/repeats << ' ' << reset.count()/repeats << ' '
            << flow_sum << ' ' << restored << '\n';
}

struct BoundedInput {
  struct Edge { int u,v; ll lo,hi; };
  std::vector<ll> supply;
  std::vector<Edge> edges;
  bool feasible;
};

std::vector<BoundedInput> read_bounded_cases(const std::string& directory) {
  std::vector<BoundedInput> inputs;
  for(auto path : input_paths(directory+"/in")) {
    std::ifstream in(path);
    int n,m;
    if(!(in >> n >> m) || n < 0 || m < 0) throw std::runtime_error("invalid case header");
    BoundedInput input{std::vector<ll>(n),std::vector<BoundedInput::Edge>(m),false};
    for(ll& b : input.supply) if(!(in >> b)) throw std::runtime_error("invalid case supply");
    for(auto& [u,v,lo,hi] : input.edges) {
      ll cost;
      if(!(in >> u >> v >> lo >> hi >> cost)) throw std::runtime_error("invalid case edge");
    }
    std::ifstream out(std::filesystem::path(directory)/"out"/path.filename().replace_extension(".out"));
    std::string answer;
    if(!(out >> answer)) throw std::runtime_error("missing expected output");
    input.feasible = answer != "infeasible";
    inputs.push_back(std::move(input));
  }
  return inputs;
}

template <class Graph> void measure_bounded(const std::vector<BoundedInput>& inputs, int repeats) {
  Time build{}, solve{};
  int feasible_count = 0;
  for(const auto& [supply,edges,feasible] : inputs) {
    for(int rep = 0; rep < repeats; ++rep) {
      const auto start = Clock::now();
      Graph graph(supply.size());
      if constexpr(requires { graph.reserve_edges(0); }) graph.reserve_edges(edges.size());
      else graph.reserve(edges.size());
      if constexpr(requires { graph.set_supply(0,ll(0)); }) {
        for(int v = 0; v < int(supply.size()); ++v) graph.set_supply(v,supply[v]);
      }
      for(const auto& [u,v,lo,hi] : edges) graph.add_edge(u,v,lo,hi);
      const auto built = Clock::now();
      auto result = [&]() -> std::optional<std::vector<ll>> {
        if constexpr(requires { graph.solve(supply); }) return graph.solve(supply);
        else {
          auto answer = graph.solve();
          if(!answer) return std::nullopt;
          return std::move(answer->flow);
        }
      }();
      const auto end = Clock::now();
      build += built-start;
      solve += end-built;
      if(bool(result) != feasible) throw std::runtime_error("incorrect feasibility");
      if(result && rep == 0) {
        ++feasible_count;
        if(result->size() != edges.size()) throw std::runtime_error("incorrect edge count");
        auto balance = supply;
        for(int i = 0; i < int(edges.size()); ++i) {
          const auto& [u,v,lo,hi] = edges[i];
          const ll f = (*result)[i];
          if(f < lo || f > hi) throw std::runtime_error("flow exceeds bounds");
          balance[u] -= f;
          balance[v] += f;
        }
        if(std::any_of(balance.begin(),balance.end(),[](ll b) { return b != 0; })) throw std::runtime_error("incorrect balance");
      }
    }
  }
  std::cout << std::fixed << std::setprecision(6) << build.count()/repeats << ' ' << solve.count()/repeats << " 0 0 "
            << feasible_count << ' ' << inputs.size() << '\n';
}

} // namespace

int main(int argc, char** argv) {
  if(argc != 6 && argc != 7) {
    std::cerr << "usage: max-flow-bench ALGORITHM CASE REPEATS SCALE SEED [reset]\n"
              << "algorithms: dinic, push, acl, scaling, hlpp, qandori, bounded, bounded-qandori\n"
              << "bounded algorithms take a generated Library Checker min_cost_b_flow directory\n"
              << "cases: chain, ladder, layered, dense, trap, sparse, dead, or a directory of AOJ inputs\n";
    return 2;
  }
  const std::string algorithm = argv[1], name = argv[2];
  const int repeats = std::stoi(argv[3]), scale = std::stoi(argv[4]);
  if(repeats < 1 || scale < 1 || scale > 100) return 2;
  const bool reuse = argc == 7;
  if(reuse && std::string(argv[6]) != "reset") return 2;
  if(algorithm == "bounded" || algorithm == "bounded-qandori") {
    if(reuse) return 2;
    const auto inputs = read_bounded_cases(name);
    if(algorithm == "bounded") measure_bounded<soryuusi::BoundedFlow<ll>>(inputs,repeats);
    else measure_bounded<MaxFlowLowerBound<ll>>(inputs,repeats);
    return 0;
  }
  const auto inputs = std::filesystem::is_directory(name) ? read_cases(name) : std::vector{generate(name,scale,std::stoul(argv[5]))};
  if(algorithm == "dinic") measure<soryuusi::MaxFlow<ll>>(inputs,repeats,reuse);
  else if(algorithm == "push") measure<PushRelabel>(inputs,repeats,reuse);
  else if(algorithm == "acl") measure<atcoder::mf_graph<ll>>(inputs,repeats,reuse);
  else if(algorithm == "scaling") measure<KactlScaling>(inputs,repeats,reuse);
  else if(algorithm == "hlpp") measure<mf_pushrelabel<ll>>(inputs,repeats,reuse);
  else if(algorithm == "qandori") measure<::MaxFlow<ll>>(inputs,repeats,reuse);
  else return 2;
}
