#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/persistent_queue.hpp"

int main() {
  int q;
  soryuusi::cin >> q;
  soryuusi::Persistent_Queue<int> queue;
  using QueuePointer = soryuusi::Persistent_Queue<int>::qp;
  std::vector<QueuePointer> versions(q+1);
  versions[0] = queue.origin;
  for(int i = 0; i < q; ++i) {
    int type, t;
    soryuusi::cin >> type >> t;
    ++t;
    if(type == 0) {
      int x;
      soryuusi::cin >> x;
      versions[i+1] = queue.push(x,versions[t]);
    } else {
      soryuusi::cout << queue.front(versions[t]) << '\n';
      versions[i+1] = queue.pop(versions[t]);
    }
  }
}
