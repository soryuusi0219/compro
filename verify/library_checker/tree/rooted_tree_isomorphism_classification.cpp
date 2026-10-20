#include "library/utility/fastio.hpp"

#include <map>
#include <vector>

#include "library/tree/tree_isom.hpp"

int main() {
  int n;
  soryuusi::cin >> n;
  std::vector<std::vector<long long>> tree(n);
  for(int vertex = 1; vertex < n; ++vertex) {
    int parent;
    soryuusi::cin >> parent;
    tree[parent].push_back(vertex);
    tree[vertex].push_back(parent);
  }
  const auto hashes = soryuusi::tree_hash::get_rooted_hashs(tree);
  std::map<int,int> ids;
  std::vector<int> answer(n);
  for(int i = 0; i < n; ++i) {
    const auto [it,inserted] = ids.emplace(hashes[i],ids.size());
    (void)inserted;
    answer[i] = it->second;
  }
  soryuusi::cout << ids.size() << '\n';
  for(int i = 0; i < n; ++i) {
    soryuusi::cout << answer[i] << " \n"[i+1 == n];
  }
}
