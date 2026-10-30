#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/segtree_2d_sparse.hpp"

long long op(long long x, long long y) {return x+y;}
long long identity() {return 0;}

struct Query {
  int type;
  long long a, b, c, d;
};

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<Query> initial(n), queries(q);
  soryuusi::Segtree2D_Sparse<long long,op,identity,long long,long long> seg(n+q);
  for(auto& query : initial) {
    soryuusi::cin >> query.a >> query.b >> query.c;
    query.type = 0;
    seg.add_point(query.a,query.b);
  }
  for(auto& query : queries) {
    soryuusi::cin >> query.type;
    if(query.type == 0) {
      soryuusi::cin >> query.a >> query.b >> query.c;
      seg.add_point(query.a,query.b);
    }else{
      soryuusi::cin >> query.a >> query.b >> query.c >> query.d;
    }
  }
  seg.build();
  for(const auto& query : initial) seg.add(query.a,query.b,query.c);
  for(const auto& query : queries) {
    if(query.type == 0) seg.add(query.a,query.b,query.c);
    else soryuusi::cout << seg.prod(query.a,query.c,query.b,query.d) << '\n';
  }
}
