#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "library/graph/max_flow.hpp"
#include "library/graph/minimum_cut_lattice.hpp"
#include "library/graph/multi_label_project_selection.hpp"
#include "library/graph/project_selection.hpp"
#include "library/graph/scc.hpp"
#include "reference_libraries/qandori/cp-library/flow/project_selection.hpp"
#include "reference_libraries/suisen-cp/cp-library-cpp/library/optimization/k_project_selection_problem.hpp"

namespace {

using ll = long long;
using Wide = __int128_t;
using Clock = std::chrono::steady_clock;
using Time = std::chrono::duration<double,std::milli>;

struct TimeParts { Time build{}, solve{}, restore{}; ll checksum = 0; };
struct Arc { int from,to; ll cost; };
struct BinaryCase {
  std::vector<ll> c0,c1;
  std::vector<Arc> arcs;
  std::vector<std::pair<int,int>> hard;
  std::vector<std::pair<int,bool>> fixed;
  std::optional<ll> expected;
  bool infeasible = false;
};

std::vector<std::filesystem::path> input_paths(const std::string& directory) {
  std::vector<std::filesystem::path> files;
  for(const auto& entry : std::filesystem::directory_iterator(directory)) {
    if(entry.path().extension() == ".in") files.push_back(entry.path());
  }
  std::sort(files.begin(),files.end());
  if(files.empty()) throw std::runtime_error("no cases");
  return files;
}

BinaryCase read_binary(std::filesystem::path path) {
  std::ifstream in(path);
  int n,m;
  std::string direction;
  in >> n >> m >> direction;
  BinaryCase input;
  input.c0.resize(n);
  input.c1.resize(n);
  for(int i = 0; i < n; ++i) in >> (direction[i] == 'L' ? input.c1[i] : input.c0[i]);
  for(int i = 0; i < m; ++i) {
    int u,v;
    ll cost;
    in >> u >> v >> cost;
    if(u > v) std::swap(u,v);
    input.arcs.push_back({u-1,v-1,cost});
  }
  std::ifstream out(path.replace_extension(".out"));
  ll answer;
  if(!in || !(out >> answer)) throw std::runtime_error("invalid AOJ 3058 case");
  input.expected = answer;
  return input;
}

BinaryCase binary_case(const std::string& name, unsigned seed) {
  std::mt19937_64 rng(seed);
  BinaryCase input;
  if(name == "ladder") {
    const int n = 10000;
    input.c0.assign(n,0);
    input.c1.assign(n,1);
    input.c0[0] = n+1;
    for(int v = 1; v < n; ++v) input.arcs.push_back({v-1,v,n});
    input.expected = n;
  } else if(name == "dense" || name == "sparse" || name == "unary" || name == "contradiction") {
    const int n = name == "dense" ? 600 : name == "contradiction" ? 100000 : 40000;
    input.c0.resize(n);
    input.c1.resize(n);
    for(int v = 0; v < n; ++v) {
      input.c0[v] = rng()%1500;
      input.c1[v] = rng()%1500;
    }
    if(name != "unary") {
      const int m = name == "dense" ? 180000 : 5*n;
      for(int i = 0; i < m; ++i) input.arcs.push_back({int(rng()%n),int(rng()%n),ll(rng()%100)});
    }
    if(name == "contradiction") {
      for(int v = 1; v < n; ++v) input.hard.emplace_back(v-1,v);
      input.fixed = {{0,true},{n-1,false}};
      input.infeasible = true;
    }
  } else {
    throw std::runtime_error("unknown binary case");
  }
  return input;
}

template <class Graph> void measure_binary(const BinaryCase& input, TimeParts& time, bool push, bool check) {
  const int n = int(input.c0.size());
  const auto start = Clock::now();
  Graph g(n);
  if constexpr(requires { g.reserve_edges(0); }) g.reserve_edges(input.arcs.size()+input.hard.size()+input.fixed.size());
  for(int v = 0; v < n; ++v) {
    if constexpr(requires { g.add_unary_cost(v,0,0); }) g.add_unary_cost(v,input.c0[v],input.c1[v]);
    else g.add_cost(v,input.c0[v],input.c1[v]);
  }
  for(const auto& [u,v,c] : input.arcs) {
    if constexpr(requires { g.add_implication_penalty(u,v,c); }) g.add_implication_penalty(u,v,c);
    else g.add_cost_if(u,v,true,false,c);
  }
  for(auto [u,v] : input.hard) {
    if constexpr(requires { g.imply(u,v); }) g.imply(u,v);
    else g.forbid(u,v,true,false);
  }
  for(auto [u,value] : input.fixed) {
    if constexpr(requires { g.fix(u,value); }) g.fix(u,value);
    else g.force(u,value);
  }
  const auto built = Clock::now();
  const auto result = [&] {
    if constexpr(requires { g.solve_push_relabel(); }) return push ? g.solve_push_relabel() : g.solve();
    else return g.solve();
  }();
  const auto end = Clock::now();
  time.build += built-start;
  time.solve += end-built;
  time.checksum += result ? ll(result->cost) : -1;
  if(!check) return;
  if(bool(result) == input.infeasible) throw std::runtime_error("wrong feasibility");
  if(result) {
    Wide cost = 0;
    const auto& x = result->selected;
    for(int v = 0; v < n; ++v) cost += x[v] ? input.c1[v] : input.c0[v];
    for(const auto& [u,v,c] : input.arcs) if(x[u] && !x[v]) cost += c;
    for(auto [u,v] : input.hard) if(x[u] && !x[v]) throw std::runtime_error("wrong implication");
    for(auto [u,value] : input.fixed) if(x[u] != value) throw std::runtime_error("wrong fixed label");
    if(cost != result->cost || (input.expected && cost != *input.expected)) throw std::runtime_error("wrong cost");
  }
}

struct MultiCase {
  struct Pair { int u,v; std::vector<std::vector<ll>> table; };
  std::vector<int> count;
  std::vector<std::vector<ll>> unary;
  std::vector<Pair> pairs;
};

MultiCase multi_case(const std::string& name, unsigned seed) {
  std::mt19937_64 rng(seed);
  MultiCase input;
  if(name == "monge-dense") input.count.assign(20,48);
  else if(name == "monge-uneven") {
    input.count.assign(25,2);
    input.count[0] = 20000;
  } else if(name == "monge-small") input.count.assign(50,5);
  else throw std::runtime_error("unknown multi-label case");
  const int n = int(input.count.size());
  input.unary.resize(n);
  for(int v = 0; v < n; ++v) {
    input.unary[v].resize(input.count[v]);
    for(auto& c : input.unary[v]) c = rng()%2000;
  }
  for(int u = 0; u < n; ++u) for(int v = u+1; v < n; ++v) {
    if(name == "monge-uneven" && u != 0) continue;
    const int h = input.count[u], w = input.count[v], shift = rng()%7, weight = 1+rng()%4;
    auto& table = input.pairs.emplace_back(MultiCase::Pair{u,v,std::vector(h,std::vector<ll>(w))}).table;
    for(int a = 0; a < h; ++a) for(int b = 0; b < w; ++b) {
      const ll d = std::abs(a-b-shift);
      table[a][b] = weight*(name == "monge-uneven" ? d : d*d);
    }
  }
  return input;
}

template <class Graph> void measure_multi(const MultiCase& input, TimeParts& time, bool push, bool check) {
  const auto start = Clock::now();
  Graph g(input.count);
  for(int v = 0; v < int(input.count.size()); ++v) {
    if constexpr(requires { g.add_unary_cost(v,input.unary[v]); }) g.add_unary_cost(v,input.unary[v]);
    else g.add_cost(v,input.unary[v]);
  }
  for(const auto& [u,v,table] : input.pairs) {
    if constexpr(requires { g.add_pair_cost(u,v,table); }) g.add_pair_cost(u,v,table);
    else g.add_cost(u,v,table);
  }
  const auto built = Clock::now();
  const auto [cost,labels] = [&]() -> std::pair<Wide,std::vector<int>> {
    if constexpr(requires { g.solve_dinic(); }) {
      auto result = push ? g.solve() : g.solve_dinic();
      return {result->cost,std::move(result->label)};
    } else {
      return g.min_cost();
    }
  }();
  const auto end = Clock::now();
  time.build += built-start;
  time.solve += end-built;
  time.checksum += ll(cost);
  if(check) {
    Wide restored = 0;
    for(int v = 0; v < int(input.count.size()); ++v) {
      if(labels[v] < 0 || labels[v] >= input.count[v]) throw std::runtime_error("wrong label");
      restored += input.unary[v][labels[v]];
    }
    for(const auto& [u,v,table] : input.pairs) restored += table[labels[u]][labels[v]];
    if(restored != cost) throw std::runtime_error("wrong cost");
  }
}

soryuusi::MaxFlow<ll> residual_input(const std::string& name, unsigned seed) {
  std::mt19937_64 rng(seed);
  int n = 150000, m = 700000;
  if(name == "dag") n = 40000;
  else if(name != "parallel" && name != "chain") throw std::runtime_error("unknown lattice case");
  soryuusi::MaxFlow<ll> g(n);
  if(name == "chain") {
    g.reserve_edges(n-3);
    for(int v = 1; v+1 < n-1; ++v) g.add_edge(v,v+1,1);
  } else {
    g.reserve_edges(m);
    for(int i = 0; i < m; ++i) {
      int u = 1+rng()%(n-2), v = 1+rng()%(n-2);
      if(name == "parallel") u = 1+(u%100), v = u+1;
      else if(u > v) std::swap(u,v);
      g.add_edge(u,v,1);
    }
  }
  return g;
}

void measure_lattice(const soryuusi::MaxFlow<ll>& g, TimeParts& time, bool baseline) {
  const auto start = Clock::now();
  if(baseline) {
    soryuusi::SCC scc(g.size());
    for(const auto& [u,v,c,f] : g.edges()) {
      if(c > f) scc.add_edge(u,v);
      if(f > 0) scc.add_edge(v,u);
    }
    auto [count,dag,component] = scc.scc_compressed();
    std::vector<unsigned char> side(count,0);
    side[component.front()] = 1;
    for(int u = 0; u < count; ++u) if(side[u] == 1) for(int v : dag[u]) side[v] = 1;
    side[component.back()] = 2;
    for(int u = count-1; u >= 0; --u) for(int v : dag[u]) if(side[v] == 2) side[u] = 2;
    for(const auto& out : dag) time.checksum += out.size();
    time.checksum += count;
    const auto built = Clock::now();
    std::vector<bool> smallest(g.size()), largest(g.size());
    for(int v = 0; v < g.size(); ++v) {
      smallest[v] = side[component[v]] == 1;
      largest[v] = side[component[v]] != 2;
    }
    const auto end = Clock::now();
    time.checksum += smallest.front()+largest.back();
    time.build += built-start;
    time.restore += end-built;
  } else {
    const auto lattice = soryuusi::minimum_cut_lattice(g,0,g.size()-1);
    time.checksum += lattice->dag.edge_size()+lattice->dag.size();
    const auto built = Clock::now();
    const auto smallest = lattice->minimum_source_side(), largest = lattice->maximum_source_side();
    const auto end = Clock::now();
    time.checksum += smallest.front()+largest.back();
    time.build += built-start;
    time.restore += end-built;
  }
}

} // namespace

