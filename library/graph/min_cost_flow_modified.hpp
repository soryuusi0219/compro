// https://github.com/soryuusi0219/compro/blob/main/library/graph/min_cost_flow_modified.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "internal/min_cost_paths.hpp"

namespace soryuusi {

using namespace std;

template <class Cap, class Cost, class Costfct, auto FR, auto FL, auto MAXCOST, auto ZEROCOST> struct mcf_graph_modified {
  static_assert(is_invocable_r_v<pair<Cost, Cap>, decltype(FR), Costfct, Cap>, "FR must be callable with (Costfct, Cap) and return a value convertible to pair<Cost, Cap>");
  static_assert(is_invocable_r_v<pair<Cost,Cap>, decltype(FL), Costfct, Cap>, "FL must be callable with (Costfct, Cap) and return a value convertible to pair<Cost,Cap>");
  static_assert(is_invocable_r_v<Cost, decltype(MAXCOST)>, "MAXCOST must be callable with no arguments and return a value convertible to Cost");
  static_assert(is_invocable_r_v<Cost, decltype(ZEROCOST)>, "ZEROCOST must be callable with no arguments and return a value convertible to Cost");

  using Price = conditional_t<is_integral_v<Cost> && sizeof(Cost) <= 8,__int128_t,Cost>;

  public:
    mcf_graph_modified() : mcf_graph_modified(0) {}
    mcf_graph_modified(int n) : _n(n), h(n,-1), shortest(n,MAXCOST(),ZEROCOST()) {}

    int add_edge(int from, int to, Cap cap, Costfct costfct) {
        assert(0 <= from && from < _n);
        assert(0 <= to && to < _n);
        assert(cap >= 0 && pos.size() < numeric_limits<int>::max()/2);
        initialized = false;
        int m = int(pos.size());
        pos.push_back({from, int(g.size())});
        nxt.push_back(h[from]);
        h[from] = g.size();
        this->to.push_back(to);
        g.push_back(_edge{cap, cap, costfct, false});
        nxt.push_back(h[to]);
        h[to] = g.size();
        this->to.push_back(from);
        g.push_back(_edge{0, cap, costfct, true});
        return m;
    }

    struct edge {
        int from, to;
        Cap cap, flow;
        Costfct costfct;
    };

    edge get_edge(int i) {
        int m = int(pos.size());
        assert(0 <= i && i < m);
        int id = pos[i].second;
        auto _e = g[id];
        auto _re = g[id^1];
        return edge{
            pos[i].first, to[id], _e.cap + _re.cap, _re.cap, _e.costfct,
        };
    }
    vector<edge> edges() {
        int m = int(pos.size());
        vector<edge> result(m);
        for (int i = 0; i < m; i++) {
            result[i] = get_edge(i);
        }
        return result;
    }

    pair<Cap, Cost> flow(int s, int t) {
        return flow(s, t, numeric_limits<Cap>::max());
    }
    pair<Cap, Cost> flow(int s, int t, Cap flow_limit) {
        return slope(s, t, flow_limit).back();
    }
    vector<pair<Cap, Cost>> slope(int s, int t) {
        return slope(s, t, numeric_limits<Cap>::max());
    }
    vector<pair<Cap, Cost>> slope(int s, int t, Cap flow_limit) {
        assert(0 <= s && s < _n);
        assert(0 <= t && t < _n);
        assert(s != t);
        assert(flow_limit >= 0);
        vector<pair<Cap, Cost>> result{{0,ZEROCOST()}};
        if(flow_limit == 0) return result;
        const auto scan = [&](int v, const auto& relax) {
            for(int i = h[v]; i != -1; i = nxt[i]) {
                if(g[i].cap > 0) relax(to[i],i,[&] { return g[i].cost(); });
            }
        };
        if(!initialized) {
            if(!shortest.initialize(scan)) throw domain_error("mcf_graph_modified: negative residual cycle");
            initialized = true;
        }
        Cap flow = 0;
        Cost cost = ZEROCOST();
        Price prev_cost = ZEROCOST();
        while(flow < flow_limit && shortest.search(s,t,scan)) {
            Cap amount = flow_limit-flow;
            for(int v = t; v != s; v = to[shortest.parent[v]^1]) {
                amount = min(amount,g[shortest.parent[v]].next_cap());
            }
            const Price marginal = shortest.price[t]-shortest.price[s];
            if constexpr(is_integral_v<Cost> && sizeof(Cost) <= 8) {
                Price change;
                if(__builtin_mul_overflow(Price(amount),marginal,&change) || __builtin_add_overflow(cost,change,&cost)) {
                    throw overflow_error("mcf_graph_modified: total cost overflow");
                }
            } else {
                cost += amount*marginal;
            }
            for(int v = t; v != s; v = to[shortest.parent[v]^1]) {
                const int i = shortest.parent[v];
                g[i].cap -= amount;
                g[i^1].cap += amount;
            }
            if(flow > 0 && prev_cost == marginal) result.pop_back();
            flow += amount;
            result.emplace_back(flow,cost);
            prev_cost = marginal;
        }
        return result;
    }

    void reset() {
        for(int i = 0; i < int(g.size()); i += 2) {
            g[i].cap = g[i].maxcap;
            g[i+1].cap = 0;
        }
        initialized = false;
    }

  private:
    int _n;
    bool initialized = false;

    struct _edge {
      Cap cap, maxcap;
      Costfct costfct;
      bool inverted;
      Price cost() const { return inverted ? -Price(FL(costfct,cap).first) : Price(FR(costfct,maxcap-cap).first); }
      Cap next_cap() const {
        const Cap x = inverted ? cap : maxcap-cap;
        const Cap end = inverted ? max<Cap>(0,FL(costfct,x).second) : min(maxcap,FR(costfct,x).second);
        if(inverted ? end >= x : end <= x) throw invalid_argument("mcf_graph_modified: invalid marginal interval");
        return inverted ? x-end : end-x;
      }
    };
    vector<pair<int, int>> pos;
    vector<int> h, nxt, to;
    vector<_edge> g;
    internal::MinCostPaths<Price> shortest;
};

}  // namespace soryuusi
