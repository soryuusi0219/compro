#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "library/math/piecewise_linear_function.hpp"

int main() {
  using Function = soryuusi::PiecewiseLinearFunction<long long>;
  Function f(0,1);
  f.push_back(2,2,3);
  f.push_back(2,-1);
  assert((f.to_vect(true) == std::vector<long long>{1,3,8,7,6}));
  for(int x = 0; x <= 4; ++x) assert(f.eval(x) == f.to_vect(true)[x]);
  assert(f.get_min_with_linear(0,true) == 1);

  Function g(0,4);
  g.push_back(4,1);
  auto sum = f+g;
  auto difference = f-g;
  auto negative = -f;
  auto maximum = Function::pointwise_max(f,g);
  auto minimum = Function::pointwise_min(f,g);
  for(int x = 0; x <= 4; ++x) {
    assert(sum.eval(x) == f.eval(x)+g.eval(x));
    assert(difference.eval(x) == f.eval(x)-g.eval(x));
    assert(negative.eval(x) == -f.eval(x));
    assert(maximum.eval(x) == std::max(f.eval(x),g.eval(x)));
    assert(minimum.eval(x) == std::min(f.eval(x),g.eval(x)));
  }

  auto cropped = f;
  cropped.pop_front(1);
  cropped.pop_back(1);
  assert(cropped.get_left() == 1 && cropped.get_right() == 3);
  assert(cropped.eval(1) == f.eval(1) && cropped.eval(3) == f.eval(3));
  cropped.shift(2,5);
  assert(cropped.eval(3) == f.eval(1)+5);

  std::cout << "piecewise_linear_function: all tests passed\n";
}
