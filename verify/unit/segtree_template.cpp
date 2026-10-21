#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include "library/data_structure/segtree_template.hpp"

int main(){
  soryuusi::SegtreeMin<int,100> mn(std::vector<int>{3,1,4});
  soryuusi::SegtreeMax<int,-100> mx(std::vector<int>{3,1,4});
  assert(mn.prod(0,3) == 1 && mn.prod(0,0) == 100);
  assert(mx.prod(0,3) == 4 && mx.prod(0,0) == -100);

  soryuusi::SegtreeMinCount<int,long long,100> mnc(std::vector<int>{2,1,1});
  soryuusi::SegtreeMaxCount<int,long long,-100> mxc(std::vector<int>{2,2,1});
  assert((mnc.prod(0,3) == std::pair<int,long long>(1,2)));
  assert((mnc.prod(0,0) == std::pair<int,long long>(100,0)));
  assert((mxc.prod(0,3) == std::pair<int,long long>(2,2)));
  assert((mxc.prod(0,0) == std::pair<int,long long>(-100,0)));

  std::cout << "segtree_template: all tests passed\n";
}
