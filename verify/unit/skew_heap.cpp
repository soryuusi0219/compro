#include <cassert>
#include <random>
#include <set>
#include <utility>
#include <vector>

#include "library/data_structure/skew_heap.hpp"

using Entry = std::pair<int,int>;
using Model = std::multiset<Entry>;

Model shifted(const Model& model, int delta){
  Model result;
  for(auto [key,value] : model) result.emplace(key+delta,value);
  return result;
}

void remove_top(Model& model, Entry top){
  assert(top.first == model.begin()->first);
  auto pos = model.find(top);
  assert(pos != model.end());
  model.erase(pos);
}

int main(){
  soryuusi::SkewHeap<int> keys;
  auto heap = keys.new_heap();
  heap.push(1);
  assert(heap.top().first == 1);
  soryuusi::PersistentSkewHeap<int> persistent_keys;
  auto key_root = persistent_keys.push(persistent_keys.empty_root(),1);
  assert(persistent_keys.top(key_root).first == 1);

  std::mt19937 rng(63425);
  using Arena = soryuusi::SkewHeap<int,int>;
  auto [a,b] = []{
    Arena arena;
    auto a = arena.new_heap();
    Arena moved = std::move(arena);
    return std::pair(std::move(a),moved.new_heap());
  }();
  a.push(7,0);
  b.push(2,1);
  a.add_all(-6);
  b.meld(std::move(a));
  assert(a.empty() && b.size() == 2 && b.top() == Entry(1,0));
  a.push(0,2);
  a = std::move(b);
  assert(b.empty() && a.size() == 2);
  a.pop();
  assert(a.top() == Entry(2,1));
  a.pop();
  assert(a.empty());

  Arena arena;
  std::vector<Arena::Heap> heaps;
  for(int i = 0; i < 8; ++i) heaps.push_back(arena.new_heap());
  std::vector<Model> models(heaps.size());
  for(int i = 0; i < 4000; ++i){
    int x = rng() % heaps.size(), y = rng() % heaps.size();
    int delta = (int)(rng() % 21)-10;
    auto& heap = heaps[x];
    auto& model = models[x];
    switch(rng()%4){
    case 0: heap.push(delta,i); model.emplace(delta,i); break;
    case 1:
      if(!model.empty()){remove_top(model,heap.top()); heap.pop();}
      break;
    case 2: heap.add_all(delta); model = shifted(model,delta); break;
    case 3:
      if(x != y){
        heap.meld(std::move(heaps[y]));
        model.merge(models[y]);
        assert(heaps[y].empty());
      }
      break;
    }
    assert(heap.size() == model.size());
    if(!model.empty()) assert(heap.top().first == model.begin()->first);
  }
  for(int i = 0; i < (int)heaps.size(); ++i){
    while(!heaps[i].empty()){remove_top(models[i],heaps[i].top()); heaps[i].pop();}
    assert(models[i].empty());
  }

  using Persistent = soryuusi::PersistentSkewHeap<int,int>;
  Persistent persistent;
  std::vector<Persistent::Root> roots{persistent.empty_root()};
  models.assign(1,{});
  for(int i = 0; i < 4000; ++i){
    int x = rng() % roots.size(), y = rng() % roots.size();
    int delta = (int)(rng() % 21)-10;
    const auto old = roots[x];
    auto root = old;
    Model model = models[x];
    switch(rng()%4){
    case 0: root = persistent.push(old,delta,i); model.emplace(delta,i); break;
    case 1:
      if(!model.empty()){
        remove_top(model,persistent.top(old));
        root = persistent.pop(old);
      }
      break;
    case 2: root = persistent.add_all(old,delta); model = shifted(model,delta); break;
    case 3:
      if(model.size() + models[y].size() <= 200){
        root = persistent.meld(old,roots[y]);
        model.insert(models[y].begin(),models[y].end());
      }
      break;
    }
    const auto& query = persistent;
    auto before = query.allocated_nodes();
    assert(query.size(old) == models[x].size() && query.size(root) == model.size());
    assert(query.empty(root) == model.empty());
    if(!models[x].empty()) assert(query.top(old).first == models[x].begin()->first);
    if(!model.empty()) assert(query.top(root).first == model.begin()->first);
    assert(query.allocated_nodes() == before);
    roots.push_back(root);
    models.push_back(std::move(model));
  }
  auto one = persistent.push(roots[0],1,42);
  auto two = persistent.meld(one,one);
  Persistent moved = std::move(persistent);
  assert(moved.size(two) == 2);
  auto shifted_two = moved.add_all(two,5);
  assert(moved.top(shifted_two) == Entry(6,42));
  assert(moved.top(moved.pop(shifted_two)) == Entry(6,42));
  assert(moved.size(one) == 1 && moved.top(one) == Entry(1,42));
  for(int i = 0; i < (int)roots.size(); i += 13){
    auto root = roots[i];
    auto model = models[i];
    while(!moved.empty(root)){remove_top(model,moved.top(root)); root = moved.pop(root);}
    assert(model.empty() && moved.size(roots[i]) == models[i].size());
  }
}
