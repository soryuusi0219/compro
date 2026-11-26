#include <cstddef>
#include <vector>
#include "library/graph/assignment_problem.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int n;
  soryuusi::cin >> n;
  std::vector<int> cost(std::size_t(n)*n);
  for(int& c : cost) soryuusi::cin >> c;
  const auto result = soryuusi::assignment_problem<int>(n,n,cost);
  soryuusi::cout << result->cost << '\n';
  for(int i = 0; i < n; ++i) soryuusi::cout << result->left_to_right[i] << (i+1 == n ? '\n' : ' ');
}
