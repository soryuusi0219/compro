#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <utility>
#include <vector>

#include "library/data_structure/area_union_rectangle.hpp"
#include "library/data_structure/bit_2d.hpp"
#include "library/data_structure/kd_tree.hpp"
#include "library/data_structure/lazy_kd_tree.hpp"
#include "library/data_structure/segtree_2d.hpp"
#include "library/data_structure/segtree_2d_sparse.hpp"
#include "library/data_structure/static_rectangle_add_rectangle_sum.hpp"
#include "library/data_structure/static_rectangle_sum.hpp"

namespace {

using LL = long long;
LL add(LL a, LL b) { return a+b; }
LL zero() { return 0; }

struct Rectangle {
  int l, r, d, u;

  bool contains(int x, int y) const { return l <= x && x < r && d <= y && y < u; }
};

Rectangle interval(std::mt19937& rng, int h, int w) {
  int l = rng()%(h+1), r = rng()%(h+1);
  int d = rng()%(w+1), u = rng()%(w+1);
  if(l > r) std::swap(l,r);
  if(d > u) std::swap(d,u);
  return {l,r,d,u};
}

void test_dense() {
  for(auto [h,w] : {std::pair{0,0},{4,0},{1,65},{65,1},{1,1},{7,9},{8,8},{15,17}}) {
    std::mt19937 rng(40+h*w);
    std::vector<std::vector<LL>> a(h,std::vector<LL>(w));
    for(auto& row : a) for(auto& x : row) x = int(rng()%101)-50;
    soryuusi::BIT2D<LL> bit(a);
    soryuusi::Segtree2D<LL,add,zero> seg(a);
    for(int k = 0; k < 400; ++k) {
      if(h && w && k%3 == 0) {
        int x = rng()%h, y = rng()%w, v = int(rng()%31)-15;
        a[x][y] += v;
        bit.add(x,y,v);
        seg.set(x,y,a[x][y]);
        assert(seg.get(x,y) == a[x][y]);
      }
      Rectangle q = k%5 == 0 ? Rectangle{0,h,0,w} : interval(rng,h,w);
      LL expected = 0;
      for(int x = q.l; x < q.r; ++x) for(int y = q.d; y < q.u; ++y) expected += a[x][y];
      assert(bit.sum(q.l,q.r,q.d,q.u) == expected);
      assert(seg.prod(q.l,q.r,q.d,q.u) == expected);
      if(k%5 == 0) assert(seg.all_prod() == expected && bit.sum(h,w) == expected);
    }
  }
}

void test_sparse() {
  constexpr LL edge = std::numeric_limits<LL>::min(), step = 1LL<<39;
  const auto xx = [=](int x){ return edge+x*step; };
  soryuusi::Segtree2D_Sparse<LL,add,zero,LL,int> tree;
  tree.build();
  assert(tree.all_prod() == 0 && tree.prod(xx(0),xx(15),-10,15) == 0);
  std::mt19937 rng(8915);
  std::vector<std::pair<int,int>> registered;
  for(int x = 0; x < 17; ++x) for(int y = -x/2; y <= x/2; y += 2) registered.emplace_back(x,y);
  std::shuffle(registered.begin(),registered.end(),rng);
  for(auto [x,y] : registered) {
    tree.add_point(xx(x),y);
    tree.add_point(xx(x),y);
  }
  auto exercise = [&] {
    tree.build();
    std::vector<LL> weights(registered.size());
    for(int t = 0; t < 330; ++t) {
      if(t%3 == 0) {
        int p = rng()%registered.size(), delta = int(rng()%41)-20;
        auto [x,y] = registered[p];
        weights[p] += delta;
        tree.add(xx(x),y,delta);
      }
      auto q = interval(rng,18,24);
      q.d -= 12, q.u -= 12;
      LL selected = 0, total = 0;
      for(int p = 0; p < int(registered.size()); ++p) {
        auto [x,y] = registered[p];
        if(q.contains(x,y)) selected += weights[p];
        total += weights[p];
      }
      assert(tree.prod(xx(q.l),xx(q.r),q.d,q.u) == selected);
      assert(tree.all_prod() == total);
    }
  };
  exercise();
  registered.emplace_back(17,11);
  tree.add_point(xx(17),11);
  exercise();
}

struct Sum { LL sum; int count; };
struct Affine { LL a, b; };
Sum combine(Sum a, Sum b) { return {a.sum+b.sum,a.count+b.count}; }
Sum empty() { return {0,0}; }
Sum map_value(Affine f, Sum a) { return {f.a*a.sum+f.b*a.count,a.count}; }
Affine compose(Affine f, Affine g) { return {f.a*g.a,f.a*g.b+f.b}; }
Affine identity() { return {1,0}; }

void test_kd(int n, int pattern) {
  std::mt19937 rng(521+n);
  std::vector<std::pair<int,int>> points(n);
  std::vector<LL> weights(n);
  std::vector<Sum> initial(n);
  soryuusi::KDTree<int,int,LL,add,zero> kd(n);
  for(int i = 0; i < n; ++i) {
    int x = rng()%17, y = rng()%17;
    if(pattern == 1) x = 6;
    if(pattern == 2) x = i, y = n-i;
    if(pattern == 3) x = 7, y = 8;
    points[i] = {x,y};
    weights[i] = i%21-10;
    initial[i] = {weights[i],1};
    assert(kd.add_point(x,y,weights[i]) == i);
  }
  kd.build();
  soryuusi::LazyKDTree<int,int,Sum,combine,empty,Affine,map_value,compose,identity> lazy(points);
  lazy.build(initial);
  auto values = weights;
  for(int t = 0; t < 160; ++t) {
    auto q = interval(rng,80,80);
    if(t%5 == 0) q = {0,80,0,80};
    const Affine f{int(rng()%3)-1,int(rng()%11)-5};
    if(t%6 == 0) {
      auto result = lazy.all_apply(f);
      LL expected = 0;
      for(LL& x : values) x = f.a*x+f.b, expected += x;
      assert(result.sum == expected && result.count == n);
    } else if(t%3 == 0) {
      lazy.apply(q.l,q.r,q.d,q.u,f);
      for(int i = 0; i < n; ++i) if(q.contains(points[i].first,points[i].second)) values[i] = f.a*values[i]+f.b;
    } else if(n) {
      int p = rng()%n;
      kd.set(p,t);
      weights[p] = t;
      lazy.set(p,{-t,1});
      values[p] = -t;
      assert(kd.get(p) == weights[p] && lazy.get(p).sum == values[p]);
    }
    Sum expected{0,0};
    LL fixed = 0;
    for(int i = 0; i < n; ++i) {
      if(q.contains(points[i].first,points[i].second)) {
        fixed += weights[i];
        expected = combine(expected,{values[i],1});
      }
    }
    Sum result = lazy.prod(q.l,q.r,q.d,q.u);
    assert(result.sum == expected.sum && result.count == expected.count);
    assert(kd.prod(q.l,q.r,q.d,q.u) == fixed);
  }
  lazy.build(initial);
  assert(lazy.all_prod().count == n);
  for(int i = 0; i < n; ++i) assert(lazy.get(i).sum == initial[i].sum);
}

void test_sweeps() {
  constexpr int w = 13, shift = 6;
  soryuusi::StaticRectangleSum<LL> points;
  soryuusi::StaticRectangleAddRectangleSum<LL> coverage;
  soryuusi::Area_Union_Rectangle<LL> area;
  std::mt19937 rng(170116);
  std::vector point_grid(w,std::vector<LL>(w));
  auto weight_grid = point_grid;
  std::vector cover_grid(w,std::vector<int>(w));
  std::vector<Rectangle> queries;
  assert(area.area() == 0 && points.solve().empty() && coverage.solve().empty());
  for(int i = 0; i < 250; ++i) {
    int x = rng()%w, y = rng()%w, v = int(rng()%39)-19;
    points.add_point(x-shift,y-shift,v);
    point_grid[x][y] += v;
    auto a = interval(rng,w,w);
    coverage.add_rectangle(a.l-shift,a.r-shift,a.d-shift,a.u-shift,v);
    area.add(a.l-shift,a.r-shift,a.d-shift,a.u-shift);
    for(int x0 = a.l; x0 < a.r; ++x0) for(int y0 = a.d; y0 < a.u; ++y0) {
      weight_grid[x0][y0] += v;
      ++cover_grid[x0][y0];
    }
    auto q = interval(rng,w,w);
    points.add_query(q.l-shift,q.r-shift,q.d-shift,q.u-shift);
    coverage.add_query(q.l-shift,q.r-shift,q.d-shift,q.u-shift);
    queries.push_back(q);
    if(i%17 != 0 && i != 249) continue;
    auto a1 = points.solve();
    auto a2 = coverage.solve();
    assert(a1.size() == queries.size() && a2.size() == queries.size());
    for(int j = 0; j < int(queries.size()); ++j) {
      LL p = 0, c = 0;
      auto [l,r,d,u] = queries[j];
      for(int x0 = l; x0 < r; ++x0) for(int y0 = d; y0 < u; ++y0) {
        p += point_grid[x0][y0];
        c += weight_grid[x0][y0];
      }
      assert(a1[j] == p && a2[j] == c);
    }
    LL expected = 0;
    for(const auto& row : cover_grid) for(int v0 : row) expected += v0 > 0;
    assert(area.area() == expected && area.area() == expected);
  }
}

}  // namespace

int main() {
  test_dense();
  test_sparse();
  for(int n : {0,1,2,3,7,8,31,63,64,65,101}) for(int pattern = 0; pattern < 4; ++pattern) test_kd(n,pattern);
  test_sweeps();
  std::cout << "rectangle_queries: all tests passed\n";
}
