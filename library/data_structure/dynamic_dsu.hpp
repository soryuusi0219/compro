// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_dsu.hpp
#pragma once
#include <stdexcept>
#include <utility>

#include "hash_table.hpp"
#include "internal/hash_table_storage.hpp"

namespace soryuusi {

using namespace std;

struct dynamic_dsu{
  private:
    internal::HashTableStorage<int,int,hash_int<int>,false> table;

    static int checked_size(int n) {
      if(n > (1 << 29)) throw length_error("dynamic_dsu capacity is too large");
      return n;
    }

    int leader_slot(int u) {
      int i = table.insert(u,[]{ return -1; });
      while(table.value(i) >= 0) {
        int p = table.value(i), g = table.value(p);
        if(g < 0) return p;
        table.value(i) = g;
        i = g;
      }
      return i;
    }
  public:
    dynamic_dsu(int n) : table(checked_size(n)) {}
    int leader(int u) { return table.key(leader_slot(u)); }
    bool same(int u, int v) { return leader_slot(u) == leader_slot(v); }
    int size(int u) { return -table.value(leader_slot(u)); }
    int merge(int u, int v) {
      int i = leader_slot(u), j = leader_slot(v);
      if(i == j) return table.key(i);
      if(table.value(i) > table.value(j)) swap(i,j);
      table.value(i) += table.value(j);
      table.value(j) = i;
      return table.key(i);
    }
};

}  // namespace soryuusi
