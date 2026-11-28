#include <algorithm>
#include <chrono>
#include <cstdint>
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

#include "library/graph/assignment_problem.hpp"
#include "library/graph/bipartite_matching.hpp"
#include "library/graph/bipartite_weighted_matching.hpp"
#include "library/graph/dulmage_mendelsohn_decomposition.hpp"
#include "library/graph/max_flow.hpp"
#include "library/graph/min_cost_flow.hpp"
#include "reference_libraries/qandori/cp-library/graph/hungarian.hpp"

#ifdef LC_FASTEST_MATCHING
#define main submission_matching_main
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include LC_FASTEST_MATCHING
#pragma GCC diagnostic pop
#undef main
#undef SKIP_RUNTIME_CHECKS
#endif

#ifdef LC_FASTEST_ASSIGNMENT
#define main submission_assignment_main
#include LC_FASTEST_ASSIGNMENT
#undef main
#endif

namespace {

using Clock = std::chrono::steady_clock;
using Time = std::chrono::duration<double,std::milli>;
using Wide = __int128_t;

struct Stats {
  Time build{}, solve{}, restore{};
  std::uint64_t value = 0, checksum = 0;
};

struct GraphCase {
  int l,r;
  std::vector<std::pair<int,int>> edges;
  std::optional<int> expected;
};

struct AssignmentCase {
  int n,m;
  std::vector<int> costs;
  std::vector<unsigned char> allowed;
  std::optional<long long> expected;
  bool impossible = false;
};

std::vector<std::filesystem::path> paths(const std::filesystem::path& root) {
  std::vector<std::filesystem::path> files;
  for(const auto& item : std::filesystem::directory_iterator(root/"in")) if(item.path().extension() == ".in") files.push_back(item.path());
  std::sort(files.begin(),files.end());
  if(files.empty()) throw std::runtime_error("empty official case directory");
  return files;
}

template <class Read> auto read_cases(const std::filesystem::path& root, const Read& read) {
  using Case = decltype(read(std::declval<std::ifstream&>(),std::declval<std::ifstream&>()));
  std::vector<Case> result;
  for(auto path : paths(root)) {
    std::ifstream in(path), out(root/"out"/path.filename().replace_extension(".out"));
    result.push_back(read(in,out));
    if(!in || !out) throw std::runtime_error("could not read official case");
  }
  return result;
}

GraphCase make_graph(const std::string& name, unsigned seed) {
  std::mt19937 rng(seed);
  GraphCase result{};
  if(name == "chain") {
    result.l = result.r = 180000;
    for(int u = 0; u < result.l-1; ++u) {
      result.edges.emplace_back(u,u+1);
      result.edges.emplace_back(u,u);
    }
    result.edges.emplace_back(result.l-1,result.r-1);
    result.expected = result.l;
  } else if(name == "complete") {
    result.l = result.r = 720;
    for(int u = 0; u < result.l; ++u) for(int v = 0; v < result.r; ++v) result.edges.emplace_back(u,v);
    result.expected = result.l;
  } else if(name == "unbalanced") {
    result.l = 240000;
    result.r = 29;
    for(int e = 0; e < 350000; ++e) result.edges.emplace_back(rng()%result.l,rng()%result.r);
    result.expected = result.r;
  } else if(name == "deficient") {
    result.l = result.r = 100000;
    for(int e = 0; e < 230000; ++e) result.edges.emplace_back(rng()%result.l,rng()%50000);
    for(int v = 0; v < 50000; ++v) result.edges.emplace_back(v,v);
    result.expected = 50000;
  } else {
    throw std::runtime_error("unknown matching case");
  }
  return result;
}

AssignmentCase make_assignment(const std::string& name, unsigned seed) {
  std::mt19937 rng(seed);
  AssignmentCase result{};
  if(name == "ties" || name == "monge" || name == "dense") result.n = result.m = 500;
  else if(name == "sparse" || name == "hall") result.n = result.m = 700;
  else if(name == "rectangle") {
    result.n = 24;
    result.m = 32000;
  } else throw std::runtime_error("unknown assignment case");
  auto& [n,m,cost,allowed,expected,impossible] = result;
  cost.resize(std::size_t(n)*m);
  for(int u = 0; u < n; ++u) for(int v = 0; v < m; ++v) {
    cost[std::size_t(u)*m+v] = name == "ties" ? 0 : name == "monge" ? (u+10)*(v+14) : int(rng()%2000001)-1000000;
  }
  if(name == "sparse" || name == "hall") {
    allowed.assign(cost.size(),0);
    for(int u = 0; u < n; ++u) for(int i = 0; i < 5; ++i) allowed[std::size_t(u)*m+rng()%(name == "hall" ? m/2 : m)] = 1;
    if(name == "sparse") {
      for(int u = 0; u < n; ++u) allowed[std::size_t(u)*m+u] = 1;
    } else impossible = true;
  }
  return result;
}

void check_matching(const GraphCase& input, const std::vector<std::pair<int,int>>& chosen, int value) {
  if(int(chosen.size()) != value || (input.expected && value != *input.expected)) throw std::runtime_error("incorrect matching value");
  std::vector<bool> left(input.l), right(input.r);
  auto edges = input.edges;
  std::sort(edges.begin(),edges.end());
  for(auto [u,v] : chosen) {
    if(u < 0 || u >= input.l || v < 0 || v >= input.r || left[u] || right[v] || !std::binary_search(edges.begin(),edges.end(),std::pair{u,v})) {
      throw std::runtime_error("incorrect matching");
    }
    left[u] = right[v] = true;
  }
}

void measure_matching(const GraphCase& input, const std::string& method, Stats& stats, bool check) {
  const auto start = Clock::now();
  Clock::time_point built, solved;
  int value = 0;
  std::vector<std::pair<int,int>> chosen;
  if(method == "hk" || method == "dm") {
    soryuusi::BipartiteMatching graph(input.l,input.r);
    graph.reserve_edges(input.edges.size());
    for(auto [u,v] : input.edges) graph.add_edge(u,v);
    built = Clock::now();
    value = graph.solve();
    solved = Clock::now();
    if(method == "dm") {
      const auto dm = soryuusi::dulmage_mendelsohn_decomposition(graph);
      stats.checksum += dm.region.size()+std::count(dm.allowed.begin(),dm.allowed.end(),true);
    }
    chosen.reserve(value);
    for(int id : graph.matching_edges()) chosen.push_back(graph.get_edge(id));
  } else if(method == "max-flow") {
    const int s = input.l+input.r, t = s+1;
    soryuusi::MaxFlow<int> graph(t+1);
    graph.reserve_edges(input.edges.size()+s);
    for(auto [u,v] : input.edges) graph.add_edge(u,input.l+v,1);
    for(int u = 0; u < input.l; ++u) graph.add_edge(s,u,1);
    for(int v = 0; v < input.r; ++v) graph.add_edge(input.l+v,t,1);
    built = Clock::now();
    value = graph.flow(s,t);
    solved = Clock::now();
    chosen.reserve(value);
    for(int id = 0; id < int(input.edges.size()); ++id) if(graph.get_edge(id).flow) chosen.push_back(input.edges[id]);
#ifdef LC_FASTEST_MATCHING
  } else if(method == "fastest-matching") {
    sirkit::BipartiteMatching graph(input.l,input.r);
    for(auto [u,v] : input.edges) graph.add_edge(u,v);
    built = Clock::now();
    value = graph.solve();
    solved = Clock::now();
    chosen = graph.pairs();
#endif
  } else throw std::runtime_error("unknown matching method");
  const auto end = Clock::now();
  stats.build += built-start;
  stats.solve += solved-built;
  stats.restore += end-solved;
  stats.value += value;
  stats.checksum += chosen.size();
  if(check) check_matching(input,chosen,value);
}

void check_assignment(const AssignmentCase& input, const std::optional<Wide>& value, const std::vector<int>& cols) {
  if(bool(value) == input.impossible || (input.expected && value != Wide(*input.expected))) throw std::runtime_error("incorrect assignment value");
  if(!value) return;
  if(cols.size() != std::size_t(input.n)) throw std::runtime_error("incorrect assignment count");
  std::vector<bool> used(input.m);
  Wide got = 0;
  for(int u = 0; u < input.n; ++u) {
    const int v = cols[u];
    const std::size_t i = std::size_t(u)*input.m+v;
    if(v < 0 || v >= input.m || used[v] || (!input.allowed.empty() && !input.allowed[i])) throw std::runtime_error("incorrect assignment");
    used[v] = true;
    got += input.costs[i];
  }
  if(got != value) throw std::runtime_error("incorrect assignment cost");
}

void measure_assignment(const AssignmentCase& input, const std::string& method, Stats& stats, bool check) {
  const int n = input.n, m = input.m;
  const auto start = Clock::now();
  Clock::time_point built = start, solved;
  std::optional<Wide> value;
  std::vector<int> cols;
  if(method == "hungarian") {
    const auto result = soryuusi::assignment_problem<int>(n,m,input.costs,input.allowed);
    solved = Clock::now();
    if(result) {
      value = result->cost;
      cols = result->left_to_right;
    }
  } else if(method == "ref-hungarian") {
    if(!input.allowed.empty()) throw std::runtime_error("reference needs a complete graph");
    const ::Hungarian<long long> result(n,m,input.costs.data());
    solved = Clock::now();
    value = result.cost;
    cols = result.row_to_col;
#ifdef LC_FASTEST_ASSIGNMENT
  } else if(method == "fastest-assignment") {
    if(!input.allowed.empty() || n != m) throw std::runtime_error("submission needs a square complete graph");
    auto result = ::hungarian(n,input.costs);
    solved = Clock::now();
    value = result.first;
    cols = std::move(result.second);
#endif
  } else if(method == "min-cost-flow") {
    const int s = n+m, t = s+1;
    soryuusi::MinCostFlow<int,int,long long> graph(t+1);
    for(int u = 0; u < n; ++u) for(int v = 0; v < m; ++v) {
      const auto idx = std::size_t(u)*m+v;
      if(input.allowed.empty() || input.allowed[idx]) graph.add_edge(u,n+v,1,input.costs[idx]);
    }
    int edges = 0;
    for(int u = 0; u < n; ++u) {
      const int id = graph.add_edge(s,u,1,0);
      if(u == 0) edges = id;
    }
    for(int v = 0; v < m; ++v) graph.add_edge(n+v,t,1,0);
    built = Clock::now();
    const auto [sent,cost] = graph.flow(s,t,n);
    solved = Clock::now();
    if(sent == n) {
      value = cost;
      cols.assign(n,-1);
      for(int i = 0; i < edges; ++i) {
        const auto e = graph.get_edge(i);
        if(e.flow) cols[e.from] = e.to-n;
      }
    }
  } else throw std::runtime_error("unknown assignment method");
  const auto end = Clock::now();
  stats.build += built-start;
  stats.solve += solved-built;
  stats.restore += end-solved;
  stats.value += value ? std::uint64_t(*value) : 0;
  stats.checksum += value.has_value();
  if(check) check_assignment(input,value,cols);
}

void measure_weighted(const AssignmentCase& input, const std::string& method, Stats& stats) {
  const int n = input.n, m = input.m;
  const auto start = Clock::now();
  Clock::time_point built, solved;
  Wide objective = 0;
  if(method == "weighted") {
    soryuusi::BipartiteWeightedMatching<int> g(n,m);
    for(int u = 0; u < n; ++u) for(int v = 0; v < m; ++v) {
      const auto i = std::size_t(u)*m+v;
      if(input.allowed.empty() || input.allowed[i]) g.add_edge(u,v,input.costs[i]);
    }
    built = Clock::now();
    const auto result = g.solve();
    solved = Clock::now();
    objective = result->weight;
    stats.checksum += result->flow.size();
  } else {
    const int s = n+m, t = s+1;
    soryuusi::MinCostFlow<int,int,long long> g(t+1);
    for(int u = 0; u < n; ++u) for(int v = 0; v < m; ++v) {
      const auto i = std::size_t(u)*m+v;
      if(input.allowed.empty() || input.allowed[i]) {
        g.add_edge(u,n+v,1,-input.costs[i]);
        ++stats.checksum;
      }
    }
    for(int u = 0; u < n; ++u) g.add_edge(s,u,1,0);
    for(int v = 0; v < m; ++v) g.add_edge(n+v,t,1,0);
    g.add_edge(s,t,std::min(n,m),0);
    built = Clock::now();
    objective = -g.flow(s,t,std::min(n,m)).second;
    solved = Clock::now();
  }
  stats.build += built-start;
  stats.solve += solved-built;
  stats.value += std::uint64_t(objective);
}

} // namespace

