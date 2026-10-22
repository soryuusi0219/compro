#include "library/utility/fastio.hpp"

#include "library/math/general_floor_sum.hpp"

struct Transform {
  long long height, coefficient, sum;
};

Transform compose(Transform first, Transform second) {
  return {
    first.height+second.height,
    first.coefficient+second.coefficient,
    first.sum+second.sum+second.coefficient*first.height
  };
}
Transform identity() {return {0,0,0};}
Transform power(Transform x, long long n) {
  Transform result = identity();
  while(n) {
    if(n&1) result = compose(result,x);
    x = compose(x,x);
    n >>= 1;
  }
  return result;
}

int main() {
  int t;
  soryuusi::cin >> t;
  while(t--) {
    long long n, m, a, b;
    soryuusi::cin >> n >> m >> a >> b;
    const Transform x{0,1,0}, y{1,0,0};
    soryuusi::cout << soryuusi::general_floor_sum<Transform,compose,identity,power>(n,m,a,b,x,y).sum << '\n';
  }
}
