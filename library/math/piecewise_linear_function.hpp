// https://github.com/soryuusi0219/compro/blob/main/library/math/piecewise_linear_function.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template <class T>
struct PiecewiseLinearFunction {
  struct Segment {
      T length;
      T slope;
      T jump;
  };

  T start_x;
  T init_y;
  deque<Segment> segments;

  PiecewiseLinearFunction(T start_x = 0, T init_y = 0) : start_x(start_x), init_y(init_y) {}

  inline T get_len() const {
    T len = 0;
    for(const auto& seg : segments) len += seg.length;
    return len;
  }
  inline T get_left() const {return start_x;}
  inline T get_right() const {return start_x + get_len();}
  inline PiecewiseLinearFunction& shift_x(T dx) {start_x += dx; return *this;}
  inline PiecewiseLinearFunction& shift_y(T dy) {init_y += dy; return *this;}
  inline PiecewiseLinearFunction& shift(T dx, T dy) {start_x += dx, init_y += dy; return *this;}
  inline size_t size() const {return segments.size();}
  
  // f += ax + b
  PiecewiseLinearFunction& add_linear(T a, T b) {
    init_y += a * start_x + b;
    for(auto& seg : segments) seg.slope += a;
    return *this;
  }

  //min f + ax
  T get_min_with_linear(T a, bool has_jmp = false) const {
    T cur_y = init_y + a * start_x;
    T min_y = cur_y;
    for(auto& seg : segments) {
      if(has_jmp) min_y = min(min_y, cur_y + (seg.slope + a) * (seg.length - 1)) + seg.jump;
      cur_y += (seg.slope + a) * seg.length + seg.jump;
      min_y = min(min_y, cur_y);
    }
    return min_y;
  }

  T eval(T x) const {
    assert(x >= start_x);
    if(x == start_x) return init_y;
    T current_x = start_x;
    T current_y = init_y;
    for(const auto& seg : segments) {
      if(x < current_x + seg.length) return current_y + seg.slope * (x - current_x);
      current_y += seg.slope * seg.length + seg.jump;
      current_x += seg.length;
    }
    assert(x <= current_x);
    return current_y;
  }

  vector<T> to_vect(bool include_right = false){
    vector<T> res;
    T current_y = init_y;
    for(const auto& seg : segments) {
      for(int i = 0; i < seg.length; ++i){
        res.push_back(current_y);
        current_y += seg.slope;
      }
      current_y += seg.jump;
    }
    if(include_right) res.push_back(current_y);
    return res;
  }

  void push_back(T length, T slope, T jump = 0) {
    if (length <= 0) {
      if (!segments.empty()) segments.back().jump += jump;
      else init_y += jump;
      return;
    }
    if(!segments.empty() && segments.back().slope == slope && segments.back().jump == 0) segments.back().length += length, segments.back().jump = jump;
    else segments.emplace_back(length, slope, jump);
  }

  void push_front(T length, T slope, T jump = 0) {
    if (length <= 0) {
      init_y -= jump;
      return;
    }
    start_x -= length;
    init_y -= slope * length + jump;
    if (!segments.empty() && segments.front().slope == slope && jump == 0) segments.front().length += length;
    else segments.emplace_front(length, slope, jump);
  }

  void pop_front(T len) {
    if (len <= 0) return;
    start_x += len;
    while(len > 0 && !segments.empty()) {
      if(len >= segments.front().length){
        init_y += segments.front().slope * segments.front().length + segments.front().jump;
        len -= segments.front().length;
        segments.pop_front();
      }
      else{
        init_y += segments.front().slope * len;
        segments.front().length -= len;
        return;
      }
    }
  }

  void pop_back(T len) {
    if(len <= 0) return;
    while(len > 0 && !segments.empty()) {
      if(len >= segments.back().length) len -= segments.back().length, segments.pop_back();
      else segments.back().length -= len, segments.back().jump = 0, len = 0;
    }
  }

