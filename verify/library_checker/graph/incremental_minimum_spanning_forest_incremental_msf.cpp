#include "library/utility/fastio.hpp"

#include <unordered_map>
#include <utility>
#include <vector>

#include "library/graph/incremental_msf.hpp"

int main() {
  int n, m;
  soryuusi::cin >> n >> m;
  std::vector<std::pair<int,int>> edges(m);
  std::vector<int> weight(m);
  std::unordered_map<long long,int> id;
  id.reserve(m*2);
  for(int i = 0; i < m; ++i) {
    soryuusi::cin >> edges[i].first >> edges[i].second >> weight[i];
    id[weight[i]] = i;
  }
  std::vector<long long> msf_weight(m+1), history;
  long long current = 0;
  auto comp = [&](int i, int j) {return weight[i] < weight[j];};
  auto add = [&](int i) {current += weight[i];};
  auto snapshot = [&] {history.push_back(current);};
  auto rollback = [&] {current = history.back(); history.pop_back();};
  auto solve = [&](int i) {msf_weight[i] = current;};
  soryuusi::incrementalMSF(n,edges,comp,add,snapshot,rollback,solve);
  for(int i = 0; i < m; ++i) {
    int answer = -1;
    if(msf_weight[i+1] != msf_weight[i]+weight[i]) {
      answer = id[msf_weight[i]+weight[i]-msf_weight[i+1]];
    }
    soryuusi::cout << answer << " \n"[i+1 == m];
  }
}
