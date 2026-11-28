#include <algorithm>
#include <cassert>
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

#include <atcoder/mincostflow>
#include "library/graph/bounded_flow.hpp"
#include "library/graph/min_cost_b_flow.hpp"
#include "library/graph/min_cost_flow.hpp"
#include "library/graph/min_cost_flow_modified.hpp"
#include "reference_libraries/hitonanode/cplib-cpp/flow/mcf_costscaling.hpp"
#include LC_B_FLOW_REFERENCE

#ifdef LC_B_FLOW_SIMPLEX
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main benchmark_submission_entry
#include LC_B_FLOW_SIMPLEX
#undef main
#undef PROBLEM
#pragma GCC diagnostic pop
#endif

namespace {

using ll = long long;
using Wide = __int128_t;
using Clock = std::chrono::steady_clock;
using Time = std::chrono::duration<double,std::milli>;

struct Edge { int u,v; ll lo,hi,cost; };
struct Case {
  int n;
  ll required = 0;
  std::vector<ll> supply;
  std::vector<Edge> edges;
  std::optional<Wide> expected;
  bool official = false;
};

Wide number(const std::string& s) {
  Wide result = 0;
  for(char c : s) if(c != '-') result = 10*result+(c-'0');
  return s[0] == '-' ? -result : result;
}

std::vector<Case> read_cases(const std::string& path, bool bounded) {
  const std::filesystem::path base(path);
  std::vector<std::filesystem::path> names;
  for(const auto& entry : std::filesystem::directory_iterator(bounded ? base/"in" : base)) {
    if(entry.path().extension() == ".in") names.push_back(entry.path().filename());
  }
  if(names.empty()) throw std::runtime_error("no cases");
  std::sort(names.begin(),names.end());
  std::vector<Case> result;
  for(auto name : names) {
    std::ifstream in((bounded ? base/"in" : base)/name);
    auto& input = result.emplace_back();
    int m;
    in >> input.n >> m;
    if(bounded) {
      input.supply.resize(input.n);
      for(auto& b : input.supply) in >> b;
    } else {
      in >> input.required;
    }
    input.edges.resize(m);
    for(auto& [u,v,l,h,c] : input.edges) {
      in >> u >> v;
      if(bounded) in >> l;
      in >> h >> c;
    }
    std::ifstream out((bounded ? base/"out" : base)/name.replace_extension(".out"));
    std::string answer;
    out >> answer;
    if(!in || !out) throw std::runtime_error("failed to read case");
    if(bounded ? answer != "infeasible" : answer != "-1") input.expected = number(answer);
    input.official = true;
  }
  return result;
}

Case generate(const std::string& name, unsigned seed) {
  std::mt19937_64 rng(seed);
  Case input{};
  auto& e = input.edges;
  if(name == "equal" || name == "strict") {
    input.n = 2;
    const int width = name == "equal" ? 12000 : 2200;
    input.required = width;
    Wide expected = 0;
    for(int i = 0; i < width; ++i) {
      const ll c = name == "equal" ? 100 : i+1;
      e.push_back({0,1,0,1,c});
      expected += c;
    }
    input.expected = expected;
  } else if(name == "large") {
    input.n = 16000;
    input.required = 1LL<<30;
    for(int v = 1; v < input.n; ++v) e.push_back({v-1,v,0,input.required,100000});
    input.expected = Wide(input.required)*(input.n-1)*100000;
  } else if(name == "assignment") {
    const int width = 180;
    input.n = 2*width+2;
    input.required = width;
    for(int i = 0; i < width; ++i) {
      e.push_back({0,i+1,0,1,0});
      e.push_back({width+i+1,input.n-1,0,1,0});
      for(int j = 0; j < width; ++j) e.push_back({i+1,width+j+1,0,1,ll(rng()%1000000)});
    }
  } else if(name == "convex-full" || name == "convex-prefix") {
    input.n = 2;
    const ll width = 1000000;
    const int pieces = name == "convex-full" ? 24 : 256, parallel = 128;
    for(int i = 0; i < parallel; ++i) e.push_back({0,1,width,width*pieces,0});
    input.required = name == "convex-full" ? width*pieces*parallel : width/2;
    input.expected = name == "convex-full" ? Wide(width)*parallel*pieces*(pieces-1)/2 : 0;
  } else if(name == "large-cycle") {
    input.n = 1500;
    input.supply.resize(input.n);
    for(int v = 0; v < input.n; ++v) {
      e.push_back({v,(v+1)%input.n,0,1LL<<45,v+1 == input.n ? -input.n*1000000LL : 1000000});
    }
    input.expected = -Wide(1LL<<45)*1000000;
  } else if(name == "signed-dense" || name == "infeasible") {
    input.n = 150;
    input.supply.resize(input.n);
    for(int u = 0; u+1 < input.n; ++u) for(int v = 0; v+1 < input.n; ++v) {
      e.push_back({u,v,-ll(rng()%(1LL<<36)),ll(rng()%(1LL<<37)),ll(rng()%2000000001)-1000000000});
    }
    if(name == "infeasible") {
      input.supply.back() = 1;
      input.supply.front() = -1;
      input.official = true;
    }
  } else {
    throw std::runtime_error("unknown case");
  }
  return input;
}

void check_cost(const Case& input, const std::optional<Wide>& answer) {
  if((input.official || input.expected) && input.expected != answer) throw std::runtime_error("incorrect cost");
}

void check_certificate(const Case& input, Wide cost, const std::vector<ll>& flow, const auto& potential) {
  if(flow.size() != input.edges.size() || potential.size() != input.supply.size()) throw std::runtime_error("incorrect certificate dimensions");
  auto balance = std::vector<Wide>(input.supply.begin(),input.supply.end());
  Wide total = 0;
  for(int i = 0; i < int(input.edges.size()); ++i) {
    const auto& [u,v,l,h,c] = input.edges[i];
    const ll f = flow[i];
    auto reduced = Wide(c)+potential[u]-potential[v];
    if(f < l || f > h || (f < h && reduced < 0) || (f > l && reduced > 0)) throw std::runtime_error("incorrect certificate");
    balance[u] -= f;
    balance[v] += f;
    total += Wide(f)*c;
  }
  if(total != cost || std::any_of(balance.begin(),balance.end(),[](Wide b) { return b != 0; })) {
    throw std::runtime_error("incorrect certificate");
  }
}

struct Measurement {
  Time build{}, solve{}, restore{};
  Wide cost = 0;
  std::uint64_t checksum = 0;
  int feasible = 0;
};

template <class Graph> void measure_st(const Case& input, Measurement& time, bool check) {
  const auto start = Clock::now();
  Graph graph(input.n);
  if constexpr(requires { graph.reserve_edges(0); }) graph.reserve_edges(input.edges.size());
  for(const auto& [u,v,l,h,c] : input.edges) graph.add_edge(u,v,h,c);
  const auto built = Clock::now();
  const auto [flow,cost] = graph.flow(0,input.n-1,input.required);
  const auto solved = Clock::now();
  const auto edges = graph.edges();
  for(const auto& e : edges) time.checksum += e.flow;
  const auto end = Clock::now();
  time.build += built-start;
  time.solve += solved-built;
  time.restore += end-solved;
  time.cost += cost;
  time.feasible += flow == input.required;
  if(check) check_cost(input,flow == input.required ? std::optional<Wide>(cost) : std::nullopt);
}

void measure_b(const Case& input, Measurement& time, bool check) {
  const auto start = Clock::now();
  soryuusi::MinCostBFlow<> graph(input.n);
  graph.reserve_edges(input.edges.size());
  for(const auto& [u,v,l,h,c] : input.edges) graph.add_edge(u,v,l,h,c);
  const auto built = Clock::now();
  auto result = graph.solve(input.supply);
  const auto end = Clock::now();
  time.build += built-start;
  time.solve += end-built;
  if(result) {
    ++time.feasible;
    time.cost += result->cost;
    if(check) check_certificate(input,result->cost,result->flow,result->potential);
  }
  if(check) check_cost(input,result ? std::optional<Wide>(result->cost) : std::nullopt);
}

void measure_capacity(const Case& input, Measurement& time, bool check) {
  const auto start = Clock::now();
  MinCostFlow<ll,ll> graph;
  graph.add_vertices(input.n);
  for(int v = 0; v < input.n; ++v) graph.add_supply(v,input.supply[v]);
  std::vector<MinCostFlow<ll,ll>::EdgePtr> handles;
  handles.reserve(input.edges.size());
  for(const auto& [u,v,l,h,c] : input.edges) handles.push_back(graph.add_edge(u,v,l,h,c));
  const auto built = Clock::now();
  const auto [status,unused] = graph.solve();
  const auto solved = Clock::now();
  std::optional<Wide> result;
  if(status == Status::OPTIMAL) {
    result = 0;
    auto potential = graph.get_potential();
    std::vector<ll> flow;
    flow.reserve(handles.size());
    for(const auto& h : handles) {
      flow.push_back(h.flow());
      *result += Wide(h.flow())*h.cost();
    }
    time.restore += Clock::now()-solved;
    ++time.feasible;
    time.cost += *result;
    if(check) check_certificate(input,*result,flow,potential);
  }
  time.build += built-start;
  time.solve += solved-built;
  if(check) check_cost(input,result);
}

void measure_scaling(const Case& input, Measurement& time, bool check) {
  const auto start = Clock::now();
  soryuusi::BoundedFlow<ll> feasibility(input.n);
  mcf_costscaling<ll,ll,3,10> graph(input.n);
  for(int v = 0; v < input.n; ++v) graph.add_supply(v,input.supply[v]);
  Wide bias = 0;
  for(const auto& [u,v,l,h,c] : input.edges) {
    feasibility.add_edge(u,v,l,h);
    graph.add_edge(u,v,h-l,c);
    graph.add_demand(u,l);
    graph.add_supply(v,l);
    bias += Wide(l)*c;
  }
  const auto built = Clock::now();
  std::optional<Wide> result;
  if(feasibility.solve(input.supply)) result = graph.solve<Wide>()+bias;
  const auto solved = Clock::now();
  if(result) {
    const auto potential = graph.potential();
    const auto edges = graph.edges();
    std::vector<ll> flow;
    flow.reserve(edges.size());
    for(int i = 0; i < int(edges.size()); ++i) flow.push_back(edges[i].flow+input.edges[i].lo);
    time.restore += Clock::now()-solved;
    ++time.feasible;
    time.cost += *result;
    if(check) check_certificate(input,*result,flow,potential);
  }
  time.build += built-start;
  time.solve += solved-built;
  if(check) check_cost(input,result);
}

#ifdef LC_B_FLOW_SIMPLEX
void measure_simplex(const Case& input, Measurement& time, bool check) {
  const auto start = Clock::now();
  m1une::flow::BoundedMinCostFlow<ll,ll,Wide> graph(input.n);
  for(const auto& [u,v,l,h,c] : input.edges) graph.add_edge(u,v,l,h,c);
  const auto built = Clock::now();
  auto result = graph.min_cost_flow(input.supply);
  const auto end = Clock::now();
  time.build += built-start;
  time.solve += end-built;
  if(result) {
    ++time.feasible;
    time.cost += result->cost;
    if(check) {
      std::vector<ll> flow;
      for(const auto& e : result->edges) flow.push_back(e.flow);
      check_certificate(input,result->cost,flow,result->potential);
    }
  }
  if(check) check_cost(input,result ? std::optional<Wide>(result->cost) : std::nullopt);
}
#endif

std::pair<ll,ll> right(std::pair<ll,ll> c, ll) { return c; }
std::pair<ll,ll> left(std::pair<ll,ll> c, ll) { return {c.first,0}; }
ll inf() { return 1LL<<61; }
ll zero() { return 0; }
using Legacy = soryuusi::mcf_graph_modified<ll,ll,std::pair<ll,ll>,right,left,inf,zero>;
struct Modified : Legacy {
  using Legacy::Legacy;
  void add_edge(int u, int v, ll cap, ll cost) { Legacy::add_edge(u,v,cap,{cost,cap}); }
};

std::pair<ll,ll> convex_right(ll width, ll x) {
  const ll piece = x/width;
  return {piece,(piece+1)*width};
}

std::pair<ll,ll> convex_left(ll width, ll x) {
  const ll piece = (x-1)/width;
  return {piece,piece*width};
}

using Convex = soryuusi::mcf_graph_modified<ll,ll,ll,convex_right,convex_left,inf,zero>;

void measure_convex(const Case& input, Measurement& time, bool expanded, bool check) {
  const auto start = Clock::now();
  auto measure = [&](auto& graph) {
    const auto built = Clock::now();
    const auto [flow,cost] = graph.flow(0,input.n-1,input.required);
    const auto solved = Clock::now();
    for(const auto& e : graph.edges()) time.checksum += e.flow;
    const auto end = Clock::now();
    time.build += built-start;
    time.solve += solved-built;
    time.restore += end-solved;
    time.cost += cost;
    time.feasible += flow == input.required;
    if(check) check_cost(input,flow == input.required ? std::optional<Wide>(cost) : std::nullopt);
  };
  if(expanded) {
    soryuusi::MinCostFlow<ll,ll,ll> graph(input.n);
    for(const auto& [u,v,width,cap,cost] : input.edges) {
      for(ll piece = 0; piece < cap/width; ++piece) graph.add_edge(u,v,width,piece+cost);
    }
    measure(graph);
  } else {
    Convex graph(input.n);
    for(const auto& [u,v,width,cap,cost] : input.edges) graph.add_edge(u,v,cap,width);
    measure(graph);
  }
}

} // namespace