int main(int argc, char** argv) {
  if(argc != 5) {
    std::cerr << "usage: matching-bench METHOD CASE REPEATS SEED\n"
              << "matching: hk, max-flow, dm, fastest-matching; case directory or chain, complete, unbalanced, deficient\n"
              << "assignment: hungarian, ref-hungarian, fastest-assignment, min-cost-flow; case directory or dense, ties, monge, sparse, rectangle, hall\n"
              << "weighted: weighted, weighted-flow; dense, sparse\n";
    return 2;
  }
  const std::string method = argv[1], name = argv[2];
  const int repeats = std::stoi(argv[3]);
  const unsigned seed = std::stoul(argv[4]);
  if(repeats < 1) return 2;
  Stats time;
  if(method == "hk" || method == "max-flow" || method == "fastest-matching" || method == "dm") {
    const auto in = std::filesystem::is_directory(name) ?
        read_cases(name,[](auto& in, auto& out) {
          GraphCase g{};
          int m,k;
          in >> g.l >> g.r >> m;
          g.edges.resize(m);
          for(auto& [u,v] : g.edges) in >> u >> v;
          out >> k;
          g.expected = k;
          return g;
        }) : std::vector{make_graph(name,seed)};
    for(const auto& g : in) for(int i = 0; i < repeats; ++i) measure_matching(g,method,time,i == 0);
  } else {
    const auto in = std::filesystem::is_directory(name) ?
        read_cases(name,[](auto& in, auto& out) {
          AssignmentCase c{};
          in >> c.n;
          c.m = c.n;
          c.costs.resize(std::size_t(c.n)*c.m);
          for(int& x : c.costs) in >> x;
          long long cost;
          out >> cost;
          c.expected = cost;
          return c;
        }) : std::vector{make_assignment(name,seed)};
    for(const auto& c : in) for(int i = 0; i < repeats; ++i) {
      if(method == "weighted" || method == "weighted-flow") measure_weighted(c,method,time);
      else measure_assignment(c,method,time,i == 0);
    }
  }
  std::cout << std::fixed << std::setprecision(6) << time.build.count()/repeats << ' ' << time.solve.count()/repeats << ' '
            << time.restore.count()/repeats << ' ' << time.value << ' ' << time.checksum << '\n';
}
