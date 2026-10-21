// https://github.com/soryuusi0219/compro/blob/main/library/tree/rerooting.hpp
#pragma once
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e, class F, auto mapping>
struct Rerooting{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(mapping), F, S>, "mapping must be callable with (F, S) and return a value convertible to S");

  int n;
  vector<S> data;
  vector<int> h, nxt, to;
  vector<F> fs;
  int m = 0;
  vector<S> edge_answers;
  vector<S> vertex_answers;
  Rerooting(int n0) : n(n0), h(n,-1){
    data.resize(n);
  }
  Rerooting(int n0, vector<S> data0) : n(n0), data(data0), h(n,-1){}
  void set(int i, S x){data[i] = x;}
  int add_edge(int i, int j, F f){
    nxt.push_back(h[i]);
    h[i] = to.size();
    to.push_back(j);
    fs.push_back(f);
    nxt.push_back(h[j]);
    h[j] = to.size();
    to.push_back(i);
    fs.push_back(f);
    return m++;
  }
  int add_edge(int i, int j, F f, F g){
    nxt.push_back(h[i]);
    h[i] = to.size();
    to.push_back(j);
    fs.push_back(g);
    nxt.push_back(h[j]);
    h[j] = to.size();
    to.push_back(i);
    fs.push_back(f);
    return m++;
  }
  void build(){
    edge_answers.resize(m << 1);
    vertex_answers.resize(n);
    vector<vector<S>> lprod(n,{e()}), rprod(n);
    auto dfs1 = [&](auto&& self, int v, int p) -> S {
      for(int id = h[v]; ~id; id = nxt[id]){
        int c = to[id];
        if(c != p) {
          S cval = mapping(fs[id],self(self,c,v));
          edge_answers[id] = cval;
          lprod[v].push_back(cval);
        }
        else lprod[v].push_back(e());
      }
      int k = lprod[v].size();
      rprod[v].resize(k);
      rprod[v][k-1] = e();
      for(int i = k-2; i >= 0; i--){
        rprod[v][i] = op(rprod[v][i+1],lprod[v][i+1]);
      }
      for(int i = 0; i < k-1; i++){
        lprod[v][i+1] = op(lprod[v][i+1],lprod[v][i]);
      }
      vertex_answers[v] = rprod[v][0];
      return op(data[v],rprod[v][0]);
    };
    dfs1(dfs1,0,-1);

    auto dfs2 = [&](auto&& self, int v, int p, S x) -> void {
      vertex_answers[v] = op(vertex_answers[v],x);
      int i = 0;
      for(int id = h[v]; ~id; id = nxt[id], i++){
        int c = to[id];
        if(c != p) {
          S y = mapping(fs[id^1], op(op(x,lprod[v][i]), rprod[v][i+1]));
          edge_answers[id^1] = y;
          self(self,c,v,op(y,data[c]));
        }
      }
    };
    dfs2(dfs2,0,-1,data[0]);
  }
  vector<S> answers(){return vertex_answers;}
  S answer(int v){return vertex_answers[v];}
  vector<S> subanswers(int v){
    vector<S> ret;
    for(int id = h[v]; ~id; id = nxt[id]) ret.push_back(edge_answers[id]);
    return ret;
  }
  S subanswer(int id){return edge_answers[id];}
};

}  // namespace soryuusi