  //a * f + b * g
  static PiecewiseLinearFunction linear_combine(PiecewiseLinearFunction F, PiecewiseLinearFunction G, T a = 1, T b = 1) {
    T F_left = F.get_left(), G_left = G.get_left();
    T F_right = F.get_right(), G_right = G.get_right();
    T max_left = max(F_left, G_left);
    T min_right = min(F_right, G_right);
    if(max_left >= min_right) return PiecewiseLinearFunction(0,0);
    if(F_left < max_left) F.pop_front(max_left - F_left);
    if(G_left < max_left) G.pop_front(max_left - G_left);
    if(F_right > min_right) F.pop_back(F_right - min_right);
    if(G_right > min_right) G.pop_back(G_right - min_right);

    PiecewiseLinearFunction res(max_left, a * F.init_y + b * G.init_y);
    size_t i = 0, j = 0;
    T rem_f = (i < F.segments.size()) ? F.segments[i].length : 0;
    T rem_g = (j < G.segments.size()) ? G.segments[j].length : 0;
    while (i < F.segments.size() && j < G.segments.size()){
      T current_slope = a * F.segments[i].slope + b * G.segments[j].slope;
      T take_len = min(rem_f, rem_g);
      rem_f -= take_len, rem_g -= take_len;
      T jmp = 0;
      if(rem_f == 0) {
        jmp += a*F.segments[i++].jump;
        if(i < F.segments.size()) rem_f = F.segments[i].length;
      }
      if(rem_g == 0) {
        jmp += b*G.segments[j++].jump;
        if(j < G.segments.size()) rem_g = G.segments[j].length;
      }
      res.push_back(take_len, current_slope, jmp);
    }
    return res;
  }

  friend PiecewiseLinearFunction operator+(const PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g) {return linear_combine(f, g, 1, 1);}
  friend PiecewiseLinearFunction operator-(const PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g) {return linear_combine(f, g, 1, -1);}
  friend PiecewiseLinearFunction& operator+=(PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g) {return f = f + g;}
  friend PiecewiseLinearFunction& operator-=(PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g) {return f = f - g;}
  friend PiecewiseLinearFunction& operator*=(PiecewiseLinearFunction& f, T c) {
    f.init_y *= c;
    for(auto& seg : f.segments) seg.slope *= c, seg.jump *= c;
    return f;
  }
  friend PiecewiseLinearFunction operator*(PiecewiseLinearFunction f, T c) {return f *= c;}
  PiecewiseLinearFunction operator-() const {
    auto res = *this;
    init_y = -init_y;
    for(auto& seg : res.segments) seg.slope = -seg.slope, seg.jump = -seg.jump;
    return *res;
  }

  void merge_max_straight(T& bk, T len, T f_y0, T g_y0, T f_slope, T g_slope) {
    if (len <= 0) return;
    T h1_y = f_y0, h2_y = g_y0;
    T h1_slope = f_slope, h2_slope = g_slope;
    if(h1_y == h2_y) {
      if(h1_slope < h2_slope) swap(h1_slope, h2_slope);
      push_back(0, 0, h1_y - bk);
      push_back(len, h1_slope, 0);
      bk = h1_y + h1_slope * len;
      return;
    }
    if(h1_y < h2_y) swap(h1_y, h2_y), swap(h1_slope, h2_slope);
    push_back(0, 0, h1_y - bk);
    if(h1_y + h1_slope * len >= h2_y + h2_slope * len) {
      push_back(len, h1_slope, 0);
      bk = h1_y + h1_slope * len;
      return;
    }
    T dx = (h1_y - h2_y - 1) / (h2_slope - h1_slope) + 1;
    T y_at_dx_h1 = h1_y + h1_slope * dx;
    T y_at_dx_h2 = h2_y + h2_slope * dx;
    T jump = y_at_dx_h2 - y_at_dx_h1;
    push_back(dx, h1_slope, jump);
    push_back(len - dx, h2_slope, 0);
    bk = h2_y + h2_slope * len;
  }

