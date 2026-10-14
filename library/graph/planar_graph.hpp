// https://github.com/soryuusi0219/compro/blob/main/library/graph/planar_graph.hpp
#pragma once
#include <algorithm>
#include <utility>
#include <vector>

#include "library/data_structure/dsu.hpp"

namespace soryuusi {

using namespace std;

template<class T>
struct planer_graph{
  int N = 0, M = 0, K = 0, C = 0;
  vector<pair<T,T>> points;
  vector<vector<pair<pair<T,T>, pair<int,int>>>> edges;
  vector<pair<int,int>> edgelist;
  vector<vector<int>> groups;
  vector<int> nextedge;
  vector<int> leaders;
  vector<T> area;
  planer_graph(int n) : N(n) {
    points.resize(N);
    edges.resize(N);
  }
  planer_graph(vector<pair<T,T>> v) : N(v.size()) , points(v){
    points.resize(N);
    edges.resize(N);
  }
  void set(int i, pair<T,T> pt){
    points[i] = pt;
  }
  void add_edge(int i, int j){
    edges[i].push_back({{points[j].first-points[i].first,points[j].second-points[i].second},{j,M}});
    edges[j].push_back({{points[i].first-points[j].first,points[i].second-points[j].second},{i,M+1}});
    edgelist.push_back({i,j});
    edgelist.push_back({j,i});
    M += 2;
  }
  int build(){
    DSU d(M);
    nextedge.resize(M);
    for(int i = 0; i < N; i++){
      sort(edges[i].begin(), edges[i].end(), [](pair<pair<T,T>, pair<int,int>> lhs, pair<pair<T,T>, pair<int,int>> rhs){
        if(lhs.first.second >= 0 && rhs.first.second < 0) return true;
        if(lhs.first.second < 0 && rhs.first.second >= 0) return false;
        if(lhs.first.second == 0 && rhs.first.second == 0) return rhs.first.second < lhs.first.second;
        return rhs.first.first * lhs.first.second < rhs.first.second * lhs.first.first;
      });
      for(int j = 0; j < (int)edges[i].size(); j++){
        nextedge[edges[i][(j+1) % edges[i].size()].second.second] = edges[i][j].second.second^1;
        d.merge(edges[i][j].second.second^1, edges[i][(j+1) % edges[i].size()].second.second);
      }
    }
    groups = d.groups();
    K = groups.size();
    vector<pair<T,vector<int>>> areagroups(K);
    for(int l = 0; l < K; l++){
      areagroups[l] = {0, groups[l]};
      int s = groups[l][0];
      int t = nextedge[s];
      auto [i,j] = edgelist[s];
      T dx = points[j].first-points[i].first;
      T dy = points[j].second-points[i].second;
      while(t != s){
        auto [i0,j0] = edgelist[t];
        T dx0 = points[j0].first-points[i0].first;
        T dy0 = points[j0].second-points[i0].second;
        areagroups[l].first += dx*dy0-dy*dx0;
        dx += dx0, dy+=dy0;
        t = nextedge[t];
      }
    }
    sort(areagroups.rbegin(), areagroups.rend());
    area.resize(K);
    C = 0;
    for(int l = 0; l < K; l++){
      area[l] = areagroups[l].first;
      groups[l] = areagroups[l].second;
      if(area[l] > 0) C++;
    }
    leaders.resize(M);
    for(int i = 0; i < K; i++) for(int j : groups[i]) leaders[j] = i;
    return C + 1;
  }
  int detect_domain(pair<T,T> pt){
    auto [x,y] = pt;
    vector<bool> cnt(K,false);
    for(int l = 0; l < M; l += 2){
      auto [i,j] = edgelist[l];
      auto [s1,t1] = points[i];
      auto [s2,t2] = points[j];
      if((s1 >= x && s2 < x && (s1-x)*(t2-y) - (s2-x)*(t1-y) > 0) || (s1 < x && s2 >= x && (s1-x)*(t2-y) - (s2-x)*(t1-y) < 0)) {
        cnt[leaders[l]] = !cnt[leaders[l]];
        cnt[leaders[l^1]] = !cnt[leaders[l^1]];
      }
    }
    for(int l = C-1; l >= 0; l--) if(cnt[l]) return l;
    return -1;
  }
  T get_area(int id){return area[id];}
  vector<T> get_areas(){return area;}
};

}  // namespace soryuusi
