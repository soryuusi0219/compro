// https://github.com/soryuusi0219/compro/blob/main/library/tree/rerooting.hpp
#pragma once
#include <functional>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, S op(S,S), S e(), class F, S mapping(F,S)>
struct Rerooting{
  struct Edge{
    int to;
    F f;
  };
  int n;
  vector<S> data;
  vector<Edge> edges;
  int m = 0;
  vector<vector<int>> G;
  vector<S> edge_answers;
  vector<S> vertex_answers;
  Rerooting(int n0) : n(n0){
    G.resize(n);
    data.resize(n);
  }
  Rerooting(int n0, vector<S> data0) : n(n0), data(data0){
    G.resize(n);
  }
  void set(int i, S x){data[i] = x;}
  int add_edge(int i, int j, F f){
    G[i].push_back(m<<1);
    G[j].push_back((m<<1)+1);
    edges.push_back({j,f});
    edges.push_back({i,f});
    return m++;
  }
  int add_edge(int i, int j, F f, F g){
    G[i].push_back(m<<1);
    G[j].push_back((m<<1)+1);
    edges.push_back({j,g});
    edges.push_back({i,f});
    return m++;
  }
  void build(){
    edge_answers.resize(m << 1);
    vertex_answers.resize(n);
    vector<vector<S>> lprod(n,{e()}), rprod(n);
    function<S(int,int)> dfs1 = [&](int v, int p){
      for(int id : G[v]){
        int c = edges[id].to;
        if(c != p) {
          S cval = mapping(edges[id].f, dfs1(c,v));
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
    dfs1(0,-1);

    function<void(int,int,S)> dfs2 = [&](int v, int p, S x){
      vertex_answers[v] = op(vertex_answers[v],x);
      for(int i = 0; i < (int)G[v].size(); i++){
        int id = G[v][i];
        int c = edges[id].to;
        if(c != p) {
          S y = mapping(edges[id^1].f, op(op(x,lprod[v][i]), rprod[v][i+1]));
          edge_answers[id^1] = y;
          dfs2(c,v,op(y,data[c]));
        }
      }
    };
    dfs2(0,-1,data[0]);
  }
  vector<S> answers(){return vertex_answers;}
  S answer(int v){return vertex_answers[v];}
  vector<S> subanswers(int v){
    vector<S> ret;
    for(int id : G[v]) ret.push_back(edge_answers[id]);
    return ret;
  }
  S subanswer(int id){return edge_answers[id];}
};

}  // namespace soryuusi