  // max(a * f, b * g)
  static PiecewiseLinearFunction max_of_scalar_multiple(PiecewiseLinearFunction F, PiecewiseLinearFunction G, T a = 1, T b = 1) {
    T F_left = F.get_left(), G_left = G.get_left();
    T F_right = F.get_right(), G_right = G.get_right();
    T max_left = max(F_left, G_left);
    T min_right = min(F_right, G_right);
    if(max_left >= min_right) return PiecewiseLinearFunction(max_left, 0);
    if(F_left < max_left) F.pop_front(max_left - F_left);
    if(G_left < max_left) G.pop_front(max_left - G_left);
    if(F_right > min_right) F.pop_back(F_right - min_right);
    if(G_right > min_right) G.pop_back(G_right - min_right);
    PiecewiseLinearFunction res(max_left, max(a*F.init_y, b*G.init_y));
    T bk = max(a*F.init_y, b*G.init_y);
    T current_f_y = F.init_y;
    T current_g_y = G.init_y;
    size_t i = 0, j = 0;
    T rem_f = (i < F.segments.size()) ? F.segments[i].length : 0;
    T rem_g = (j < G.segments.size()) ? G.segments[j].length : 0;
    while (i < F.segments.size() && j < G.segments.size()) {
      T take_len = min(rem_f, rem_g);
      T slope_f = F.segments[i].slope;
      T slope_g = G.segments[j].slope;
      res.merge_max_straight(bk, take_len, a*current_f_y, b*current_g_y, a*slope_f, b*slope_g);
      current_f_y += slope_f * take_len;
      current_g_y += slope_g * take_len;
      rem_f -= take_len;
      rem_g -= take_len;
      if (rem_f == 0) {
        current_f_y += F.segments[i++].jump;
        if (i < F.segments.size()) rem_f = F.segments[i].length;
      }
      if (rem_g == 0) {
        current_g_y += G.segments[j++].jump;
        if (j < G.segments.size()) rem_g = G.segments[j].length;
      }
    }
    res.push_back(0, 0, max(a*current_f_y, b*current_g_y) - bk);
    return res;
  }
  static PiecewiseLinearFunction pointwise_max(const PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g) {return max_of_scalar_multiple(f, g, 1, 1);}
  static PiecewiseLinearFunction pointwise_min(const PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g) {return -max_of_scalar_multiple(f, g, -1, -1);}

  // max_t f(t) + g(x-t), O((N+M)^2 log(N+M))
  static PiecewiseLinearFunction max_plus_convolution(const PiecewiseLinearFunction& F, const PiecewiseLinearFunction& G) {
    if(!F.size()) {
      auto res = G;
      return res.shift(F.start_x,F.init_y);
    }
    if(!G.size()) {
      auto res = F;
      return res.shift(G.start_x,G.init_y);
    }

    // static const T MAX_Y = numeric_limits<T>::max() / 2;
    static const T MAX_Y = 10000;
    T F_len = F.get_len(), G_len = G.get_len();

    vector<PiecewiseLinearFunction> Fcts(F.size()+G.size()+2);
    Fcts[0] = F;
    Fcts[0].push_front(G_len,0,MAX_Y);
    Fcts[0].push_back(0,0,-MAX_Y);
    Fcts[0].push_back(G_len-1,0,0);
    Fcts[G.size()+1] = G;
    Fcts[G.size()+1].push_front(F_len,0,MAX_Y);
    Fcts[G.size()+1].push_back(0,0,-MAX_Y);
    Fcts[G.size()+1].push_back(F_len-1,0,0);
    for(size_t i = 0; i < G.size(); ++i) Fcts[i+1] = Fcts[0];
    for(size_t i = 0; i < F.size(); ++i) Fcts[i+G.size()+2] = Fcts[G.size()+1];

    int cur = 0;
    T cur_x = G.start_x, cur_y = G.init_y;
    Fcts[cur++].shift(cur_x, cur_y);
    for(auto seg : G.segments) {
      cur_x += seg.length, cur_y += seg.slope * seg.length + seg.jump;
      Fcts[cur++].shift(cur_x, cur_y);
    }
    cur_x = F.start_x, cur_y = F.init_y;
    Fcts[cur++].shift(cur_x, cur_y);
    for(auto seg : F.segments) {
      cur_x += seg.length, cur_y += seg.slope * seg.length + seg.jump;
      Fcts[cur++].shift(cur_x, cur_y);
    }

    auto cmp = [&](const PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g) -> bool {return f.size() > g.size();};
    priority_queue<PiecewiseLinearFunction, vector<PiecewiseLinearFunction>, decltype(cmp)> que(cmp,Fcts);

    while(que.size() > 1){
      auto H1 = que.top(); que.pop();
      auto H2 = que.top(); que.pop();
      que.push(pointwise_max(H1,H2));
    }
    return que.top();
  }
};

}  // namespace soryuusi
