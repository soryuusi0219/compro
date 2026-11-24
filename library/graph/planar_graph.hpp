// https://github.com/soryuusi0219/compro/blob/main/library/graph/planar_graph.hpp
#pragma once
#include <algorithm>
#include <utility>
#include <vector>

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
  planer_graph(vector<pair<T,T>> v) : N(v.size()), points(move(v)){
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
    nextedge.resize(M);
    for(auto& adjacent : edges){
      if(adjacent.empty()) continue;
      sort(adjacent.begin(),adjacent.end(),[](const auto& lhs, const auto& rhs){
        const auto& [x,y] = lhs.first;
        const auto& [u,v] = rhs.first;
        const bool lower = y < 0 || (y == 0 && x < 0);
        const bool rhs_lower = v < 0 || (v == 0 && u < 0);
        if(lower != rhs_lower) return lower < rhs_lower;
        return u*y < v*x;
      });
      int previous = adjacent.back().second.second;
      for(const auto& e : adjacent){
        const int id = e.second.second;
        nextedge[id] = previous^1;
        previous = id;
      }
    }
    vector<int> face(M,-1), sizes;
    vector<pair<T,vector<int>>> areagroups;
    for(int start = 0; start < M; ++start) if(face[start] == -1){
      const int id = areagroups.size();
      T& sum = areagroups.emplace_back().first;
      const auto& [x,y] = points[edgelist[start].first];
      int edge = start, size = 0;
      do {
        face[edge] = id;
        ++size;
        auto [u,v] = edgelist[edge];
        // nextedge follows the preceding half-edge around the face.
        sum += (points[u].second-y)*(points[v].first-x)-(points[u].first-x)*(points[v].second-y);
        edge = nextedge[edge];
      } while(edge != start);
      sizes.push_back(size);
    }
    K = areagroups.size();
    for(int i = 0; i < K; ++i) areagroups[i].second.reserve(sizes[i]);
    for(int e = 0; e < M; ++e) areagroups[face[e]].second.push_back(e);
    groups.resize(K);
    sort(areagroups.rbegin(), areagroups.rend());
    area.resize(K);
    C = 0;
    for(int l = 0; l < K; l++){
      area[l] = areagroups[l].first;
      groups[l] = move(areagroups[l].second);
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
