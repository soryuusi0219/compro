#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>

#include "library/data_structure/hash_table.hpp"

int main() {

  int q;
  soryuusi::cin >> q;
  soryuusi::HashTable<long long, long long, soryuusi::hash_int<long long>> hash_map(q);
  while (q--) {
    int type;
    soryuusi::cin >> type;
    if(type == 0) {
      long long k, v;
      soryuusi::cin >> k >> v;
      hash_map[k] = v;
    } else {
      long long k;
      soryuusi::cin >> k;
      soryuusi::cout << hash_map[k] << '\n';
    }
  }
}
