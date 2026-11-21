#include <bits/stdc++.h>
using namespace std;

template<class T>
struct Schnyder{
  using Point = pair<T,T>;
  int n;
  vector<Point> vertices;
  vector<pair<int,int>> edges;
  Schnyder(int _n) : n(_n) {vertices.resize(n);}
  Schnyder(vector<Point>& v) : n(v.size()), vertices(v) {}
  void add_edge(int i, int j) {edges.emplace_back(i,j),edges.emplace_back(j,i);}
  int add_vertex(Point p) {vertices.push_back(p); return ++n;}
  int add_vertex(T x, T y) {vertices.emplace_back(x,y); return ++n;}
  void set_point(int i, Point p) {vertices[i] = p;}
  void set_point(int i, T x, T y) {vertices[i] = {x,y};}
  vector<int> build(){
    int m = edges.size()/2;
    vector<vector<int>> nbh(n);
    vector<int> next(2*m), prev(2*m);
    for(int i = 0; i < 2*m; i++) nbh[edges[i].first].emplace_back(i);
    for(int i = 0; i < n; i++) {
      sort(nbh[i].begin(),nbh[i].end(),[&](int l, int r){
      T lx = vertices[edges[l].second].first - vertices[i].first, 
      ly = vertices[edges[l].second].second - vertices[i].second,
      rx = vertices[edges[r].second].first - vertices[i].first,
      ry = vertices[edges[r].second].second - vertices[i].second;
      if(ly >= 0 && ry < 0) return true;
      if(ly < 0 && ry >= 0) return false;
      if(ly == 0 && ry == 0) return lx > rx;
      return lx * ry > ly * rx;
      });
      for(int j = 0; j < (int)nbh[i].size(); j++){
        next[nbh[i][j]] = nbh[i][(j+1)%nbh[i].size()];
        prev[nbh[i][(j+1)%nbh[i].size()]] = nbh[i][j];
      }
    }
    stack<int> st;
    vector<int> deg(n), left(n), right(n), res(m);
    vector<bool> vis(n);
    int v0 = min_element(vertices.begin(),vertices.end())-vertices.begin();
    left[v0] = nbh[v0].front(), right[v0] = nbh[v0].back();
    int v1 = edges[left[v0]].second;
    int v2 = edges[right[v0]].second;
    vis[v1] = true, vis[v2] = true;
    st.push(v0);
    for(auto i : nbh[v0]) deg[edges[i].second]++;
    for(auto i : nbh[v1]) deg[edges[i].second]++;
    for(auto i : nbh[v2]) deg[edges[i].second]++;
    while(!st.empty()){
      int v = st.top(); st.pop();
      if(vis[v] || deg[v] != 2) continue;
      vis[v] = true;
      res[left[v]/2] = 1;
      res[right[v]/2] = 2;
      int vl = edges[left[v]].second;
      int vr = edges[right[v]].second;
      right[vl] = prev[right[vl]];
      left[vr] = next[left[vr]];
      for(int e = next[left[v]]; e != right[v]; e = next[e]){
        int w = edges[e].second;
        left[w] = next[e^1];
        right[w] = prev[e^1];
        for(int e2 = left[w];; e2 = next[e2]){
          deg[edges[e2].second]++;
          if(e2 == right[w]) break;
        }
      }
      for(int e = left[v];; e = next[e]){
        int w = edges[e].second;
        if(--deg[w] == 2 && !vis[w]) st.push(w);
        if(e == right[v]) break;
      }
    }
    return res;
  }
};