int main(int argc, char** argv) {
  if(argc != 5) {
    std::cerr << "usage: selection-bench METHOD CASE REPEATS SEED\n"
              << "binary, binary-push, qandori: AOJ 3058 case directory, ladder, dense, sparse, unary, contradiction\n"
              << "multi, multi-push, suisen: monge-dense, monge-uneven, monge-small\n"
              << "lattice, sorted: dag, parallel, chain\n";
    return 2;
  }
  const std::string method = argv[1], name = argv[2];
  const int repeats = std::stoi(argv[3]);
  const unsigned seed = std::stoul(argv[4]);
  if(repeats < 1) return 2;
  TimeParts time;
  if(method == "binary" || method == "binary-push" || method == "qandori") {
    std::vector<BinaryCase> inputs;
    if(std::filesystem::is_directory(name)) for(const auto& path : input_paths(name)) inputs.push_back(read_binary(path));
    else inputs.push_back(binary_case(name,seed));
    for(int i = 0; i < repeats; ++i) for(const auto& input : inputs) {
      if(method == "qandori") measure_binary<::ProjectSelection<ll>>(input,time,false,i == 0);
      else measure_binary<soryuusi::ProjectSelection<ll>>(input,time,method == "binary-push",i == 0);
    }
  } else if(method == "multi" || method == "multi-push" || method == "suisen") {
    const auto input = multi_case(name,seed);
    for(int i = 0; i < repeats; ++i) {
      if(method == "suisen") measure_multi<suisen::KProjectSelection<ll>>(input,time,false,i == 0);
      else measure_multi<soryuusi::MultiLabelProjectSelection<ll>>(input,time,method == "multi-push",i == 0);
    }
  } else if(method == "lattice" || method == "sorted") {
    const auto g = residual_input(name,seed);
    for(int i = 0; i < repeats; ++i) measure_lattice(g,time,method == "sorted");
  } else {
    return 2;
  }
  std::cout << std::fixed << std::setprecision(6) << time.build.count()/repeats << ' ' << time.solve.count()/repeats << ' '
            << time.restore.count()/repeats << ' ' << time.checksum << '\n';
}
