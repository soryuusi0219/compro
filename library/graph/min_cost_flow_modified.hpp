// https://github.com/soryuusi0219/compro/blob/main/library/graph/min_cost_flow_modified.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template <class Cap, class Cost, class Costfct, auto FR, auto FL, auto MAXCOST, auto ZEROCOST> struct mcf_graph_modified {
  static_assert(is_invocable_r_v<pair<Cost, Cap>, decltype(FR), Costfct, Cap>, "FR must be callable with (Costfct, Cap) and return a value convertible to pair<Cost, Cap>");
  static_assert(is_invocable_r_v<pair<Cost,Cap>, decltype(FL), Costfct, Cap>, "FL must be callable with (Costfct, Cap) and return a value convertible to pair<Cost,Cap>");
  static_assert(is_invocable_r_v<Cost, decltype(MAXCOST)>, "MAXCOST must be callable with no arguments and return a value convertible to Cost");
  static_assert(is_invocable_r_v<Cost, decltype(ZEROCOST)>, "ZEROCOST must be callable with no arguments and return a value convertible to Cost");

  public:
    mcf_graph_modified() {}
    mcf_graph_modified(int n) : _n(n), h(n,-1) {}

    int add_edge(int from, int to, Cap cap, Costfct costfct) {
        assert(0 <= from && from < _n);
        assert(0 <= to && to < _n);
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
        vector<Cost> dual(_n, ZEROCOST()), dist(_n);
        vector<int> pv(_n), pe(_n);
        vector<bool> vis(_n);
        auto dual_ref = [&]() {
            fill(dist.begin(), dist.end(), MAXCOST());
            fill(pv.begin(), pv.end(), -1);
            fill(pe.begin(), pe.end(), -1);
            fill(vis.begin(), vis.end(), false);
            struct Q {
                Cost key;
                int to;
                bool operator<(Q r) const { return key > r.key; }
            };
            priority_queue<Q> que;
            dist[s] = ZEROCOST();
            que.push(Q{ZEROCOST(), s});
            while (!que.empty()) {
                int v = que.top().to;
                que.pop();
                if (vis[v]) continue;
                vis[v] = true;
                if (v == t) break;
                for (int i = h[v]; ~i; i = nxt[i]) {
                    auto e = g[i];
                    if (vis[to[i]] || !e.cap) continue;
                    Cost cost = e.cost() - dual[to[i]] + dual[v];
                    if (dist[to[i]] - dist[v] > cost) {
                        dist[to[i]] = dist[v] + cost;
                        pv[to[i]] = v;
                        pe[to[i]] = i;
                        que.push(Q{dist[to[i]], to[i]});
                    }
                }
            }
            if (!vis[t]) {
                return false;
            }

            for (int v = 0; v < _n; v++) {
                if (!vis[v]) continue;
                dual[v] -= dist[t] - dist[v];
            }
            return true;
        };
        Cap flow = 0;
        Cost cost = ZEROCOST(), prev_cost_per_flow = -MAXCOST();
        vector<pair<Cap, Cost>> result;
        result.push_back({flow, cost});
        while (flow < flow_limit) {
            if (!dual_ref()) break;
            Cap c = flow_limit - flow;
            for (int v = t; v != s; v = pv[v]) {
                c = min(c, g[pe[v]].next_cap());
            }
            for (int v = t; v != s; v = pv[v]) {
                auto& e = g[pe[v]];
                e.cap -= c;
                g[pe[v]^1].cap += c;
            }
            Cost d = -dual[s];
            flow += c;
            cost += c * d;
            if (prev_cost_per_flow == d) {
                result.pop_back();
            }
            result.push_back({flow, cost});
            prev_cost_per_flow = d;
        }
        return result;
    }

  private:
    int _n;

    struct _edge {
      Cap cap, maxcap;
      Costfct costfct;
      bool inverted;
      Cost cost() {return (inverted ? -FL(costfct, cap).first : FR(costfct, maxcap-cap).first);}
      Cap next_cap() {return min((inverted ? cap - FL(costfct, cap).second : FR(costfct, maxcap-cap).second + cap - maxcap), cap);}
    };
    vector<pair<int, int>> pos;
    vector<int> h, nxt, to;
    vector<_edge> g;
};
// using Cap = ll;
// using Cost = ll;
// using Costfct = pi;
// constexpr Cost MAXCOST(){return INF;}
// constexpr Cost ZEROCOST(){return 0;}
// pair<Cost,Cap> FL(Costfct f, Cap x){
//   if(x < 0) return {0,0};
//   if(f.fi) return {f.fi * x + f.se, x+1};
//   return {f.se,INF};
// }
// pair<Cost,Cap> FR(Costfct f, Cap x){
//   if(x <= 0) return {0,0};
//   if(f.fi) return {f.fi * (x-1) + f.se, x-1}; 
//   return {f.se,0};
// }

}  // namespace soryuusi
