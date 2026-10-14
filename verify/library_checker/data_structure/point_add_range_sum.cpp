#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/bit.hpp"

using namespace std;
using soryuusi::BIT;

int main() {

  int n, q;
  soryuusi::cin >> n >> q;

  BIT<long long> bit(n);
  for (int i = 0; i < n; ++i) {
    long long value;
    soryuusi::cin >> value;
    bit.add(i, value);
  }

  while (q--) {
    int type, first;
    long long second;
    soryuusi::cin >> type >> first >> second;
    if (type == 0) {
      bit.add(first, second);
    } else {
      soryuusi::cout << bit.sum(first, static_cast<int>(second)) << '\n';
    }
  }
}