int main(int argc, char** argv) {
  if(argc != 5) {
    std::cerr << "usage: min-cost-flow-bench ALGORITHM CASE REPEATS SEED\n"
              << "s-t: primal, acl, modified with equal, strict, large, assignment or AOJ case directory\n"
              << "convex: convex-expand, convex-callback with convex-full or convex-prefix\n"
              << "b-flow: cost, capacity, scaling-ref, simplex (optional) with large-cycle, signed-dense, infeasible or LC problem directory\n";
    return 2;
  }
  const std::string algorithm = argv[1], name = argv[2];
  const bool bounded = algorithm == "cost" || algorithm == "capacity" || algorithm == "scaling-ref" || algorithm == "simplex";
  const int repeats = std::stoi(argv[3]);
  if(repeats < 1) return 2;
  const bool convex = algorithm == "convex-expand" || algorithm == "convex-callback";
  if(convex != (name == "convex-full" || name == "convex-prefix")) return 2;
  const auto inputs = std::filesystem::is_directory(name) ? read_cases(name,bounded) : std::vector{generate(name,std::stoul(argv[4]))};
  Measurement time;
  for(const auto& input : inputs) for(int i = 0; i < repeats; ++i) {
    if(algorithm == "primal") measure_st<soryuusi::MinCostFlow<ll,ll,ll>>(input,time,i == 0);
    else if(algorithm == "acl") measure_st<atcoder::mcf_graph<ll,ll>>(input,time,i == 0);
    else if(algorithm == "modified") measure_st<Modified>(input,time,i == 0);
    else if(algorithm == "convex-expand" || algorithm == "convex-callback") measure_convex(input,time,algorithm == "convex-expand",i == 0);
    else if(algorithm == "cost") measure_b(input,time,i == 0);
    else if(algorithm == "capacity") measure_capacity(input,time,i == 0);
    else if(algorithm == "scaling-ref") measure_scaling(input,time,i == 0);
#ifdef LC_B_FLOW_SIMPLEX
    else if(algorithm == "simplex") measure_simplex(input,time,i == 0);
#endif
    else return 2;
  }
  std::cout << std::fixed << std::setprecision(6) << time.build.count()/repeats << ' ' << time.solve.count()/repeats << ' '
            << time.restore.count()/repeats << ' ' << time.feasible/repeats << ' ' << std::uint64_t(time.cost) << ' ' << time.checksum << '\n';
}
